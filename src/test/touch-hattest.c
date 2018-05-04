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
		char inputtext[] = { '-', '-', '-', '-', '-', '-', 0 };
		
		dothat_lcd_clear(d);
		dothat_lcd_home(d);
		if( (inp & DOTHAT_CANCEL) != 0 ) { inputtext[0] = 'Q'; active = 0; }
		if( (inp & DOTHAT_LEFT) != 0 ) { inputtext[1] = 'L'; }
		if( (inp & DOTHAT_RIGHT) != 0 ) { inputtext[2] = 'R'; }
		if( (inp & DOTHAT_UP) != 0 ) { inputtext[3] = 'U'; }
		if( (inp & DOTHAT_DOWN) != 0 ) { inputtext[4] = 'D'; }
		if( (inp & DOTHAT_BUTTON) != 0 ) { inputtext[5] = 'B'; }
		
		dothat_lcd_write_text(d, (char*)&inputtext);
		usleep(150000);
	}

	dothat_shutdown(d);
}
