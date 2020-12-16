#include "platform.hpp"
#include "stats.hpp"

Stats::Stats() : valid_samples{0}, samples_idx{0}, zero_samples{0}, stat_freq{0.f}, stat_duty_cycle{0.f} {}

void Stats::reset() {
	valid_samples = 0;
	zero_samples = 0;
}

SampleStatus Stats::add_sample(int16_t raw_current_val) {
	// Ignore invalid current value
	if (raw_current_val == INT16_MAX)
		return SampleStatus::Invalid;

	auto result = SampleStatus::Valid;

	// New valid sample
	if (valid_samples < SAMPLE_COUNT)
		valid_samples++;

	if (raw_current_val < CurrentThreshold) {
		// Zero sample
		result = SampleStatus::Zero;

		if (zero_samples < SAMPLE_COUNT)
			zero_samples++;
	} else {
		// Non-zero sample
		if (zero_samples > 10) {
			// There was a big chunk of 0 samples, restart buffer
			valid_samples = 1;
		} // else, there was only a few samples, so keep them, they're just noise

		zero_samples = 0;
	}

	// Add sample to buffer
	samples[samples_idx].i = raw_current_val;
	samples[samples_idx].t = Platform.micros();

	// Increment ring buffer index
	samples_idx = (samples_idx + 1) % SAMPLE_COUNT;

	return result;
}

bool Stats::update_stats() {
	// Reset statistics
	stat_freq = 0.f;
	stat_duty_cycle = 0.f;

	// How many samples can we use
	auto ok_samples = valid_samples - zero_samples;

	// Skip stat computation if no valid samples in buffer
	if (ok_samples == 0)
		return false;

	// First pass, determine edges
	bool high = samples[samples_idx].i > CurrentStepThreshold;
	size_t falling_ptr = 0;
	size_t rising_ptr = 0;
	unsigned long last_t = samples[samples_idx].t;
	uint32_t low_samples = 0;
	uint32_t high_samples = 0;

	for (size_t i = 0; i < ok_samples; ++i) {
		auto &s{samples[(samples_idx + i) % SAMPLE_COUNT]};

		if (high && s.i < CurrentStepThreshold) {
			// Falling edge
			high = false;
			falling_edges[falling_ptr++] = (last_t + s.t) / 2;
		} else if (!high && s.i > CurrentStepThreshold) {
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

	// Second pass: compute period and duty cycle
	size_t max_ptr = falling_ptr < rising_ptr ? falling_ptr : rising_ptr;

	uint32_t falling_sum = 0, rising_sum = 0;
	size_t valid = 0;

	for (size_t i = 1; i < max_ptr; ++i) {
		if (falling_edges[i] > falling_edges[i - 1] && rising_edges[i] > rising_edges[i - 1]) {
			// Frequency computation
			falling_sum += falling_edges[i] - falling_edges[i - 1];
			rising_sum += rising_edges[i] - rising_edges[i - 1];

			valid++;
		}
	}

	if (valid > 0) {
		// Compute period from average of rising and falling
		// edge distance
		float period = (falling_sum + rising_sum) / (2.0f * valid);

		// Compute duty cycle from this
		stat_duty_cycle = 100.0f * high_samples / (high_samples + low_samples);

		// Compute frequency
		stat_freq = 1.0e6f / period;

		return true;
	}

	return false;
}
