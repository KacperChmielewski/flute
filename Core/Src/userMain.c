#include <bmp280.h>
#include <stm32l4xx.h>
#include "userMain.h"

#include "stm32l4xx_hal.h"
#include <stdio.h>

extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;

extern UART_HandleTypeDef huart4;


int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&huart2, (uint8_t*) ptr, len, 500);
//	HAL_UART_Transmit_DMA(&huart2, (uint8_t*) ptr, len);
	return len;
}

/*!
 *  I2C ADDRESS/BITS/SETTINGS
 */
#define BMP280_ADDRESS 		(0xec) /**< The default I2C address for the sensor. */
#define BMP280_CHIPID 		(0x58) /**< Default chip ID. */

/** Oversampling rate for the sensor. */
enum sensor_sampling {
	/** No over-sampling. */
	SAMPLING_NONE = 0x00,
	/** 1x over-sampling. */
	SAMPLING_X1 = 0x01,
	/** 2x over-sampling. */
	SAMPLING_X2 = 0x02,
	/** 4x over-sampling. */
	SAMPLING_X4 = 0x03,
	/** 8x over-sampling. */
	SAMPLING_X8 = 0x04,
	/** 16x over-sampling. */
	SAMPLING_X16 = 0x05
};

/** Operating mode for the sensor. */
enum sensor_mode {
	/** Sleep mode. */
	MODE_SLEEP = 0x00,
	/** Forced mode. */
	MODE_FORCED = 0x01,
	/** Normal mode. */
	MODE_NORMAL = 0x03,
	/** Software reset. */
	MODE_SOFT_RESET_CODE = 0xB6
};

/** Filtering level for sensor data. */
enum sensor_filter {
	/** No filtering. */
	FILTER_OFF = 0x00,
	/** 2x filtering. */
	FILTER_X2 = 0x01,
	/** 4x filtering. */
	FILTER_X4 = 0x02,
	/** 8x filtering. */
	FILTER_X8 = 0x03,
	/** 16x filtering. */
	FILTER_X16 = 0x04
};

/** Standby duration in ms */
enum standby_duration {
	/** 1 ms standby. */
	STANDBY_MS_1 = 0x00,
	/** 62.5 ms standby. */
	STANDBY_MS_63 = 0x01,
	/** 125 ms standby. */
	STANDBY_MS_125 = 0x02,
	/** 250 ms standby. */
	STANDBY_MS_250 = 0x03,
	/** 500 ms standby. */
	STANDBY_MS_500 = 0x04,
	/** 1000 ms standby. */
	STANDBY_MS_1000 = 0x05,
	/** 2000 ms standby. */
	STANDBY_MS_2000 = 0x06,
	/** 4000 ms standby. */
	STANDBY_MS_4000 = 0x07
};

/*!
 * Registers available on the sensor.
 */
enum {
	BMP280_REGISTER_DIG_T1 = 0x88,
	BMP280_REGISTER_DIG_T2 = 0x8A,
	BMP280_REGISTER_DIG_T3 = 0x8C,
	BMP280_REGISTER_DIG_P1 = 0x8E,
	BMP280_REGISTER_DIG_P2 = 0x90,
	BMP280_REGISTER_DIG_P3 = 0x92,
	BMP280_REGISTER_DIG_P4 = 0x94,
	BMP280_REGISTER_DIG_P5 = 0x96,
	BMP280_REGISTER_DIG_P6 = 0x98,
	BMP280_REGISTER_DIG_P7 = 0x9A,
	BMP280_REGISTER_DIG_P8 = 0x9C,
	BMP280_REGISTER_DIG_P9 = 0x9E,
	BMP280_REGISTER_CHIPID = 0xD0,
	BMP280_REGISTER_VERSION = 0xD1,
	BMP280_REGISTER_SOFTRESET = 0xE0,
	BMP280_REGISTER_CAL26 = 0xE1, /**< R calibration = 0xE1-0xF0 */
	BMP280_REGISTER_STATUS = 0xF3,
	BMP280_REGISTER_CONTROL = 0xF4,
	BMP280_REGISTER_CONFIG = 0xF5,
	BMP280_REGISTER_PRESSUREDATA = 0xF7,
	BMP280_REGISTER_TEMPDATA = 0xFA,
};

struct BMP280_config {
	/** Inactive duration (standby time) in normal mode */
	uint8_t t_sb :3;
	/** Filter settings */
	uint8_t filter :3;
	/** Unused - don't set */
	uint8_t none :1;
	/** Enables 3-wire SPI */
	uint8_t spi3w_en :1;
};

