#include "load_simulator.hpp"

LoadSimulator::LoadSimulator() : impl_(new_load_simulator_impl()) {}

LoadSimulator::LoadSimulator(LoadSimulatorImpl *impl) : impl_(impl) {}

LoadSimulator::~LoadSimulator() {
	delete impl_;
	impl_ = nullptr;
}

void LoadSimulator::begin() { impl_->begin(); }

void LoadSimulator::set_load(LoadType type, float value) {
	bool base = value > 0.f;

	switch (type) {
	case LoadType::Liner:
		impl_->set_load(base, value, 0.f);
		break;
	case LoadType::Shader:
		impl_->set_load(base, 0.f, value);
		break;
	}
}
