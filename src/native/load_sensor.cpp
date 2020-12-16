#include "native/load_sensor.hpp"
#include "platform.hpp"

#include "stats.hpp"

#include <fstream>
#include <sstream>
#include <vector>

class NativeLoadSensorImpl : public LoadSensorImpl {
	const LoadSimulatorState &state_;

      public:
	NativeLoadSensorImpl(const LoadSimulatorState &state) : state_(state) {}

	void begin() override {}

	int16_t read(LoadType type) override {
		// PWM signal phase
		auto phase = Platform.micros() % state_.period_us / (float)state_.period_us;

		if (!state_.ltb_value)
			return 0;

		switch (type) {
		case LoadType::Liner:
			return (phase < state_.ltl_value ? 1. : 0.) * 7500 + (state_.ltb_value ? 3500. : 0.);
		case LoadType::Shader:
			return (phase < state_.lts_value ? 1. : 0.) * 7500 + (state_.ltb_value ? 3500. : 0.);
		}

		return INT16_MAX;
	}
};

class ValuesLoadSensorImpl : public LoadSensorImpl {
	std::vector<Sample> samples_;
	size_t current_idx_;
	unsigned long max_t_;

      public:
	ValuesLoadSensorImpl(const std::string &values_path) : max_t_{0} {
		std::ifstream ifs{values_path};

		if (!ifs.is_open())
			throw std::runtime_error("failed to open values file");

		std::string il;
		bool first = true;
		while (std::getline(ifs, il)) {
			if (first) {
				first = false;
				continue;
			}

			std::istringstream ss{il};
			Sample s;
			ss >> s.t;
			ss.get();
			ss >> s.i;

			if (s.t >= max_t_)
				max_t_ = s.t;
			else
				throw std::runtime_error("samples must be sorted");

			samples_.push_back(s);
		}

		current_idx_ = 0;
	}

	void begin() override {}

	int16_t read(LoadType type) override {
		// Locate the latest sample
		auto current_t = Platform.micros() % max_t_;
		for (current_idx_ = current_t < samples_[current_idx_].t ? 0 : current_idx_;
		     current_idx_ < samples_.size() && samples_[current_idx_].t < current_t; ++current_idx_)
			;

		// Return its value
		switch (type) {
		case LoadType::Liner:
			return samples_[std::max(current_idx_, static_cast<size_t>(1)) - 1].i;
		default:
			return 0;
		}
	}
};

LoadSensorImpl *new_native_load_sensor_impl(const LoadSimulatorState &state) { return new NativeLoadSensorImpl(state); }

LoadSensorImpl *new_native_load_sensor_impl(const std::string &values_path) {
	return new ValuesLoadSensorImpl(values_path);
}
