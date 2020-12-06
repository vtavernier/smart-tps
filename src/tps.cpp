#include "tps.hpp"

Tps::Tps(Gui &gui, LoadSimulator &load_simulator, RgbLed &rgb_led, LoadSensor &load_sensor)
    : gui_(gui), load_simulator_(load_simulator), rgb_led_(rgb_led), load_sensor_(load_sensor) {}

void Tps::begin() {
	// Initialize load simulator
	load_simulator_.begin();

	// Initialize LED
	rgb_led_.begin();

	// Initialize load sensor
	load_sensor_.begin();

	// Initialize GUI
	gui_.begin();
}

void Tps::poll() {
	// TODO: Implement loop behavior
}
