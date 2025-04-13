/**************************************************************
 * main.c
 * rev 1.2 14-Apr-2025 caddi
 * Lab6a - SOS Morse Code using onboard LED (GPIO 25)
 **************************************************************/

 #include "hardware/gpio.h"
 #include "pico/stdlib.h"
 #include <stdio.h>
 #include <string.h>
 
 #define GPIO_OUT_PING 25   // Onboard LED pin
 #define DOT 100            // duration of a dot (ms)
 #define DASH (3 * DOT)     // duration of a dash (ms)
 #define GAP (DOT)          // gap between dits and dahs
 #define LETTER_GAP (3 * DOT)
 #define WORD_GAP (7 * DOT)
 
 // Setup LED output pin
 void setup_gpio() {
     gpio_init(GPIO_OUT_PING);
     gpio_set_dir(GPIO_OUT_PING, GPIO_OUT);
     gpio_put(GPIO_OUT_PING, 0);  // start low
 }
 
 // Morse dot
 void dit() {
     gpio_put(GPIO_OUT_PING, 1);
     sleep_ms(DOT);
     gpio_put(GPIO_OUT_PING, 0);
     sleep_ms(GAP);
 }
 
 // Morse dash
 void dah() {
     gpio_put(GPIO_OUT_PING, 1);
     sleep_ms(DASH);
     gpio_put(GPIO_OUT_PING, 0);
     sleep_ms(GAP);
 }
 
 int main(void) {
     setup_gpio();
 
     while (true) {
         // S = ···
         dit();
         dit();
         dit();
         sleep_ms(LETTER_GAP);
 
         // O = ---
         dah();
         dah();
         dah();
         sleep_ms(LETTER_GAP);
 
         // S = ···
         dit();
         dit();
         dit();
         sleep_ms(WORD_GAP);  // long pause between repetitions
     }
 
     return 0;
 }
 