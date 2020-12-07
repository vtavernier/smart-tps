#include <Arduino.h>

#include "tps.hpp"

#include "nano33ble/load_sensor.hpp"
#include "nano33ble/project_pins.h"

/// I2C instance (shared among some peripherals)
mbed::I2C i2c(digitalPinToPinName(TPS_PIN_SDA), digitalPinToPinName(TPS_PIN_SCL));

// Abstract components
Gui gui;
LoadSimulator load_simulator;
RgbLed rgb_led;
LoadSensor load_sensor(new_nano33ble_load_sensor_impl(i2c));
Stats stats;

Tps tps(gui, load_simulator, rgb_led, load_sensor, stats);

void setup() {
	// Set I2C bus frequency
	// TODO: Support 1MHz?
	i2c.frequency(400000);

	// Initialize components
	tps.begin();
}

void loop() { tps.poll(); }
