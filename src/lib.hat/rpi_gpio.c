// Based on RPi.GPIO 0.5.11 c_gpio.c

/*
Copyright (c) 2012-2015 Ben Croston

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define _DEFAULT_SOURCE // usleep
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "dot3k_private.h"

#define BCM2708_PERI_BASE_DEFAULT    0x20000000
#define GPIO_BASE_OFFSET                0x200000
#define FSEL_OFFSET                      0    // 0x0000
#define SET_OFFSET                        7    // 0x001c / 4
#define CLR_OFFSET                        10  // 0x0028 / 4
#define PINLEVEL_OFFSET                 13  // 0x0034 / 4
#define EVENT_DETECT_OFFSET            16  // 0x0040 / 4
#define RISING_ED_OFFSET                19  // 0x004c / 4
#define FALLING_ED_OFFSET              22  // 0x0058 / 4
#define HIGH_DETECT_OFFSET             25  // 0x0064 / 4
#define LOW_DETECT_OFFSET              28  // 0x0070 / 4
#define PULLUPDN_OFFSET                 37  // 0x0094 / 4
#define PULLUPDNCLK_OFFSET             38  // 0x0098 / 4

#define PAGE_SIZE  (4*1024)
#define BLOCK_SIZE (4*1024)

static volatile uint32_t *gpio_map = NULL;

static void short_wait(void)
{
    usleep((useconds_t)10000); // way longer than this should be
}

int rpi_gpio_open(void)
{
    int mem_fd;
    uint8_t *gpio_mem;
    uint32_t peri_base = BCM2708_PERI_BASE_DEFAULT;
    uint32_t gpio_base;
    unsigned char buf[4];
    FILE *fp;

    if ((uint32_t)gpio_map) return 0; // Already open

    // get peri base from device tree
    if ((fp = fopen("/proc/device-tree/soc/ranges", "rb")) != NULL) {
        fseek(fp, 4, SEEK_SET);
        if (fread(buf, 1, sizeof buf, fp) == sizeof buf) {
            peri_base = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3] << 0;
        }
        fclose(fp);
    }

    gpio_base = peri_base + GPIO_BASE_OFFSET;

    // mmap the GPIO memory registers
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
        perror("rpi_gpio: /dev/mem");
        return 1;
    }

    if ((gpio_mem = malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == NULL) {
        perror("rpi_gpio: malloc");
        return 2;
    }

    if ((uint32_t)gpio_mem % PAGE_SIZE)
        gpio_mem += PAGE_SIZE - ((uint32_t)gpio_mem % PAGE_SIZE);

    void *gpio_map_ptr = mmap(gpio_mem, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, mem_fd, gpio_base);

    if (gpio_map_ptr == MAP_FAILED) {
        perror("rpi_gpio: mmap");
        return 3;
    }
    
    gpio_map = (uint32_t *)gpio_map_ptr;

    return 0;
}

static void set_pullupdn(int gpio, int pud)
{
    int clk_offset = PULLUPDNCLK_OFFSET + (gpio/32);
    int shift = (gpio%32);

    if (pud == GPIO_PUD_DOWN)
        *(gpio_map+PULLUPDN_OFFSET) = (*(gpio_map+PULLUPDN_OFFSET) & ~3) | GPIO_PUD_DOWN;
    else if (pud == GPIO_PUD_UP)
        *(gpio_map+PULLUPDN_OFFSET) = (*(gpio_map+PULLUPDN_OFFSET) & ~3) | GPIO_PUD_UP;
    else  // pud == PUD_OFF
        *(gpio_map+PULLUPDN_OFFSET) &= ~3;

    short_wait();
    *(gpio_map+clk_offset) = 1 << shift;
    short_wait();
    *(gpio_map+PULLUPDN_OFFSET) &= ~3;
    *(gpio_map+clk_offset) = 0;
}

void rpi_gpio_setup_pin(int gpio, int direction, int pud)
{
    if(!gpio_map) return;
    int offset = FSEL_OFFSET + (gpio/10);
    int shift = (gpio%10)*3;

    set_pullupdn(gpio, pud);
    if (direction == GPIO_OUT)
        *(gpio_map+offset) = (*(gpio_map+offset) & ~(7<<shift)) | (1<<shift);
    else  // direction == INPUT
        *(gpio_map+offset) = (*(gpio_map+offset) & ~(7<<shift));
}

// Contribution by Eric Ptak <trouch@trouch.com>
int rpi_gpio_function(int gpio)
{
    if(!gpio_map) return 0;
    int offset = FSEL_OFFSET + (gpio/10);
    int shift = (gpio%10)*3;
    int value = *(gpio_map+offset);
    value >>= shift;
    value &= 7;
    return value; // 0=input, 1=output, 4=alt0
}

void rpi_gpio_output(int gpio, int value)
{
    int offset, shift;
    if(!gpio_map) return;
    if (value) // value == HIGH
        offset = SET_OFFSET + (gpio/32);
    else         // value == LOW
        offset = CLR_OFFSET + (gpio/32);

    shift = (gpio%32);

    *(gpio_map+offset) = 1 << shift;
}

int rpi_gpio_input(int gpio)
{
    int offset, value, mask;
    if(!gpio_map) return 0;
    offset = PINLEVEL_OFFSET + (gpio/32);
    mask = (1 << gpio%32);
    value = *(gpio_map+offset) & mask;
    return value;
}

void rpi_gpio_close(void)
{
    if(!gpio_map) return;
    munmap((void*) gpio_map, BLOCK_SIZE);
    gpio_map = NULL;
}

