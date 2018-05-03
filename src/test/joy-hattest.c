#include "dothat.h"
#include <unistd.h>
#include <stdio.h>

int main() {
	DOTHAT* d = dothat_init();

	dothat_input_recalibrate(d);

	int active = 1;
	while( active != 0 )
	{
		uint8_t inp = dothat_input_poll(d);
		
		dothat_lcd_clear(d);
		dothat_lcd_home(d);
		if( (inp & DOTHAT_CANCEL) != 0 ) { dothat_lcd_write_text(d, "Quit"); active = 0; }
		if( (inp & DOTHAT_LEFT) != 0 ) { dothat_lcd_write_text(d, "Left"); }
		if( (inp & DOTHAT_RIGHT) != 0 ) { dothat_lcd_write_text(d, "Right"); }
		if( (inp & DOTHAT_UP) != 0 ) { dothat_lcd_write_text(d, "Up"); }
		if( (inp & DOTHAT_DOWN) != 0 ) { dothat_lcd_write_text(d, "Down"); }
		if( (inp & DOTHAT_BUTTON) != 0 ) { dothat_lcd_write_text(d, "Button"); }
		
		usleep(150000);
	}

	dothat_shutdown(d);
}
