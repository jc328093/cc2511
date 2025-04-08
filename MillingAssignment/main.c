#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include <string.h>
#include <ctype.h>

//Defining the GPIO Pins used
// Enable pin
#define ENABLE_PIN   0  

// X pins
#define X_STEP_PIN 2  // pin pulses one step in x direction
#define X_DIR_PIN 3 

// Y pins
#define Y_STEP_PIN 4 // pin pulses one step in y direction
#define Y_DIR_PIN  5

// Z pins
#define Z_STEP_PIN 6 // pin pulses one step in z direction
#define Z_DIR_PIN  7  

// Stepping parameters
#define DIR_CHANGE_DELAY_MS 100 // wait time after changing direction before stepping
#define STEPS 100 // number of steps to take per movement.
MIN_DELAY_US=200;   // Fastest speed
#define MAX_DELAY_US 600   // Slowest speed
int command[100];

typedef union { // Define a union to hold either an integer or a string
    int i_val;
    char s_val[MAX_STRING_LENGTH];
} Value;

typedef enum { // Enum to track whether the element is an integer or a string

    INT,
    STRING
} ValueType;

typedef struct { // Struct to store the type and value
    ValueType type;
    Value value;
} Element;

Element* Storage1;



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
    gpio_put(ENABLE_PIN, 0); // enables the motor
    gpio_put(X_DIR_PIN, dir); // sets direction pin
    sleep_ms(DIR_CHANGE_DELAY_MS); 
    motor_motion(X_STEP_PIN, steps, MIN_DELAY_US, MAX_DELAY_US);// gives motor steps for x pin causing motion
    gpio_put(ENABLE_PIN, 1); // disbles motor once motion is complete
}

void move_y_direction(int steps, bool dir) {
    gpio_put(ENABLE_PIN, 0); // enables motor
    gpio_put(Y_DIR_PIN, dir); // sets y direction
    sleep_ms(DIR_CHANGE_DELAY_MS);
    motor_motion(Y_STEP_PIN, steps, MIN_DELAY_US, MAX_DELAY_US);// gives motor steps for y pin causing motion
    gpio_put(ENABLE_PIN, 1); // disables motor once motion is complete
}

void move_z_direction(int steps, bool direction) {
    gpio_put(ENABLE_PIN, 0); // enables motor
    gpio_put(Z_DIR_PIN, direction); // sets z direction
    sleep_ms(DIR_CHANGE_DELAY_MS);
    motor_motion(Z_STEP_PIN, steps, MIN_DELAY_US, MAX_DELAY_US); // gives motor steps for z pin causing motion
    gpio_put(ENABLE_PIN, 1); // disables motor once moton is complete
}

int is_number(const char* str) {// Function to check if the string represents a valid integer
    if (str == NULL || *str == '\0') return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i]) && (i > 0 || str[i] != '-')) {
            return 0;
        }
    }
    return 1;
}

Element* split(const char* str, int* size) {
    int count = 1; // Start with one element, assuming at least one item
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ',') {
            count++; // Count the number of elements separated by commas
        }
    }

    Element* arr = (Element*)malloc(count * sizeof(Element));
    if (arr == NULL) {
        *size = 0;
        return NULL; // Memory allocation failed
    }

    char* str_copy = strdup(str);  // Make a mutable copy of the string
    char* token = strtok(str_copy, ",");
    int index = 0;

    while (token != NULL) {
        if (is_number(token)) {
            arr[index].type = INT;
            arr[index].value.i_val = atoi(token);  // Convert string to integer
        } else {
            arr[index].type = STRING;
            strncpy(arr[index].value.s_val, token, MAX_STRING_LENGTH - 1);
            arr[index].value.s_val[MAX_STRING_LENGTH - 1] = '\0'; // Ensure null-termination
        }
        index++;
        token = strtok(NULL, ",");  // Get next token
    }

    *size = count;
    free(str_copy);

    return arr;
}

// Main 
int main() {
    stdio_init_all();

    gpio_init(ENABLE_PIN);
    // Initialising and setting all motor control pins as output
    gpio_set_dir(ENABLE_PIN, GPIO_OUT); 

    gpio_init(X_STEP_PIN);
    gpio_init(X_DIR_PIN);
    gpio_set_dir(X_STEP_PIN, GPIO_OUT);
    gpio_set_dir(X_DIR_PIN, GPIO_OUT);

    gpio_init(Y_STEP_PIN);
    gpio_init(Y_DIR_PIN);
    gpio_set_dir(Y_STEP_PIN, GPIO_OUT);
    gpio_set_dir(Y_DIR_PIN, GPIO_OUT);

    gpio_init(Z_STEP_PIN);
    gpio_init(Z_DIR_PIN);
    gpio_set_dir(Z_STEP_PIN, GPIO_OUT);
    gpio_set_dir(Z_DIR_PIN, GPIO_OUT);

    gpio_put(ENABLE_PIN, 0); // turning on the motor
    printf("X-Axis left and right (A/D), Y-Axis forward and back (W/S), Z-Axis up and down (Q/E)\n");

    char input;
    while (true) {
        char input_buffer[16];
            int new_delay;
            fgets(input_buffer, sizeof(input_buffer), stdin); // reads a single character input from serial terminal
            storage1= split(input_buffer,2);
            

            switch (storage1[0]) {
                case 'a': case 'A': // motion if 'a' key is pressed
                    printf("Move LEFT %d steps\n", storage1[1]);
                    move_x_direction(storage1[1], 0);
                    break; // ends this case and goes back to the loop, does NOT exit program
            
                case 'd': case 'D': // motion if 'd' key is pressed
                    printf("Move RIGHT %d steps\n", storage1[1]);
                    move_x_direction(storage1[1], 1);
                    break;
            
                case 'w': case 'W': // motion if 'w' key is pressed
                    printf("Move FORWARD %d steps\n", storage1[1]);
                    move_y_direction(storage1[1], 1);
                    break;
            
                case 's': case 'S': // motion if 's' key is pressed
                    printf("Move BACKWARD %d steps\n", storage1[1]);
                    move_y_direction(storage1[1], 0);
                    break;
            
                case 'q': case 'Q': // motion if 'q' key is pressed
                    printf("Move UP %d steps\n", storage1[1]);
                    move_z_direction(storage1[1], 1);
                    break;
            
                case 'e': case 'E': // motion if 'e' key is pressed
                    printf("Move DOWN %d steps\n", storage1[1]);
                    move_z_direction(storage1[1], 0);
                    break;
            case 'p': case 'P': // case if p key is pressed
            printf("Enter new minimum delay (microseconds): ");
            
        
            // Read from standard input
            fgets(input_buffer, sizeof(input_buffer), stdin);
        
            // Convert string to integer
            if (sscanf(input_buffer, "%d", &new_delay) == 1) {
                MIN_DELAY_US = new_delay;
                printf("Updated minimum delay to %d us\n", MIN_DELAY_US);
            } else {
                printf("Invalid input. Please enter a number.\n");
            }
            break;



            default:
                printf("Invalid input: %c\n", input);
        }
    }
}