/** Encapsulates trhe ctrl_meas register */
struct BMP280_ctrl_meas {
	/** Temperature oversampling. */
	uint8_t osrs_t :3;
	/** Pressure oversampling. */
	uint8_t osrs_p :3;
	/** Device mode */
	uint8_t mode :2;
};
//
//struct bmp280 {
//	I2C_HandleTypeDef* hi2c;
//	uint8_t addr;
//	bool isCalibrationDataReady;
//	uint8_t calibrationData[26];
//
//};
//
//bool bmp280_validateChipId(struct bmp280* b) {
//	uint8_t res;
//	uint8_t data[1];
//	res = HAL_I2C_Mem_Read(b, b->addr, BMP280_REGISTER_CHIPID, 1,
//			data, 1, 100);
//	printf("Chip ID %02x\r\n", data[0]);
//	return true;
//
//}
//
//void bmp280_init(struct bmp280* b) {
//	b->hi2c = &hi2c1;
//	b->addr = BMP280_ADDRESS;
//
//	b->isCalibrationDataReady = false;
//
//	bmp280_validateChipId(b);
//}

void doNote(uint8_t note) {
	uint8_t midiData[5];
	midiData[0] = 0x90;
	midiData[1] = note;
	midiData[2] = 0x7f;

	midiData[2] = 0x7f;
	HAL_UART_Transmit(&huart4, (uint8_t*) midiData, 3, 500);
	HAL_Delay(500);

	midiData[2] = 0x00;
	HAL_UART_Transmit(&huart4, (uint8_t*) midiData, 3, 500);
	HAL_Delay(500);

	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

}

void userMain() {

	while (1) {
		doNote(0x2a);
		doNote(0x2a + 2);
		doNote(0x2a + 4);
		doNote(0x2a + 5);
	}


	BMP280_HandleTypedef bmp280;

	int32_t temperature;
	uint32_t pressure;

	printf("START!\r\n");

	bmp280_init_default_params(&bmp280.params);
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = &hi2c1;
	bmp280.params.standby = BMP280_STANDBY_05;




	while (!bmp280_init(&bmp280, &bmp280.params)) {
		printf("BMP280 initialization failed\r\n");
		//HAL_UART_Transmit(&huart1, Data, size, 1000);
		HAL_Delay(200);
	}
	bool bme280p = bmp280.id == BME280_CHIP_ID;
	printf("BMP280: found %s\r\n", bme280p ? "BME280" : "BMP280");
	//HAL_UART_Transmit(&huart1, Data, size, 1000);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		HAL_Delay(10);
		while (!bmp280_read_fixed(&bmp280, &temperature, &pressure, NULL)) {
			printf("Temperature/pressure reading failed\n");
			//HAL_UART_Transmit(&huart1, Data, size, 1000);
			HAL_Delay(200);
		}

		printf("%10d,%5d\r\n", pressure, temperature);
	}
//
//	BMP280_HandleTypedef bmp280;
//
//	float pressure, temperature, humidity;
//
//	uint16_t size;
//	uint8_t Data[256];
//
//	bmp280_init_default_params(&bmp280.params);
//	bmp280.addr = BMP280_I2C_ADDRESS_0;
//	bmp280.i2c = &hi2c1;
//
//	while (!bmp280_init(&bmp280, &bmp280.params)) {
//		printf("BMP280 initialization failed\r\n");
//		HAL_Delay(2000);
//	}
//	bool bme280p = bmp280.id == BME280_CHIP_ID;
//	printf("BMP280: found %s\r\n", bme280p ? "BME280" : "BMP280");
//
//	while (1) {
//		HAL_Delay(100);
//		while (!bmp280_read_float(&bmp280, &temperature, &pressure, &humidity)) {
//			printf("Temperature/pressure reading failed\r\n");
//			HAL_Delay(2000);
//		}
//
//		printf("Pressure: %.2f Pa, Temperature: %.2f C", pressure, temperature);
//		if (bme280p) {
//			printf(", Humidity: %.2f\r\n", humidity);
//		}
//
//		else {
//			printf("\r\n");
//		}
//		HAL_Delay(2000);
//	}
//
//	return;

