#include "motor_foc.h"
#include <stdint.h>
#include "./config.h"
#include "../../include/config.h"
#include "driver/rmt_tx.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include <stdlib.h>

void command_position(motor_t *motor, double target)
{
	int64_t target_steps = (int64_t)(target * STEPS_PER_DEGREE + 0.5);
	if(ABS(target - motor->position) > MAX_STEP_DIR_MOVE) {
		
		// pošlji po uartu
	} else {
		int64_t diff = target_steps - motor->target_position_steps;
		pulse_generator_send(&(motor->pg), ABS(diff));
	}
	motor->target_position = target;
	motor->target_position_steps = target_steps;
}

void command_velocity(motor_t *motor, double target)
{
	
}

void sync(motor_t *motor, double target)
{
	motor->position = target;
	motor->target_position = target;
	// pošlji po uartu
}

void loop(motor_t *motor)
{
	if(!motor->step_dir) {
		return;
	}
}

static esp_err_t pulse_generator_send(pulse_generator_t *pg, uint32_t pulse_count)
{
    rmt_transmit_config_t tx_cfg = {
        .loop_count = 0,
    };

    esp_err_t err = rmt_transmit(pg->channel, pg->encoder, &pulse_count, sizeof(pulse_count), &tx_cfg);

    return err;
}

static esp_err_t pulse_generator_init(pulse_generator_t *pg, gpio_num_t gpio)
{
    rmt_tx_channel_config_t tx_cfg = {
        .gpio_num = gpio,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .mem_block_symbols = 64,
        .trans_queue_depth = 2,
    };

    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_cfg, &pg->channel));

	rmt_simple_encoder_config_t enc_cfg = {
	    .callback = pulse_encode_cb,
	    .arg = NULL,
	    .min_chunk_size = 1,
	};

	rmt_encoder_handle_t encoder;
	ESP_ERROR_CHECK(rmt_new_simple_encoder(&cfg, &encoder));

    ESP_ERROR_CHECK(rmt_enable(pg->channel));

    return ESP_OK;
}

void motor_foc_init(motor_t *motor)
{
	motor->position = 0.0;
	motor->target_position = 0.0;
	motor->move = move;
	motor->sync = sync;
	motor->loop = loop;

	motor->accumulator = 0.0f;
	motor->position_steps = 0;
	motor->target_position_steps = 0;

	pulse_generator_init(&(motor->pg), motor->step);
}