/**************************************************************
 * main.c
 * rev 1.0 01-Apr-2025 caddi
 * Lab6a
 * ***********************************************************/

 #include "hardware/gpio.h"
 #include "pico/stdlib.h"
 #include <stdio.h>
 
 #define GPIO_OUT_PINR 15
 #define GPIO_OUT_PING 16
 #define GPIO_OUT_PINB 17
 
 #define MAX_COMMAND_LENGTH 100  // Maximum command length
 
 void setup_gpio() {
     gpio_init(GPIO_OUT_PINR);
     gpio_init(GPIO_OUT_PING);
     gpio_init(GPIO_OUT_PINB);
 
     gpio_set_dir(GPIO_OUT_PINR, GPIO_OUT);
     gpio_set_dir(GPIO_OUT_PING, GPIO_OUT);
     gpio_set_dir(GPIO_OUT_PINB, GPIO_OUT);
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

 
 int main(void) {
     setup_gpio();  // Initialize GPIO pins
     handshake();
 
     char command[MAX_COMMAND_LENGTH];
     char ch;
 
     while (true) {
         printf("Enter a command (R/G/B): ");
         ch = getchar();
 
        
 
         switch (ch) {
             case 'R':  // Red command
                 printf("Red command received\n");
                 gpio_put(GPIO_OUT_PINR, !gpio_get(GPIO_OUT_PINR));
                 break;
             case 'G':  // Green command
                 printf("Green command received\n");
                 gpio_put(GPIO_OUT_PING, !gpio_get(GPIO_OUT_PING));
                 break;
             case 'B':  // Blue command
                 printf("Blue command received\n");
                 gpio_put(GPIO_OUT_PINB, !gpio_get(GPIO_OUT_PINB));
                 break;
             default:
                 printf("Awaiting command\n");
         }
     }
 
     return 0;
 }
 