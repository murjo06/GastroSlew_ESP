#include "motor.h"

motor_t motor_ra;
motor_t motor_dec;

void park()
{
	motor_ra.move(0.0);	//todo: uporabi startup ra za park
	motor_dec.move(90.0);
}

void slew(double ra, double dec)
{
	motor_ra.move(ra);
	motor_dec.move(dec);
}

void abort()
{
	motor_ra.abort();
	motor_dec.abort();
}

void sync(double ra, double dec)
{
	motor_ra.sync(ra);
	motor_dec.sync(dec);
}