#include "funshield.h"

/* DEFINITIONS */
// buttons
constexpr int BUTTONS = 3;
constexpr int BUTTON_PINS[BUTTONS] = {button1_pin, button2_pin, button3_pin};

// buffer/related things
constexpr int BUFFER_SIZE = 20;
bool button_buffer[BUTTONS][BUFFER_SIZE];  // for taking the button average
int buffer_counter = 0;  // so we can continuously write somewhere to the buffer
long previous_state[BUTTONS] = {false, false, false};  // for tracking the state in the previous loop

// display counter -- cycles from 0 to 3
byte display = 0;

// stopwatch variables
byte app_state = 0;  // the state that the app is in
long start = 0;  // offset from start
long lap = 0;  // lap value


/* BUTTON FUNCTIONS */
// checking a button status, depending on the buffer
bool is_button_on(int button) {
	int average = 0;

	for (int i = 0; i < BUFFER_SIZE; i++)
		average += button_buffer[button][i];

	return (bool)(average > BUFFER_SIZE / 2);
}

// check for whether the button was just pressed
bool check_clicks(int button) {
	if (is_button_on(button) && !previous_state[button]) {
		// the implementation of the watch state diagram
		if (button == 0) {
			if (app_state == 0) {
				start = millis() - lap;
				app_state = 1;
			} else if (app_state == 1) {
				lap = millis() - start;
				app_state = 0;
			}
		} else if (button == 1) {
			if (app_state == 1) {
				lap =  millis() - start;
				app_state = 2;
			} else if (app_state == 2) app_state = 1;
		} else if (button == 2) {
			if (app_state == 0) lap = 0;
		}
	}
}

/* DISPLAY SETTING STUFF */
// update the state of the given display to the given value
void set_display(int display, byte state) {
	digitalWrite(latch_pin, LOW);
	shiftOut(data_pin, clock_pin, MSBFIRST, 1 << display);

	digitalWrite(latch_pin, HIGH);
	shiftOut(data_pin, clock_pin, MSBFIRST, state);
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

// display the time (in millis) on the display
void display_time(long time) {
	// get the appropriate digit
	time = time / 100;
	for (int i = 0; i < display; i++)
		time /= 10;

	byte display_number = (3 - display + 1) % 4;  // shifted to match the display
	byte value_to_show = digits[(byte)time % 10];

	// display the dot
	if (display_number == 3) value_to_show ^= 0b10000000;

	// hide leading zeroes
	if (value_to_show == digits[0] && time == 0 && display_number != 0)
		value_to_show = 0b11111111;

	set_display(display_number, value_to_show);
}

void loop() {
	buffer_counter = (buffer_counter + 1) % BUFFER_SIZE;

	// read the current buttons value to the buffer
	for (int i = 0; i < BUTTONS; i++) {
		bool current_state = !digitalRead(BUTTON_PINS[i]);
		button_buffer[i][buffer_counter] = current_state;
	}

	// check for short/long clicks on the buttons
	for (int i = 0; i < BUTTONS; i++) {
		check_clicks(0);
		check_clicks(1);
		check_clicks(2);
	}

	display = (display + 1) % 4;

	if (app_state == 0 || app_state == 2) display_time(lap);
	else display_time(millis() - start);

	// remember previous button states
	for (int i = 0; i < BUTTONS; i++)
		previous_state[i] = is_button_on(i);
}
