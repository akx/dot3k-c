#include <math.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "i2c-dev.h"
#include "dot3k_private.h"

#define CMD_ENABLE_OUTPUT 0x00
#define CMD_SET_PWM_VALUES 0x01
#define CMD_ENABLE_LEDS 0x13
#define CMD_UPDATE 0x16
#define CMD_RESET 0x17

#define NOT_OPEN(dot3k) (dot3k->backlight_i2c_fd <= 0)

static uint8_t clamp_u8(int val) {
    if(val < 0) return 0;
    if(val > 255) return 255;
    return (uint8_t) val;
}

/*
void disco(int fd) {
	uint8_t x[18];
	for(int i = 0; i < 18; i++) x[i] = rand() & 1 ? 255 : 0;
	i2c_smbus_write_i2c_block_data(fd, CMD_SET_PWM_VALUES, 18, x);
}

void set_all_leds(int fd, int value) {
	uint8_t x[18];
	memset(x, value, 18);
	i2c_smbus_write_i2c_block_data(fd, CMD_SET_PWM_VALUES, 18, x);
}
*/


static void dot3k_bl_update(DOT3K *dot3k) {
	if(NOT_OPEN(dot3k)) return;
	i2c_smbus_write_i2c_block_data(dot3k->backlight_i2c_fd, CMD_UPDATE, 1, (uint8_t[]){0x01});
}

void dot3k_bl_reset(DOT3K *dot3k) {
	if(NOT_OPEN(dot3k)) return;
	i2c_smbus_write_i2c_block_data(dot3k->backlight_i2c_fd, CMD_RESET, 1, (uint8_t[]){0xFF});
}

void dot3k_bl_enable(DOT3K *dot3k, int enable) {
	if(NOT_OPEN(dot3k)) return;
	i2c_smbus_write_i2c_block_data(dot3k->backlight_i2c_fd, CMD_ENABLE_OUTPUT, 1, (uint8_t[]){enable ? 0x01 : 0x00});
}

void dot3k_bl_enable_leds(DOT3K *dot3k, uint32_t leds) {
	if(NOT_OPEN(dot3k)) return;
	dot3k->backlight_enable = leds;
	uint8_t data[3] = {
		leds & 0xFF,
		(leds >> 8) & 0xFF,
		(leds >> 16) & 0xFF
	};
	i2c_smbus_write_i2c_block_data(dot3k->backlight_i2c_fd, CMD_ENABLE_LEDS, 3, data);
}

void dot3k_bl_update_brightnesses(DOT3K *dot3k) {
	if(NOT_OPEN(dot3k)) return;
    uint8_t data[18]={0};
    for(int led = 0; led < 18; led++) {
        uint8_t level = dot3k->backlight_level[led];
        data[led] = dot3k->backlight_level_map[led][level];
        //printf("%02d: %02x %02x\n", led, level, data[led]);
    }
	i2c_smbus_write_i2c_block_data(dot3k->backlight_i2c_fd, CMD_SET_PWM_VALUES, 18, data);
	dot3k_bl_update(dot3k);
}

void dot3k_bl_set_brightnesses(DOT3K *dot3k, int offset, int count, uint8_t brightnesses[]) {
	if(NOT_OPEN(dot3k)) return;
	for(int i = offset; i < count; i++) {
		if(i >= 18) break;
		dot3k->backlight_level[i] = *brightnesses;
		brightnesses++;
	}
}

void dot3k_bl_set_brightness(DOT3K *dot3k, int offset, int count, uint8_t brightness) {
	if(NOT_OPEN(dot3k)) return;
	for(int i = offset; i < count; i++) {
		if(i >= 18) break;
		dot3k->backlight_level[i] = brightness;
	}
}

void dot3k_bl_set_screen_rgb(DOT3K *dot3k, uint8_t pos, uint8_t r, uint8_t g, uint8_t b) {
	if(NOT_OPEN(dot3k)) return;
	if(pos >= 3) return;
    if(pos == -1) {
        dot3k_bl_set_screen_rgb(dot3k, 0, r, g, b);
        dot3k_bl_set_screen_rgb(dot3k, 1, r, g, b);
        dot3k_bl_set_screen_rgb(dot3k, 2, r, g, b);
        return;
    }
	dot3k->backlight_level[pos * 3 + 0] = r;
	dot3k->backlight_level[pos * 3 + 1] = g;
	dot3k->backlight_level[pos * 3 + 2] = b;
}

void dot3k_bl_set_bar_graph(DOT3K *dot3k, float value, float intensity) {
	if(NOT_OPEN(dot3k)) return;
	for(int i = 0; i < 9; i++) {
		float p = i / 9.0f;
		float dst = (value >= p ? intensity : 0);
		dot3k->backlight_level[9 + i] = clamp_u8((int)(dst * 255));
	}
}

void dot3k_bl_close(DOT3K *dot3k) {
	if(NOT_OPEN(dot3k)) return;
	dot3k_bl_enable(dot3k, 0);
	close(dot3k->backlight_i2c_fd);
	dot3k->backlight_i2c_fd = 0;
}

int dot3k_bl_open(DOT3K *dot3k) {
	dot3k_bl_close(dot3k);
	int fd = open("/dev/i2c-1", O_RDWR);
	if(fd < 0) {
		perror("opening i2c for backlight");
		return 1;
	}
	dot3k->backlight_i2c_fd = fd;
	ioctl(fd, I2C_SLAVE, 0x54);
    dot3k_bl_reset(dot3k);
    for(int led = 0; led < 18; led++) {
        for(int level = 0; level < 256; level++) {
            dot3k->backlight_level_map[led][level] = level;
        }
    }
    return 0;
}

void dot3k_bl_calibrate(DOT3K *dot3k, float gamma, float r_div, float g_div, float b_div) {
    for(int level = 0; level < 256; level++) {
        float p = powf(level / 256.0f, gamma);
        for(int off = 0; off < 3; off++) {
			uint8_t r = clamp_u8(p / r_div * 255.0f);
			uint8_t g = clamp_u8(p / g_div * 255.0f);
			uint8_t b = clamp_u8(p / b_div * 255.0f);
            dot3k->backlight_level_map[off * 3 + 0][level] = clamp_u8(r);
            dot3k->backlight_level_map[off * 3 + 1][level] = clamp_u8(g);
            dot3k->backlight_level_map[off * 3 + 2][level] = clamp_u8(b);
            //printf("%02x: %02x %02x %02x\n", level, r, g, b);
        }
    }
}
