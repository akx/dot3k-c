#define _DEFAULT_SOURCE // usleep
#include <stdio.h>
#include "dothat.h"

int main() {
	DOTHAT* d = dothat_init();

	dothat_bl_enable(d, 1);
	dothat_bl_enable_leds(d, 0xFFFFFF);
	dothat_bl_set_brightness(d, 0, 18, 254);
	dothat_bl_update_brightnesses(d);
	usleep(500000);
	dothat_bl_set_screen_rgb(d, 0, 0, 0, 0);
	dothat_bl_set_screen_rgb(d, 1, 255, 0, 0);
	dothat_bl_set_screen_rgb(d, 2, 0, 255, 0);
	dothat_bl_set_screen_rgb(d, 3, 0, 0, 255);
	dothat_bl_set_screen_rgb(d, 4, 255, 255, 0);
	dothat_bl_set_screen_rgb(d, 5, 255, 0, 255);
	dothat_bl_update_brightnesses(d);
	usleep(500000);
	
	dothat_bl_set_bar_graph_leds( d, 0b00101010 );
	usleep(500000);
	dothat_bl_set_bar_graph_leds( d, 0b00010101 );
	usleep(500000);
	dothat_bl_set_bar_graph_leds( d, 0b00111111 );

	usleep(1000000);

	dothat_bl_enable(d, 0);
	dothat_bl_set_bar_graph_leds( d, 0x00 );
	
	dothat_shutdown(d);
	return 0;
}
