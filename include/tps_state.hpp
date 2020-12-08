#ifndef _TPS_STATE_HPP_
#define _TPS_STATE_HPP_

/**
 * @brief Main application state machine state
 */
enum class TpsState {
	/// Initial state, tps waiting for input on either load sensor (very low freq.)
	Inactive,
	/// Sensing liner (low freq.) load
	SensingLiner,
	/// Sensing shader (low freq.) load
	SensingShader,
	/// Actively measuring (max freq.) liner load
	MeasuringLiner,
	/// Actively measuring (max freq.) shader load
	MeasuringShader,
};

/**
 * @brief Return the string representation of state
 */
const char *stateToString(TpsState state);

#endif /* _TPS_STATE_HPP_ */
