#include "load_simulator.hpp"

LoadSimulator::LoadSimulator() : impl_(new_load_simulator_impl()) {}

LoadSimulator::LoadSimulator(LoadSimulatorImpl *impl) : impl_(impl) {}

LoadSimulator::~LoadSimulator() {
	delete impl_;
	impl_ = nullptr;
}

void LoadSimulator::begin() { impl_->begin(); }

void LoadSimulator::set_load(bool base, float liner, float shader) { impl_->set_load(base, liner, shader); }
