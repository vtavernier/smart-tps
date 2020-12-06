#include "tps.hpp"

#include "native/load_sensor.hpp"
#include "native/load_simulator.hpp"

#include <chrono>
#include <iostream>
#include <unistd.h>

using namespace std;

chrono::high_resolution_clock::time_point platform_start;

int main(int argc, char *argv[]) {
	LoadSimulatorState state;

	// Initialize time start
	platform_start = chrono::high_resolution_clock::now();

	// Create abstract components
	Gui gui;
	LoadSimulator load_simulator(new_native_load_simulator_impl(state));
	RgbLed led;
	LoadSensor load_sensor(new_native_load_sensor_impl(state));

	// Start program
	Tps tps(gui, load_simulator, led, load_sensor);

	tps.begin();

	for (;;) {
		tps.poll();
		usleep(2000);
	}

	return 0;
}
