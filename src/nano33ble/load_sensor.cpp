#include "load_sensor.hpp"

#include <Arduino.h>

#include "mbed_INA219.h"
#include "nano33ble/load_sensor.hpp"
#include "nano33ble/project_pins.h"

class DualIna219LoadSensorImpl : public LoadSensorImpl {
	/// INA219 instance for liner
	mbed_INA219 ina219_liner_;
	/// INA219 instance for shader
	mbed_INA219 ina219_shader_;

      public:
	DualIna219LoadSensorImpl(mbed::I2C &i2c)
	    : ina219_liner_(i2c, TPS_INA219_LINER_ADDR), ina219_shader_(i2c, TPS_INA219_SHADER_ADDR) {}

	void begin() override {
		ina219_liner_.begin();
		ina219_shader_.begin();
	}

	int16_t read(LoadType type) override {
		switch (type) {
		case LoadType::Liner:
			return ina219_liner_.get_current();
		case LoadType::Shader:
			return ina219_shader_.get_current();
		}

		return INT16_MAX;
	}
};

LoadSensorImpl *new_nano33ble_load_sensor_impl(mbed::I2C &i2c) { return new DualIna219LoadSensorImpl(i2c); }
