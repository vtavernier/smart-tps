#ifndef _NATIVE_LOAD_SENSOR_HPP_
#define _NATIVE_LOAD_SENSOR_HPP_

#include "../load_sensor.hpp"
#include "load_simulator.hpp"

#include <string>

extern LoadSensorImpl *new_native_load_sensor_impl(const LoadSimulatorState &state);
extern LoadSensorImpl *new_native_load_sensor_impl(const std::string &values_path);

#endif /* _NATIVE_LOAD_SENSOR_HPP_ */
