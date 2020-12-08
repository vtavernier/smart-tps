#include "platform.hpp"

#include <Arduino.h>

PlatformClass::PlatformClass() {
	// Disable PWR LED to save some power
	digitalWrite(LED_PWR, LOW);
}

unsigned long PlatformClass::millis() { return ::millis(); }
unsigned long PlatformClass::micros() { return ::micros(); }

PlatformClass Platform;
