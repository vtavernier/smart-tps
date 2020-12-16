#include "platform.hpp"

#include <stdlib.h>
#include <unistd.h>

#include <chrono>
using namespace std::chrono;

extern high_resolution_clock::time_point platform_start;

PlatformClass::PlatformClass() {}

void PlatformClass::delay(unsigned long ms) { usleep(ms * 1000); }

unsigned long PlatformClass::millis() {
	auto now = high_resolution_clock::now();
	return duration_cast<milliseconds>(now - platform_start).count();
}

unsigned long PlatformClass::micros() {
	auto now = high_resolution_clock::now();
	return duration_cast<microseconds>(now - platform_start).count();
}

void PlatformClass::halt() { exit(0); }

void PlatformClass::print_sensor_measurement(int16_t val) {
	// TODO: Implement native output for sensor measurements
}

PlatformClass Platform;
