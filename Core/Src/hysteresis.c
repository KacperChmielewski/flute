#include "hysteresis.h"

#include "stm32l4xx_hal.h"
#include <stdbool.h>

void hysteresis_init(struct hysteresis *hys, hysteresis_type activeTh,
		hysteresis_type releaseTh, bool isPositive) {
	hys->activeTh = activeTh;
	hys->releaseTh = releaseTh;
	hys->isPositive = isPositive;
	hys->isActive = false;
}

bool hysteresis_th(struct hysteresis *hys, hysteresis_type value) {
	if (hys->isActive) {
		if (value < hys->releaseTh) {
			hys->isActive = false;
		}
	} else {
		if (value > hys->activeTh) {
			hys->isActive = true;
		}
	}
	return hys->isActive;
}
