#include "funshield.h"



/* DEFINITIONS */
constexpr int BUTTONS = 3;
constexpr int BUTTON_PINS[BUTTONS] = {button1_pin, button2_pin, button3_pin};

constexpr int BUFFER_SIZE = 20;
bool button_buffer[BUTTONS][BUFFER_SIZE];  // for taking the button average
int buffer_counter = 0;  // so we can continuously write somewhere to the buffer

long time_pressed[BUTTONS] = {0, 0, 0};  // for tracking, when was it pressed last
long previous_state[BUTTONS] = {false, false, false};  // for tracking the state in the previous loop

constexpr int BUTTON_WAIT_DURATION = 500;  // how long to wait before continuously incrementing
constexpr int BUTTON_DELAY = 150;  // how quickly to continuously increment

long counter = 0;



/* BUTTON FUNCTIONS */
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
			counter = (counter + delta) % 40;

		// long
		else if (millis() - time_pressed[button] > BUTTON_WAIT_DURATION) {
			counter = (counter + delta) % 40;
			time_pressed[button] += BUTTON_DELAY;
		}
	}
}



/* DISPLAY SETTING STUFF */ 
// update the state of the given display
void set_display(int display, byte state) {
	digitalWrite(latch_pin, LOW);
	shiftOut(data_pin, clock_pin, MSBFIRST, 1 << display);

	digitalWrite(latch_pin, HIGH);
	shiftOut(data_pin, clock_pin, MSBFIRST, state);
}


// parse the counter to display on the digit display
void display_numbers(int counter) {
	set_display(counter / 10, digits[counter % 10]);
}



/* MAIN CODE */
void setup() {
	// initialize all buttons to output
	for (int i = 0; i < BUTTONS; i++)
		pinMode(BUTTON_PINS[i], INPUT);
	
	// initialize display pins to output
	pinMode(latch_pin, OUTPUT);
	pinMode(data_pin, OUTPUT);
	pinMode(clock_pin, OUTPUT);
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
	check_clicks(1, 39);
	check_clicks(2, 10);

	// DISPLAY
	display_numbers(counter);

	// remember previous button state
	for (int i = 0; i < BUTTONS; i++)
		previous_state[i] = is_button_on(i);
}
