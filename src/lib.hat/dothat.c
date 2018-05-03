#include <math.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "i2c-dev.h"
#include "dothat_private.h"


DOTHAT* dothat_init() {
	DOTHAT* dothat = (DOTHAT*)calloc(1, sizeof(DOTHAT));

	// Open GPIO
	if(rpi_gpio_open()) {
		return 1;
	}
	
	// Open I2C (Backlight, Graph and Input)
	dothat->i2c_fd = open("/dev/i2c-1", O_RDWR);
	if(dothat->i2c_fd < 0) {
		perror("Failed to open I2C");
		return 0;
	}

	// Open SPI (LCD Text)
	dothat->lcd_register_select_gpio_pin = 25;
	rpi_gpio_setup_pin(dothat->lcd_register_select_gpio_pin, GPIO_OUT, GPIO_PUD_OFF);
	rpi_gpio_output(dothat->lcd_register_select_gpio_pin, 1);
	dothat->spi_fd = open("/dev/spidev0.0", O_RDWR);
	if(dothat->spi_fd <= 0) {
		perror("Failed to open SPI");
		return 0;
	}

	// Initialise Backlight
	dothat_bl_reset(dothat);
	for(int led = 0; led < 18; led++) {
		for(int level = 0; level < 256; level++) {
			dothat->backlight_level_map[led][level] = level;
		}
	}

	// Initialise Graph LEDs
	i2c_smbus_write_i2c_block_data(dothat->i2c_fd, R_LED_POLARITY, 1, (uint8_t[]){0x00});		// Set default polarity
	dothat_graph_set_brightness(dothat, 0b00000100);
	dothat_graph_set_leds(dothat, 0b00000000);

	// Initialise LCD
	dothat->lcd_width = 16;
	dothat->lcd_height = 3;
	dothat_lcd_write_command(dothat, 0x06, 0); // no shift, cursor direction
	dothat_lcd_write_command(dothat, 0x1D, 1); // ???
	dothat_lcd_clear(dothat);
	dothat_lcd_home(dothat);
	dothat_lcd_set_contrast(dothat, 0x28);
	dothat_lcd_set_mode(dothat, 1, 0, 0);
	
	return dothat;
}

void dothat_shutdown(DOTHAT *dothat)
{
	// Shutdown I2C interface
	dothat_bl_enable(dothat, 0);
	close(dothat->i2c_fd);
	dothat->i2c_fd = 0;

	// Shutdown SPI interface
	close(dothat->spi_fd);
	dothat->spi_fd = 0;
}
