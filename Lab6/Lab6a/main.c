/**************************************************************
 * main.c
 * rev 1.1 14-Apr-2025 caddi
 * Lab6a - RGB GPIO control via USB serial
 **************************************************************/

 #include "hardware/gpio.h"
 #include "pico/stdlib.h"
 #include <stdio.h>
 #include <string.h>
 
 #define GPIO_OUT_PINR 11
 #define GPIO_OUT_PING 12
 #define GPIO_OUT_PINB 13
 
 #define MAX_COMMAND_LENGTH 100  // Maximum input length
 
 // Initialize GPIO pins for RGB output
 void setup_gpio() {
     gpio_init(GPIO_OUT_PINR);
     gpio_init(GPIO_OUT_PING);
     gpio_init(GPIO_OUT_PINB);
 
     gpio_set_dir(GPIO_OUT_PINR, GPIO_OUT);
     gpio_set_dir(GPIO_OUT_PING, GPIO_OUT);
     gpio_set_dir(GPIO_OUT_PINB, GPIO_OUT);
 
     // Start all pins low
     gpio_put(GPIO_OUT_PINR, 0);
     gpio_put(GPIO_OUT_PING, 0);
     gpio_put(GPIO_OUT_PINB, 0);
 }
 
 // Wait for USB serial connection before continuing
 void handshake() {
     stdio_usb_init();
     while (!stdio_usb_connected()) {
         sleep_ms(100);
     }
     setvbuf(stdout, NULL, _IONBF, 0);
     setvbuf(stdin, NULL, _IONBF, 0);
 }
 
 // Live line input with optional confirmation trimming
 char* fgets_live(char* buffer, int maxlen, FILE* stream) {
     int i = 0;
     int ch;
 
     if (maxlen <= 0) return NULL;
 
     while (i < maxlen - 1) {
         ch = getchar();
 
         if (ch == '\r' || ch == '\n') {
             buffer[i] = '\0';  // Null-terminate
             putchar('\n');
 
             // Debug/confirmation responses
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
 
                 char clean_command[20];
                 strncpy(clean_command, buffer, command_len);
                 clean_command[command_len] = '\0';
                 strcpy(buffer, clean_command);
                 printf("%s, command confirmed\n", buffer);
             }
 
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
 
 int main(void) {
     setup_gpio();
     handshake();
 
     char command[MAX_COMMAND_LENGTH];
 
     while (true) {
         printf("Enter a command (R/G/B or 'hello?' or with 'confirm command'): ");
         fgets_live(command, MAX_COMMAND_LENGTH, stdin);
 
         if (strcasecmp(command, "R") == 0 || strcasecmp(command, "red") == 0) {
             printf("Red command received\n");
             gpio_put(GPIO_OUT_PINR, !gpio_get(GPIO_OUT_PINR));
         } else if (strcasecmp(command, "G") == 0 || strcasecmp(command, "green") == 0) {
             printf("Green command received\n");
             gpio_put(GPIO_OUT_PING, !gpio_get(GPIO_OUT_PING));
         } else if (strcasecmp(command, "B") == 0 || strcasecmp(command, "blue") == 0) {
             printf("Blue command received\n");
             gpio_put(GPIO_OUT_PINB, !gpio_get(GPIO_OUT_PINB));
         } else {
             printf("Unknown command: '%s'\n", command);
         }
     }
 
     return 0;
 }
 