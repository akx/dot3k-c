#define _DEFAULT_SOURCE // usleep
#include <stdio.h>
#include "dot3k.h"

int main() {
    DOT3K* d = dot3k_init();
    if(dot3k_bl_open(d)) return 1;
    
    dot3k_bl_enable(d, 1);
    dot3k_bl_enable_leds(d, 0xFFFFFF);
    dot3k_bl_set_brightness(d, 0, 9, 254);
    dot3k_bl_update_brightnesses(d);
    usleep(500000);
    for(float gamma = 1.0; gamma <= 3.0; gamma += 0.1) {
        dot3k_bl_calibrate(d, gamma, 1.6, 1.4, 1);
        dot3k_bl_set_screen_rgb(d, 1, 255, 43, 64);
        dot3k_bl_set_screen_rgb(d, 2, 121, 255, 44);
        dot3k_bl_set_screen_rgb(d, 0, 45, 68, 255);
        dot3k_bl_update_brightnesses(d);
        usleep(50000);
    }
    for(int i = 0; i < 100; i++) {
        float f = i / 100.0f;
        dot3k_bl_set_bar_graph(d, f, 0x30);
        dot3k_bl_update_brightnesses(d);
        usleep(10000);
    }
    //dot3k_bl_enable(d, 0);
    return 0;
}
