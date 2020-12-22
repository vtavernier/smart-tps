#ifndef _TPS_STATE_HPP_
#define _TPS_STATE_HPP_

/// Inactivity timeout from Inactive to system halt
const unsigned long Inactive_Inactivity_Timeout = 15000;
/// How often (period) should loads be checked, transitions to Measuring
const unsigned long Inactive_Sense_Period = 125;
/// Inactivity timeout from Sensing to Inactive state
const unsigned long Sensing_Inactivity_Timeout = 30000;
/// How often (period) should the current load be checked, transitions to MeasuringCurrent
const unsigned long Sensing_Current_Period = 75;
/// How often (period) should the alternate load be checked, transitions to MeasuringAlternate
const unsigned long Sensing_Alternate_Period = 125;
/// How often (period) should the screen be updated
const unsigned long Sensing_Screen_Update_Period = 1000;
/// Inactivity timeout from Measuring to Sensing state
const unsigned long Measuring_Inactivity_Timeout = 2500;
/// How often (period) should the screen be updated
const unsigned long Measuring_Screen_Update_Period = 125;

/// Load simulator expected value period
const unsigned long LoadSimulator_Value_Period = 2500;
/// Number of steps
const int LoadSimulator_Value_Steps = 8;
/// Load simulator type period
const unsigned long LoadSimulator_Type_Period = LoadSimulator_Value_Steps * LoadSimulator_Value_Period;

/**
 * @brief Main application state machine state
 */
enum class TpsState {
	/// Initial state, tps waiting for input on either load sensor (very low freq.)
	Inactive,
	/// Sensing liner (low freq.) load, check shader (occasionally)
	SensingLiner,
	/// Sensing shader (low freq.) load, check liner (occasionally)
	SensingShader,
	/// Actively measuring (max freq.) liner load, don't check shader
	MeasuringLiner,
	/// Actively measuring (max freq.) shader load, don't check liner
	MeasuringShader,
};

/**
 * @brief Return the string representation of state
 */
const char *stateToString(TpsState state);

/**
 * @brief Return the short string representation of state
 */
const char *stateToShortString(TpsState state);

#endif /* _TPS_STATE_HPP_ */
