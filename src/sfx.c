#include "nes.h"
#include "music.h"
#include "sfx.h"

static const music_note_t success_sfx[] = {
    {NOTE_C5, 6},
    {NOTE_E5, 6},
    {NOTE_G5, 12},
    {NOTE_REST, 0}
};

static const music_note_t fail_sfx[] = {
    {NOTE_E3, 10},
    {NOTE_D3, 10},
    {NOTE_C3, 20},
    {NOTE_REST, 0}
};

static const music_note_t* current_sfx = 0;
static unsigned char sfx_index = 0;
static unsigned char sfx_timer = 0;

void init_sfx(void) {
    /* Configure pulse channel 2 for short SFX (music uses pulse 1). */
    APU_PULSE2_CTRL = 0x30;   /* Silence */
    APU_PULSE2_SWEEP = 0x08;  /* No sweep */
    current_sfx = 0;
    sfx_index = 0;
    sfx_timer = 0;
}

static void start_sfx(const music_note_t* sfx) {
    current_sfx = sfx;
    sfx_index = 0;
    sfx_timer = 0;
}

void play_sfx_success(void) {
    start_sfx(success_sfx);
}

void play_sfx_fail(void) {
    start_sfx(fail_sfx);
}

void update_sfx(void) {
    unsigned int note;

    if (current_sfx == 0) {
        return;
    }

    if (sfx_timer > 0) {
        sfx_timer--;
        return;
    }

    note = current_sfx[sfx_index].note;

    if (note == NOTE_REST && current_sfx[sfx_index].duration == 0) {
        current_sfx = 0;
        APU_PULSE2_CTRL = 0x30;
        return;
    }

    if (note == NOTE_REST) {
        APU_PULSE2_CTRL = 0x30;
    } else {
        /* 50% duty, constant volume (louder than music so it's noticeable). */
        APU_PULSE2_CTRL = 0xBF;
        APU_PULSE2_TIMER_LO = (unsigned char)(note & 0xFF);
        APU_PULSE2_TIMER_HI = (unsigned char)((note >> 8) & 0x07);
    }

    sfx_timer = current_sfx[sfx_index].duration;
    sfx_index++;
}

