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

MMC5883MA mmc5883ma;
CoopTask mmc5883ma_task;

void mmc5883ma_task_tick(void) {
    while(true) {
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

	mmc5883ma_init_i2c();

	const XMC_GPIO_CONFIG_t config_input = {
		.mode                = XMC_GPIO_MODE_INPUT_PULL_UP,
		.input_hysteresis    = XMC_GPIO_INPUT_HYSTERESIS_STANDARD
	};
	XMC_GPIO_Init(MMC5883MA_INT_PIN, &config_input);

	coop_task_init(&mmc5883ma_task, mmc5883ma_task_tick);
}