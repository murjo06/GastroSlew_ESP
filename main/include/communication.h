#pragma once

#include <stdbool.h>

#define USB_START_CHAR			':'
#define USB_END_CHAR			'#'
#define USB_MAX_SIZE			64

bool process_command(char *command, uint16_t size);