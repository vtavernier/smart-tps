#include "stats.hpp"
#include "platform.hpp"

template <typename T> void insertionsort(T *a, size_t n) {
	for (size_t i = 1; i < n; i++) {
		T e = a[i];
		int j = i;
		while (--j >= 0 && a[j] > e) {
			a[j + 1] = a[j];
		}
		a[j + 1] = e;
	}
}

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
	size_t rising_ptr = 0;

	unsigned long last_t = samples[samples_idx].t;
	uint32_t low_samples = 0;
	uint32_t high_samples = 0;

	unsigned long last_rising_edge = 0;

	for (size_t i = 0; i < ok_samples; ++i) {
		auto &s{samples[(samples_idx + i) % SAMPLE_COUNT]};

		if (s.t < last_t)
			continue;

		auto tdiff = s.t - last_t;

		if (high && s.i < CurrentStepThreshold) {
			// Falling edge
			high = false;

			high_samples += tdiff / 2;
			low_samples += tdiff / 2;
		} else if (!high && s.i > CurrentStepThreshold) {
			// Rising edge
			high = true;

			// If we have already seen a rising edge
			if (last_rising_edge != 0) {
				// Add the duration from last to current rising edge to the array
				rising_edges[rising_ptr++] = (last_t + s.t) / 2 - last_rising_edge;
			}

			// Update the current rising edge time
			last_rising_edge = (last_t + s.t) / 2;

			high_samples += tdiff / 2;
			low_samples += tdiff / 2;
		} else {
			// Add duration of the current sample to the target
			if (high) {
				high_samples += tdiff;
			} else {
				low_samples += tdiff;
			}
		}

		last_t = s.t;
	}

	if (rising_ptr < SAMPLE_COUNT / 8)
		return false;

	// Sort samples
	insertionsort(rising_edges, rising_ptr);

	// Compute mean, skip lower and higher 25%
	int32_t period_mean_sum = 0;
	for (size_t i = rising_ptr / 4; i < (rising_ptr - rising_ptr / 4); ++i) {
		period_mean_sum += rising_edges[i];
	}

	// Compute period from average
	float period = static_cast<float>(period_mean_sum) / static_cast<float>(rising_ptr / 2);

	// Compute duty cycle from this
	stat_duty_cycle = 100.0f * high_samples / (high_samples + low_samples);

	// Compute frequency
	stat_freq = 1.0e6f / period;

	return true;
}
