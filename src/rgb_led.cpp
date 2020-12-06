#include "rgb_led.hpp"

RgbLed::RgbLed() : impl_(new_rgb_led_impl()) {}

RgbLed::~RgbLed() {
	delete impl_;
	impl_ = nullptr;
}

void RgbLed::begin() { impl_->begin(); }

void RgbLed::set_led(float r, float g, float b) { impl_->set_led(r, g, b); }
