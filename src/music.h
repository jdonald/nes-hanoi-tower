#ifndef MUSIC_H
#define MUSIC_H

/* Note definitions (period values for APU) */
#define NOTE_C3  0x07F1
#define NOTE_D3  0x0713
#define NOTE_E3  0x0640
#define NOTE_F3  0x05F3
#define NOTE_G3  0x0580
#define NOTE_A3  0x04FA
#define NOTE_B3  0x047C
#define NOTE_C4  0x03F8
#define NOTE_D4  0x0389
#define NOTE_E4  0x031F
#define NOTE_F4  0x02F9
#define NOTE_G4  0x02BF
#define NOTE_A4  0x027C
#define NOTE_B4  0x023D
#define NOTE_C5  0x01FB
#define NOTE_D5  0x01C4
#define NOTE_E5  0x018F
#define NOTE_F5  0x017C
#define NOTE_G5  0x015F
#define NOTE_A5  0x013D
#define NOTE_B5  0x011E
#define NOTE_REST 0x0000

/* Music track structure */
typedef struct {
    unsigned int note;     /* Note frequency */
    unsigned char duration; /* Note duration in frames */
} music_note_t;

/* Song selections */
enum {
    SONG_NONE = 0,
    SONG_JINGLE_BELLS,
    SONG_ODE_TO_JOY
};

/* Music control functions */
void init_music(void);
void play_song(unsigned char song_id);
void stop_music(void);
void update_music(void);

#endif /* MUSIC_H */
