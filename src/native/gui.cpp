#include "gui.hpp"

#include <ncurses.h>

#define PREFIX "StdoutGui: "

class StdoutGuiImpl : public GuiImpl {
      public:
	void begin() override {
		printw(PREFIX "begin\n");
		show_splash({});
	}

	void clear() override {
		::clear();
		move(0, 0);
	}

	void display() override { refresh(); }

	void show_splash(const SplashData &data) override { printw(PREFIX "version: " TPS_VERSION "\n"); }

	void show_stats(const StatsData &data) override {
		printw(PREFIX
		       "frequency: %3.2fHz duty_cycle: %3.2f%% expected_duty_cycle: %3.2f%% screen_lastms: %lums\n",
		       data.frequency, data.duty_cycle, data.expected_duty_cycle, data.screen_lastms);
	}
};

GuiImpl *new_gui_impl() { return new StdoutGuiImpl(); }
