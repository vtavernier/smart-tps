#ifndef _STATS_HPP_
#define _STATS_HPP_

#include <stddef.h>
#include <stdint.h>

/// Threshold value for detecting high samples, in 10ths of mA
const int16_t CurrentThreshold = 1000; // 100 mA

/// Lower bound for histogram
const int16_t HistogramFrequencyLow = 0; // 0Hz

/// Higher bound for histogram
const int16_t HistogramFrequencyHigh = 2000; // 200Hz

/// Histogram bin step
const int16_t HistogramFrequencyStep = 5; // .5Hz

/// Histogram bin count
const int16_t HistogramFrequencyBinCount =
    (HistogramFrequencyHigh - HistogramFrequencyLow) / HistogramFrequencyStep + 1;

/// Higher bound of uncommitted low samples: 10 periods of the max frequency at the typical sample rate
const uint16_t MaxUncomittedLowSamples = (10 * 1600 / (HistogramFrequencyHigh / 10));

/// Histogram data structure
typedef struct {
	/// Sample counts
	uint8_t counts[HistogramFrequencyBinCount];
	/// Total count
	uint16_t total;
} Histogram;

/// Minimum number of samples needed to compute stats
const uint16_t MinSampleCount = 25;

enum class SampleStatus {
	Invalid,
	High,
	Low,
};

class Stats {
	/// Frequency histogram
	Histogram hist_;

	/// Last rising edge time
	unsigned long last_rising_edge_t_;
	/// Was the last sample a high or low sample
	bool currently_high_;

	/// Number of low samples
	uint16_t low_samples_;
	/// Number of high samples
	uint16_t high_samples_;
	/// Number of uncommitted low samples
	uint16_t uncommitted_low_samples_;

	float stat_freq_;
	float stat_duty_cycle_;

      public:
	Stats();

	inline float get_freq() const { return stat_freq_; }
	inline float get_duty_cycle() const { return stat_duty_cycle_; }

	void reset();

	/**
	 * @brief Add a new sample to the accumulator
	 *
	 * @param raw_current_val Current value returned by the sensor
	 *
	 * @return the state of the given sample
	 */
	SampleStatus add_sample(int16_t raw_current_val);

	/**
	 * @brief Compute frequency and duty cycle information from stored samples
	 *
	 * @return true if non-zero results were computed, false otherwise
	 */
	bool update_stats();
};

#endif /* _STATS_HPP_ */
