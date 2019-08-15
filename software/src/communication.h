/* compass-bricklet
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.h: TFP protocol message handling
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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/bootloader/bootloader.h"

// Default functions
BootloaderHandleMessageResponse handle_message(const void *data, void *response);
void communication_tick(void);
void communication_init(void);

// Constants
#define COMPASS_THRESHOLD_OPTION_OFF 'x'
#define COMPASS_THRESHOLD_OPTION_OUTSIDE 'o'
#define COMPASS_THRESHOLD_OPTION_INSIDE 'i'
#define COMPASS_THRESHOLD_OPTION_SMALLER '<'
#define COMPASS_THRESHOLD_OPTION_GREATER '>'

#define COMPASS_DATA_RATE_100HZ 0
#define COMPASS_DATA_RATE_200HZ 1
#define COMPASS_DATA_RATE_400HZ 2
#define COMPASS_DATA_RATE_600HZ 3

#define COMPASS_BOOTLOADER_MODE_BOOTLOADER 0
#define COMPASS_BOOTLOADER_MODE_FIRMWARE 1
#define COMPASS_BOOTLOADER_MODE_BOOTLOADER_WAIT_FOR_REBOOT 2
#define COMPASS_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_REBOOT 3
#define COMPASS_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_ERASE_AND_REBOOT 4

#define COMPASS_BOOTLOADER_STATUS_OK 0
#define COMPASS_BOOTLOADER_STATUS_INVALID_MODE 1
#define COMPASS_BOOTLOADER_STATUS_NO_CHANGE 2
#define COMPASS_BOOTLOADER_STATUS_ENTRY_FUNCTION_NOT_PRESENT 3
#define COMPASS_BOOTLOADER_STATUS_DEVICE_IDENTIFIER_INCORRECT 4
#define COMPASS_BOOTLOADER_STATUS_CRC_MISMATCH 5

#define COMPASS_STATUS_LED_CONFIG_OFF 0
#define COMPASS_STATUS_LED_CONFIG_ON 1
#define COMPASS_STATUS_LED_CONFIG_SHOW_HEARTBEAT 2
#define COMPASS_STATUS_LED_CONFIG_SHOW_STATUS 3

// Function and callback IDs and structs
#define FID_GET_HEADING 1
#define FID_SET_HEADING_CALLBACK_CONFIGURATION 2
#define FID_GET_HEADING_CALLBACK_CONFIGURATION 3
#define FID_GET_MAGNETIC_FLUX_DENSITY 5
#define FID_SET_MAGNETIC_FLUX_DENSITY_CALLBACK_CONFIGURATION 6
#define FID_GET_MAGNETIC_FLUX_DENSITY_CALLBACK_CONFIGURATION 7
#define FID_SET_CONFIGURATION 9
#define FID_GET_CONFIGURATION 10
#define FID_SET_CALIBRATION 11
#define FID_GET_CALIBRATION 12

#define FID_CALLBACK_HEADING 4
#define FID_CALLBACK_MAGNETIC_FLUX_DENSITY 8

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetMagneticFluxDensity;

typedef struct {
	TFPMessageHeader header;
	int32_t x;
	int32_t y;
	int32_t z;
} __attribute__((__packed__)) GetMagneticFluxDensity_Response;

typedef struct {
	TFPMessageHeader header;
	uint32_t period;
	bool value_has_to_change;
} __attribute__((__packed__)) SetMagneticFluxDensityCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetMagneticFluxDensityCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint32_t period;
	bool value_has_to_change;
} __attribute__((__packed__)) GetMagneticFluxDensityCallbackConfiguration_Response;

typedef struct {
	TFPMessageHeader header;
	int32_t x;
	int32_t y;
	int32_t z;
} __attribute__((__packed__)) MagneticFluxDensity_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t data_rate;
	bool background_calibration;
} __attribute__((__packed__)) SetConfiguration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t data_rate;
	bool background_calibration;
} __attribute__((__packed__)) GetConfiguration_Response;

typedef struct {
	TFPMessageHeader header;
	int16_t offset[3];
	int16_t gain[3];
} __attribute__((__packed__)) SetCalibration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetCalibration;

typedef struct {
	TFPMessageHeader header;
	int16_t offset[3];
	int16_t gain[3];
} __attribute__((__packed__)) GetCalibration_Response;


// Function prototypes
BootloaderHandleMessageResponse get_magnetic_flux_density(const GetMagneticFluxDensity *data, GetMagneticFluxDensity_Response *response);
BootloaderHandleMessageResponse set_magnetic_flux_density_callback_configuration(const SetMagneticFluxDensityCallbackConfiguration *data);
BootloaderHandleMessageResponse get_magnetic_flux_density_callback_configuration(const GetMagneticFluxDensityCallbackConfiguration *data, GetMagneticFluxDensityCallbackConfiguration_Response *response);
BootloaderHandleMessageResponse set_configuration(const SetConfiguration *data);
BootloaderHandleMessageResponse get_configuration(const GetConfiguration *data, GetConfiguration_Response *response);
BootloaderHandleMessageResponse set_calibration(const SetCalibration *data);
BootloaderHandleMessageResponse get_calibration(const GetCalibration *data, GetCalibration_Response *response);

// Callbacks
bool handle_heading_callback(void);
bool handle_magnetic_flux_density_callback(void);

#define COMMUNICATION_CALLBACK_TICK_WAIT_MS 1
#define COMMUNICATION_CALLBACK_HANDLER_NUM 2
#define COMMUNICATION_CALLBACK_LIST_INIT \
	handle_heading_callback, \
	handle_magnetic_flux_density_callback, \


#endif
