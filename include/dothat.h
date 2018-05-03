#include <stdint.h>
#include <unistd.h>

#ifndef BUILDING_DOTHAT
typedef struct DOTHAT DOTHAT;
#endif

//enable use of functions in cpp program
#ifdef __cplusplus
extern "C" {
#endif
  
// General
DOTHAT* dothat_init();
void dothat_shutdown(DOTHAT *dothat);

// Backlight
void dothat_bl_reset(DOTHAT *dothat);
void dothat_bl_enable(DOTHAT *dothat, int enable);
void dothat_bl_enable_leds(DOTHAT *dothat, uint32_t leds);
void dothat_bl_set_brightnesses(DOTHAT *dothat, int offset, int count, uint8_t brightnesses[]);
void dothat_bl_set_brightness(DOTHAT *dothat, int offset, int count, uint8_t brightness);
void dothat_bl_set_screen_rgb(DOTHAT *dothat, int8_t pos, uint8_t r, uint8_t g, uint8_t b);
void dothat_bl_update_brightnesses(DOTHAT *dothat);

// Graph
void dothat_graph_set_brightness(DOTHAT *dothat, uint8_t brightness);
void dothat_graph_set_leds(DOTHAT *dothat, uint8_t bitmask);

// Input
#define DOTHAT_CANCEL  0b00000001
#define DOTHAT_UP      0b00000010
#define DOTHAT_DOWN    0b00000100
#define DOTHAT_LEFT    0b00001000
#define DOTHAT_BUTTON  0b00010000
#define DOTHAT_RIGHT   0b00100000

void dothat_input_recalibrate(DOTHAT *dothat);
uint8_t dothat_input_poll(DOTHAT *dothat);

// LCD
int dothat_lcd_write_command(DOTHAT *dothat, uint8_t command, uint8_t instr_set);
void dothat_lcd_set_contrast(DOTHAT *dothat, uint8_t contrast);
void dothat_lcd_write_text(DOTHAT *dothat, const char *text);
void dothat_lcd_set_pos(DOTHAT *dothat,int row, int col);
void dothat_lcd_set_mode(DOTHAT *dothat, int display, int cursor, int blink);
void dothat_lcd_clear(DOTHAT *dothat);
void dothat_lcd_home(DOTHAT *dothat);

//enable use of functions in cpp program
#ifdef __cplusplus
}
#endif
