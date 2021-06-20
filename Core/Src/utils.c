#include "stm32l4xx_hal.h"

extern UART_HandleTypeDef huart2;

int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&huart2, (uint8_t*) ptr, len, 500);
//	HAL_UART_Transmit_DMA(&huart2, (uint8_t*) ptr, len);
	return len;
}
