#define _DEFAULT_SOURCE // usleep
#include <stdio.h>
#include "dothat.h"
#include <time.h>

int main() {
    DOTHAT* d = dothat_init();

    dothat_bl_enable(d, 0);
    
		char outstr[200];
		time_t t;
		struct tm *tmp;
		t = time(NULL);
		tmp = localtime(&t);
		strftime(outstr, sizeof(outstr), "%Y-%m-%d", tmp);
		dothat_lcd_set_pos(d, 0, 0);
		dothat_lcd_write_text(d, outstr);
		strftime(outstr, sizeof(outstr), "%H:%M:%S", tmp);
		dothat_lcd_set_pos(d, 1, 0);
		dothat_lcd_write_text(d, outstr);
		dothat_lcd_set_pos(d, 2, 0);
		dothat_lcd_write_text(d, "Test Three Lines");
		
		dothat_shutdown(d);
    return 0;
}
