#include <stdint.h>
#include <poll.h>

typedef struct {
    // Backlight
    int backlight_i2c_fd;
    uint8_t backlight_level[18];
    uint32_t backlight_enable;
    uint8_t backlight_level_map[18][256];
    // Joystick
    uint8_t joystick_state;
    // LCD
    int lcd_spi_fd;
    int lcd_register_select_gpio_pin;
    int lcd_width;
    int lcd_height;
} DOT3K;

int rpi_gpio_open(void);
void rpi_gpio_setup_pin(int gpio,int direction,int pud);
int rpi_gpio_function(int gpio);
void rpi_gpio_output(int gpio,int value);
int rpi_gpio_input(int gpio);
void rpi_gpio_close(void);

#define GPIO_IN 1
#define GPIO_OUT 0
#define GPIO_PUD_OFF 0
#define GPIO_PUD_DOWN 1
#define GPIO_PUD_UP 2


#define BUILDING_DOT3K
#include "dot3k.h"

