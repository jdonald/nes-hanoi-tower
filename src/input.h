#ifndef INPUT_H
#define INPUT_H

/* Controller state variables */
extern unsigned char controller1;
extern unsigned char controller1_prev;

/* Read controller input */
void read_controller(void);

/* Check if button was just pressed (not held) */
unsigned char button_pressed(unsigned char button);

/* Check if button is currently held down */
unsigned char button_held(unsigned char button);

#endif /* INPUT_H */