//
//	  Serial.println(F("BMP280 Sensor event test"));
//
//	  if (!bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
//	    //  if (!bmp.begin()) {
//	    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
//	                     "try a different address!"));
//	    while (1) delay(10);
//	  }
//	  Serial.println("Costam");
//
//
//	  /* Default settings from datasheet. */
//	  //  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
//	  //                  Adafruit_BMP280::SAMPLING_NONE,     /* Temp. oversampling */
//	  //                  Adafruit_BMP280::SAMPLING_NONE,    /* Pressure oversampling */
//	  //                  Adafruit_BMP280::FILTER_OFF,      /* Filtering. */
//	  //                  Adafruit_BMP280::STANDBY_MS_1); /* Standby time. */
//	  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
//	                  Adafruit_BMP280::SAMPLING_X16,
//	                  Adafruit_BMP280::SAMPLING_X16,
//	                  Adafruit_BMP280::FILTER_OFF,
//	                  Adafruit_BMP280::STANDBY_MS_1);
//	//                  Serial.println("Costam2");
//
//	struct bmp280 b = {};
//
//	bmp280_init(&b);
//
//	while(1) {
//
//	}

	uint8_t res = 0;
	uint8_t tmp[8];

	printf("START!\r\n");

	printf("res : %02x\r\n", res);
	printf("tmp: %02x\r\n", tmp[0]);
	struct BMP280_config b_config = { 0 };
	struct BMP280_ctrl_meas b_ctrl_meas = { 0 };
//
//	//	b_ctrl_meas.mode = mode;
//	//	b_ctrl_meas.osrs_t = tempSampling;
//	//	b_ctrl_meas.osrs_p = pressSampling;
//	//
//	//	b_config.filter = filter;
//	//	b_config.t_sb = duration;

	b_ctrl_meas.mode = MODE_NORMAL;
	b_ctrl_meas.osrs_t = SAMPLING_X16;
	b_ctrl_meas.osrs_p = SAMPLING_X16;

	b_config.filter = FILTER_OFF;
	b_config.t_sb = STANDBY_MS_1;
	b_config.spi3w_en = 0;
	b_config.none = 0;

	printf("CONF: %02x\r\n", b_config);
	printf("CTRL: %02x\r\n", b_ctrl_meas);

	HAL_Delay(1000);

	uint8_t elo;
	elo = 0x00;
	res = HAL_I2C_Mem_Write(&hi2c1, BMP280_ADDRESS, BMP280_REGISTER_CONFIG, 1,
			&b_config, 1, 100);
	printf("res : %02x\r\n", res);
	HAL_Delay(1000);

	elo = 0xB7;
	res = HAL_I2C_Mem_Write(&hi2c1, BMP280_ADDRESS, BMP280_REGISTER_CONTROL, 1,
			&elo, 1, 100);
	printf("res : %02x\r\n", res);
	HAL_Delay(1000);

	//	res = HAL_I2C_Mem_Write(&hi2c1, BMP280_ADDRESS, BMP280_REGISTER_CONTROL, 1,
	//			&elo, 1, 100);
	//	printf("res : %02x\r\n", res);

	////
//////	tmp[0] = (uint8_t) b_ctrl_meas;
//////	tmp[1] = (uint8_t) b_config;
//	elo = 0;
//	res = HAL_I2C_Mem_Write(&hi2c1, BMP280_ADDRESS, BMP280_REGISTER_CONTROL, 1,
//			&elo, 1, 100);
//	printf("res : %02x\r\n", res);
//
//	HAL_Delay(1000);
//
//	res = HAL_I2C_Mem_Write(&hi2c1, BMP280_ADDRESS, BMP280_REGISTER_CONFIG, 1,
//			&elo, 1, 100);
//	printf("res : %02x\r\n", res);
//
//	HAL_Delay(1000);

//	res = HAL_I2C_Mem_Read(&hi2c1, 0xec, 0xd0, 1, tmp, 1, 100);
//
//	BMP280_Write8(BMP280_CONFIG, (((standby_time & 0x7) << 5) | ((filter & 0x7) << 2)) & 0xFC);

	uint32_t pressureValue;
	uint32_t t = 0;
	while (1) {

		res = HAL_I2C_Mem_Read(&hi2c1, BMP280_ADDRESS, BMP280_REGISTER_STATUS,
				1, tmp, 1, 100);
		res = HAL_I2C_Mem_Read(&hi2c1, BMP280_ADDRESS, 0xF7, 1, tmp, 8, 100);

		pressureValue = (tmp[0] << 16) + (tmp[1] << 8) + (tmp[2] >> 4);
		printf("test %d\r\n", (int) t++);
		printf("%02x%02x%02x\r\n", tmp[0], tmp[1], tmp[2]);
		printf("p: %5d\r\n", pressureValue);

		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

		HAL_Delay(100);
	}

}
