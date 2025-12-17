#include "nes.h"
#include "input.h"

unsigned char controller1;
unsigned char controller1_prev;

/* Read controller state */
void read_controller(void) {
    unsigned char i;

    /* Save previous state */
    controller1_prev = controller1;
    controller1 = 0;

    /* Strobe the controller */
    CONTROLLER1 = 1;
    CONTROLLER1 = 0;

    /* Read 8 buttons */
    for (i = 0; i < 8; i++) {
        controller1 <<= 1;
        controller1 |= (CONTROLLER1 & 1);
    }
}

/* Check if button was just pressed this frame */
unsigned char button_pressed(unsigned char button) {
    return (controller1 & button) && !(controller1_prev & button);
}

/* Check if button is currently held down */
unsigned char button_held(unsigned char button) {
    return (controller1 & button);
}
