#ifndef _TPS_H_
#define _TPS_H_

#include <Arduino.h>
#include <SFE_MicroOLED.h>
#include <mbed/mbed.h>

#include "mbed_INA219.h"

class Tps {
	/// OLED screen instance
	MicroOLED oled_;
	/// Load injector shader pwm
	//mbed::PwmOut pin_lts_;
	/// Load injector liner pwm
	mbed::PwmOut pin_ltl_;
	/// Load injector base
	mbed::DigitalOut pin_ltb_;
	/// Red LED
	mbed::PwmOut pin_led_r_;
	/// Green LED
	mbed::PwmOut pin_led_g_;
	/// Blue LED
	mbed::PwmOut pin_led_b_;
	/// I2C instance for battery and INA219s
	mbed::I2C i2c_;
	/// INA219 instance for liner
	mbed_INA219 ina219_liner_;
	/// INA219 instance for shader
	mbed_INA219 ina219_shader_;

      public:
	/// Construct a new Tps instance
	Tps();

	/// Initialize the peripherals, to be called in setup
	void begin();

	/// Poll devices for changes, to be called in loop
	void poll();

	/**
	 * @brief Set the RGB LED color
	 *
	 * @param r Amount of red
	 * @param g Amount of green
	 * @param b Amount of blue
	 */
	void set_led(float r, float g, float b);

	/**
	 * @brief Set the load status
	 *
	 * @param base true if the base should be on
	 * @param liner pulse width of the liner
	 * @param shader pulse width of the shader
	 */
	void set_load(bool base, float liner, float shader);

	/**
	 * @brief Get the current running through the liner unit
	 *
	 * @return Current in mA
	 */
	float get_liner_current();

	/**
	 * @brief Get the current running through the shader unit
	 *
	 * @return Current in mA
	 */
	float get_shader_current();

	/**
	 * @brief Get a reference to the OLED screen driver
	 */
	inline MicroOLED &get_oled() { return oled_; }
};

#endif /* _TPS_H_ */
