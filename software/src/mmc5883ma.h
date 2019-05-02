/* compass-bricklet
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * mmc5883ma.h: Driver for MMC5883MA magnetic sensor
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

#ifndef MMC5883MA_H
#define MMC5883MA_H

#include <stdint.h>
#include <stdbool.h>
#include "bricklib2/hal/i2c_fifo/i2c_fifo.h"

#define MMC5883MA_AXIS_NUM 3

typedef struct {
    int32_t magnetic_flux_density[MMC5883MA_AXIS_NUM];

    int16_t heading;

    bool bridge_calibration;

    int16_t bridge_offset[MMC5883MA_AXIS_NUM];

    bool calibration_new;
    int16_t calibration_offset[MMC5883MA_AXIS_NUM];
    int16_t calibration_multiplier[MMC5883MA_AXIS_NUM];

    uint32_t mfd_period;
    bool mfd_value_has_to_change;
    
    bool config_data_rate_new;
    uint8_t config_data_rate;
    bool config_background_calibration;

	I2CFifo i2c_fifo;
} MMC5883MA;

extern MMC5883MA mmc5883ma;

void mmc5883ma_tick(void);
void mmc5883ma_init(void);
int16_t mmc5883ma_get_heading(void);

#define MMC5883MA_REG_XOUT_LOW      0x00
#define MMC5883MA_REG_XOUT_HIGH     0x01
#define MMC5883MA_REG_YOUT_LOW      0x02
#define MMC5883MA_REG_YOUT_HIGH     0x03
#define MMC5883MA_REG_ZOUT_LOW      0x04
#define MMC5883MA_REG_ZOUT_HIGH     0x05
#define MMC5883MA_REG_TEMPERATURE   0x06
#define MMC5883MA_REG_STATUS        0x07
#define MMC5883MA_REG_CONTROL_0     0x08
#define MMC5883MA_REG_CONTROL_1     0x09
#define MMC5883MA_REG_CONTROL_2     0x0A
#define MMC5883MA_REG_X_THRESHOLD   0x0B
#define MMC5883MA_REG_Y_THRESHOLD   0x0C
#define MMC5883MA_REG_Z_THRESHOLD   0x0D
#define MMC5883MA_REG_PRODUCT_ID    0x2F

#define MMC5883MA_CMD_CTRL0_TM_M      (1 << 0)
#define MMC5883MA_CMD_CTRL0_TM_T      (1 << 1)
#define MMC5883MA_CMD_CTRL0_START_MDT (1 << 2)
#define MMC5883MA_CMD_CTRL0_SET       (1 << 3)
#define MMC5883MA_CMD_CTRL0_RESET     (1 << 4)
#define MMC5883MA_CMD_CTRL0_OTP_READ  (1 << 6)

#define MMC5883MA_CMD_CTRL1_BW_100    (0b00 << 0)
#define MMC5883MA_CMD_CTRL1_BW_200    (0b01 << 0)
#define MMC5883MA_CMD_CTRL1_BW_400    (0b10 << 0)
#define MMC5883MA_CMD_CTRL1_BW_600    (0b11 << 0)
#define MMC5883MA_CMD_CTRL1_X_INHIBIT (1 << 2)
#define MMC5883MA_CMD_CTRL1_Y_INHIBIT (1 << 3)
#define MMC5883MA_CMD_CTRL1_Z_INHIBIT (1 << 4)
#define MMC5883MA_CMD_CTRL1_SW_RST    (1 << 7)

#define MMC5883MA_CMD_CTRL2_FREQ_OFF         (0b0000 << 0) // assumes BW[1:0] == 0, off = motion detector off
#define MMC5883MA_CMD_CTRL2_INT_MDT_EN       (1 << 5)
#define MMC5883MA_CMD_CTRL2_INT_MEAS_DONE_EN (1 << 6)


#endif