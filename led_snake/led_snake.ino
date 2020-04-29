#include "funshield.h"


// snake animation

constexpr int STATE_COUNT = 12;

// display number | state | delay after the state is shown
constexpr int STATES[STATE_COUNT][3] = {
	{0, 0b00000001, 100},
	{1, 0b00000001, 50},
	{2, 0b00000001, 50},
	{3, 0b00000001, 100},
	{3, 0b00000010, 200},
	{3, 0b00000100, 200},
	{3, 0b00001000, 100},
	{2, 0b00001000, 50},
	{1, 0b00001000, 50},
	{0, 0b00001000, 100},
	{0, 0b00010000, 200},
	{0, 0b00100000, 200},
};

int current_state = 0;


// waiting
long time = 0;


/* DISPLAY SETTING STUFF */ 
// update the state of the given display
void set_display(int display, byte state) {
	digitalWrite(latch_pin, LOW);
	shiftOut(data_pin, clock_pin, MSBFIRST, 1 << display);

	digitalWrite(latch_pin, HIGH);
	shiftOut(data_pin, clock_pin, MSBFIRST, ~state);
}



/* MAIN CODE */
void setup() {
	// initialize display pins to output
	pinMode(latch_pin, OUTPUT);
	pinMode(data_pin, OUTPUT);
	pinMode(clock_pin, OUTPUT);
}

void loop() {
	if (time < millis()) {
		current_state = (current_state + 1) % STATE_COUNT;
		time += STATES[current_state][2];
	}

	set_display(STATES[current_state][0], STATES[current_state][1]);
}
