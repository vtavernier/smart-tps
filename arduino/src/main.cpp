//#include <Adafruit_INA219.h>
#include <Arduino.h>
#include <SFE_MicroOLED.h>

const int OLED_CS = 10;
const int OLED_RST = 9;
const int OLED_DC = 8;

MicroOLED oled(OLED_RST, OLED_DC, OLED_CS);

const int LOAD_SIM = 2;

#include <mbed/mbed.h>
mbed::PwmOut pinLoadSim(digitalPinToPinName(LOAD_SIM));
mbed::PwmOut pinLedR(digitalPinToPinName(LEDR));
mbed::PwmOut pinLedG(digitalPinToPinName(LEDG));
mbed::PwmOut pinLedB(digitalPinToPinName(LEDB));

#include <mbed_INA219.h>

mbed::I2C i2c(digitalPinToPinName(PIN_WIRE_SDA),
	      digitalPinToPinName(PIN_WIRE_SCL));

#define INA219_ADDR 0x41
mbed_INA219 *ina219;

void setup() {
	// Set frequency on the PWM pin
	pinLoadSim.period_us(6667);
	pinLoadSim.write(0.5f);

	// Setup LEDS
	pinLedR.write(1.0f);
	pinLedG.write(1.0f);
	pinLedB.write(1.0f);
	for (auto *pin : {&pinLedR, &pinLedG, &pinLedB}) {
		pin->period_ms(10);
	}

	pinLedR.write(0.0f);

	// Set I2C bus frequency
	// TODO: 1MHz on PCB?
	i2c.frequency(400000);
	ina219 = new mbed_INA219(i2c, INA219_ADDR);

	pinLedR.write(1.0f);
	pinLedB.write(0.0f);

	// Initialize, clear screen
	oled.begin();
	oled.setFontType(0);
	oled.clear(PAGE);
	oled.clear(ALL);
	oled.display();

	pinLedB.write(1.0f);
	pinLedG.write(0.0f);
}

struct Sample {
	unsigned long t;
	float i;
};

const size_t SAMPLE_COUNT = 100;
typedef Sample SampleBuffer[SAMPLE_COUNT];

SampleBuffer samples = {0};
size_t samples_idx = 0;
unsigned long falling_edges[SAMPLE_COUNT] = {0};
unsigned long rising_edges[SAMPLE_COUNT] = {0};


const size_t STAT_COUNT = 10;
float stat_freq = 0.f;
float stat_duty_cycle_samples[STAT_COUNT] = {0};
size_t stat_duty_cycle_ptr = 0;

/**
 * Set the current load status
 *
 * @param pwm Pulse width of the load
 */
void setLoad(float pwm) {
	pinLoadSim.write(pwm);
	pinLedR.write(1.0f - pwm);
}

unsigned long mean_lastms = 0;
unsigned long screen_lastms = 0;
bool setaddr = true;

void loop() {
	float expected = ((millis() / 10000) % 4) / 4.f;
	// Update load LED
	setLoad(expected);

	pinLedG.write(1.0f);
	pinLedB.write(0.0f);

	// Update current average
	auto val = ina219->get_current();
	if (val != MAXFLOAT) {
		auto &c = samples[samples_idx];
		c.t = micros();
		c.i = val;

		// Dump to serial
		// TODO: Remove later or make it async?
		// SerialUSB.print(c.t);
		// SerialUSB.print(",");
		// SerialUSB.println(c.i);

		// Update index
		samples_idx =
		    (samples_idx + 1) % (sizeof(samples) / sizeof(samples[0]));
	}

	pinLedB.write(1.0f);

	unsigned long now = millis();

	if (now - mean_lastms > 50) {
		mean_lastms = now;

		// First pass, determine max and min
		float max = -10000.f, min = 10000.f;
		for (auto &s : samples) {
			if (s.i > max) max = s.i;
			if (s.i < min) min = s.i;
		}

		// Currently no current, skip it
		if (max < 5.0f) {
			return;
		}

		float mid = (max + min) / 2.0;

		// Second pass, determine edges
		memset(falling_edges, 0, sizeof(falling_edges));
		memset(rising_edges, 0, sizeof(rising_edges));

		bool high = samples[0].i > mid;
		size_t falling_ptr = 0;
		size_t rising_ptr = 0;
		unsigned long last_t = samples[0].t;

		for (size_t i = 0; i < SAMPLE_COUNT; ++i) {
			auto &s = samples[(i + samples_idx) % SAMPLE_COUNT];

			if (high && s.i < mid) {
				// Falling edge
				high = false;
				falling_edges[falling_ptr++] =
				    (last_t + s.t) / 2;
			} else if (!high && s.i > mid) {
				// Rising edge
				high = true;
				rising_edges[rising_ptr++] = (last_t + s.t) / 2;
			}

			last_t = s.t;
		}

		// Third pass: compute period and duty cycle
		size_t max_ptr =
		    falling_ptr < rising_ptr ? falling_ptr : rising_ptr;

		unsigned long falling_sum = 0, rising_sum = 0,
			      dt_duration_sum = 0;
		unsigned long valid = 0;

		for (size_t i = 1; i < max_ptr; ++i) {
			if (falling_edges[i] > falling_edges[i - 1] &&
			    rising_edges[i] > rising_edges[i - 1]) {
				// Frequency computation
				falling_sum +=
				    falling_edges[i] - falling_edges[i - 1];
				rising_sum +=
				    rising_edges[i] - rising_edges[i - 1];

				// Duty-cycle computation
				if (falling_edges[i] < rising_edges[i]) {
					dt_duration_sum += falling_edges[i] -
							   rising_edges[i - 1];
				} else {
					dt_duration_sum += rising_edges[i] -
							   falling_edges[i - 1];
				}
				valid++;
			}
		}

		if (valid > 0) {
			// Compute period from average of rising and falling
			// edge distance
			float period =
			    (falling_sum + rising_sum) / (2.0f * valid);

			// Compute duty cycle from this
			float dtc = (100.0f * dt_duration_sum / valid) / period;

			// Compute frequency
			float freq = 1.0e6f / period;

			// Update stats
			stat_duty_cycle_samples[stat_duty_cycle_ptr] = dtc;
			stat_duty_cycle_ptr = (stat_duty_cycle_ptr + 1) % STAT_COUNT;
			stat_freq = freq;
		}
	}

	if (now - screen_lastms > 250) {
		screen_lastms = now;

		oled.clear(PAGE);

		oled.setCursor(0, 0);
		oled.print(stat_freq);
		oled.setCursor(oled.getFontWidth() * 6, 0);
		oled.println("Hz");

		float dtc = 0.0f;
		for (size_t i = 0; i < STAT_COUNT; ++i)
			dtc += stat_duty_cycle_samples[i];
		dtc /= STAT_COUNT;

		oled.print(dtc);
		oled.setCursor(oled.getFontWidth() * 6, oled.getFontHeight());
		oled.println("%");

		oled.println();
		oled.print(100.f * expected);
		oled.setCursor(oled.getFontWidth() * 6,
			       3 * oled.getFontHeight());
		oled.println("%");

		oled.display();
	}
}
