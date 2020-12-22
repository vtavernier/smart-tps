#include "stats.hpp"
#include "platform.hpp"

#include <string.h>

Stats::Stats()
    : hist_{0}, last_rising_edge_t_{0}, currently_high_{false}, low_samples_{0}, high_samples_{0},
      uncommitted_low_samples_{0}, stat_freq_{0}, stat_duty_cycle_{0} {}

void Stats::reset() {
	memset(&hist_, 0, sizeof(hist_));
	last_rising_edge_t_ = 0;
	currently_high_ = false;
	low_samples_ = 0;
	high_samples_ = 0;
	uncommitted_low_samples_ = 0;
	stat_freq_ = 0.f;
	stat_duty_cycle_ = 0.f;
}

SampleStatus Stats::add_sample(int16_t raw_current_val) {
	// Ignore invalid current value
	if (raw_current_val == INT16_MAX)
		return SampleStatus::Invalid;

	// Classify sample type
	auto result = SampleStatus::High;

	if (raw_current_val < CurrentThreshold) {
		// Low sample
		result = SampleStatus::Low;

		// We are now at a low step
		currently_high_ = false;

		// Add it to uncommitted_low_samples_
		uncommitted_low_samples_++;
	} else {
		// Detect rising edge?
		if (!currently_high_) {
			// We are now at the high step
			currently_high_ = true;

			auto now = Platform.micros();

			// Commit low samples
			if (now < last_rising_edge_t_ || (now - last_rising_edge_t_) > 500000) {
				// Too long since the last rising edge, reset
				low_samples_ = 0;
				high_samples_ = 0;

				// Also reset the histogram
				memset(&hist_, 0, sizeof(hist_));
				last_rising_edge_t_ = 0;
			} else {
				// Just a few samples, add them to the count
				low_samples_ += uncommitted_low_samples_;
			}

			uncommitted_low_samples_ = 0;

			// Record last rising edge as now
			if (last_rising_edge_t_ != 0 && now > last_rising_edge_t_) {
				// Frequency of the last period in .1 Hz
				auto f = 10000000 / static_cast<int32_t>(now - last_rising_edge_t_);

				// Idx in histogram
				auto idx = (f - HistogramFrequencyLow) / HistogramFrequencyStep;

				// If the idx is in range of the samples we're interested in
				if (idx >= 0 && idx < HistogramFrequencyBinCount) {
					hist_.counts[idx]++;
					hist_.total++;
				}
			}

			last_rising_edge_t_ = now;
		}

		// High sample
		high_samples_++;
	}

	return result;
}

bool Stats::update_stats() {
	// Reset statistics
	stat_freq_ = 0.f;
	stat_duty_cycle_ = 0.f;

	// Check we can compute duty cycle
	if (low_samples_ + high_samples_ < MinSampleCount)
		return false;

	// Check we can compute median frequency
	if (hist_.total < MinSampleCount)
		return false;

	// Compute duty cycle
	stat_duty_cycle_ = high_samples_ / static_cast<float>(high_samples_ + low_samples_) * 100.f;

	// Compute median frequency
	uint16_t tgt, i;
	for (tgt = hist_.total / 2, i = 0; tgt > 0 && hist_.counts[i] <= tgt; tgt -= hist_.counts[i], ++i)
		;

	stat_freq_ =
	    (HistogramFrequencyLow + (i * HistogramFrequencyStep) +
	     (hist_.counts[i] == 0
		  ? 0
		  : (HistogramFrequencyStep * ((hist_.total / 2) - tgt) / static_cast<float>(hist_.counts[i])))) /
	    10.f;

	return true;
}
