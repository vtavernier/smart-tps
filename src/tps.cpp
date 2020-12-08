#include "tps.hpp"

#include "platform.hpp"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

Tps::Tps(Gui &gui, LoadSimulator &load_simulator, RgbLed &rgb_led, LoadSensor &load_sensor, Stats &stats)
    : gui_{gui}, load_simulator_{load_simulator}, rgb_led_{rgb_led}, load_sensor_{load_sensor}, stats_{stats} {}

void Tps::begin() {
	// Initialize state
	state_ = TpsState::Inactive;
	// TODO: This should be configurable
	simulate_load_ = true;
	last_load_simulator_type_ = LoadType::Liner;
	// Negative value should not be expected
	last_load_simulator_expected_ = -1.0f;
	screen_lastms_ = Platform.millis();
	stat_freq_ = 0.;
	stat_duty_cycle_ = 0.;

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
	unsigned long now = Platform.millis();

	// Update load simulator, this always runs regardless of the state machine state
	LoadType expected_type = ((now / 20000) % 2) == 0 ? LoadType::Liner : LoadType::Shader;
	float expected = simulate_load_ ? ((now / 5000) % 4) / 4.f : 0.f;

	if (last_load_simulator_type_ != expected_type || expected != last_load_simulator_expected_) {
		// Update load
		load_simulator_.set_load(expected_type, expected);

		if (expected_type == LoadType::Liner) {
			// Update amount of red to show simulated load value
			rgb_led_.set_led(expected, 0.f, 0.f);
		} else {
			// Update amount of blue to show simulated load value
			rgb_led_.set_led(0.0, 0.f, expected);
		}

		last_load_simulator_type_ = expected_type;
		last_load_simulator_expected_ = expected;
	}

	// Update current average
	auto raw_val = load_sensor_.read(LoadType::Liner);
	stats_.add_sample(raw_val);

	// Add green to the LED
	rgb_led_.set_led(0.0f, 1.0f, expected);

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
				 .expected_enabled = simulate_load_,
				 .expected_load_type = expected_type,
				 .expected_duty_cycle = 100.0f * expected,
				 .screen_lasts = screen_lastms_ / 1000,
				 .current_state = state_});

		gui_.display();
	}
}
