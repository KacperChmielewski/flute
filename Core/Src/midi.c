#include "midi.h"

#include "stm32l4xx_hal.h"



void midi_doNote(struct midi_handle *midi, uint8_t note, uint8_t vel) {
	uint8_t midiData[5];
	midiData[0] = 0x90;
	midiData[1] = note;
	midiData[2] = vel;
	HAL_UART_Transmit(midi->huart, (uint8_t*) midiData, 3, 500);
}
