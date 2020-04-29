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

// text variables
char *text = "AHOJ TEST SLOVO COOL 12345"; // mind the space padding after
int text_size = strlen(text);
int text_position = 0; // where the window currently is

int ascii[128];

constexpr long DELAY = 500;  // the delay duration between text shift
constexpr long TRANSITION_DELAY = 100;  // the delay in between the transition

long time = DELAY;           // next time when the text should shift

/* BUTTON FUNCTIONS */
// checking a button status, depending on the buffer
// at least half of previous ones must be true
bool is_button_on(int button) {
	int average = 0;

	for (int i = 0; i < BUFFER_SIZE; i++)
		average += button_buffer[button][i];

	return (bool)(average > BUFFER_SIZE / 2);
}

/* DISPLAY SETTING STUFF */
// update the state of the given display to the given value
void set_display(byte state) {
	digitalWrite(latch_pin, LOW);

	// 0 3 2 1 -> 0 1 2 3, since the display order is weing
	int correct_display = (3 - ((3 - display + 1) % 4));
	shiftOut(data_pin, clock_pin, MSBFIRST, 1 << correct_display);

	digitalWrite(latch_pin, HIGH);
	shiftOut(data_pin, clock_pin, MSBFIRST, state);
}

// display the given character on the current display
void displayChar(char c) {
	set_display(ascii[c]);
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
		ascii[i] = 0b11111111;

	// set digits from funshield header
	for (int i = 0; i < 10; i++)
		ascii[i  + 48] = digits[i];

	// this was quite the pain; were we supposed to use something built'in?
	// inspired by https://www.keshikan.net/fonts-e.html
	//
	// self-reminder:
	//  H
	// C G       0 1 2 3 4 5 6 7
	//  B        A B C D E F G H
	// D F
	//  E  A
	ascii[65] = 0b10001000;  // A
	ascii[66] = 0b10000000;  // B
	ascii[67] = 0b11000110;  // C
	ascii[68] = 0b11000000;  // D
	ascii[69] = 0b10000110;  // E
	ascii[70] = 0b10001110;  // F
	ascii[71] = 0b10000010;  // G
	ascii[72] = 0b10001001;  // H
	ascii[73] = 0b11111001;  // I
	ascii[74] = 0b11100001;  // J
	ascii[75] = 0b10001010;  // K
	ascii[76] = 0b11000111;  // L
	ascii[77] = 0b11001000;  // M
	ascii[78] = 0b11001000;  // N
	ascii[79] = 0b11000000;  // O = D
	ascii[80] = 0b10001100;  // P
	ascii[81] = 0b10011000;  // Q
	ascii[82] = 0b10001000;  // R = A
	ascii[83] = 0b10010010;  // S
	ascii[84] = 0b11111000;  // T
	ascii[85] = 0b11000001;  // U
	ascii[86] = 0b11000001;  // V = U
	ascii[87] = 0b11000001;  // W = U
	ascii[88] = 0b10001001;  // X = H
	ascii[89] = 0b10010001;  // Y
	ascii[90] = 0b10100100;  // Z
}

void loop() {
	buffer_counter = (buffer_counter + 1) % BUFFER_SIZE;

	// read the current buttons value to the buffer
	for (int i = 0; i < BUTTONS; i++) {
		bool current_state = !digitalRead(BUTTON_PINS[i]);
		button_buffer[i][buffer_counter] = current_state;
	}

	// iterate the display to display the digits on
	display = (display + 1) % 4;

	// remember previous button states
	for (int i = 0; i < BUTTONS; i++)
		previous_state[i] = is_button_on(i);

	// display the part of the text that should be displayed
	// use pointer syntax because we can
	displayChar(text[(display + text_position) % text_size]);

	// do the shift of the character
	if (millis() > time) {
		time += DELAY;
		text_position += 1;
	}
}
