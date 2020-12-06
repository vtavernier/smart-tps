#include "native/load_sensor.hpp"

class NativeLoadSensorImpl : public LoadSensorImpl {
	const LoadSimulatorState &state_;

      public:
	NativeLoadSensorImpl(const LoadSimulatorState &state) : state_(state) {}

	void begin() override {}

	int16_t read(LoadType type) override {
		switch (type) {
		case LoadType::Liner:
			return state_.ltl_value * 15000;
		case LoadType::Shader:
			return state_.lts_value * 15000;
		}

		return INT16_MAX;
	}
};

LoadSensorImpl *new_native_load_sensor_impl(const LoadSimulatorState &state) { return new NativeLoadSensorImpl(state); }
