#include "include/dot3k.h"

int main(int argc, char **argv) {
	DOT3K* d = dot3k_init();
	dot3k_bl_open(d);
	dot3k_bl_enable(d, 1);
	dot3k_bl_enable_leds(d, 0xFFFFFF);
	dot3k_bl_set_screen_rgb(d, 1, 255, 0, 0);
	dot3k_bl_set_screen_rgb(d, 2, 0, 255, 0);
	dot3k_bl_set_screen_rgb(d, 0, 0, 0, 255);
    for(int i = 0; i < 100; i++) {
        float f = i / 100.0f;
        dot3k_bl_set_bar_graph(d, f, 0.2);
        usleep(10000);
    }
    dot3k_bl_enable(d, 0);
	return 0;
}
