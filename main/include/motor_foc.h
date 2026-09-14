#pragma once

#include "telescope.h"

#include <stdbool.h>
#include <stdint.h>

extern uint32_t guide_remaining_ra;

typedef struct motor_t motor_t;

struct motor_t {
	double position;

    bool (*command_position)(motor_t *motor, double target);
    bool (*command_velocity)(motor_t *motor, double target);

    bool (*loop)(motor_t *motor);

    bool (*sync)(motor_t *motor, double target);
    bool (*enable)(motor_t *motor, bool enable);

    bool enabled;

    axis_t axis;
};

extern motor_t motor_ra;
extern motor_t motor_dec;

void motor_foc_init(motor_t *motor);