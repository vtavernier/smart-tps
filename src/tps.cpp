#include "tps.hpp"

#include "platform.hpp"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

Tps::Tps(Gui &gui, LoadSimulator &load_simulator, RgbLed &rgb_led, LoadSensor &load_sensor, Stats &stats)
    : gui_{gui}, load_simulator_{load_simulator}, rgb_led_{rgb_led}, load_sensor_{load_sensor}, stats_{stats},
      screen_lastms_{0}, stat_freq_{0}, stat_duty_cycle_{0} {}

void Tps::begin() {
	// Initialize load simulator
	load_simulator_.begin();

	// Initialize LED
	rgb_led_.begin();

	// Initialize load sensor
	load_sensor_.begin();

	// Initialize GUI
	gui_.begin();

	// Reset stats instance
	stats_.reset();
}

void Tps::poll() {
	float expected = ((Platform.millis() / 5000) % 4) / 4.f;

	// Update load
	load_simulator_.set_load(LoadType::Liner, expected);
	rgb_led_.set_led(0.0f, 0.0f, expected);

	// Update current average
	auto raw_val = load_sensor_.read(LoadType::Liner);
	stats_.add_sample(raw_val);

	// Add green to the LED
	rgb_led_.set_led(0.0f, 1.0f, expected);

	unsigned long now = Platform.millis();

	if (now - screen_lastms_ > 250) {
		screen_lastms_ = now;

		// Update statistics
		if (stats_.update_stats()) {
			stat_freq_ = stats_.get_freq();
			stat_duty_cycle_ = stats_.get_duty_cycle();
		}

		// Update GUI
		gui_.show_stats({.frequency = stat_freq_,
				 .duty_cycle = stat_duty_cycle_,
				 .expected_duty_cycle = 100.0f * expected,
				 .screen_lasts = screen_lastms_ / 1000});

		gui_.display();
	}
}
