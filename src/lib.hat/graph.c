#include <math.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "i2c-dev.h"
#include "dothat_private.h"

#define CMD_ENABLE_OUTPUT 0x00
#define CMD_SET_PWM_VALUES 0x01
#define CMD_ENABLE_LEDS 0x13
#define CMD_UPDATE 0x16
#define CMD_RESET 0x17

#define NOT_OPEN(dothat) (dothat->i2c_fd <= 0)

void dothat_graph_set_brightness(DOTHAT *dothat, uint8_t brightness)
{
	if(NOT_OPEN(dothat)) return;

	// Target graph
	ioctl(dothat->i2c_fd, I2C_SLAVE, 0x2C);

	// Set direct duty 0..8
	uint8_t b = (brightness << 4);
	i2c_smbus_write_i2c_block_data(dothat->i2c_fd, R_LED_DIRECT_DUT, 1, (uint8_t[]){b});
}

void dothat_graph_set_leds(DOTHAT *dothat, uint8_t bitmask)
{
	if(NOT_OPEN(dothat)) return;

	// Target graph
	ioctl(dothat->i2c_fd, I2C_SLAVE, 0x2C);

	uint8_t actualvalue = bitmask & 0b00111111;	// 6 LEDS

	// Set light mask
	i2c_smbus_write_i2c_block_data(dothat->i2c_fd, R_LED_OUTPUT_CON, 1, (uint8_t[]){actualvalue});
}

