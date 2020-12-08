#include "tps_state.hpp"

const char *stateToString(TpsState state) {
	switch (state) {
	case TpsState::Inactive:
		return "Inactive";
	case TpsState::SensingLiner:
		return "Sense: liner";
	case TpsState::SensingShader:
		return "Sense: shader";
	case TpsState::MeasuringLiner:
		return "Measure: liner";
	case TpsState::MeasuringShader:
		return "Measure: shader";
	}

	return "Invalid";
}
