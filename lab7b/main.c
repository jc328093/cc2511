/**************************************************************
 * main.c
 * rev 1.0 23-Mar-2025 Riley
 * lab7b
 * ***********************************************************/
#include "hardware/gpio.h"
 #include "pico/stdlib.h"
 #include <stdio.h>
 
 #define LED_PIN 25  // GPIO pin for onboard LED
 
 #define GPIO_IN_PIN1 2
 #define GPIO_OUT_PIN1 20
 #define GPIO_OUT_PIN2 21

 #define MAX_COMMAND_LENGTH 100  // Maximum command length



 void setup_gpio() {


  gpio_init(GPIO_IN_PIN1);
  gpio_init(GPIO_OUT_PIN1);
  gpio_init(GPIO_OUT_PIN2);


  gpio_set_dir(GPIO_IN_PIN1, GPIO_IN);
  gpio_set_dir(GPIO_OUT_PIN1, GPIO_OUT);
  gpio_set_dir(GPIO_OUT_PIN2, GPIO_OUT);
  

  

}

 int main() {
     stdio_init_all();  // Initialize serial communication (USB serial)
      setup_gpio();


     gpio_init(LED_PIN);  // Initialize LED pin
     gpio_set_dir(LED_PIN, GPIO_OUT);  // Set LED pin as 
     
     char command[MAX_COMMAND_LENGTH];  // Buffer to store input command
	
    scanf("%s", command);

    printf("gods i hate skype, yeah its working now\n");
 
     while (true) {
        printf("...");
        sleep_ms(20);
        printf("waiting...");
        sleep_ms(20);
        printf("Ready.");

        scanf(command, MAX_COMMAND_LENGTH, stdin);



         char ch = command[0];  // Read one character from the terminal
         printf("You entered: %c\n", ch);  // Output the character entered
 
         if (ch == '1') {
             gpio_put(GPIO_OUT_PIN1, 1);  // Turn on the LED
             gpio_put(LED_PIN, 1);
             printf("LED is ON\n");
         } else if (ch == '0') {
             gpio_put(LED_PIN, 0);  // Turn off the LED
             printf("LED is OFF\n");
         } else {
             printf("Invalid input! Enter '1' to turn ON or '0' to turn OFF the LED.\n");
         }

         for (int i = 1; i <= 150; i++) {
          // Code to execute for each iteration
          
              
            
            gpio_put(GPIO_OUT_PIN2, 1);
          
            sleep_ms(10);
    
            
            gpio_put(GPIO_OUT_PIN2, 0);
          
            sleep_ms(10);
          
    
    
    
        }



     }

 
     return 0;
 }
 
