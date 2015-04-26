#include "dot3k.h"
#include <unistd.h>
#include <stdio.h>

int main() {
	DOT3K* d = dot3k_init();
	if(dot3k_joy_open(d)) {
        perror("joy open");
        return 1;
    }
    for(;;) {
        uint8_t state = dot3k_joy_poll(d);
        printf("%02X : %2s %2s %2s %2s %2s\n",
            state,
            ((state & DOT3K_JOY_LEFT) ? "lf" : ""),
            ((state & DOT3K_JOY_UP) ? "up" : ""),
            ((state & DOT3K_JOY_DOWN) ? "dn" : ""),
            ((state & DOT3K_JOY_RIGHT) ? "rt" : ""),
            ((state & DOT3K_JOY_BTN) ? "button" : "")
        );
        usleep(50000);
    }

    dot3k_joy_close(d);
}
