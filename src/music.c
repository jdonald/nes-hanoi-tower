#include "nes.h"
#include "music.h"

/* Jingle Bells melody (simplified) */
const music_note_t jingle_bells[] = {
    {NOTE_E4, 15}, {NOTE_E4, 15}, {NOTE_E4, 30},
    {NOTE_E4, 15}, {NOTE_E4, 15}, {NOTE_E4, 30},
    {NOTE_E4, 15}, {NOTE_G4, 15}, {NOTE_C4, 15}, {NOTE_D4, 15},
    {NOTE_E4, 60},
    {NOTE_F4, 15}, {NOTE_F4, 15}, {NOTE_F4, 15}, {NOTE_F4, 15},
    {NOTE_F4, 15}, {NOTE_E4, 15}, {NOTE_E4, 15}, {NOTE_E4, 8}, {NOTE_E4, 7},
    {NOTE_E4, 15}, {NOTE_D4, 15}, {NOTE_D4, 15}, {NOTE_E4, 15},
    {NOTE_D4, 30}, {NOTE_G4, 30},
    {NOTE_E4, 15}, {NOTE_E4, 15}, {NOTE_E4, 30},
    {NOTE_E4, 15}, {NOTE_E4, 15}, {NOTE_E4, 30},
    {NOTE_E4, 15}, {NOTE_G4, 15}, {NOTE_C4, 15}, {NOTE_D4, 15},
    {NOTE_E4, 60},
    {NOTE_F4, 15}, {NOTE_F4, 15}, {NOTE_F4, 15}, {NOTE_F4, 15},
    {NOTE_F4, 15}, {NOTE_E4, 15}, {NOTE_E4, 15}, {NOTE_E4, 8}, {NOTE_E4, 7},
    {NOTE_G4, 15}, {NOTE_G4, 15}, {NOTE_F4, 15}, {NOTE_D4, 15},
    {NOTE_C4, 60},
    {NOTE_REST, 0}  /* End marker */
};

/* Ode to Joy melody (Beethoven's 9th Symphony) */
const music_note_t ode_to_joy[] = {
    {NOTE_E4, 20}, {NOTE_E4, 20}, {NOTE_F4, 20}, {NOTE_G4, 20},
    {NOTE_G4, 20}, {NOTE_F4, 20}, {NOTE_E4, 20}, {NOTE_D4, 20},
    {NOTE_C4, 20}, {NOTE_C4, 20}, {NOTE_D4, 20}, {NOTE_E4, 20},
    {NOTE_E4, 30}, {NOTE_D4, 10}, {NOTE_D4, 40},

    {NOTE_E4, 20}, {NOTE_E4, 20}, {NOTE_F4, 20}, {NOTE_G4, 20},
    {NOTE_G4, 20}, {NOTE_F4, 20}, {NOTE_E4, 20}, {NOTE_D4, 20},
    {NOTE_C4, 20}, {NOTE_C4, 20}, {NOTE_D4, 20}, {NOTE_E4, 20},
    {NOTE_D4, 30}, {NOTE_C4, 10}, {NOTE_C4, 40},

    {NOTE_D4, 20}, {NOTE_D4, 20}, {NOTE_E4, 20}, {NOTE_C4, 20},
    {NOTE_D4, 20}, {NOTE_E4, 10}, {NOTE_F4, 10}, {NOTE_E4, 20}, {NOTE_C4, 20},
    {NOTE_D4, 20}, {NOTE_E4, 10}, {NOTE_F4, 10}, {NOTE_E4, 20}, {NOTE_D4, 20},
    {NOTE_C4, 20}, {NOTE_D4, 20}, {NOTE_G3, 40},

    {NOTE_E4, 20}, {NOTE_E4, 20}, {NOTE_F4, 20}, {NOTE_G4, 20},
    {NOTE_G4, 20}, {NOTE_F4, 20}, {NOTE_E4, 20}, {NOTE_D4, 20},
    {NOTE_C4, 20}, {NOTE_C4, 20}, {NOTE_D4, 20}, {NOTE_E4, 20},
    {NOTE_D4, 30}, {NOTE_C4, 10}, {NOTE_C4, 40},

    {NOTE_REST, 0}  /* End marker */
};

/* Music state */
static const music_note_t* current_song = 0;
static unsigned char note_index = 0;
static unsigned char note_timer = 0;
static unsigned char music_playing = 0;

/* Initialize the APU for music */
void init_music(void) {
    /* Enable square wave channels */
    APU_STATUS = 0x0F;

    /* Configure pulse channel 1 */
    APU_PULSE1_CTRL = 0xBF;    /* Duty cycle 50%, constant volume */
    APU_PULSE1_SWEEP = 0x08;    /* No sweep */

    /* Configure triangle channel for bass */
    APU_TRIANGLE_CTRL = 0x81;

    music_playing = 0;
}

/* Start playing a song */
void play_song(unsigned char song_id) {
    switch (song_id) {
        case SONG_JINGLE_BELLS:
            current_song = jingle_bells;
            break;
        case SONG_ODE_TO_JOY:
            current_song = ode_to_joy;
            break;
        default:
            current_song = 0;
            break;
    }

    note_index = 0;
    note_timer = 0;
    music_playing = (current_song != 0);
}

/* Stop music playback */
void stop_music(void) {
    music_playing = 0;
    APU_PULSE1_CTRL = 0x30;  /* Silence */
}

/* Update music (call once per frame) */
void update_music(void) {
    unsigned int note;

    if (!music_playing || current_song == 0) {
        return;
    }

    /* Check if current note is finished */
    if (note_timer > 0) {
        note_timer--;
        return;
    }

    /* Get next note */
    note = current_song[note_index].note;

    /* Check for end of song */
    if (note == NOTE_REST && current_song[note_index].duration == 0) {
        /* Loop the song */
        note_index = 0;
        note = current_song[note_index].note;
    }

    /* Play the note */
    if (note == NOTE_REST) {
        APU_PULSE1_CTRL = 0x30;  /* Silence */
    } else {
        APU_PULSE1_CTRL = 0xBF;
        APU_PULSE1_TIMER_LO = (unsigned char)(note & 0xFF);
        APU_PULSE1_TIMER_HI = (unsigned char)(note >> 8);
    }

    /* Set note duration */
    note_timer = current_song[note_index].duration;
    note_index++;
}
