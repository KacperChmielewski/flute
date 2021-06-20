#ifndef INC_HYSTERESIS_H_
#define INC_HYSTERESIS_H_

#include "stm32l4xx_hal.h"
#include <stdbool.h>

typedef int32_t hysteresis_type;

struct hysteresis {
	hysteresis_type activeTh;
	hysteresis_type releaseTh;
	bool isPositive;
	bool isActive;
};

void hysteresis_init(struct hysteresis *hys, hysteresis_type activeTh,
		hysteresis_type releaseTh, bool isPositive);
bool hysteresis_th(struct hysteresis *hys, hysteresis_type value);

#endif /* INC_HYSTERESIS_H_ */
