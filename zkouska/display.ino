/* A module for working with displays. */

#include "funshield.h"


constexpr int DISPLAY_COUNT = 4;
int current_display = 0;  // which display to update on a given iteration
byte display_state[DISPLAY_COUNT];  // states of each of the display digits


/* Initialize the display pins. */
void initialize_display() {
	pinMode(latch_pin, OUTPUT);
	pinMode(data_pin, OUTPUT);
	pinMode(clock_pin, OUTPUT);

	reset_display();
}

/* Update the state of the given display to the given value (permanently).
    H
   C G
    B     0bABCDEFGH
   D F
    E  A
*/
void set_display(int display, byte state) {
	display_state[display] = state;
}

/* Get the state of the given display. */
byte get_display(int display) {
	return display_state[display];
}

/* Clear the display. */
void reset_display() {
	for (int i = 0; i < DISPLAY_COUNT; i++)
		display_state[i] = 0b00000000;
}

/* Refresh the display by moving to the next display digit and setting its state. */
void refresh_display() {
	current_display = (current_display + 1) % 4;
	write_to_display(current_display, display_state[current_display]);
}

/* A function to do the actual writing of the state to the display. */
void write_to_display(int display, byte state) {
	digitalWrite(latch_pin, LOW);
	shiftOut(data_pin, clock_pin, MSBFIRST, ~state);  // inverse logic!
	shiftOut(data_pin, clock_pin, MSBFIRST, 1 << display);
	digitalWrite(latch_pin, HIGH);
}

