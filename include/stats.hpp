#ifndef _STATS_HPP_
#define _STATS_HPP_

#include <stddef.h>
#include <stdint.h>

/// Threshold value for detecting current, in 10ths of mA
const int16_t CurrentThreshold = 50; // 5 mA

struct Sample {
	unsigned long t;
	uint16_t i;
};

const size_t SAMPLE_COUNT = 100;

typedef Sample SampleBuffer[SAMPLE_COUNT];

enum class SampleStatus {
	Invalid,
	Valid,
	Zero,
};

class Stats {
	SampleBuffer samples;
	size_t valid_samples;
	size_t samples_idx;
	size_t zero_samples;

	unsigned long falling_edges[SAMPLE_COUNT];
	unsigned long rising_edges[SAMPLE_COUNT];

	float stat_freq;
	float stat_duty_cycle;

      public:
	Stats();

	inline float get_freq() const { return stat_freq; }
	inline float get_duty_cycle() const { return stat_duty_cycle; }

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
