#include "funshield.h"

// pin definitions
constexpr int PINS = 4;
constexpr int led_pins[PINS] = {led1_pin, led2_pin, led3_pin, led4_pin};

constexpr long DELAY = 500;	// the delay duration

constexpr int PWM_SMOOTHNESS = 15;

// calculates, whether to turn on or turn off the LED via PWM
// uses the following trick:
// - a % b is essentially a random number
// - a / b gradually goes up as b does
bool pwm(int d) {
	return (d % PWM_SMOOTHNESS) >= (d / PWM_SMOOTHNESS);
}

void setup() {
	// initialize all pins to output
	// we also have to set them to off, since they're on by default for some reason
	for (int i = 0; i < PINS; i++) {
		pinMode(led_pins[i], OUTPUT);
		digitalWrite(led_pins[i], OFF);
	}
}

// the position within the LEDs
int i = -1;

// time since startup
long time = DELAY;

void loop() {
	if (millis() > time) {
		// make sure to really fade the LED
		digitalWrite(led_pins[i], OFF);
		time += DELAY;
		i = (i + 1) % PINS;
		return;
	}

	// fade the current LED
	int time_since_update = time - millis();
	digitalWrite(led_pins[i], pwm(time_since_update));

	// turn on the next LED
	// same as the previous, but with reversed time since last update
	time_since_update = DELAY - time_since_update;
	digitalWrite(led_pins[(i + 1) % PINS], pwm(time_since_update));
}
