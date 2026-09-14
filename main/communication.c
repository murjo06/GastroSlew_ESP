#include "communication.h"
#include "telescope.h"
#include "config.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool process_command(char *command, uint16_t size)
{
	if(command[0] != USB_START_CHAR || command[size - 1] != USB_END_CHAR || size < 3) {
		return false;		// invalid command
	}

	char response[USB_MAX_SIZE] = {0};
	uint16_t length = 0;

	switch(command[1]) {
		case 'e': {			//* dobi ekvitorialne koordinate
			uint32_t ra_buffer = 0;
			float ra = (float)motor_ra.position - (float)lst;
			memcpy(&ra_buffer, &ra, sizeof(float));

			uint32_t dec_buffer = 0;
			float dec = (float)motor_dec.position;
			memcpy(&dec_buffer, &dec, sizeof(float));

			char pier_side = (motor_ra.position > 0.0) ? 'W' : 'E';

			length = snprintf(response + 1, USB_MAX_SIZE - 2, "e%08lx,%08lx,%c", ra_buffer, dec_buffer, pier_side);
		} case 'S': {		//* sync
			if(size < 20) {
				return false;
			}
			uint32_t ra_buffer = 0;
			uint32_t dec_buffer = 0;
			if(sscanf(command + 2, "%08lx,%08lx", &ra_buffer, &dec_buffer) != 2) {
				return false;
			}

			float ra = 0.0f;
			memcpy(&ra, &ra_buffer, sizeof(float));

			float dec = 0.0f;
			memcpy(&dec, &dec_buffer, sizeof(float));

			motor_ra.position = lst + (double)ra;
			motor_dec.position = (double)dec;
		} case 'P': {		//* goto
			if(size < 20) {
				return false;
			}
			uint32_t ra_buffer = 0;
			uint32_t dec_buffer = 0;
			if(sscanf(command + 2, "%08lx,%08lx", &ra_buffer, &dec_buffer) != 2) {
				return false;
			}

			float ra = 0.0f;
			memcpy(&ra, &ra_buffer, sizeof(float));

			float dec = 0.0f;
			memcpy(&dec, &dec_buffer, sizeof(float));

			motor_ra.enable(&motor_ra, true);
			motor_dec.enable(&motor_dec, true);

			motor_ra.command_position(&motor_ra, lst + (double)ra);
			motor_dec.command_position(&motor_dec, (double)dec);
		} case 'A': {		//* abort
			motor_ra.enable(&motor_ra, false);
			motor_dec.enable(&motor_dec, false);
		} case 'G': {		//* guide
			if(size < 8) {
				return false;
			}
			if(command[2] == 'N' || command[2] == 'S') {
				float direction = (command[2] == 'N') ? 1.0f : -1.0f;

				int32_t ms_buffer = 0;
				if(sscanf(command + 3, "%04lx", &ms_buffer) != 1) {
					return false;
				}

				float guide_pulse = GUIDE_SPEED * (float)TRACKING_RATE * (float)STEPS_PER_DEGREE_DEC * (float)ms_buffer * 0.001f;
				motor_dec.command_position(&motor_dec, motor_dec.position + (double)(guide_pulse * direction));
			} else if(command[2] == 'E' || command[2] == 'W') {
				int32_t direction = (command[2] == 'E') ? 1 : -1;

				int32_t ms_buffer = 0;
				if(sscanf(command + 3, "%04lx", &ms_buffer) != 1) {
					return false;
				}

				guide_remaining_ra = ms_buffer * direction;
			} else {
				return false;	// invalid direction command
			}
		} case 'v': {		//* verzija programske opreme
			length = snprintf(response + 1, USB_MAX_SIZE - 2, "v%01u.%01u", FIRMWARE_VERSION_MAJOR, FIRMWARE_VARSION_MINOR);
		} case 'm': {		//* dobi status motorjev
			bool enabled = (motor_ra.enabled && motor_dec.enabled);

			motor_ra.enable(&motor_ra, enabled);
			motor_dec.enable(&motor_dec, enabled);

			length = snprintf(response + 1, USB_MAX_SIZE - 2, "%c", enabled ? '+' : '-');
		} case 'M': {		//* nastavi status motorjev
			if(size < 4) {
				return false;
			}
			if(command[2] != '+' && command[2] != '-') {
				return false;
			}
			motor_ra.enable(&motor_ra, (command[2] == '+'));
			motor_dec.enable(&motor_dec, (command[2] == '+'));
		} case 'l': {		//* dobi LST
			uint32_t lst_buffer = 0;
			float lst_f = (float)lst;
			memcpy(&lst_buffer, &lst_f, sizeof(float));

			length = snprintf(response + 1, USB_MAX_SIZE - 2, "l%08lx", lst_buffer);
		} case 'L': {		//* nastavi LST
			if(size < 11) {
				return false;
			}
			uint32_t lst_buffer = 0;
			if(sscanf(command + 2, "%08lx", &lst_buffer) != 1) {
				return false;
			}

			float lst_f = 0.0f;
			memcpy(&lst_f, &lst_buffer, sizeof(float));

			lst = (double)lst_f;
		} case 'f': {		//* dobi zemljepisno širino
			uint32_t lat_buffer = 0;
			float lat_f = (float)latitude;
			memcpy(&lat_buffer, &lat_f, sizeof(float));

			length = snprintf(response + 1, USB_MAX_SIZE - 2, "f%08lx", lat_buffer);
		} case 'F': {		//* nastavi zemljepisno širino
			if(size < 11) {
				return false;
			}
			uint32_t lat_buffer = 0;
			if(sscanf(command + 2, "%08lx", &lat_buffer) != 1) {
				return false;
			}

			float lat_f = 0.0f;
			memcpy(&lat_f, &lat_buffer, sizeof(float));

			latitude = (double)lat_f;
		} case 'T': {		//* track
			motor_ra.enable(&motor_ra, true);
			motor_dec.enable(&motor_dec, true);

			motor_ra.command_velocity(&motor_ra, TRACKING_RATE * STEPS_PER_DEGREE_RA);
		} default: {return false;}
	}

	if(length > USB_MAX_SIZE - 3) {
		return false;
	}

	response[0] = USB_START_CHAR;
	response[length + 1] = USB_END_CHAR;
	response[length + 2] = '\0';

	//todo: usb print

	return true;
}