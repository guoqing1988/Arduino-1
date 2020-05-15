#include "funshield.h"

// # BUTTONS
constexpr int BUTTONS = 3;
constexpr int BUTTON_PINS[BUTTONS] = {button1_pin, button2_pin, button3_pin};

constexpr int BUFFER_SIZE = 20;
bool button_buffer[BUTTONS][BUFFER_SIZE];  // for taking the button average value
int button_buffer_position = 0;  // so we can continuously write somewhere to the buffer
long prev_button_state[BUTTONS] = {false, false, false};


// # DISPLAY
constexpr byte DISPLAY_COUNT = 4;
byte display = 0;  // which display to update on a given iteration
byte display_state[DISPLAY_COUNT];  // states of each of the display digits

byte ascii_to_bytes[128];  // character to display encoding


// # DND
enum Mode { CONFIGURATION, NORMAL };
Mode mode = NORMAL;

constexpr byte DICE_COUNT = 7;
long dice_sides[DICE_COUNT] = {4, 6, 8, 10, 12, 20, 100};

constexpr byte MAX_THROWS = 9;
byte throws = 0; // (0 to 8, displayed with +1)
byte dice = 0;

bool being_generated = false;
long generation_start_time = 0;


// ## GENERATING ANIMATION
constexpr int STATE_COUNT = 12;

// display | state | delay between the next phase
constexpr int STATES[STATE_COUNT][3] = {
	{0, 0b11111110, 100},
	{1, 0b11111110, 50},
	{2, 0b11111110, 50},
	{3, 0b11111110, 100},
	{3, 0b11111101, 150},
	{3, 0b11111011, 150},
	{3, 0b11110111, 100},
	{2, 0b11110111, 50},
	{1, 0b11110111, 50},
	{0, 0b11110111, 100},
	{0, 0b11101111, 150},
	{0, 0b11011111, 150},
};

int current_state = 0;
long time = 0;



// check a button status, returning true if it's on, else false
bool is_pressed(int button) {
	int average = 0;

	for (int i = 0; i < BUFFER_SIZE; i++)
		average += button_buffer[button][i];

	return average > BUFFER_SIZE / 2;
}

// return true if the button was just pressed/released
bool was_pressed(int button) { return !prev_button_state[button] && is_pressed(button); }
bool was_released(int button) { return prev_button_state[button] && !is_pressed(button); }


// update the state of the given display to the given value
// self-reminder (1 is off, 0 is on):
//  H
// C G
//  B     0bABCDEFGH
// D F
//  E  A
void set_display(byte state, byte display) {
	digitalWrite(latch_pin, LOW);

	// 0 3 2 1 -> 0 1 2 3, since the display order is wrong
	int correct_display = (3 - ((3 - display + 1) % 4));

	shiftOut(data_pin, clock_pin, MSBFIRST, 1 << correct_display);
	digitalWrite(latch_pin, HIGH);
	shiftOut(data_pin, clock_pin, MSBFIRST, state);
}

// set the display status to show the current throw and dice configuration
void set_display_to_config() {
	display_state[0] = ascii_to_bytes[(throws + 1) + '0'];
	display_state[1] = ascii_to_bytes['d'];
	display_state[2] = ascii_to_bytes[(dice_sides[dice] % 100) / 10 + '0'];
	display_state[3] = ascii_to_bytes[dice_sides[dice] % 10 + '0'];
}

// return a random number, given a long (by multiplying and adding primes)
long random_number(long seed, long maximum) { return ((seed * 257) + 127) % maximum; }

// return the current time (in an arbitrary unit)
long current_time() {return micros() / 4;}

// clear the display
void reset_display() {
	for (int i = 0; i < DISPLAY_COUNT; i++)
		display_state[i] = ascii_to_bytes[' '];
}

// start generating a random number
void start_generation() {
	generation_start_time = current_time();
	being_generated = true;
	reset_display();
}

//  H           E
// C G         F D
//  B     ->    B
// D F         G C
//  E  A        H A
byte mirror_state(byte state) {
	return (state & 0b11000000
		| (state & 0b00100000) >> 3
		| (state & 0b00010000) >> 3
		| (state & 0b00001000) >> 3
		| (state & 0b00000100) << 3
		| (state & 0b00000010) << 3
		| (state & 0b00000001) << 3
	);
}

// write the current button state to the buffer
void update_button_buffer_state() {
	button_buffer_position = (button_buffer_position + 1) % BUFFER_SIZE;

	for (int i = 0; i < BUTTONS; i++) {
		bool current_state = !digitalRead(BUTTON_PINS[i]);
		button_buffer[i][button_buffer_position] = current_state;
	}
}

// a function to update the current state of the app
void update_dnd_logic() {
	if (was_pressed(0)) {
		if (mode != NORMAL) mode = NORMAL;

		start_generation();
	}
	else if (was_pressed(1) && !being_generated) {
		if (mode != CONFIGURATION) mode = CONFIGURATION;
		else throws = (throws + 1) % MAX_THROWS;

		set_display_to_config();
	}
	else if (was_pressed(2) && !being_generated) {
		if (mode != CONFIGURATION) mode = CONFIGURATION;
		else dice = (dice + 1) % DICE_COUNT;

		set_display_to_config();
	}
	else if (was_released(0)) {
		long sum = 0;

		// generate the random number by repeatedly throwing the dice
		for (int i = 0; i < throws + 1; i++)
			sum += random_number(current_time() - generation_start_time, dice_sides[dice]) + 1;

		// display the random number on the screen
		reset_display();
		int i = 3;
		while (sum > 0) {
			display_state[i--] = ascii_to_bytes[sum % 10 + 48];
			sum /= 10;
		}

		being_generated = false;
	}
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

	// set to blank by default
	for (int i = 0; i < 128; i++)
		ascii_to_bytes[i] = 0b11111111;

	// set digits from funshield header
	for (int i = 0; i < 10; i++)
		ascii_to_bytes[i  + 48] = digits[i];

	ascii_to_bytes[100] = 0b10100001;  // d

	reset_display();
}

void loop() {
	// move toggle between animation states
	if (time < millis()) {
		time += STATES[current_state][2];

		if (being_generated) {
			// TODO refactor
			display_state[STATES[current_state][0]] = ascii_to_bytes[' '];
			display_state[3 - STATES[current_state][0]] = ascii_to_bytes[' '];
			current_state = (current_state + 1) % STATE_COUNT;
			display_state[STATES[current_state][0]] = STATES[current_state][1];
			display_state[3 - STATES[current_state][0]] = mirror_state(STATES[current_state][1]);
		}
	}

	update_button_buffer_state();

	// update the display
	display = (display + 1) % 4;
	set_display(display_state[display], display);

	update_dnd_logic();

	// remember previous button states
	for (int i = 0; i < BUTTONS; i++)
		prev_button_state[i] = is_pressed(i);
}
