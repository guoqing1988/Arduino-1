#include "funshield.h"

void setup() {
	initialize_display();
	initialize_buttons();
	initialize_leds();
}

void loop() {
	update_button_buffer_state();

	// logic

	refresh_display();
	update_previous_button_states();
}
