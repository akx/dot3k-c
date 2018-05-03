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

static uint8_t clamp_u8(int val)
{
	if(val < 0) return 0;
	if(val > 255) return 255;
	return (uint8_t) val;
}

static void dothat_bl_update(DOTHAT *dothat)
{
	if(NOT_OPEN(dothat)) return;
	ioctl(dothat->i2c_fd, I2C_SLAVE, 0x54);
	i2c_smbus_write_i2c_block_data(dothat->i2c_fd, CMD_UPDATE, 1, (uint8_t[]){0x01});
}

void dothat_bl_reset(DOTHAT *dothat)
{
	if(NOT_OPEN(dothat)) return;
	ioctl(dothat->i2c_fd, I2C_SLAVE, 0x54);
	i2c_smbus_write_i2c_block_data(dothat->i2c_fd, CMD_RESET, 1, (uint8_t[]){0xFF});
}

void dothat_bl_enable(DOTHAT *dothat, int enable)
{
	if(NOT_OPEN(dothat)) return;
	ioctl(dothat->i2c_fd, I2C_SLAVE, 0x54);
	i2c_smbus_write_i2c_block_data(dothat->i2c_fd, CMD_ENABLE_OUTPUT, 1, (uint8_t[]){enable ? 0x01 : 0x00});
}

void dothat_bl_enable_leds(DOTHAT *dothat, uint32_t leds)
{
	if(NOT_OPEN(dothat)) return;
	dothat->backlight_enable = leds;
	uint8_t data[3] = {
		leds & 0xFF,
		(leds >> 8) & 0xFF,
		(leds >> 16) & 0xFF
	};
	i2c_smbus_write_i2c_block_data(dothat->i2c_fd, CMD_ENABLE_LEDS, 3, data);
}

void dothat_bl_update_brightnesses(DOTHAT *dothat)
{
	if(NOT_OPEN(dothat)) return;
	uint8_t data[18]={0};
	for(int led = 0; led < 18; led++) {
		uint8_t level = dothat->backlight_level[led];
		data[led] = dothat->backlight_level_map[led][level];
	}
	ioctl(dothat->i2c_fd, I2C_SLAVE, 0x54);
	i2c_smbus_write_i2c_block_data(dothat->i2c_fd, CMD_SET_PWM_VALUES, 18, data);
	dothat_bl_update(dothat);
}

void dothat_bl_set_brightnesses(DOTHAT *dothat, int offset, int count, uint8_t brightnesses[])
{
	if(NOT_OPEN(dothat)) return;
	for(int i = offset; i < count; i++) {
		if(i >= 18) break;
		dothat->backlight_level[i] = *brightnesses;
		brightnesses++;
	}
}

void dothat_bl_set_brightness(DOTHAT *dothat, int offset, int count, uint8_t brightness)
{
	if(NOT_OPEN(dothat)) return;
	for(int i = offset; i < count; i++) {
		if(i >= 18) break;
		dothat->backlight_level[i] = brightness;
	}
}

void dothat_bl_set_screen_rgb(DOTHAT *dothat, int8_t pos, uint8_t r, uint8_t g, uint8_t b)
{
    if(NOT_OPEN(dothat)) return;
    if(pos >= 6) return;
    if(pos == -1) {
			for(int i = 0; i < 6; i++)
			{
        dothat_bl_set_screen_rgb(dothat, i, r, g, b);
      }
			return;
    }
    dothat->backlight_level[(pos * 3) + 0] = b;
    dothat->backlight_level[(pos * 3) + 1] = g;
    dothat->backlight_level[(pos * 3) + 2] = r;
}
