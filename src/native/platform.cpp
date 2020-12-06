#include "platform.hpp"

#include <chrono>
using namespace std::chrono;

extern high_resolution_clock::time_point platform_start;

unsigned long PlatformClass::millis() {
	auto now = high_resolution_clock::now();
	return duration_cast<milliseconds>(now - platform_start).count();
}

unsigned long PlatformClass::micros() {
	auto now = high_resolution_clock::now();
	return duration_cast<microseconds>(now - platform_start).count();
}

PlatformClass Platform;
