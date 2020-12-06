#include <iostream>

#include <unistd.h>

#include "tps.hpp"

#include "native/load_sensor.hpp"
#include "native/load_simulator.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	LoadSimulatorState state;

	Gui gui;
	LoadSimulator load_simulator(new_native_load_simulator_impl(state));
	RgbLed led;
	LoadSensor load_sensor(new_native_load_sensor_impl(state));

	Tps tps(gui, load_simulator, led, load_sensor);

	tps.begin();

	for (;;) {
		tps.poll();
		usleep(2000);
	}

	return 0;
}
