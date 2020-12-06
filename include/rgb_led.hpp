#ifndef _RGB_LED_HPP_
#define _RGB_LED_HPP_

class RgbLedImpl {
      public:
	virtual ~RgbLedImpl() = default;

	virtual void begin() = 0;

	/**
	 * @brief Set the RGB LED color
	 *
	 * @param r Amount of red
	 * @param g Amount of green
	 * @param b Amount of blue
	 */
	virtual void set_led(float r, float g, float b) = 0;
};

extern RgbLedImpl *new_rgb_led_impl();

class RgbLed {
	RgbLedImpl *impl_;

      public:
	RgbLed();
	~RgbLed();

	void begin();

	/**
	 * @brief Set the RGB LED color
	 *
	 * @param r Amount of red
	 * @param g Amount of green
	 * @param b Amount of blue
	 */
	void set_led(float r, float g, float b);
};

#endif /* _RGB_LED_HPP_ */
