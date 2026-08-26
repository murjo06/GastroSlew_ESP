#include <stdbool.h>
#include <stdint.h>

typedef struct {
    rmt_channel_handle_t channel;
    rmt_encoder_handle_t encoder;
} pulse_generator_t;

typedef struct {
	double position;
    double target_position;
    void (*move)(motor_t *motor, double target);
    void (*sync)(motor_t *motor, double target);
    void (*loop)(motor_t *motor);
    void (*abort)(motor_t *motor);

	int64_t position_steps;
	int64_t target_position_steps;
	pulse_generator_t pg;
	uint32_t step_pin;
	uint32_t dir_pin
} motor_t;