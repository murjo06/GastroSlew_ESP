#include "esp_err.h"
#include "pinmap.h"
#include "config.h"

#include <stdint.h>

esp_err_t gpio_init(void)
{
	gpio_config_t cfg = {
	    .pin_bit_mask = (1ULL << EN_BUS) |
			(1ULL << EN_5V) | 
			(1ULL << EN_BRAKE) | 
			(1ULL << EN_5V) | 
			(1ULL << WC) | 
			(1ULL << EN_RA) | 
			(1ULL << DIR_RA) | 
			(1ULL << STEP_RA) | 
			(1ULL << EN_DEC) | 
			(1ULL << DIR_DEC) | 
			(1ULL << STEP_DEC),
	    .mode = GPIO_MODE_OUTPUT,
	    .pull_up_en = GPIO_PULLUP_DISABLE,
	    .pull_down_en = GPIO_PULLDOWN_DISABLE,
	    .intr_type = GPIO_INTR_DISABLE,
	};

	ESP_ERROR_CHECK(gpio_config(&cfg));

	return ESP_OK;
}

esp_err_t adc_init(void)
{
	adc_oneshot_unit_handle_t adc;
	adc_oneshot_unit_init_cfg_t init = {
	    .unit_id = ADC_UNIT_1,
	};

	ESP_ERROR_CHECK(adc_oneshot_new_unit(&init, &adc));

	adc_oneshot_chan_cfg_t cfg = {
	    .atten = ADC_ATTEN_DB_12,
	    .bitwidth = ADC_BITWIDTH_DEFAULT,
	};

	ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, ADC_CHANNEL_0, &cfg));
	ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, ADC_CHANNEL_8, &cfg));

	return ESP_OK;
}

esp_err_t uart_init(void)
{
	uart_config_t uart_cfg = {
	    .baud_rate = 115200,
	    .data_bits = UART_DATA_8_BITS,
	    .parity    = UART_PARITY_DISABLE,
	    .stop_bits = UART_STOP_BITS_1,
	    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	    .source_clk = UART_SCLK_DEFAULT,
	};

	ESP_ERROR_CHECK(uart_driver_install(UART_RA, 1024, 1024, 0, NULL, 0));
	ESP_ERROR_CHECK(uart_param_config(UART_RA, &uart_cfg));

	ESP_ERROR_CHECK(uart_set_pin(UART_RA, TX_RA, RX_RA, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	ESP_ERROR_CHECK(uart_driver_install(UART_DEC, 1024, 1024, 0, NULL, 0));
	ESP_ERROR_CHECK(uart_param_config(UART_DEC, &uart_cfg));

	ESP_ERROR_CHECK(uart_set_pin(UART_DEC, TX_DEC, RX_DEC, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	return ESP_OK;
}

esp_err_t i2c_init(void)
{
	i2c_master_bus_handle_t bus;

	i2c_master_bus_config_t cfg = {
	    .i2c_port = I2C_NUM_0,
	    .sda_io_num = SDA,
	    .scl_io_num = SCL,
	    .clk_source = I2C_CLK_SRC_DEFAULT,
	    .glitch_ignore_cnt = 7,
	    .flags.enable_internal_pullup = false,
	};

	ESP_ERROR_CHECK(i2c_new_master_bus(&cfg, &bus));

	i2c_master_dev_handle_t dev;

	i2c_device_config_t dev_cfg = {
	    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
	    .device_address = 0b1010000,
	    .scl_speed_hz = 100000,
	};

	ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &dev_cfg, &dev));	// dodaj M24C64

	return ESP_OK;
}