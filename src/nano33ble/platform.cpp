#include "platform.hpp"

#include <Arduino.h>

unsigned long PlatformClass::millis() { return ::millis(); }
unsigned long PlatformClass::micros() { return ::micros(); }

PlatformClass Platform;
