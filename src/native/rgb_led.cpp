#include "rgb_led.hpp"

#include <ncurses.h>

#define PREFIX "RgbLed: "

class StdoutRgbLedImpl : public RgbLedImpl {
      public:
	void begin() override {
		init_color(20, 255, 0, 255);
		init_pair(20, COLOR_WHITE, 20);
		set_led(0.f, 0.f, 0.f);
	}

	void set_led(float r, float g, float b) override {
		init_color(20, static_cast<int>(1000 * r), static_cast<int>(1000 * g), static_cast<int>(1000 * b));
		move(2, 0);
		printw(PREFIX);
		attron(COLOR_PAIR(20));
		printw("  ");
		attroff(COLOR_PAIR(20));
		refresh();
	}
};

RgbLedImpl *new_rgb_led_impl() { return new StdoutRgbLedImpl(); }
