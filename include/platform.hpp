#ifndef _PLATFORM_HPP_
#define _PLATFORM_HPP_

class PlatformClass {
      public:
	PlatformClass();

	void delay(unsigned long ms);

	unsigned long millis();
	unsigned long micros();

	void halt();
};

extern PlatformClass Platform;

#endif /* _PLATFORM_HPP_ */
