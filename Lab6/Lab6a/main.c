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
 
 void setup_serial() {
     printf("Gods I hate Skype, yeah it's working now\n");  // Nonsense to confirm serial interface
     sleep_ms(20);
     printf("Waiting...\n");
     sleep_ms(20);
     printf("Ready.\n");
 }
 
 int main(void) {
     setup_gpio();  // Initialize GPIO pins
     stdio_init_all();  // Initialize serial interface
 
     char command[MAX_COMMAND_LENGTH];
     char ch;
 
     while (true) {
         printf("Enter a command (R/G/B): ");
         scanf("%s", command);
 
         ch = command[0];
 
         switch (ch) {
             case 'R':  // Red command
                 printf("Red command received\n");
                 break;
             case 'G':  // Green command
                 printf("Green command received\n");
                 break;
             case 'B':  // Blue command
                 printf("Blue command received\n");
                 break;
             default:
                 printf("Awaiting command\n");
         }
     }
 
     return 0;
 }
 