#ifndef _PLATFORM_HPP_
#define _PLATFORM_HPP_

class PlatformClass {
      public:
	unsigned long millis();
	unsigned long micros();
};

extern PlatformClass Platform;

#endif /* _PLATFORM_HPP_ */
