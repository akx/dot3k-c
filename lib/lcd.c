#define __USE_XOPEN_EXTENDED  // for usleep
#include <linux/spi/spidev.h>
#include <math.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dot3k_private.h"

#define NOT_OPEN(dot3k) (dot3k->lcd_spi_fd <= 0)

#define REGISTER_SELECT_PIN 25
#define COMMAND_DELAY_USEC 100

static void spi_xfer(int fd, uint8_t len, uint8_t *buf) {
    struct spi_ioc_transfer xfer;
    memset(&xfer, 0, sizeof(struct spi_ioc_transfer));
    xfer.tx_buf = (unsigned long)buf;
    xfer.rx_buf = (unsigned long)NULL;
    xfer.len = len;
    xfer.speed_hz = 1000000;
    xfer.bits_per_word = 0;
    if(ioctl(fd, SPI_IOC_MESSAGE(1), &xfer) < 1) {
        perror("spi ioctl");
    }
}

static void spi_xfer_byte(int fd, uint8_t byte) {
    spi_xfer(fd, 1, &byte);
}

int dot3k_lcd_write_command(DOT3K *dot3k, uint8_t command, uint8_t instr_set) {
    if(NOT_OPEN(dot3k)) return 1;
    rpi_gpio_output(dot3k->lcd_register_select_gpio_pin, 0);
    spi_xfer_byte(dot3k->lcd_spi_fd, 0b00111000 | instr_set);
    usleep(COMMAND_DELAY_USEC);
    spi_xfer_byte(dot3k->lcd_spi_fd, command);
    usleep(COMMAND_DELAY_USEC);
    return 0;
}

int dot3k_lcd_write_text(DOT3K *dot3k, const char *text) {
    if(NOT_OPEN(dot3k)) return 1;
    rpi_gpio_output(dot3k->lcd_register_select_gpio_pin, 1);
    while(*text) {
        uint8_t ch = (*text) & 0b1111111;
        spi_xfer_byte(dot3k->lcd_spi_fd, ch);
        usleep(COMMAND_DELAY_USEC);
        text++;
    }
}

int dot3k_lcd_set_contrast(DOT3K *dot3k, uint8_t contrast) {
    if(contrast < 0) contrast = 0;
    if(contrast > 0x3F) contrast = 0x3F;
    // For 3.3v operation the booster must be on, which is
    // on the same command as the (2-bit) high-nibble of contrast
    dot3k_lcd_write_command(dot3k, (0b01010100 | ((contrast >> 4) & 0x03)), 1);
    dot3k_lcd_write_command(dot3k, 0b01101011, 1);
    // Set low-nibble of the contrast
    dot3k_lcd_write_command(dot3k, (0b01110000 | (contrast & 0x0F)), 1);
}

void dot3k_lcd_set_pos(DOT3K *dot3k, int row, int col) {
    if(NOT_OPEN(dot3k)) return;
    uint8_t offset = (row * dot3k->lcd_width + col) % (dot3k->lcd_width * dot3k->lcd_height);
    dot3k_lcd_write_command(dot3k, 0b10000000 | offset, 0);
}

void dot3k_lcd_set_mode(DOT3K *dot3k, int display, int cursor, int blink) {
    uint8_t command = 0b1000;
    if(display) command |= 0b100;
    if(cursor) command |= 0b10;
    if(blink) command |= 0b1;
    dot3k_lcd_write_command(dot3k, command, 0);
}

void dot3k_lcd_clear(DOT3K *dot3k) {
    dot3k_lcd_write_command(dot3k, 0x01, 0);
}

void dot3k_lcd_home(DOT3K *dot3k) {
    dot3k_lcd_write_command(dot3k, 0x02, 0);
}


int dot3k_lcd_close(DOT3K *dot3k) {
    if(dot3k->lcd_spi_fd) {
        close(dot3k->lcd_spi_fd);
        dot3k->lcd_spi_fd = 0;
    }
    return 0;
}

int dot3k_lcd_open(DOT3K *dot3k) {
    dot3k_lcd_close(dot3k);
    if(rpi_gpio_open()) {
        return 1;
    }
    dot3k->lcd_register_select_gpio_pin = 25;
    rpi_gpio_setup_pin(dot3k->lcd_register_select_gpio_pin, GPIO_OUT, GPIO_PUD_OFF);
    rpi_gpio_output(dot3k->lcd_register_select_gpio_pin, 1);
    int fd = open("/dev/spidev0.0", O_RDWR);
    if(fd <= 0) {
        perror("SPI open");
        return 1;
    }
    dot3k->lcd_spi_fd = fd;
    dot3k->lcd_width = 16;
    dot3k->lcd_height = 16;
    
    // Initialize!
    dot3k_lcd_write_command(dot3k, 0x06, 0); // no shift, cursor direction
    dot3k_lcd_write_command(dot3k, 0x1D, 1); // ???
    dot3k_lcd_clear(dot3k);
    dot3k_lcd_home(dot3k);
    dot3k_lcd_set_contrast(dot3k, 0x28);
    dot3k_lcd_set_mode(dot3k, 1, 0, 0);
    return 0;
}
