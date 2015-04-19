#include "dot3k_private.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#define NOT_OPEN(dot3k) (dot3k->joystick_fdset == NULL)
#define N_JOYPINS 5

typedef struct {
    uint8_t mnemonic;
    uint8_t gpio_pin;
} JOYPIN;

JOYPIN joypins[N_JOYPINS] = {
    {DOT3K_JOY_UP, 27},
    {DOT3K_JOY_LEFT, 17},
    {DOT3K_JOY_RIGHT, 22},
    {DOT3K_JOY_DOWN, 9},
    {DOT3K_JOY_BTN, 4},
};

int dot3k_joy_open(DOT3K *dot3k) {
    if(rpi_gpio_open()) return 1;
    for(int i = 0; i < N_JOYPINS; i++) {
        rpi_gpio_setup_pin(joypins[i].gpio_pin, GPIO_IN, GPIO_PUD_UP);
    }
    return 0;
}

int dot3k_joy_close(DOT3K *dot3k) {
    // No point in actually closing the GPIO mapping
    return 0;
}

uint8_t dot3k_joy_poll(DOT3K *dot3k) {
    int len, rc;
    char buf[8];
    uint8_t new_state;
    for(int i = 0; i < N_JOYPINS; i++) {
        if(!rpi_gpio_input(joypins[i].gpio_pin)) new_state |= joypins[i].mnemonic;
    }
    dot3k->joystick_state = new_state;
    return dot3k->joystick_state;
}
