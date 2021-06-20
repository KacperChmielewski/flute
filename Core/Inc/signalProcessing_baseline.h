#ifndef INC_SIGNALPROCESSING_BASELINE_H_
#define INC_SIGNALPROCESSING_BASELINE_H_

#include <stdint.h>

uint32_t baseline_Calc(uint32_t sampleIn, uint8_t isTouched);
void baseline_Reset();

#endif /* INC_SIGNALPROCESSING_BASELINE_H_ */
