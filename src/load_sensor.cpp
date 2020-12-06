#include "load_sensor.hpp"

LoadSensor::LoadSensor(LoadSensorImpl *impl) : impl_(impl) {}

LoadSensor::~LoadSensor() {
	delete impl_;
	impl_ = nullptr;
}

void LoadSensor::begin() { impl_->begin(); }

int16_t LoadSensor::read(LoadType type) { return impl_->read(type); }
