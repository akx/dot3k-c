#define _DEFAULT_SOURCE // usleep
#include <linux/spi/spidev.h>
#include <math.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dothat_private.h"

#define NOT_OPEN(dothat) (dothat->spi_fd <= 0)

#define REGISTER_SELECT_PIN 25
#define COMMAND_DELAY_USEC 100

static void spi_xfer(int fd, uint8_t len, uint8_t *buf)
{
	struct spi_ioc_transfer xfer;
	memset(&xfer, 0, sizeof(struct spi_ioc_transfer));
	xfer.tx_buf = (unsigned long)buf;
	xfer.rx_buf = (unsigned long)NULL;
	xfer.len = len;
	xfer.speed_hz = 1000000;
	xfer.bits_per_word = 0;
	if(ioctl(fd, SPI_IOC_MESSAGE(1), &xfer) < 1)
	{
		perror("spi ioctl");
	}
}

static void spi_xfer_byte(int fd, uint8_t byte)
{
	spi_xfer(fd, 1, &byte);
}

int dothat_lcd_write_command(DOTHAT *dothat, uint8_t command, uint8_t instr_set)
{
	if(NOT_OPEN(dothat)) { return 1; }
	rpi_gpio_output(dothat->lcd_register_select_gpio_pin, 0);
	spi_xfer_byte(dothat->spi_fd, 0b00111000 | instr_set);
	usleep(COMMAND_DELAY_USEC);
	spi_xfer_byte(dothat->spi_fd, command);
	usleep(COMMAND_DELAY_USEC);
	return 0;
}

void dothat_lcd_write_text(DOTHAT *dothat, const char *text)
{
	if(NOT_OPEN(dothat)) { return; }
	rpi_gpio_output(dothat->lcd_register_select_gpio_pin, 1);
	while(*text)
	{
		uint8_t ch = (*text) & 0b1111111;
		spi_xfer_byte(dothat->spi_fd, ch);
		usleep(COMMAND_DELAY_USEC);
		text++;
	}
}

void dothat_lcd_set_contrast(DOTHAT *dothat, uint8_t contrast)
{
	if(contrast > 0x3F) { contrast = 0x3F; }
	// For 3.3v operation the booster must be on, which is
	// on the same command as the (2-bit) high-nibble of contrast
	dothat_lcd_write_command(dothat, (0b01010100 | ((contrast >> 4) & 0x03)), 1);
	dothat_lcd_write_command(dothat, 0b01101011, 1);
	// Set low-nibble of the contrast
	dothat_lcd_write_command(dothat, (0b01110000 | (contrast & 0x0F)), 1);
}

void dothat_lcd_set_pos(DOTHAT *dothat, int row, int col)
{
	if(NOT_OPEN(dothat)) { return; }
	uint8_t offset = (row * dothat->lcd_width + col) % (dothat->lcd_width * dothat->lcd_height);
	dothat_lcd_write_command(dothat, 0b10000000 | offset, 0);
}

void dothat_lcd_set_mode(DOTHAT *dothat, int display, int cursor, int blink)
{
	uint8_t command = 0b1000;
	if(display) command |= 0b100;
	if(cursor) command |= 0b10;
	if(blink) command |= 0b1;
	dothat_lcd_write_command(dothat, command, 0);
}

void dothat_lcd_clear(DOTHAT *dothat)
{
	dothat_lcd_write_command(dothat, 0x01, 0);
}

void dothat_lcd_home(DOTHAT *dothat)
{
	dothat_lcd_write_command(dothat, 0x02, 0);
}

