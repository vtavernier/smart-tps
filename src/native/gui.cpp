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
		printw(PREFIX "%s: frequency: %3.1fHz duty_cycle: %2.0f%% state: %s screen_lasts: %lus\n",
		       data.type == LoadType::Liner ? "L" : "S", data.frequency, data.duty_cycle,
		       stateToString(data.current_state), data.screen_lasts);
		if (data.expected_enabled) {
			printw(PREFIX "expected_duty_cycle: %s: %2.0f%%\n",
			       data.expected_load_type == LoadType::Liner ? "L" : "S", data.expected_duty_cycle);
		} else {
			printw("\n");
		}
	}
};

GuiImpl *new_gui_impl() { return new StdoutGuiImpl(); }
