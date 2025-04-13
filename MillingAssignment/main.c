#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Pin definitions
#define ENABLE_PIN   0
#define X_STEP_PIN   2
#define X_DIR_PIN    3
#define Y_STEP_PIN   4
#define Y_DIR_PIN    5
#define Z_STEP_PIN   6
#define Z_DIR_PIN    7

// Parameters
#define DIR_CHANGE_DELAY_MS 100
#define STEPS 100
#define MAX_LINE_LENGTH 100
#define MAX_STRING_LENGTH 100

int MIN_DELAY_US = 200;
int MAX_DELAY_US = 600;

// Line buffer
char** lines = NULL; // dynamically allocated list of string lines
int line_count = 0;

typedef union {
    int i_val;
    char s_val[MAX_STRING_LENGTH];
} Value;

typedef enum {
    INT,
    STRING
} ValueType;

typedef struct {
    ValueType type;
    Value value;
} Element;

Element* Storage1;

// Drop-in fgets() replacement using getchar
char* fgets_live(char* buffer, int maxlen, FILE* stream) {
    int i = 0;
    int ch;

    if (maxlen <= 0) return NULL;

    while (i < maxlen - 1) {
        ch = getchar();

        if (ch == '\r' || ch == '\n') {
            buffer[i] = '\0';  // Null-terminate early
            putchar('\n');

            // === DEBUG/CONFIRMATION BLOCK START ===
            if (strcmp(buffer, "hello?") == 0) {
                printf("yes I'm here\n");
            }

            const char* marker = "confirm command";
            int marker_len = strlen(marker);
            int buffer_len = strlen(buffer);

            if (buffer_len >= marker_len &&
                strcasecmp(buffer + buffer_len - marker_len, marker) == 0) {
                int command_len = buffer_len - marker_len - 1;
                if (command_len < 0) command_len = 0;

                char clean_command[MAX_LINE_LENGTH];
                strncpy(clean_command, buffer, command_len);
                clean_command[command_len] = '\0';
                strcpy(buffer, clean_command);
                printf("%s, command confirmed\n", buffer);
            }
            // === DEBUG/CONFIRMATION BLOCK END ===

            return buffer;
        }

        if ((ch == '\b' || ch == 127) && i > 0) {
            i--;
            putchar('\b'); putchar(' '); putchar('\b');
            continue;
        }

        if (ch >= 32 && ch <= 126) {
            buffer[i++] = (char)ch;
            putchar(ch);
        }
    }

    buffer[i] = '\0';
    return buffer;
}

// Functions used for motion
void motor_motion(uint step_pin, int total_steps, int min_delay_us, int max_delay_us) {
    for (int i = 0; i < total_steps; i++) {
        float ratio = (float)i / total_steps;
        float delay = max_delay_us - (max_delay_us - min_delay_us) * (0.5f - 0.5f * cosf(ratio * M_PI));
        gpio_put(step_pin, 1);
        sleep_us((int)delay);
        gpio_put(step_pin, 0);
        sleep_us((int)delay);
    }
}

void move_x_direction(int steps, bool dir) {
    gpio_put(ENABLE_PIN, 0);
    gpio_put(X_DIR_PIN, dir);
    sleep_ms(DIR_CHANGE_DELAY_MS);
    motor_motion(X_STEP_PIN, steps, MIN_DELAY_US, MAX_DELAY_US);
    gpio_put(ENABLE_PIN, 1);
}

void move_y_direction(int steps, bool dir) {
    gpio_put(ENABLE_PIN, 0);
    gpio_put(Y_DIR_PIN, dir);
    sleep_ms(DIR_CHANGE_DELAY_MS);
    motor_motion(Y_STEP_PIN, steps, MIN_DELAY_US, MAX_DELAY_US);
    gpio_put(ENABLE_PIN, 1);
}

void move_z_direction(int steps, bool dir) {
    gpio_put(ENABLE_PIN, 0);
    gpio_put(Z_DIR_PIN, dir);
    sleep_ms(DIR_CHANGE_DELAY_MS);
    motor_motion(Z_STEP_PIN, steps, MIN_DELAY_US, MAX_DELAY_US);
    gpio_put(ENABLE_PIN, 1);
}

int is_number(const char* str) {
    if (str == NULL || *str == '\0') return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i]) && (i > 0 || str[i] != '-')) return 0;
    }
    return 1;
}

Element* split(const char* str, int* size) {
    int count = 1;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ',') count++;
    }

    Element* arr = (Element*)malloc(count * sizeof(Element));
    if (!arr) {
        *size = 0;
        return NULL;
    }

    char* str_copy = strdup(str);
    char* token = strtok(str_copy, ",");
    int index = 0;

    while (token != NULL && index < count) {
        if (is_number(token)) {
            arr[index].type = INT;
            arr[index].value.i_val = atoi(token);
        } else {
            arr[index].type = STRING;
            strncpy(arr[index].value.s_val, token, MAX_STRING_LENGTH - 1);
            arr[index].value.s_val[MAX_STRING_LENGTH - 1] = '\0';
        }
        index++;
        token = strtok(NULL, ",");
    }

    *size = count;
    free(str_copy);
    return arr;
}

