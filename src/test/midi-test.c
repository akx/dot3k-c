#include "dot3k.h"
#include <portmidi.h>
#include <porttime.h>
#include <stdio.h>

static void list_devices() {
    int default_in, default_out;
    /* list device information */
    default_in = Pm_GetDefaultInputDeviceID();
    default_out = Pm_GetDefaultOutputDeviceID();
    for (int i = 0; i < Pm_CountDevices(); i++) {
        char *deflt;
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
        printf("%d: %s, %s", i, info->interf, info->name);
        if (info->input) {
            deflt = (i == default_in ? "default " : "");
            printf(" (%sinput)", deflt);
        }
        if (info->output) {
            deflt = (i == default_out ? "default " : "");
            printf(" (%soutput)", deflt);
        }
        printf("\n");
    }
}

int main() {
    PmStream * midi;
    PmError status, length;    
    PmEvent buffer[1];
    int i = 0;
    DOT3K* d = dot3k_init();
    
    
    list_devices();
    Pt_Start(1, 0, 0);
    Pm_OpenInput(&midi, Pm_GetDefaultInputDeviceID(), NULL, 100, Pt_Time, NULL);
    Pm_SetFilter(midi, PM_FILT_ACTIVE | PM_FILT_CLOCK | PM_FILT_SYSEX);
   
    if(dot3k_bl_open(d)) return 1;
    dot3k_bl_enable(d, 1);
    dot3k_bl_enable_leds(d, 0xFFFFFF);
    dot3k_bl_set_brightness(d, 0, 18, 0);
    dot3k_bl_update_brightnesses(d);
    dot3k_bl_calibrate(d, 2.4, 1.6, 1.4, 1);
    
    uint8_t brightness[18] = {0};
    for(;;) {
        status = Pm_Poll(midi);
        if (status == TRUE) {
            length = Pm_Read(midi,buffer, 1);
            if (length <= 0) continue;
            if(Pm_MessageStatus(buffer[0].message) == 0xB0) {
                uint8_t led = Pm_MessageData1(buffer[0].message) % 18;
                uint8_t brt = Pm_MessageData2(buffer[0].message) % 255;
                printf("%02d: %02d\n", led, brt);
                brightness[led] = brt;
                dot3k_bl_set_brightnesses(d, 0, 18, brightness);
                dot3k_bl_update_brightnesses(d);
                continue;
            } 
            printf("Got message %d: time %ld, %2lx %2lx %2lx\n",
                   i,
                   (long) buffer[0].timestamp,
                   (long) Pm_MessageStatus(buffer[0].message),
                   (long) Pm_MessageData1(buffer[0].message),
                   (long) Pm_MessageData2(buffer[0].message));
            i++;
        }
    }
    /*
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
        dot3k_bl_set_bar_graph(d, f, 0.2);
        dot3k_bl_update_brightnesses(d);
        usleep(10000);
    }
    //dot3k_bl_enable(d, 0);
    * */
    return 0;
}
