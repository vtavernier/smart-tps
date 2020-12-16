#include "tps.hpp"

#include "platform.hpp"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

Tps::Tps(Gui &gui, LoadSimulator &load_simulator, RgbLed &rgb_led, LoadSensor &load_sensor, Stats &stats)
    : gui_{gui}, load_simulator_{load_simulator}, rgb_led_{rgb_led}, load_sensor_{load_sensor}, stats_{stats} {}

bool Tps::try_switch_to_measure(LoadType type) {
	auto raw_val = load_sensor_.read(type);

	if (raw_val != INT16_MAX && raw_val > CurrentThreshold) {
		// Reset timer
		inactivity_start_ = Platform.millis();

		// Record sample
		stats_.reset();
		stats_.add_sample(raw_val);

		// Change state
		switch (type) {
		case LoadType::Liner:
			state_ = TpsState::MeasuringLiner;
			break;
		case LoadType::Shader:
			state_ = TpsState::MeasuringShader;
			break;
		}

		return true;
	}

	return false;
}

void Tps::display_stats() {
	// Update GUI
	gui_.show_stats({.frequency = stat_freq_,
			 .duty_cycle = stat_duty_cycle_,
			 .type = stat_type_,
			 .expected_enabled = simulate_load_,
			 .expected_load_type = last_load_simulator_type_,
			 .expected_duty_cycle = 100.0f * last_load_simulator_expected_,
			 .screen_lasts = screen_lastms_ / 1000,
			 .current_state = state_});

	gui_.display();
}

void Tps::poll_inactive() {
	auto now = Platform.millis();

	// Check if we have some current somewhere
	if (try_switch_to_measure(LoadType::Liner)) {
		// Switch handled by try_switch_to_measure
	} else if (try_switch_to_measure(LoadType::Shader)) {
		// Switch handled by try_switch_to_measure
	} else {
		// No current detected

		// Check for inactivity timeout
		if (now - inactivity_start_ > Inactive_Inactivity_Timeout) {
			// Time to turn off

			// Reset timer in case we're running from external power and halt has no effect
			inactivity_start_ = now;

			// Turn off
			Platform.halt();
		} else {
			// Not yet time to turn off, so wait a bit before sensing current again
			Platform.delay(Inactive_Sense_Period);
		}
	}
}

void Tps::poll_sense(LoadType type) {
	auto now = Platform.millis();

	// Check for expired timers
	if (now - last_sense_main_ > Sensing_Current_Period) {
		last_sense_main_ = now;

		// Sense main
		if (try_switch_to_measure(type))
			return; // Nothing more to do if we switched to measuring
	}

	if (now - last_sense_alternate_ > Sensing_Alternate_Period) {
		last_sense_alternate_ = now;

		// Sense alternate
		if (try_switch_to_measure(alternate(type)))
			return; // Nothing more to do if we switched to measuring
	}

	if (now - inactivity_start_ > Inactive_Inactivity_Timeout) {
		// We are now inactive

		// Reset timer
		inactivity_start_ = now;

		// Show splash screen
		gui_.show_splash({});
		gui_.display();

		// Switch to inactive state
		state_ = TpsState::Inactive;
	} else {
		// Check if we need to update the display
		if (now - screen_lastms_ > Sensing_Screen_Update_Period) {
			screen_lastms_ = now;
			display_stats();
		}

		// Get the time again
		now = Platform.millis();

		// Not yet inactive, wait for next event
		auto next_sense_main = last_sense_main_ + Sensing_Current_Period;
		auto next_sense_alternate = last_sense_alternate_ + Sensing_Alternate_Period;

		if (next_sense_main < next_sense_alternate) {
			if (next_sense_main > now) // Do not wait for a negative delay
				Platform.delay(next_sense_main - now);
		} else {
			if (next_sense_alternate > now) // Do not wait for a negative delay
				Platform.delay(next_sense_alternate - now);
		}
	}
}

void Tps::poll_measure(LoadType type) {
	auto now = Platform.millis();

	// Update current average
	auto res = stats_.add_sample(load_sensor_.read(type));

	// An active sample resets the inactivity timer
	switch (res) {
	case SampleStatus::Valid:
	case SampleStatus::Invalid:
		inactivity_start_ = now;
		break;
	case SampleStatus::Zero:
		break;
	}

	// Update screen if needed
	if (now - screen_lastms_ > Measuring_Screen_Update_Period) {
		screen_lastms_ = now;

		// Update statistics
		if (stats_.update_stats()) {
			stat_freq_ = (stat_freq_ + stats_.get_freq()) / 2.f;
			stat_duty_cycle_ = (stat_duty_cycle_ + stats_.get_duty_cycle()) / 2.f;
			stat_type_ = type;
		}

		display_stats();
	}

	// Check the inactivity timer
	if (now - inactivity_start_ > Measuring_Inactivity_Timeout) {
		// Measuring timed out

		// Reset timer
		inactivity_start_ = now;

		// Reset sensing targets
		last_sense_main_ = now;
		last_sense_alternate_ = now;

		// Change state
		switch (type) {
		case LoadType::Liner:
			state_ = TpsState::SensingLiner;
			break;
		case LoadType::Shader:
			state_ = TpsState::SensingShader;
			break;
		}
	}
}

void Tps::begin() {
	auto now = Platform.millis();

	// Initialize state
	state_ = TpsState::Inactive;
	inactivity_start_ = now;
	simulate_load_ = true; // TODO: This should be configurable
	last_load_simulator_type_ = LoadType::Liner;
	last_load_simulator_expected_ = -1.0f; // Negative value should not be expected
	screen_lastms_ = now;
	stat_freq_ = 0.;
	stat_duty_cycle_ = 0.;
	stat_type_ = LoadType::Liner;

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
	auto now = Platform.millis();

	// Update load simulator, this always runs regardless of the state machine state
	LoadType expected_type = ((now / LoadSimulator_Type_Period) % 2) == 0 ? LoadType::Liner : LoadType::Shader;
	float expected = simulate_load_ ? ((now / LoadSimulator_Value_Period) % LoadSimulator_Value_Steps) /
					      static_cast<float>(LoadSimulator_Value_Steps)
					: 0.f;

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

	switch (state_) {
	case TpsState::Inactive:
		poll_inactive();
		break;
	case TpsState::SensingLiner:
		poll_sense(LoadType::Liner);
		break;
	case TpsState::SensingShader:
		poll_sense(LoadType::Shader);
		break;
	case TpsState::MeasuringLiner:
		poll_measure(LoadType::Liner);
		break;
	case TpsState::MeasuringShader:
		poll_measure(LoadType::Shader);
		break;
	}
}
