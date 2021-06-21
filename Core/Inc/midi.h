#ifndef INC_MIDI_H_
#define INC_MIDI_H_

#include "stm32l4xx_hal.h"

struct midi_handle {
	struct UART_HandleTypeDef *huart;
};

enum midi_note {
	MIDI_NOTE_C4 = 0x24 + 24,
	MIDI_NOTE_D4 = MIDI_NOTE_C4 + 2,
	MIDI_NOTE_E4 = MIDI_NOTE_C4 + 4,
	MIDI_NOTE_F4 = MIDI_NOTE_C4 + 5,
	MIDI_NOTE_G4 = MIDI_NOTE_C4 + 7,
	MIDI_NOTE_A4 = MIDI_NOTE_C4 + 9,
	MIDI_NOTE_H4 = MIDI_NOTE_C4 + 11,
	MIDI_NOTE_C5 = MIDI_NOTE_C4 + 12,
};

void midi_doNote(struct midi_handle *midi, uint8_t note, uint8_t vel);

//11111111 C
//01111111 D
//00111111 E
//11011111 F
//00001111 G
//00000111 A
//00000011 H
//00000101 C

#endif /* INC_MIDI_H_ */
