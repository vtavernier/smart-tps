#include "native/load_sensor.hpp"
#include "platform.hpp"

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

LoadSensorImpl *new_native_load_sensor_impl(const LoadSimulatorState &state) { return new NativeLoadSensorImpl(state); }
