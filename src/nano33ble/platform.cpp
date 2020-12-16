#include "platform.hpp"

#include <Arduino.h>

#include "nano33ble/project_pins.h"

PlatformClass::PlatformClass() {
	// Setup MAX16054AZT pin
	pinMode(TPS_PIN_AOFF, OUTPUT);
	pinMode(TPS_PIN_AOFF, LOW);

	// Disable PWR LED to save some power
	digitalWrite(LED_PWR, LOW);
}

void PlatformClass::delay(unsigned long ms) { ::delay(ms); }

unsigned long PlatformClass::millis() { return ::millis(); }
unsigned long PlatformClass::micros() { return ::micros(); }

void PlatformClass::halt() {
	// Trigger the MAX16054AZT RESET pin
	// This will stop execution if running from battery
	digitalWrite(TPS_PIN_AOFF, HIGH);

	// Wait for power to disappear
	delay(100);
}

void PlatformClass::print_sensor_measurement(int16_t raw_val) {
	Serial.print(micros());
	Serial.print(",");
	Serial.println(raw_val);
}

PlatformClass Platform;
