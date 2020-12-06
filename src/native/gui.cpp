#include "gui.hpp"

#include <iostream>

using namespace std;

#define PREFIX "StdoutGui: "

class StdoutGuiImpl : public GuiImpl {
      public:
	void begin() override {
		cout << PREFIX "begin" << endl;
		show_splash({});
	}

	void clear() override {
		// nothing to clear on stdout
	}

	void display() override {
		// nothing to display, already handled by state updates
	}

	void show_splash(const SplashData &data) override { cout << PREFIX "version: " TPS_VERSION << endl; }

	void show_stats(const StatsData &data) override {
		cout << PREFIX << "frequency: " << data.frequency << "Hz"
		     << " "
		     << "duty_cycle: " << data.duty_cycle << "%"
		     << " "
		     << "expected_duty_cycle: " << data.expected_duty_cycle << "%"
		     << " "
		     << "screen_lastms: " << data.screen_lastms << "ms";
	}
};

GuiImpl *new_gui_impl() { return new StdoutGuiImpl(); }
