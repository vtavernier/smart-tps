#include "nano33ble/nrf_pwm_port.hpp"

#include <mbed/platform/mbed_assert.h>
#include <string.h>

/* 0x7FFF is the max of COUNTERTOP pulse for the PWM peripherial of the nRF52.
 */
#define MAX_PWM_COUNTERTOP (0x7FFF)

/* The PWM is driven by a 1 MHz clock to fit the 1 us resolution expected by the
 * API. */
#define MAX_PWM_PERIOD_US (MAX_PWM_COUNTERTOP)
#define MAX_PWM_PERIOD_MS (MAX_PWM_PERIOD_US / 1000)
#define MAX_PWM_PERIOD_S ((float)MAX_PWM_PERIOD_US / 1000000.0f)

/* Sequence bit that denotes the polarity of the pwm waveform. */
#define SEQ_POLARITY_BIT (0x8000)

void NrfPwmPort::restart() {
	nrfx_pwm_uninit(&instance_);
	ret_code_t result = nrfx_pwm_init(&instance_, &config_, NULL);
	MBED_ASSERT(result == NRFX_SUCCESS);
}

// TODO: Configure instance number?
NrfPwmPort::NrfPwmPort(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4)
    : instance_ NRFX_PWM_INSTANCE(3),
      config_
      /* Default configuration:
       * 1 pin per instance, otherwise they would share base count.
       * 1 MHz clock source to match the 1 us resolution.
       */
      {
	  .output_pins =
	      {
		  pin1,
		  pin2,
		  pin3,
		  pin4,
	      },
	  .irq_priority = PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
	  .base_clock = NRF_PWM_CLK_1MHz,
	  .count_mode = NRF_PWM_MODE_UP,
	  .top_value = 0, // TODO: Default period?
	  .load_mode = NRF_PWM_LOAD_INDIVIDUAL,
	  .step_mode = NRF_PWM_STEP_AUTO,
      },
      sequence_{0}, pulses_{0} {
	/* Initialize instance with new configuration. */
	ret_code_t result = nrfx_pwm_init(&instance_, &config_, NULL);
	MBED_ASSERT(result == NRFX_SUCCESS);
}

NrfPwmPort::~NrfPwmPort() { nrfx_pwm_uninit(&instance_); }

void NrfPwmPort::period(float seconds) {
	if (seconds > MAX_PWM_PERIOD_S)
		seconds = MAX_PWM_PERIOD_S;
	period_us(seconds * 1000000);
}

void NrfPwmPort::period_ms(uint16_t ms) {
	if (ms > MAX_PWM_PERIOD_MS)
		ms = MAX_PWM_PERIOD_S;
	period_us(ms * 1000);
}

void NrfPwmPort::period_us(uint16_t us) {
	if (us > MAX_PWM_PERIOD_US)
		us = MAX_PWM_PERIOD_US;
	config_.top_value = us;
	restart();
}

#define _CLAMP(x, a, b) ((x) < a ? a : (((x) > b) ? b : (x)))

void NrfPwmPort::write(float pin1, float pin2, float pin3, float pin4) {
	// Prepare pulse values for individual channels
	uint16_t pulses[] = {static_cast<uint16_t>(_CLAMP(pin1, 0.f, 1.f) * config_.top_value),
			     static_cast<uint16_t>(_CLAMP(pin2, 0.f, 1.f) * config_.top_value),
			     static_cast<uint16_t>(_CLAMP(pin3, 0.f, 1.f) * config_.top_value),
			     static_cast<uint16_t>(_CLAMP(pin4, 0.f, 1.f) * config_.top_value)};

	for (uint16_t &pulse : pulses) {
		pulse = SEQ_POLARITY_BIT |
			(~SEQ_POLARITY_BIT & ((pulse > MAX_PWM_COUNTERTOP) ? MAX_PWM_COUNTERTOP : pulse));
	}

	// Copy pulse data to array
	memcpy(&pulses_, pulses, sizeof(pulses_));

	// Send playback data
	sequence_.values.p_individual = &pulses_;
	sequence_.length = NRF_PWM_VALUES_LENGTH(pulses_);
	sequence_.repeats = 0;
	sequence_.end_delay = 0;

	ret_code_t result = nrfx_pwm_simple_playback(&instance_, &sequence_, 1, NRFX_PWM_FLAG_LOOP);
	MBED_ASSERT(result == NRFX_SUCCESS);
}
