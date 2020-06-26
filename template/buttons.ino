/* A module for working with buttons. */

#include "funshield.h"


constexpr int BUTTON_COUNT = 3;
constexpr int BUTTON_PINS[BUTTON_COUNT] = {button1_pin, button2_pin, button3_pin};

// a buffer of previous button states for removing the jitter
constexpr int BUFFER_SIZE = 20;
bool button_buffer[BUTTON_COUNT][BUFFER_SIZE];
int button_buffer_position = 0;

// previous states of buttons to determine whether it was just now pressed
long previous_button_state[BUTTON_COUNT] = {false, false, false};


/* Initialize the button pins (and clear the button buffer). */
void initialize_buttons() {
	// set pins
	for (int i = 0; i < BUTTON_COUNT; i++) {
		pinMode(BUTTON_PINS[i], INPUT);

		for (int j = 0; j < BUFFER_SIZE; j++)
			button_buffer[i][j] = false;
	}
}

/* Return true if the button is being pressed, else false. */
bool is_pressed(int button) {
	int average = 0;

	for (int i = 0; i < BUFFER_SIZE; i++)
		average += button_buffer[button][i];

	return average > BUFFER_SIZE / 2;
}

/* Return true if the button was just now pressed, else false. */
bool was_pressed(int button) {
	return !previous_button_state[button] && is_pressed(button);
}

/* Return true if the button was just now released, else false. */
bool was_released(int button) {
	return previous_button_state[button] && !is_pressed(button);
}

/* Update the buffer to the current state of the button. */
void update_button_buffer_state() {
	button_buffer_position = (button_buffer_position + 1) % BUFFER_SIZE;

	for (int i = 0; i < BUTTON_COUNT; i++) {
		bool current_state = !digitalRead(BUTTON_PINS[i]);
		button_buffer[i][button_buffer_position] = current_state;
	}
}

/* Update the previous button state to the current button state */
void update_previous_button_states() {
	for (int i = 0; i < BUTTON_COUNT; i++)
		previous_button_state[i] = is_pressed(i);
}
