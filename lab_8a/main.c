/**************************************************************
 * main.c
 * rev 1.0 01-Apr-2025 caddi
 * Lab6a
 * ***********************************************************/

 #include "hardware/gpio.h"
 #include "pico/stdlib.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <ctype.h>
 #include "hardware/pwm.h"
 
 // You must define this struct and helper somewhere:
 typedef enum { INT, STRING } ValueType;
 #define MAX_STRING_LENGTH 50
 
 typedef struct {
     ValueType type;
     union {
         int i_val;
         char s_val[MAX_STRING_LENGTH];
     } value;
 } Element;
 
 extern int is_number(const char* str); // Ensure this is implemented
 
 #define GPIO_OUT_PINR 11
 #define GPIO_OUT_PING 12
 #define GPIO_OUT_PINB 13
 
 #define MAX_COMMAND_LENGTH 100  // Maximum command length
 
 // Make these global so they're accessible in main
 uint slice_numR, slice_numG, slice_numB;
 
 void setup_gpio() {
     gpio_init(GPIO_OUT_PINR);
     gpio_init(GPIO_OUT_PING);
     gpio_init(GPIO_OUT_PINB);
 
     gpio_set_function(GPIO_OUT_PINR, GPIO_FUNC_PWM);
     gpio_set_function(GPIO_OUT_PING, GPIO_FUNC_PWM);
     gpio_set_function(GPIO_OUT_PINB, GPIO_FUNC_PWM);   // Set pin to PWM
 
     slice_numR = pwm_gpio_to_slice_num(GPIO_OUT_PINR);  // Get PWM slice
     slice_numG = pwm_gpio_to_slice_num(GPIO_OUT_PING);  // Get PWM slice
     slice_numB = pwm_gpio_to_slice_num(GPIO_OUT_PINB);  // Get PWM slice
 
     pwm_set_wrap(slice_numR, 255);          // PWM counter range
     pwm_set_wrap(slice_numG, 255);          // PWM counter range
     pwm_set_wrap(slice_numB, 255);          // PWM counter range
 
     pwm_set_chan_level(slice_numR, pwm_gpio_to_channel(GPIO_OUT_PINR), 0);
     pwm_set_chan_level(slice_numG, pwm_gpio_to_channel(GPIO_OUT_PING), 0);
     pwm_set_chan_level(slice_numB, pwm_gpio_to_channel(GPIO_OUT_PINB), 0);
 
     pwm_set_enabled(slice_numR, true);       // Start PWM
     pwm_set_enabled(slice_numG, true);
     pwm_set_enabled(slice_numB, true);
 }
 
 void handshake() {
     stdio_usb_init();
     while (!stdio_usb_connected()) sleep_ms(100);
     setvbuf(stdout, NULL, _IONBF, 0);
     setvbuf(stdin, NULL, _IONBF, 0);
 }
 
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
 
                 char clean_command[MAX_COMMAND_LENGTH];
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
 
 int is_number(const char* str) {
     if (*str == '\0') return 0;
     while (*str) {
         if (!isdigit((unsigned char)*str)) return 0;
         str++;
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
 
 int main(void) {
     setup_gpio();  // Initialize GPIO pins
     handshake();
 
     char input_buffer[MAX_COMMAND_LENGTH];
     Element* Storage1 = NULL;
 
     double outputval;
 
     while (true) {
         printf("Enter a command (R/G/B): ");
 
         fgets_live(input_buffer, sizeof(input_buffer), stdin);
         int sz = 0;
         Storage1 = split(input_buffer, &sz);
 
         if (sz < 2 || Storage1 == NULL) {
             printf("Invalid input.\n");
             continue;
         }
 
         outputval = ((Storage1[1].value.i_val * Storage1[1].value.i_val) / 10000.0) * 255.0; // perceptual brightness correction
 
         switch (Storage1[0].value.s_val[0]) {
             case 'R': case 'r': // Red command
                 printf("Red command received\n");
                 pwm_set_chan_level(slice_numR, pwm_gpio_to_channel(GPIO_OUT_PINR), (uint16_t)outputval);
                 break;
             case 'G': case 'g': // Green command
                 printf("Green command received\n");
                 pwm_set_chan_level(slice_numG, pwm_gpio_to_channel(GPIO_OUT_PING), (uint16_t)outputval);
                 break;
             case 'B': case 'b': // Blue command
                 printf("Blue command received\n");
                 pwm_set_chan_level(slice_numB, pwm_gpio_to_channel(GPIO_OUT_PINB), (uint16_t)outputval);
                 break;
             default:
                 printf("Awaiting command\n");
         }
 
         free(Storage1); // Clean up allocated memory
     }
 
     return 0;
 }
 