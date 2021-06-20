#include "signalProcessing_baseline.h"

#include <stdint.h>

#define REFRESH_FREQUENCY	77
#define REFRESH_BUFFER_SIZE	16

static uint32_t baseline = 0;
static n = 0;

uint32_t baseline_Calc(uint32_t sampleIn, uint8_t isTouched) {

	// baseline init
	if (baseline == 0) {
		baseline = sampleIn;
	}

	if (isTouched) {
		n = 0;
		return baseline;
	}

//	if (sampleIn < baseline) {
//		n = 0;
//		baseline = sampleIn;
//	}

	if (n++ < REFRESH_FREQUENCY/2) {
		return baseline;
	}
	n = 0;
	baseline = (uint32_t) ((uint32_t) (REFRESH_BUFFER_SIZE - 1) * baseline
			+ sampleIn) / REFRESH_BUFFER_SIZE;

	return baseline;
}

void baseline_Reset() {
	baseline = 0;
	n = 0;
}
