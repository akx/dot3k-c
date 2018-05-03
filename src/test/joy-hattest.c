#include "dothat.h"
#include <unistd.h>
#include <stdio.h>

int main() {
    DOTHAT* d = dothat_init();
    
		dothat_input_recalibrate(d);
		
		// TODO: Fix test
		/*
    for(;;) {
        uint8_t state = dothat_joy_poll(d);
        printf("%02X : %2s %2s %2s %2s %2s\n",
            state,
            ((state & DOTHAT_JOY_LEFT) ? "lf" : ""),
            ((state & DOTHAT_JOY_UP) ? "up" : ""),
            ((state & DOTHAT_JOY_DOWN) ? "dn" : ""),
            ((state & DOTHAT_JOY_RIGHT) ? "rt" : ""),
            ((state & DOTHAT_JOY_BTN) ? "button" : "")
        );
        usleep(50000);
    }
		*/

    dothat_shutdown(d);
}
