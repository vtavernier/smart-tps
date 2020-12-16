#ifndef _LOAD_SENSOR_HPP_
#define _LOAD_SENSOR_HPP_

#include <stdint.h>

#include "load_type.hpp"

class LoadSensorImpl {
      public:
	virtual ~LoadSensorImpl() = default;

	virtual void begin() = 0;

	/**
	 * Measure the current load in 10ths of mA
	 *
	 * @param type Type of the load to measure
	 */
	virtual int16_t read(LoadType type) = 0;
};

class LoadSensor {
	LoadSensorImpl *impl_;
	bool dump_measurements_;

      public:
	LoadSensor(LoadSensorImpl *impl);
	~LoadSensor();

	void begin();

	/**
	 * @brief Enable dumping measurements to the system's output
	 *
	 * @param value true to enable dumping measurements
	 */
	void set_dump_measurements(bool value);

	/**
	 * Measure the current load in 10ths of mA
	 *
	 * @param type Type of the load to measure
	 */
	int16_t read(LoadType type);
};

#endif /* _LOAD_SENSOR_HPP_ */
