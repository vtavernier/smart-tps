#include "tps.hpp"

#include "project_pins.h"

Tps::Tps()
    : oled_(TPS_PIN_OLED_RESET, TPS_PIN_OLED_DC, TPS_PIN_OLED_CS),
      pin_ltl_(digitalPinToPinName(TPS_PIN_LTL)),
      //pin_lts_(digitalPinToPinName(TPS_PIN_LTS)),
      pin_ltb_(digitalPinToPinName(TPS_PIN_LTB)),
      pin_led_r_(digitalPinToPinName(LEDR)),
      pin_led_g_(digitalPinToPinName(LEDG)),
      pin_led_b_(digitalPinToPinName(LEDB)),
      i2c_(digitalPinToPinName(TPS_PIN_SDA), digitalPinToPinName(TPS_PIN_SCL)),
      ina219_liner_(i2c_, TPS_INA219_LINER_ADDR),
      ina219_shader_(i2c_, TPS_INA219_SHADER_ADDR) {}

void Tps::begin() {
	// Set PWM pin frequencies and suspend
	pin_ltl_.period_us(6667);
	//pin_lts_.period_us(6667);
	set_load(false, 0.f, 0.f);

	// Do not suspend the LED since we're using it most of the time
	pin_led_r_.period_ms(10);
	pin_led_g_.period_ms(10);
	pin_led_b_.period_ms(10);

	// Set LED to blue initially
	set_led(0.f, 0.f, 1.f);
	delay(1000);

	// Set I2C bus frequency
	// TODO: Support 1MHz?
	i2c_.frequency(400000);

	// Initialize INA219s
	ina219_liner_.begin();
	ina219_shader_.begin();

	// Clear display at boot
	oled_.begin();
	oled_.setFontType(0);
	oled_.clear(ALL);
	oled_.display();
}

void Tps::poll() {
	// TODO: Implement loop behavior
}

void Tps::set_led(float r, float g, float b) {
	pin_led_r_.write(1.0f - r);
	pin_led_g_.write(1.0f - g);
	pin_led_b_.write(1.0f - b);
}

void Tps::set_load(bool base, float liner, float shader) {
	pin_ltb_.write(base ? 1 : 0);
	pin_ltl_.write(liner);
	//pin_lts_.write(shader);
}

float Tps::get_liner_current() { return ina219_liner_.get_current(); }

float Tps::get_shader_current() { return ina219_shader_.get_current(); }
