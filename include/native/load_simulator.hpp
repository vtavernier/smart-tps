#ifndef _NATIVE_LOAD_SIMULATOR_HPP_
#define _NATIVE_LOAD_SIMULATOR_HPP_

#include <stdint.h>

#include "../load_simulator.hpp"

struct LoadSimulatorState {
	float lts_value;
	float ltl_value;
	bool ltb_value;
	uint16_t period_us;
};

extern LoadSimulatorImpl *new_native_load_simulator_impl(LoadSimulatorState &state);

#endif /* _NATIVE_LOAD_SIMULATOR_HPP_ */
