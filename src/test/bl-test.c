#define _DEFAULT_SOURCE // usleep
#include <stdio.h>
#include "dot3k.h"

int main() {
	DOT3K* d = dot3k_init();
	if(dot3k_bl_open(d)) return 1;

	dot3k_bl_enable(d, 1);
	dot3k_bl_enable_leds(d, 0xFFFFFF);
	dot3k_bl_set_brightness(d, 0, 18, 254);
	dot3k_bl_update_brightnesses(d);
	usleep(500000);
	dot3k_bl_set_screen_rgb(d, 0, 0, 0, 0);
	dot3k_bl_set_screen_rgb(d, 1, 255, 0, 0);
	dot3k_bl_set_screen_rgb(d, 2, 0, 255, 0);
	dot3k_bl_set_screen_rgb(d, 3, 0, 0, 255);
	dot3k_bl_set_screen_rgb(d, 4, 255, 255, 0);
	dot3k_bl_set_screen_rgb(d, 5, 255, 0, 255);
	dot3k_bl_update_brightnesses(d);
	usleep(500000);
	/*
	for(int i = 0; i < 100; i++) {
		float f = i / 100.0f;
		dot3k_bl_set_bar_graph(d, f, 0x30);
		dot3k_bl_update_brightnesses(d);
		usleep(10000);
	}
	*/
	dot3k_bl_enable(d, 0);
	return 0;
}
