#ifndef _TPS_H_
#define _TPS_H_

#include "gui.hpp"
#include "load_sensor.hpp"
#include "load_simulator.hpp"
#include "rgb_led.hpp"
#include "stats.hpp"

#include "tps_state.hpp"

class Tps {
	/// GUI instance
	Gui &gui_;
	/// Load simulator
	LoadSimulator &load_simulator_;
	/// RGB LED instance
	RgbLed &rgb_led_;
	/// Load sensor
	LoadSensor &load_sensor_;
	/// Stats accumulator
	Stats &stats_;

	// Internal state
	TpsState state_;

	bool simulate_load_;
	LoadType last_load_simulator_type_;
	float last_load_simulator_expected_;

	unsigned long screen_lastms_;
	float stat_freq_;
	float stat_duty_cycle_;

      public:
	/**
	 * Construct a new Tps instance.
	 *
	 * @param gui GUI instance to use
	 * @param load_simulator Load simulator instance to use
	 * @param rgb_led RGB LED instance to use
	 * @param load_sensor Load sensor instance to use
	 * @param stats Statistics accumulator to use
	 */
	Tps(Gui &gui, LoadSimulator &load_simulator, RgbLed &rgb_led, LoadSensor &load_sensor, Stats &stats);

	/// Initialize the peripherals, to be called in setup
	void begin();

	/// Poll devices for changes, to be called in loop
	void poll();
};

#endif /* _TPS_H_ */
