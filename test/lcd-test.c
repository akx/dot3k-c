#include <stdio.h>
#include "include/dot3k.h"
#include <time.h>

int main(int argc, char **argv) {
	DOT3K* d = dot3k_init();
	if(dot3k_bl_open(d)) return 1;
    
	dot3k_bl_enable(d, 1);
	dot3k_bl_enable_leds(d, 0xFFFFFF);
    dot3k_bl_set_brightness(d, 0, 9, 64);
    dot3k_bl_update_brightnesses(d);
    
    if(dot3k_lcd_open(d)) return 1;
    for(;;) {
        char outstr[200];
        time_t t;
        struct tm *tmp;
        t = time(NULL);
        tmp = localtime(&t);
        strftime(outstr, sizeof(outstr), "%Y-%m-%d", tmp);
        dot3k_lcd_set_pos(d, 0, 0);
        dot3k_lcd_write_text(d, outstr);
        strftime(outstr, sizeof(outstr), "%H:%M:%S", tmp);
        dot3k_lcd_set_pos(d, 1, 0);
        dot3k_lcd_write_text(d, outstr);
        dot3k_bl_set_bar_graph_train(d, 0xF0, 0, t, 1);
        dot3k_bl_update_brightnesses(d);
        usleep(500000);
    }
    dot3k_lcd_close(d);
	return 0;
}
