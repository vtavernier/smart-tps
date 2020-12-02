#include "tps.hpp"

Tps tps;

void setup() { tps.begin(); }

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

unsigned long mean_lastms = 0;
unsigned long screen_lastms = 0;
bool setaddr = true;

void loop() {
	// TODO: Move rest of loop code into the Tps::poll method
	tps.poll();

	float expected = ((millis() / 10000) % 4) / 4.f;

	// Update load
	tps.set_load(expected > 0.f, expected, 0.);
	tps.set_led(0.0f, 0.0f, expected);

	// Update current average
	auto val = tps.get_liner_current();
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

	// Add green to the LED
	tps.set_led(0.0f, 1.0f, expected);

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
		size_t low_samples = 0;
		size_t high_samples = 0;

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

			// Add duration of the current sample to the target
			if (s.t > last_t) {
				if (high) {
					high_samples += s.t - last_t;
				} else {
					low_samples += s.t - last_t;
				}
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
			// float dtc = (100.0f * dt_duration_sum / valid) /
			// period;
			float dtc = 100.0f * high_samples /
				    (high_samples + low_samples);

			// Compute frequency
			float freq = 1.0e6f / period;

			// Update stats
			stat_duty_cycle_samples[stat_duty_cycle_ptr] = dtc;
			stat_duty_cycle_ptr =
			    (stat_duty_cycle_ptr + 1) % STAT_COUNT;
			stat_freq = freq;
		}
	}

	if (now - screen_lastms > 250) {
		screen_lastms = now;
		auto &oled(tps.get_oled());

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
