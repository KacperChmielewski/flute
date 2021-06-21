#include <stm32l4xx.h>
#include "userMain.h"

#include "stm32l4xx_hal.h"
#include <stdio.h>

#include <bmp280.h>
#include <hysteresis.h>
#include <midi.h>
#include <playSong.h>
#include <signalProcessing_baseline.h>

#include <tsl_linrot.h>
#include <tsl_touchkey.h>
#include <tsl_user.h>

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
	switch (buttons) {
	case 0b11111111:
		*noteOut = MIDI_NOTE_C4;
		break;
	case 0b01111111:
		*noteOut = MIDI_NOTE_D4;
		break;
	case 0b00111111:
		*noteOut = MIDI_NOTE_E4;
		break;
	case 0b11011111:
		*noteOut = MIDI_NOTE_F4;
		break;
	case 0b00001111:
		*noteOut = MIDI_NOTE_G4;
		break;
	case 0b00000111:
		*noteOut = MIDI_NOTE_A4;
		break;
	case 0b00000011:
		*noteOut = MIDI_NOTE_H4;
		break;
	case 0b00000101:
		*noteOut = MIDI_NOTE_C5;
		break;
	}

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
	printf("flut: %d %02x\r\n", isBlow, buttons);
	if (isBlow == false) {
		myFlute.isActive = false;
		midiMachine_relase();
		return;
	}

	uint8_t currentNote;

	if (flute_buttonsToNote(buttons, &currentNote) == false) {
		printf("invalid note\r\n");
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

void readKeys(uint16_t *holes) {

	while (TSL_USER_STATUS_BUSY == tsl_user_Exec()) {
		HAL_Delay(1);
	}

#if 0
	printf("%3d %3d %3d %3d %3d %3d %3d %3d\r\n", holes[0], holes[1], holes[2],
			holes[3], holes[4], holes[5], holes[6], holes[7]);
#endif

	holes[0] = 500 - MyTKeys[0].p_ChD->Meas;
	holes[1] = 500 - MyTKeys[3].p_ChD->Meas;
	holes[2] = 500 - MyTKeys[6].p_ChD->Meas;
	holes[3] = 500 - MyTKeys[2].p_ChD->Meas;
	holes[4] = 500 - MyTKeys[5].p_ChD->Meas;
	holes[5] = 500 - MyTKeys[7].p_ChD->Meas;
	holes[6] = 500 - MyTKeys[1].p_ChD->Meas;
	holes[7] = 500 - MyTKeys[4].p_ChD->Meas;

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

	// init holes
	uint16_t myHoles[8];
	struct hysteresis holeHys[8] = {};

	for (int i = 0; i < 8; ++i) {
		hysteresis_init(&holeHys[i], 300, 280, true);
	}
	hysteresis_init(&holeHys[6], 499, 495, true);
	hysteresis_init(&holeHys[7], 499, 495, true);

	uint8_t temp;

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

//		playSong_isBlow(isBlow);

//		printf("%8d,%8d\r\n", pressure, baseline);
		readKeys(myHoles);

		uint8_t buttons = 0;
		for (int i = 0; i < 8; ++i) {
			temp = hysteresis_th(&holeHys[i], myHoles[i]);
//			printf("%d %d\r\n", i, temp);
//			printf("%d", temp);

			if(temp) {
				buttons |= 1 << (7-i);
			}

		}
//		printf("\r\n");

//		printf("%02x\r\n", buttons);
		buttons += 3;
		flute_feed(isBlow, buttons);

//		HAL_Delay(100);
	}

}
