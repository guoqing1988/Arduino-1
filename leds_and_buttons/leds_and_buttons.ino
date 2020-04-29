#include "funshield.h"


// VARIABLES //
constexpr int LEDS = 4;
constexpr int LED_PINS[LEDS] = {led1_pin, led2_pin, led3_pin, led4_pin};

constexpr int BUTTONS = 3;
constexpr int BUTTON_PINS[LEDS] = {button1_pin, button2_pin, button3_pin};

constexpr int BUFFER_SIZE = 20;
bool button_buffer[BUTTONS][BUFFER_SIZE];  // for taking the button average
int buffer_counter = 0;  // so we can continuously write somewhere to the buffer

long time_pressed[BUTTONS] = {0, 0, 0};  // for tracking, when was it pressed last
long previous_state[BUTTONS] = {false, false, false};  // for tracking the state in the previous loop


constexpr int BUTTON_WAIT_DURATION = 500;  // how long to wait before continuously incrementing
constexpr int BUTTON_DELAY = 150;  // how quickly to continuously increment

int counter = 0;  // the current LED number


// HELPER FUNCTIONS //
// mirrors a binary representation of a number using binary magix
int mirror(int number) {
	// 0001 0001 0001 0 |
	//             abcd | *
	// -----------------+
	// abcd abcd abcd 0 |
	number *= 0b0001000100010;

	// abcd abcd abcd 0 |
	// 1001 0100 0010 0 | &
	// -----------------+
	// a00d 0b00 00c0 0 |
	number &= 0b1001010000100;

	// 00000a |
	// 00d0b0 |
	// 000c00 | +
	// -------+
	// 00dcba |
	number %= 0b111111;

	return number;
}

// turns on the leds, depending on the counter
void turn_leds_on(int counter) {
	for (int i = 0; i < LEDS; i++)
		digitalWrite(LED_PINS[i], !(counter & (1 << i)));
}

// checking a button status, depending on the buffer
bool is_button_on(int button) {
	int average = 0;

	for (int i = 0; i < BUFFER_SIZE; i++)
		average += button_buffer[button][i];

	return (bool)(average > BUFFER_SIZE / 2);
}


// check for whether a short or a long click has been pressed on a button
bool check_clicks(int button, int delta) {
	if (is_button_on(button)) {
		// short
		if (!previous_state[button])
			counter = (counter + delta) % 16;

		// long
		else if (millis() - time_pressed[button] > BUTTON_WAIT_DURATION) {
			counter = (counter + delta) % 16;
			time_pressed[button] += BUTTON_DELAY;
		}
	}
}


// THE MAIN PROGRAM CODE //
void setup() {
	// initialize all LED to output
	for (int i = 0; i < LEDS; i++) {
		pinMode(LED_PINS[i], OUTPUT);
		digitalWrite(LED_PINS[i], OFF);
	}

	// initialize all buttons to output
	for (int i = 0; i < BUTTONS; i++)
		pinMode(BUTTON_PINS[i], INPUT);
	
	// clean the button buffer
	for (int i = 0; i < BUTTONS; i++)
		for (int j = 0; j < BUFFER_SIZE; j++)
			button_buffer[i][j] = false;
}

void loop() {
	buffer_counter = (buffer_counter + 1) % BUFFER_SIZE;

	// read the current buttons value to the buffer
	for (int i = 0; i < BUTTONS; i++) {
		bool current_state = !digitalRead(BUTTON_PINS[i]);

		// if it was pressed just now but wasn't before
		if (current_state && !previous_state[i])
			time_pressed[i] = millis();

		button_buffer[i][buffer_counter] = current_state;
	}

	// check for short/long clicks on the buttons
	// buttons 0 and 1 are normal
	check_clicks(0, 1);
	check_clicks(1, 15);
	check_clicks(2, - counter + mirror(counter));

	turn_leds_on(counter);

	// remember previous button state
	for (int i = 0; i < BUTTONS; i++)
		previous_state[i] = is_button_on(i);
}
