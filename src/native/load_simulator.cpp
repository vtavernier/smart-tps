#include "native/load_simulator.hpp"

class NativeLoadSimulator : public LoadSimulatorImpl {
	LoadSimulatorState &state_;

      public:
	NativeLoadSimulator(LoadSimulatorState &state) : state_(state) { state_.period_us = 6667; }

	void begin() override { set_load(false, 0.f, 0.f); }

	void set_load(bool base, float liner, float shader) override {
		state_.ltb_value = base;
		state_.ltl_value = liner;
		state_.lts_value = shader;
	}
};

LoadSimulatorImpl *new_load_simulator_impl() {
	// We need target values for this impl
	return nullptr;
}

LoadSimulatorImpl *new_native_load_simulator_impl(LoadSimulatorState &state) { return new NativeLoadSimulator(state); }
