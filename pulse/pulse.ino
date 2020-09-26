#include "funshield.h"

constexpr int LED_PIN = led1_pin;

constexpr float weight = 0.1;
int previous_value = 0;

int threshold = 510;

void setup() {
	pinMode(LED_PIN, OUTPUT);
	pinMode(A5, INPUT);
}

void loop() {
	previous_value = (int)analogRead(A5) * weight + previous_value * (1 - weight);

	if (previous_value > threshold) digitalWrite(LED_PIN, LOW);
	else digitalWrite(LED_PIN, HIGH);
}
