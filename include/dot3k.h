#include <stdint.h>
#include <unistd.h>

#ifndef BUILDING_DOT3K
typedef struct DOT3K DOT3K;
#endif

// General
DOT3K *dot3k_init();

// Backlight
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

// Joystick
#define DOT3K_JOY_UP 1
#define DOT3K_JOY_DOWN 2
#define DOT3K_JOY_LEFT 4
#define DOT3K_JOY_RIGHT 8
#define DOT3K_JOY_BTN 16

int dot3k_joy_open(DOT3K *dot3k);
int dot3k_joy_close(DOT3K *dot3k);
uint8_t dot3k_joy_poll(DOT3K *dot3k);

// LCD
int dot3k_lcd_write_command(DOT3K *dot3k, uint8_t command, uint8_t instr_set);
int dot3k_lcd_set_contrast(DOT3K *dot3k, uint8_t contrast);
int dot3k_lcd_write_text(DOT3K *dot3k, const char *text);
void dot3k_lcd_set_pos(DOT3K *dot3k,int row, int col);
void dot3k_lcd_set_mode(DOT3K *dot3k, int display, int cursor, int blink);
void dot3k_lcd_clear(DOT3K *dot3k);
void dot3k_lcd_home(DOT3K *dot3k);
int dot3k_lcd_close(DOT3K *dot3k);
int dot3k_lcd_open(DOT3K *dot3k);
