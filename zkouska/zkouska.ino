#include "funshield.h"


constexpr int SLOT_COUNT = 4;
long slots[SLOT_COUNT] = {0, 0, 0, 0};
long last_slot = 0;
long displayed_slot = 0;

bool running = false;

long offset_from_millis = 0;
long last_time = 0;

// for displaying the lap time on the screen after being pressed
constexpr long LAP_TIME_SHOW_DURATION = 1000;
long lap_time_show_offset = -LAP_TIME_SHOW_DURATION;


void setup() {
	initialize_display();
	initialize_buttons();
	initialize_leds();
}

/* Display the current lap time. */
void display_time(long time) {
	time /= 100;

	for (int i = 0; i < 4; i++) {
		byte state = ~digits[time % 10];

		if (i == 1)
			state |= 0b10000000;

		set_display(3 - i, state);

		time /= 10;
	}
}

/* Update the LEDs to reflect whatever the state of the clock is. */
void update_leds() {
	reset_leds();

	// if the clock is running (or we're showing the current time), display how full the slots are
	if (running || (displayed_slot == SLOT_COUNT)) {
		for (int i = 0; i < SLOT_COUNT; i++) {
			if (slots[i] != 0)
				set_led(i, 1);
		}
	} else {
		if (slots[0] != 0) {
			set_led(displayed_slot, 1);
		}
	}
}

/* Start the clock. */
void start_clock() {
	running = true;
	reset_current_lap();
}

/* Stop the clock. */
void stop_clock() {
	running = false;
	record_time();
}

/* Record the current time as the last lap time. */
void record_time() {
	last_time = millis() - offset_from_millis;
}

void record_lap() {
	// save the time to the current slot
	long current_time = millis();
	slots[last_slot] = current_time - offset_from_millis;
	last_slot = (last_slot + 1) % SLOT_COUNT;

	record_time();
	reset_current_lap();

	lap_time_show_offset = current_time;

	// set the displayed_slot to the special value to show the recorded value
	displayed_slot = SLOT_COUNT;
}

/* Start the lap by resetting the time. */
void reset_current_lap() {
	offset_from_millis = millis();
}

void reset_slots() {
	// reset slots
	for (int i = 0; i < SLOT_COUNT; i++)
		slots[i] = 0;
	last_slot = 0;
}

/* Display the next time (called when paused). */
void display_next_time() {
	// find the next slot that contains some data
	for (int i = 1; i < SLOT_COUNT; i++) {
		int index = displayed_slot + i;

		// if we're at the special value, display the last time
		if (index == SLOT_COUNT) {
			display_time(last_time);
			displayed_slot = SLOT_COUNT;
			return;
		}

		index = index % (SLOT_COUNT + 1);

		// else display some non/zero time
		if (slots[index] != 0) {
			display_time(slots[index]);
			displayed_slot = index;
			return;
		}
	}
}

void loop() {
	update_button_buffer_state();

	if (was_pressed(0)) {
		if (!running) start_clock();
		else stop_clock();
	}

	if (was_pressed(1)) {
		if (running) record_lap();
		else display_next_time();
	}

	// special case for button 2 (if it's a long press, reset the entire clock)
	if (running && !is_pressed(2)) {
		long current_time = millis();
		
		if ((current_time - lap_time_show_offset) < LAP_TIME_SHOW_DURATION)
			display_time(last_time);
		else
			display_time(current_time - offset_from_millis);
	}

	if (is_held(2)) {
		reset_slots();
		reset_current_lap();
		display_time(0);
	}

	if (was_released(2)) {
		reset_current_lap();
		display_time(0);
	}

	update_leds();
	refresh_display();
	update_previous_button_states();
}
