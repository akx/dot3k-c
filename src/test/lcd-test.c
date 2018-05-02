#define _DEFAULT_SOURCE // usleep
#include <stdio.h>
#include "dot3k.h"
#include <time.h>

int main() {
    DOT3K* d = dot3k_init();
    if(dot3k_bl_open(d)) return 1;
    
    dot3k_bl_enable(d, 0);
    
    if(dot3k_lcd_open(d)) return 1;
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
		dot3k_lcd_set_pos(d, 2, 0);
		dot3k_lcd_write_text(d, "Test Three Lines");
    dot3k_lcd_close(d);
    return 0;
}
