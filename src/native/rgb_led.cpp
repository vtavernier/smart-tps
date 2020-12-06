#include "rgb_led.hpp"

#include <iostream>

using namespace std;

#define PREFIX "RgbLed: "

class StdoutRgbLedImpl : public RgbLedImpl {
      public:
	void begin() override { set_led(0.f, 0.f, 0.f); }

	void set_led(float r, float g, float b) override {
		printf(PREFIX "\e[48;2;%d;%d;%dm%s\e[0m\n", static_cast<int>(255 * r), static_cast<int>(255 * g),
		       static_cast<int>(255 * b), "  ");
	}
};

RgbLedImpl *new_rgb_led_impl() { return new StdoutRgbLedImpl(); }
