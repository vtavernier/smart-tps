#include "load_simulator.hpp"

#include <Arduino.h>

#include "mbed_INA219.h"
#include "nano33ble/project_pins.h"

class MbedLoadSimulator : public LoadSimulatorImpl {
	/// Load injector shader pwm
	mbed::PwmOut pin_lts_;
	/// Load injector liner pwm
	mbed::PwmOut pin_ltl_;
	/// Load injector base
	mbed::DigitalOut pin_ltb_;

      public:
	MbedLoadSimulator()
	    : pin_lts_(digitalPinToPinName(TPS_PIN_LTS)), pin_ltl_(digitalPinToPinName(TPS_PIN_LTL)),
	      pin_ltb_(digitalPinToPinName(TPS_PIN_LTB)) {}

	void begin() override {
		// Set PWM pin frequencies and suspend
		pin_ltl_.period_us(6667);
		pin_lts_.period_us(6667);
		set_load(false, 0.f, 0.f);
	}

	void set_load(bool base, float liner, float shader) override {
		pin_ltb_.write(base ? 1 : 0);
		pin_ltl_.write(liner);
		pin_lts_.write(shader);
	}
};

LoadSimulatorImpl *new_load_simulator_impl() { return new MbedLoadSimulator(); }
