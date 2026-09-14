#include "motor_foc.h"
#include "config.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "pinmap.h"
#include "telescope.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define UART_START_CHAR			'<'
#define UART_END_CHAR			'>'
#define UART_MAX_SIZE			64

motor_t motor_ra;
motor_t motor_dec;

int32_t guide_remaining_ra = 0;

bool foc_command_position(motor_t *motor, double target)
{
	int64_t target_steps = (int64_t)((target * (motor->axis == RA) ? STEPS_PER_DEGREE_RA : STEPS_PER_DEGREE_DEC) + 0.5);

	char buffer[20] = {0};
	uint32_t len = snprintf(buffer, UART_MAX_SIZE, "%cP%016llx%c", UART_START_CHAR, (uint64_t)target_steps, UART_END_CHAR);
	if(len > UART_MAX_SIZE) {
		return;
	}

	return (len ==uart_write_bytes((motor->axis == RA) ? UART_RA : UART_DEC, buffer, len));
}

bool foc_command_velocity(motor_t *motor, double target)
{
	float velocity = (float)(target) * (float)((motor->axis == RA) ? STEPS_PER_DEGREE_RA : STEPS_PER_DEGREE_DEC);
	uint32_t velocity_buffer = 0;

	memcpy(&velocity_buffer, &velocity, sizeof(float));

	char buffer[12] = {0};
	uint16_t len = snprintf(buffer, UART_MAX_SIZE, "%cV%08lx%c", UART_START_CHAR, velocity_buffer, UART_END_CHAR);
	if(len > UART_MAX_SIZE) {
		return;
	}

	return (len == uart_write_bytes((motor->axis == RA) ? UART_RA : UART_DEC, buffer, len));
}

bool foc_sync(motor_t *motor, double target)
{
	motor->position = target;

	int64_t target_steps = (int64_t)(target * (motor->axis == RA) ? STEPS_PER_DEGREE_RA : STEPS_PER_DEGREE_DEC + 0.5);

	char buffer[20] = {0};
	uint32_t len = snprintf(buffer, UART_MAX_SIZE, "%cP%016llx%c", UART_START_CHAR, (uint64_t)target_steps, UART_END_CHAR);
	if(len > UART_MAX_SIZE) {
		return;
	}

	return (len == uart_write_bytes((motor->axis == RA) ? UART_RA : UART_DEC, buffer, len));
}

bool foc_enable(motor_t *motor, bool enable)
{
	gpio_set_level((motor->axis == RA) ? EN_RA : EN_DEC, (uint8_t)enable);
	motor->enabled = enable;
	return true;
}

bool foc_loop(motor_t *motor)
{
	bool ret = true;
	if(motor->axis == RA) {
		uint32_t period = (uint32_t)(1000.0f * LOOP_PERIOD);
		if(guide_remaining_ra > period) {
			guide_remaining_ra -= period;
		} else {
			guide_remaining_ra = 0;
			bool rc = motor_ra.command_velocity(&motor_ra, TRACKING_RATE * STEPS_PER_DEGREE_RA);
			if(!rc) {
				ret = false;
			}
		}
	}

	return ret;
}

void motor_foc_init(motor_t *motor)
{
	motor->position = 0.0;

	motor->command_position = foc_command_position;
	motor->command_velocity = foc_command_velocity;

	motor->loop = foc_loop;

	motor->sync = foc_sync;
	motor->enable = foc_enable;

	motor->position_steps = 0;
}