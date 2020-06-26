/* A module for working with LEDs. */

#include "funshield.h"


constexpr int LED_COUNT = 4;
constexpr int LED_PINS[LED_COUNT] = {led1_pin, led2_pin, led3_pin, led4_pin};


/* Initialize all LEDs to output. */
void initialize_leds() {
	for (int i = 0; i < LED_COUNT; i++)
		pinMode(LED_PINS[i], OUTPUT);
	
	reset_leds();
}

/* Set all LEDs to off. */
void reset_leds() {
	for (int i = 0; i < LED_COUNT; i++)
		digitalWrite(LED_PINS[i], OFF);
}

/* Set the LED to on/off. */
void set_led(byte led, bool state) {
	digitalWrite(LED_PINS[led], !state);
}
