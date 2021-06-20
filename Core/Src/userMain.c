#include <stm32l4xx.h>
#include "userMain.h"

#include "stm32l4xx_hal.h"
#include <stdio.h>

#include <bmp280.h>
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

		if (delta > 10000) {
			isBlow = true;
		}

		if (delta < 800) {
			isBlow = false;
		}

		playSong_isBlow(isBlow);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, isBlow);

		printf("%8d,%8d\r\n", pressure, baseline);
		HAL_Delay(10);

	}

}
