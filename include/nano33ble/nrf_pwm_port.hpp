#ifndef _NANO33BLE_NRF_PWM_PORT_HPP_
#define _NANO33BLE_NRF_PWM_PORT_HPP_

#include <stdint.h>

// clang-format off
#include <mbed_config.h>
#include <mbed/targets/TARGET_NORDIC/TARGET_NRF5x/TARGET_SDK_15_0/modules/nrfx/mdk/nrf52840.h>
#include <mbed/targets/TARGET_NORDIC/TARGET_NRF5x/TARGET_SDK_15_0/modules/nrfx/hal/nrf_pwm.h>
#include <mbed/targets/TARGET_NORDIC/TARGET_NRF5x/TARGET_SDK_15_0/modules/nrfx/drivers/nrfx_common.h>
#include <mbed/targets/TARGET_NORDIC/TARGET_NRF5x/TARGET_SDK_15_0/modules/nrfx/drivers/include/nrfx_pwm.h>
// clang-format on

/**
 * Represents an nRF PWM port (up to 4 channels with shared period and clock).
 *
 * Currently only one instance of this class can exist since it uses the
 * NRF_PWM_INSTANCE 3. This frees up the first 3 instances for regular
 * mbed::PwmOut and analogWrite usage.
 */
class NrfPwmPort {
	/// PWM instance
	nrfx_pwm_t instance_;
	/// Configuration structure for the PWM instance
	nrfx_pwm_config_t config_;
	/// Value sequence for PWM
	nrf_pwm_sequence_t sequence_;
	/// Value sequence data (pinned by sequence_)
	nrf_pwm_values_individual_t pulses_;

	/// Restart the PWM instance after changing config_
	void restart();

      public:
	/**
	 * Build a new instance of NrfPwmPort.
	 *
	 * @param pin1 First channel pin number
	 * @param pin2 Second channel pin number
	 * @param pin3 Third channel pin number
	 * @param pin4 Fourth channel pin number
	 */
	NrfPwmPort(uint8_t pin1, uint8_t pin2 = NRFX_PWM_PIN_NOT_USED, uint8_t pin3 = NRFX_PWM_PIN_NOT_USED,
		   uint8_t pin4 = NRFX_PWM_PIN_NOT_USED);

	/**
	 * Free up resources used by this NrfPwmPort.
	 */
	~NrfPwmPort();

	// Disable copying and moving since sequence_ requires pulses_ to be pinned

	NrfPwmPort(NrfPwmPort &&) = delete;
	NrfPwmPort &operator=(NrfPwmPort &&) = delete;
	NrfPwmPort(const NrfPwmPort &) = delete;
	NrfPwmPort &operator=(const NrfPwmPort &) = delete;

	/**
	 * Set the period of this PWM port.
	 *
	 * @param seconds Period expressed in seconds
	 */
	void period(float seconds);
	/**
	 * Set the period of this PWM port.
	 *
	 * @param ms Period expressed in milliseconds
	 */
	void period_ms(uint16_t ms);
	/**
	 * Set the period of this PWM port.
	 *
	 * @param us Period expressed in microseconds
	 */
	void period_us(uint16_t us);

	/**
	 * Write the duty cycle of the PWM channels
	 *
	 * @param pin1 Duty cycle for channel 1 (between 0 and 1)
	 * @param pin2 Duty cycle for channel 1 (between 0 and 1)
	 * @param pin3 Duty cycle for channel 1 (between 0 and 1)
	 * @param pin4 Duty cycle for channel 1 (between 0 and 1)
	 */
	void write(float pin1, float pin2 = 0.0f, float pin3 = 0.0f, float pin4 = 0.0f);
};

#endif /* _NANO33BLE_NRF_PWM_PORT_HPP_ */
