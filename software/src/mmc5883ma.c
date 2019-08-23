/* compass-bricklet
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * mmc5883ma.c: Driver for MMC5883MA magnetic sensor
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "mmc5883ma.h"
#include "configs/config_mmc5883ma.h"

#include "bricklib2/os/coop_task.h"
#include "bricklib2/hal/i2c_fifo/i2c_fifo.h"
#include "bricklib2/hal/system_timer/system_timer.h"
#include "bricklib2/bootloader/bootloader.h"

#include "xmc_gpio.h"

#include <math.h>

MMC5883MA mmc5883ma;
CoopTask mmc5883ma_task;

void mmc5883ma_calibration_write(void) {
	uint32_t page[EEPROM_PAGE_SIZE/sizeof(uint32_t)];

	page[MMC5883_CALIBRATION_MAGIC_POS]      = MMC5883_CALIBRATION_MAGIC;
	page[MMC5883_CALIBRATION_OFFSET_POS + 0] = mmc5883ma.calibration_offset[0];
	page[MMC5883_CALIBRATION_OFFSET_POS + 1] = mmc5883ma.calibration_offset[1];
	page[MMC5883_CALIBRATION_OFFSET_POS + 2] = mmc5883ma.calibration_offset[2];
	page[MMC5883_CALIBRATION_GAIN_POS + 0]   = mmc5883ma.calibration_gain[0];
	page[MMC5883_CALIBRATION_GAIN_POS + 1]   = mmc5883ma.calibration_gain[1];
	page[MMC5883_CALIBRATION_GAIN_POS + 2]   = mmc5883ma.calibration_gain[2];

	bootloader_write_eeprom_page(MMC5883_CALIBRATION_PAGE, page);
}

void mmc5883ma_calibration_read(void) {
	uint32_t page[EEPROM_PAGE_SIZE/sizeof(uint32_t)];
	bootloader_read_eeprom_page(MMC5883_CALIBRATION_PAGE, page);

	// The magic number is not where it is supposed to be.
	// This is either our first startup or something went wrong.
	// We initialize the calibration data with sane default values.
	if(page[0] != MMC5883_CALIBRATION_MAGIC) {
		mmc5883ma.calibration_offset[0] = 0;
		mmc5883ma.calibration_offset[1] = 0;
		mmc5883ma.calibration_offset[2] = 0;
		mmc5883ma.calibration_gain[0] = 10000;
		mmc5883ma.calibration_gain[1] = 10000;
		mmc5883ma.calibration_gain[2] = 10000;
		mmc5883ma_calibration_write();
		return;
	}

	mmc5883ma.calibration_offset[0] = page[MMC5883_CALIBRATION_OFFSET_POS + 0];
	mmc5883ma.calibration_offset[1] = page[MMC5883_CALIBRATION_OFFSET_POS + 1];
	mmc5883ma.calibration_offset[2] = page[MMC5883_CALIBRATION_OFFSET_POS + 2];
	mmc5883ma.calibration_gain[0]   = page[MMC5883_CALIBRATION_GAIN_POS + 0];
	mmc5883ma.calibration_gain[1]   = page[MMC5883_CALIBRATION_GAIN_POS + 1];
	mmc5883ma.calibration_gain[2]   = page[MMC5883_CALIBRATION_GAIN_POS + 2];
}

// Takes about 20ms
void mmc5883ma_task_bridge_calibration(void) {
	uint16_t values[3];
	uint8_t value = 0;

	// SET + MEASURE
	value = MMC5883MA_CMD_CTRL0_SET;
	i2c_fifo_coop_write_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_CONTROL_0, 1, &value, true);
	coop_task_sleep_ms(2);
	value = MMC5883MA_CMD_CTRL0_TM_M;
	i2c_fifo_coop_write_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_CONTROL_0, 1, &value, true);

	while(!XMC_GPIO_GetInput(MMC5883MA_INT_PIN)) {
		coop_task_yield();
	}

	i2c_fifo_coop_read_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_XOUT_LOW, 6, (uint8_t *)values);
	value = 0b111;
	i2c_fifo_coop_write_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_STATUS, 1, &value, true);

	int16_t out1_x = values[0] - UINT16_MAX/2;
	int16_t out1_y = values[1] - UINT16_MAX/2;
	int16_t out1_z = values[2] - UINT16_MAX/2;

	// RESET + MEASURE
	value = MMC5883MA_CMD_CTRL0_RESET;
	i2c_fifo_coop_write_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_CONTROL_0, 1, &value, true);
	coop_task_sleep_ms(2);
	value = MMC5883MA_CMD_CTRL0_TM_M;
	i2c_fifo_coop_write_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_CONTROL_0, 1, &value, true);

	while(!XMC_GPIO_GetInput(MMC5883MA_INT_PIN)) {
		coop_task_yield();
	}

	i2c_fifo_coop_read_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_XOUT_LOW, 6, (uint8_t *)values);
	value = 0b111;
	i2c_fifo_coop_write_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_STATUS, 1, &value, true);

	int16_t out2_x = values[0] - UINT16_MAX/2;
	int16_t out2_y = values[1] - UINT16_MAX/2;
	int16_t out2_z = values[2] - UINT16_MAX/2;

	mmc5883ma.bridge_offset[0] = (out1_x + out2_x)/2;
	mmc5883ma.bridge_offset[1] = (out1_y + out2_y)/2;
	mmc5883ma.bridge_offset[2] = (out1_z + out2_z)/2;

	// SET + MEASURE
	value = MMC5883MA_CMD_CTRL0_SET;
	i2c_fifo_coop_write_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_CONTROL_0, 1, &value, true);
	coop_task_sleep_ms(2);
}

void mmc5883ma_task_tick(void) {
	uint32_t last_time = 0;
	uint8_t value = 0;
	coop_task_sleep_ms(10);

	value = MMC5883MA_CMD_CTRL2_INT_MEAS_DONE_EN;
	i2c_fifo_coop_write_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_CONTROL_2, 1, &value, true);
	value = MMC5883MA_CMD_CTRL1_BW_100;
	i2c_fifo_coop_write_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_CONTROL_1, 1, &value, true);

	while(true) {
		if(mmc5883ma.calibration_new) {
			mmc5883ma.calibration_new = false;
			mmc5883ma_calibration_write();
		}

		if(mmc5883ma.config_data_rate_new) {
			mmc5883ma.config_data_rate_new = false;
			value = mmc5883ma.config_data_rate & 0b11;
			i2c_fifo_coop_write_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_CONTROL_1, 1, &value, true);
		}

		if(system_timer_is_time_elapsed_ms(last_time, 1000)) {
			last_time = system_timer_get_ms();
			if(mmc5883ma.config_background_calibration) {
				mmc5883ma.bridge_calibration = true;
			}
		}

		if(mmc5883ma.bridge_calibration) {
			mmc5883ma.bridge_calibration = false;
			mmc5883ma_task_bridge_calibration();
		}

		value = MMC5883MA_CMD_CTRL0_TM_M;
		i2c_fifo_coop_write_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_CONTROL_0, 1, &value, true);

		while(!XMC_GPIO_GetInput(MMC5883MA_INT_PIN)) {
			coop_task_yield();
		}

		uint16_t values[3];
		i2c_fifo_coop_read_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_XOUT_LOW, 6, (uint8_t *)values);

		value = 0b111;
		i2c_fifo_coop_write_register(&mmc5883ma.i2c_fifo, MMC5883MA_REG_STATUS, 1, &value, true);
		for(uint8_t i = 0; i < MMC5883MA_AXIS_NUM; i++) {
			int32_t raw = (values[i] - UINT16_MAX/2 - mmc5883ma.bridge_offset[i])*10000/4096;
			mmc5883ma.magnetic_flux_density[i] = (raw - mmc5883ma.calibration_offset[i])*mmc5883ma.calibration_gain[i]/10000;
		}

		mmc5883ma.heading = ((int32_t)(atan2(mmc5883ma.magnetic_flux_density[1], mmc5883ma.magnetic_flux_density[0])*1800))/M_PI;
		if(mmc5883ma.heading < 0) {
			mmc5883ma.heading += 3600;
		}

		coop_task_yield();
	}
}

void mmc5883ma_tick(void) {
	coop_task_tick(&mmc5883ma_task);
}

void mmc5883ma_init_i2c(void) {
	mmc5883ma.i2c_fifo.baudrate         = MMC5883MA_I2C_BAUDRATE;
	mmc5883ma.i2c_fifo.address          = MMC5883MA_I2C_ADDRESS;
	mmc5883ma.i2c_fifo.i2c              = MMC5883MA_I2C;

	mmc5883ma.i2c_fifo.scl_port         = MMC5883MA_SCL_PORT;
	mmc5883ma.i2c_fifo.scl_pin          = MMC5883MA_SCL_PIN;
	mmc5883ma.i2c_fifo.scl_mode         = MMC5883MA_SCL_PIN_MODE;
	mmc5883ma.i2c_fifo.scl_input        = MMC5883MA_SCL_INPUT;
	mmc5883ma.i2c_fifo.scl_source       = MMC5883MA_SCL_SOURCE;
	mmc5883ma.i2c_fifo.scl_fifo_size    = MMC5883MA_SCL_FIFO_SIZE;
	mmc5883ma.i2c_fifo.scl_fifo_pointer = MMC5883MA_SCL_FIFO_POINTER;

	mmc5883ma.i2c_fifo.sda_port         = MMC5883MA_SDA_PORT;
	mmc5883ma.i2c_fifo.sda_pin          = MMC5883MA_SDA_PIN;
	mmc5883ma.i2c_fifo.sda_mode         = MMC5883MA_SDA_PIN_MODE;
	mmc5883ma.i2c_fifo.sda_input        = MMC5883MA_SDA_INPUT;
	mmc5883ma.i2c_fifo.sda_source       = MMC5883MA_SDA_SOURCE;
	mmc5883ma.i2c_fifo.sda_fifo_size    = MMC5883MA_SDA_FIFO_SIZE;
	mmc5883ma.i2c_fifo.sda_fifo_pointer = MMC5883MA_SDA_FIFO_POINTER;

	i2c_fifo_init(&mmc5883ma.i2c_fifo);
}

void mmc5883ma_init(void) {
	memset(&mmc5883ma, 0, sizeof(MMC5883MA));

	mmc5883ma_calibration_read();
	mmc5883ma_init_i2c();

	const XMC_GPIO_CONFIG_t config_input = {
		.mode                = XMC_GPIO_MODE_INPUT_PULL_DOWN,
		.input_hysteresis    = XMC_GPIO_INPUT_HYSTERESIS_STANDARD
	};
	XMC_GPIO_Init(MMC5883MA_INT_PIN, &config_input);

	coop_task_init(&mmc5883ma_task, mmc5883ma_task_tick);

	mmc5883ma.bridge_calibration = true;
}

int16_t mmc5883ma_get_heading(void) {
	return mmc5883ma.heading;
}