// Free stored line memory
void free_lines() {
    if (lines) {
        for (int i = 0; i < line_count; i++) {
            free(lines[i]);
        }
        free(lines);
        lines = NULL;
    }
    line_count = 0;
}

// Interactive file input mode
void line_wire_aquire(int max_lines) {
    free_lines();
    lines = malloc(max_lines * sizeof(char*));
    if (!lines) return;

    line_count = 0;
    printf("Enter %d line(s):\n", max_lines);
    while (line_count < max_lines) {
        char buffer[MAX_LINE_LENGTH];
        if (fgets_live(buffer, sizeof(buffer), stdin)) {
            lines[line_count] = malloc(MAX_LINE_LENGTH);
            strncpy(lines[line_count], buffer, MAX_LINE_LENGTH - 1);
            lines[line_count][MAX_LINE_LENGTH - 1] = '\0';
            printf("Stored line %d: %s\n", line_count, lines[line_count]);
            line_count++;
        }
    }
}

// Main
int main() {
    stdio_usb_init();
    while (!stdio_usb_connected()) sleep_ms(100);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);

    // GPIO setup
    gpio_init(ENABLE_PIN); gpio_set_dir(ENABLE_PIN, GPIO_OUT);
    gpio_init(X_STEP_PIN); gpio_set_dir(X_STEP_PIN, GPIO_OUT);
    gpio_init(X_DIR_PIN); gpio_set_dir(X_DIR_PIN, GPIO_OUT);
    gpio_init(Y_STEP_PIN); gpio_set_dir(Y_STEP_PIN, GPIO_OUT);
    gpio_init(Y_DIR_PIN); gpio_set_dir(Y_DIR_PIN, GPIO_OUT);
    gpio_init(Z_STEP_PIN); gpio_set_dir(Z_STEP_PIN, GPIO_OUT);
    gpio_init(Z_DIR_PIN); gpio_set_dir(Z_DIR_PIN, GPIO_OUT);
    gpio_put(ENABLE_PIN, 0);

    printf("X-Axis left and right (A/D), Y-Axis forward and back (W/S), Z-Axis up and down (Q/E)\n");

    bool run_from_file = false;

    while (true) {
        char input_buffer[MAX_LINE_LENGTH];
        printf("--");

        if (run_from_file) {
            if (line_count > 0) {
                int sz = 0;
                Storage1 = split(lines[0], &sz);
                for (int i = 1; i < line_count; i++) {
                    lines[i - 1] = lines[i];
                }
                line_count--;
            } else {
                run_from_file = false;
                printf("ending run from file\n");
                Storage1 = NULL;
            }
        } else {
            fgets_live(input_buffer, sizeof(input_buffer), stdin);
            int sz = 0;
            Storage1 = split(input_buffer, &sz);
        }

        if (!Storage1) continue;

        if (Storage1[0].type == STRING && Storage1[1].type == INT) {
            char command = Storage1[0].value.s_val[0];
            int val = Storage1[1].value.i_val;

            switch (command) {
                case 'a': case 'A':
                    printf("Move LEFT %d steps\n", val);
                    move_x_direction(val, 0);
                    break;
                case 'd': case 'D':
                    printf("Move RIGHT %d steps\n", val);
                    move_x_direction(val, 1);
                    break;
                case 'w': case 'W':
                    printf("Move FORWARD %d steps\n", val);
                    move_y_direction(val, 1);
                    break;
                case 's': case 'S':
                    printf("Move BACKWARD %d steps\n", val);
                    move_y_direction(val, 0);
                    break;
                case 'q': case 'Q':
                    printf("Move UP %d steps\n", val);
                    move_z_direction(val, 1);
                    break;
                case 'e': case 'E':
                    printf("Move DOWN %d steps\n", val);
                    move_z_direction(val, 0);
                    break;
                case 'n': case 'N':
                    printf("New MIN_DELAY = %d us\n", val);
                    MIN_DELAY_US = val;
                    break;
                case 'm': case 'M':
                    printf("New MAX_DELAY = %d us\n", val);
                    MAX_DELAY_US = val;
                    break;
                case 'i': case 'I':
                    free_lines();
                    line_wire_aquire(val);
                    run_from_file = true;
                    break;
                default:
                    printf("Invalid input: %c\n", command);
                    break;
            }
        }

        free(Storage1);
        Storage1 = NULL;
    }
}
