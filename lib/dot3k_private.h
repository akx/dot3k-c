#include <stdint.h>

typedef struct {
    int backlight_i2c_fd;
    uint8_t backlight_level[18];
    uint32_t backlight_enable;
    uint8_t backlight_level_map[18][255];
} DOT3K;
