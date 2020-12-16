#include "load_sensor.hpp"

#include "platform.hpp"

LoadSensor::LoadSensor(LoadSensorImpl *impl) : impl_(impl), dump_measurements_(false) {}

LoadSensor::~LoadSensor() {
	delete impl_;
	impl_ = nullptr;
}

void LoadSensor::begin() { impl_->begin(); }

void LoadSensor::set_dump_measurements(bool value) { dump_measurements_ = value; }

int16_t LoadSensor::read(LoadType type) {
	auto val = impl_->read(type);

	if (dump_measurements_) {
		Platform.print_sensor_measurement(val);
	}

	return val;
}
