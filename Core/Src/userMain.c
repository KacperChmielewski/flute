#include <stm32l4xx.h>
#include "userMain.h"

#include "stm32l4xx_hal.h"
#include <stdio.h>

#include <bmp280.h>
#include <hysteresis.h>
#include <midi.h>
#include <playSong.h>
#include <signalProcessing_baseline.h>

extern I2C_HandleTypeDef hi2c1;

extern UART_HandleTypeDef huart4;

struct midi_handle myMidi = { };

struct midiMachine {
	uint8_t isNotePlayed;
	uint8_t lastNote;
};

struct midiMachine machine = { };

void midiMachine_relase() {
	if (machine.isNotePlayed) {
		midi_doNote(&myMidi, machine.lastNote, 0);
		machine.isNotePlayed = 0;
	}
}

void midiMachine_play(uint8_t note) {
	midiMachine_relase();
	midi_doNote(&myMidi, note, 0x7f);
	machine.isNotePlayed = 1;
	machine.lastNote = note;
}

/* *** */

struct {
	bool isActive;
	uint8_t lastNote;
} myFlute;

bool flute_buttonsToNote(uint8_t buttons, uint8_t *noteOut) {
	*noteOut = 0;
//	switch (buttons) {
//	case b11111111:
//		*noteOut = MIDI_NOTE_C4;
//		break;
//	case b01111111:
//		*noteOut = MIDI_NOTE_D4;
//		break;
//	case b00111111:
//		*noteOut = MIDI_NOTE_E4;
//		break;
//	case b11011111:
//		*noteOut = MIDI_NOTE_F4;
//		break;
//	case b00001111:
//		*noteOut = MIDI_NOTE_G4;
//		break;
//	case b00000111:
//		*noteOut = MIDI_NOTE_A4;
//		break;
//	case b00000011:
//		*noteOut = MIDI_NOTE_H4;
//		break;
//	case b00000101:
//		*noteOut = MIDI_NOTE_C5;
//		break;
//	}

	if (*noteOut != 0) {
		return true;
	} else {
		return false;
	}
}

void flute_play(uint8_t note) {
	midiMachine_play(note);
	myFlute.lastNote = note;
	myFlute.isActive = true;
}

void flute_feed(bool isBlow, uint8_t buttons) {
	if (isBlow == false) {
		myFlute.isActive = false;
		midiMachine_relase();
		return;
	}

	uint8_t currentNote;

	if (flute_buttonsToNote(buttons, &currentNote) == false) {
		return;
	}

	if (myFlute.isActive == false) {
		flute_play(currentNote);
		return;
	}

	if (myFlute.lastNote != currentNote) {
		flute_play(currentNote);
	}

}

/* *** */

void userMain() {
	BMP280_HandleTypedef bmp280;

//	printf("START!\r\n");

// BMP init
	int32_t temperature;
	uint32_t pressure;
	bmp280_init_default_params(&bmp280.params);
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = &hi2c1;
	bmp280.params.standby = BMP280_STANDBY_05;
	while (!bmp280_init(&bmp280, &bmp280.params)) {
		printf("BMP280 initialization failed\r\n");
		HAL_Delay(200);
	}

	// MIDI init
	myMidi.huart = &huart4;

	// dataProcessing pressure init
	baseline_Reset();
	uint32_t baseline;
	uint32_t i = 0;
	int32_t delta;
	bool isBlow = false;

	struct hysteresis blowTh;

	hysteresis_init(&blowTh, 10000, 3000, true);

	while (1) {
		i++;

		while (!bmp280_read_fixed(&bmp280, &temperature, &pressure, NULL)) {
			printf("Temperature/pressure reading failed\n");
			HAL_Delay(200);
		}
		if (i < 5) {
			baseline = pressure;

		} else {
			baseline = baseline_Calc(pressure, isBlow);
		}
		delta = pressure - baseline;
		isBlow = hysteresis_th(&blowTh, delta);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, isBlow);

		playSong_isBlow(isBlow);

		printf("%8d,%8d\r\n", pressure, baseline);
		HAL_Delay(10);
	}

}
