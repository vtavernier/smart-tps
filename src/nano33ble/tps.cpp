#include <SPI.h>

#include "tps.hpp"

#include "project_pins.h"

Tps::Tps()
    : gui_(), pin_lts_(digitalPinToPinName(TPS_PIN_LTS)), pin_ltl_(digitalPinToPinName(TPS_PIN_LTL)),
      pin_ltb_(digitalPinToPinName(TPS_PIN_LTB)),
      pin_led_(digitalPinToPinName(LEDR), digitalPinToPinName(LEDG), digitalPinToPinName(LEDB)),
      i2c_(digitalPinToPinName(TPS_PIN_SDA), digitalPinToPinName(TPS_PIN_SCL)),
      ina219_liner_(i2c_, TPS_INA219_LINER_ADDR), ina219_shader_(i2c_, TPS_INA219_SHADER_ADDR) {}

void Tps::begin() {
	// Set PWM pin frequencies and suspend
	pin_ltl_.period_us(6667);
	// pin_lts_.period_us(6667);
	set_load(false, 0.f, 0.f);

	// Do not suspend the LED since we're using it most of the time
	pin_led_.period_ms(10);

	// Set LED to blue initially
	set_led(0.f, 0.f, 1.f);
	delay(1000);

	// Set I2C bus frequency
	// TODO: Support 1MHz?
	i2c_.frequency(400000);

	// Initialize INA219s
	ina219_liner_.begin();
	ina219_shader_.begin();

	// Initialize GUI
	gui_.begin();
}

void Tps::poll() {
	// TODO: Implement loop behavior
}

void Tps::set_led(float r, float g, float b) { pin_led_.write(1.0f - r, 1.0f - g, 1.0f - b); }

void Tps::set_load(bool base, float liner, float shader) {
	pin_ltb_.write(base ? 1 : 0);
	pin_ltl_.write(liner);
	pin_lts_.write(shader);
}

float Tps::get_liner_current() { return ina219_liner_.get_current(); }

float Tps::get_shader_current() { return ina219_shader_.get_current(); }
