/* compass-bricklet
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.c: TFP protocol message handling
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

#include "communication.h"

#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/protocols/tfp/tfp.h"

#include "mmc5883ma.h"

#define CALLBACK_VALUE_TYPE CALLBACK_VALUE_TYPE_INT16
#include "bricklib2/utility/callback_value.h"
CallbackValue_int16_t callback_value_heading;


BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_GET_HEADING: return get_callback_value_int16_t(message, response, &callback_value_heading);
		case FID_SET_HEADING_CALLBACK_CONFIGURATION: return set_callback_value_callback_configuration_int16_t(message, &callback_value_heading);
		case FID_GET_HEADING_CALLBACK_CONFIGURATION: return get_callback_value_callback_configuration_int16_t(message, response, &callback_value_heading);
		case FID_GET_MAGNETIC_FLUX_DENSITY: return get_magnetic_flux_density(message, response);
		case FID_SET_MAGNETIC_FLUX_DENSITY_CALLBACK_CONFIGURATION: return set_magnetic_flux_density_callback_configuration(message);
		case FID_GET_MAGNETIC_FLUX_DENSITY_CALLBACK_CONFIGURATION: return get_magnetic_flux_density_callback_configuration(message, response);
		case FID_SET_CONFIGURATION: return set_configuration(message);
		case FID_GET_CONFIGURATION: return get_configuration(message, response);
		case FID_SET_CALIBRATION: return set_calibration(message);
		case FID_GET_CALIBRATION: return get_calibration(message, response);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}


BootloaderHandleMessageResponse get_magnetic_flux_density(const GetMagneticFluxDensity *data, GetMagneticFluxDensity_Response *response) {
	response->header.length = sizeof(GetMagneticFluxDensity_Response);
	response->x             = mmc5883ma.magnetic_flux_density[0];
	response->y             = mmc5883ma.magnetic_flux_density[1];
	response->z             = mmc5883ma.magnetic_flux_density[2];

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_magnetic_flux_density_callback_configuration(const SetMagneticFluxDensityCallbackConfiguration *data) {
	mmc5883ma.mfd_period              = data->period;
	mmc5883ma.mfd_value_has_to_change = data->value_has_to_change;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_magnetic_flux_density_callback_configuration(const GetMagneticFluxDensityCallbackConfiguration *data, GetMagneticFluxDensityCallbackConfiguration_Response *response) {
	response->header.length       = sizeof(GetMagneticFluxDensityCallbackConfiguration_Response);
	response->period              = mmc5883ma.mfd_period;
	response->value_has_to_change = mmc5883ma.mfd_value_has_to_change;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_configuration(const SetConfiguration *data) {
	if(data->data_rate > COMPASS_DATA_RATE_600HZ) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	mmc5883ma.config_data_rate              = data->data_rate;
	mmc5883ma.config_background_calibration = data->background_calibration;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_configuration(const GetConfiguration *data, GetConfiguration_Response *response) {
	response->header.length          = sizeof(GetConfiguration_Response);
	response->data_rate              = mmc5883ma.config_data_rate;
	response->background_calibration = mmc5883ma.config_background_calibration;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_calibration(const SetCalibration *data) {
	for(uint8_t i = 0; i < MMC5883MA_AXIS_NUM; i++) {
		mmc5883ma.calibration_offset[i]     = data->offset[i];
		mmc5883ma.calibration_multiplier[i] = data->gain[i];
	}

	mmc5883ma.calibration_new = true;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_calibration(const GetCalibration *data, GetCalibration_Response *response) {
	response->header.length = sizeof(GetCalibration_Response);
	for(uint8_t i = 0; i < MMC5883MA_AXIS_NUM; i++) {
		response->offset[i]     = mmc5883ma.calibration_offset[i];
		response->gain[i] = mmc5883ma.calibration_multiplier[i];
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}


bool handle_heading_callback(void) {
	return handle_callback_value_callback_int16_t(&callback_value_heading, FID_CALLBACK_HEADING);
}

bool handle_magnetic_flux_density_callback(void) {
	static bool is_buffered = false;
	static MagneticFluxDensity_Callback cb;
	static int32_t last_value[MMC5883MA_AXIS_NUM] = {0, 0, 0};
	static uint32_t last_time = 0;

	if(!is_buffered) {
		if(mmc5883ma.mfd_period == 0 || !system_timer_is_time_elapsed_ms(last_time, mmc5883ma.mfd_period)) {
			return false;
		}

		if(mmc5883ma.mfd_value_has_to_change && (mmc5883ma.magnetic_flux_density[0] == last_value[0] && mmc5883ma.magnetic_flux_density[1] == last_value[1] && mmc5883ma.magnetic_flux_density[2] == last_value[2])) {
			return false;
		}

		tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(MagneticFluxDensity_Callback), FID_CALLBACK_MAGNETIC_FLUX_DENSITY);

		cb.x = mmc5883ma.magnetic_flux_density[0];
		cb.y = mmc5883ma.magnetic_flux_density[1];
		cb.z = mmc5883ma.magnetic_flux_density[2];

		last_value[0] = mmc5883ma.magnetic_flux_density[0];
		last_value[1] = mmc5883ma.magnetic_flux_density[1];
		last_value[2] = mmc5883ma.magnetic_flux_density[2];

		last_time = system_timer_get_ms();
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(MagneticFluxDensity_Callback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

void communication_tick(void) {
	communication_callback_tick();
}

void communication_init(void) {
	callback_value_init_int16_t(&callback_value_heading, mmc5883ma_get_heading);

	communication_callback_init();
}
