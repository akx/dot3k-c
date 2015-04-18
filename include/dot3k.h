#include <stdint.h>
#include <unistd.h>

typedef struct DOT3K DOT3K;

DOT3K *dot3k_init();
void dot3k_bl_close(DOT3K *dot3k);
int dot3k_bl_open(DOT3K *dot3k);
void dot3k_bl_reset(DOT3K *dot3k);
void dot3k_bl_enable(DOT3K *dot3k, int enable);
void dot3k_bl_enable_leds(DOT3K *dot3k, uint32_t leds);
void dot3k_bl_calibrate(DOT3K *dot3k, float gamma, float r_div, float g_div, float b_div);
void dot3k_bl_set_brightnesses(DOT3K *dot3k, int offset, int count, uint8_t brightnesses[]);
void dot3k_bl_set_brightness(DOT3K *dot3k, int offset, int count, uint8_t brightness);
void dot3k_bl_set_screen_rgb(DOT3K *dot3k, uint8_t pos, uint8_t r, uint8_t g, uint8_t b);
void dot3k_bl_set_bar_graph(DOT3K *dot3k, float value, float intensity);
void dot3k_bl_update_brightnesses(DOT3K *dot3k);


