#include "rgb_led.hpp"

#include <Arduino.h>

#include "nano33ble/nrf_pwm_port.hpp"
#include "nano33ble/project_pins.h"

class NrfRgbLedImpl : public RgbLedImpl {
	/// RGB LED PWM port
	NrfPwmPort pin_led_;

      public:
	NrfRgbLedImpl() : pin_led_(digitalPinToPinName(LEDR), digitalPinToPinName(LEDG), digitalPinToPinName(LEDB)) {}

	void begin() override {
		pin_led_.period_ms(10);
		set_led(0.f, 0.f, 0.f);
	}

	void set_led(float r, float g, float b) override { pin_led_.write(1.0f - r, 1.0f - g, 1.0f - b); }
};

RgbLedImpl *new_rgb_led_impl() { return new NrfRgbLedImpl(); }
