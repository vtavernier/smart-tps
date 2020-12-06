#ifndef _NANO33BLE_LOAD_SENSOR_HPP_
#define _NANO33BLE_LOAD_SENSOR_HPP_

#include <mbed/mbed.h>

extern LoadSensorImpl *new_nano33ble_load_sensor_impl(mbed::I2C &i2c);

#endif /* _NANO33BLE_LOAD_SENSOR_HPP_ */
