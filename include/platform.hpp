#ifndef _PLATFORM_HPP_
#define _PLATFORM_HPP_

#include <stdint.h>

class PlatformClass {
      public:
	PlatformClass();

	void delay(unsigned long ms);

	unsigned long millis();
	unsigned long micros();

	void halt();

	void print_sensor_measurement(int16_t raw_val);
};

extern PlatformClass Platform;

#endif /* _PLATFORM_HPP_ */
