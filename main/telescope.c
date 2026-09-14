#include "telescope.h"
#include "motor_foc.h"
#include "config.h"
#include "pinmap.h"

double lst = 0.0;
double latitude = 0.0;

void telescope_init(void)
{
	motor_foc_init(&motor_ra);
	motor_foc_init(&motor_dec);

	motor_ra.axis = RA;

	motor_dec.axis = DEC;
}

void slew(double ra, double dec)
{
	motor_ra.command_position(&motor_ra, ra);
	motor_dec.command_position(&motor_dec, dec);
}

void telescope_enable(bool enable)
{
	motor_ra.enable(&motor_ra, enable);
	motor_dec.enable(&motor_dec, enable);
}

void sync(double ra, double dec)
{
	motor_ra.sync(&motor_ra, ra);
	motor_dec.sync(&motor_dec, dec);
}

// požene se vsake nekaj časa, recimo 10 ms
void telescope_loop(void)
{
	// inkrementiraj lts

	motor_ra.loop(&motor_ra);
	motor_dec.loop(&motor_dec);
}