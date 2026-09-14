#pragma once

typedef enum {
    RA  = 0x00U,
    DEC = 0x01U
} axis_t;

#include "motor_foc.h"

extern double lst;
extern double latitude;

void telescope_init(void);

void telescope_loop(void);

void slew(double ra, double dec);

void sync(double ra, double dec);

void telescope_enable(bool enable);