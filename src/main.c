#include "nes.h"
#include "text.h"
#include "input.h"
#include "music.h"
#include "hanoi.h"

/* Game states */
enum {
    STATE_TITLE,
    STATE_GAMEPLAY,
    STATE_LEVEL_COMPLETE,
    STATE_GAME_OVER,
    STATE_WIN_GAME
};

/* Global game state */
static unsigned char game_state;
static game_state_t hanoi_game;
static unsigned char frame_counter;
static unsigned char level_complete_timer;

/* Initialize NES hardware */
void init_nes(void) {
    /* Disable rendering during setup */
    PPU_CTRL = 0;
    PPU_MASK = 0;

    /* Initialize palette */
    PPU_STATUS;
    PPU_ADDR = 0x3F;
    PPU_ADDR = 0x00;

    /* Background palette 0 - will be set dynamically by set_bg_color */
    PPU_DATA = COLOR_BLACK;       /* Universal background color */
    PPU_DATA = COLOR_WHITE;       /* Text color */
    PPU_DATA = COLOR_GRAY;        /* Color 2 */
    PPU_DATA = COLOR_DARK_GRAY;   /* Color 3 */

    /* Background palette 1 */
    PPU_DATA = COLOR_BLACK;
    PPU_DATA = COLOR_RED;
    PPU_DATA = COLOR_YELLOW;
    PPU_DATA = COLOR_GREEN;

    /* Background palette 2 */
    PPU_DATA = COLOR_BLACK;
    PPU_DATA = COLOR_MAGENTA;
    PPU_DATA = COLOR_ORANGE;
    PPU_DATA = COLOR_TEAL;

    /* Background palette 3 - for title screen text */
    PPU_DATA = COLOR_BLACK;
    PPU_DATA = COLOR_TEAL;      /* Cyan text */
    PPU_DATA = COLOR_WHITE;
    PPU_DATA = COLOR_BLUE;

    /* Sprite palette 0 - Blocks 1-3 (Magenta, Red, Orange) */
    PPU_DATA = COLOR_BLACK;
    PPU_DATA = COLOR_MAGENTA;
    PPU_DATA = COLOR_RED;
    PPU_DATA = COLOR_ORANGE;

    /* Sprite palette 1 - Blocks 4-6 (Yellow, Yellow-Green, Green) */
    PPU_DATA = COLOR_BLACK;
    PPU_DATA = COLOR_YELLOW;
    PPU_DATA = COLOR_YELLOW_GREEN;
    PPU_DATA = COLOR_GREEN;

    /* Sprite palette 2 - Blocks 7-8 (Teal, Deep Blue) */
    PPU_DATA = COLOR_BLACK;
    PPU_DATA = COLOR_TEAL;
    PPU_DATA = COLOR_DEEP_BLUE;
    PPU_DATA = COLOR_WHITE;

    /* Sprite palette 3 - Unused */
    PPU_DATA = COLOR_BLACK;
    PPU_DATA = COLOR_WHITE;
    PPU_DATA = COLOR_WHITE;
    PPU_DATA = COLOR_WHITE;

    /* Clear any previous screen data (fixes soft reset residue) */
    clear_screen();

    /* Reset scroll position */
    PPU_STATUS;
    PPU_SCROLL = 0;
    PPU_SCROLL = 0;

    /* Enable NMI */
    PPU_CTRL = PPU_CTRL_NMI;
}

/* Display title screen */
void show_title_screen(void) {
    unsigned int addr;
    unsigned char i;

    /* Set pink background for title screen */
    set_bg_color(COLOR_PINK);

    /* Disable rendering for PPU writes */
    PPU_MASK = 0;

    /* Clear nametable */
    PPU_STATUS;
    PPU_ADDR = 0x20;
    PPU_ADDR = 0x00;
    for (i = 0; i < 240; i++) {
        PPU_DATA = 0x00;
        PPU_DATA = 0x00;
        PPU_DATA = 0x00;
        PPU_DATA = 0x00;
    }

    /* Clear attribute table to use palette 0 (white text) */
    for (i = 0; i < 64; i++) {
        PPU_DATA = 0x00;
    }

    /* Write "TOWER OF HANOI" title at row 8 (centered) */
    addr = 0x2000 + (8 * 32) + 7;
    PPU_STATUS;
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);
    PPU_DATA = 0x54; /* T */
    PPU_DATA = 0x4F; /* O */
    PPU_DATA = 0x57; /* W */
    PPU_DATA = 0x45; /* E */
    PPU_DATA = 0x52; /* R */
    PPU_DATA = 0x00; /* space */
    PPU_DATA = 0x4F; /* O */
    PPU_DATA = 0x46; /* F */
    PPU_DATA = 0x00; /* space */
    PPU_DATA = 0x48; /* H */
    PPU_DATA = 0x41; /* A */
    PPU_DATA = 0x4E; /* N */
    PPU_DATA = 0x4F; /* O */
    PPU_DATA = 0x49; /* I */

    /* Write "PRESS START" at row 16 */
    addr = 0x2000 + (16 * 32) + 8;
    PPU_STATUS;
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);
    PPU_DATA = 0x50; /* P */
    PPU_DATA = 0x52; /* R */
    PPU_DATA = 0x45; /* E */
    PPU_DATA = 0x53; /* S */
    PPU_DATA = 0x53; /* S */
    PPU_DATA = 0x00; /* space */
    PPU_DATA = 0x53; /* S */
    PPU_DATA = 0x54; /* T */
    PPU_DATA = 0x41; /* A */
    PPU_DATA = 0x52; /* R */
    PPU_DATA = 0x54; /* T */

    /* Set attribute table to use palette 3 (cyan/blue) for title text area */
    addr = 0x23C0 + 16;  /* Attribute byte for row 8 area */
    PPU_STATUS;
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);
    for (i = 0; i < 8; i++) {
        PPU_DATA = 0xFF;  /* Palette 3 for all quadrants */
    }

    /* Reset scroll and enable rendering */
    PPU_STATUS;
    PPU_SCROLL = 0;
    PPU_SCROLL = 0;
    PPU_CTRL = PPU_CTRL_NMI;
    PPU_MASK = PPU_MASK_SHOW_BG;
}

/* Display level complete screen */
void show_level_complete(void) {
    unsigned int addr;
    unsigned char i;

    /* Disable rendering for PPU writes */
    PPU_MASK = 0;

    /* Clear nametable */
    PPU_STATUS;
    PPU_ADDR = 0x20;
    PPU_ADDR = 0x00;
    for (i = 0; i < 240; i++) {
        PPU_DATA = 0x00;
        PPU_DATA = 0x00;
        PPU_DATA = 0x00;
        PPU_DATA = 0x00;
    }

    /* Clear attribute table */
    for (i = 0; i < 64; i++) {
        PPU_DATA = 0x00;
    }

    /* Write "NICE!" centered */
    addr = 0x2000 + (12 * 32) + 14;
    PPU_STATUS;
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);
    PPU_DATA = 0x4E; /* N */
    PPU_DATA = 0x49; /* I */
    PPU_DATA = 0x43; /* C */
    PPU_DATA = 0x45; /* E */
    PPU_DATA = 0x21; /* ! */

    /* Reset scroll and enable rendering */
    PPU_STATUS;
    PPU_SCROLL = 0;
    PPU_SCROLL = 0;
    PPU_CTRL = PPU_CTRL_NMI;
    PPU_MASK = PPU_MASK_SHOW_BG;
}

/* Display failed level screen */
void show_level_failed(void) {
    clear_screen();

    write_text(9, 8, "TOO MANY MOVES");
    write_text(10, 10, "LIFE LOST");
    write_text(7, 14, "PRESS START");

    PPU_CTRL = PPU_CTRL_NMI;
    PPU_MASK = PPU_MASK_SHOW_BG;
}

/* Display game over screen */
void show_game_over(void) {
    clear_screen();

    write_text(10, 10, "GAME OVER");
    write_text(7, 14, "PRESS START");

    PPU_CTRL = PPU_CTRL_NMI;
    PPU_MASK = PPU_MASK_SHOW_BG;
}

/* Display win screen */
void show_win_screen(void) {
    clear_screen();

    write_text(9, 8, "YOU WIN");
    write_text(6, 12, "ALL LEVELS COMPLETE");
    write_text(7, 16, "PRESS START");

    PPU_CTRL = PPU_CTRL_NMI;
    PPU_MASK = PPU_MASK_SHOW_BG;
}

/* Wait for vblank */
void wait_vblank(void) {
    while (!(PPU_STATUS & PPU_STATUS_VBLANK));
}

/* Main function */
void main(void) {
    unsigned char win_status;

    /* Initialize hardware */
    init_nes();
    init_music();

    /* Initialize game state */
    game_state = STATE_TITLE;
    frame_counter = 0;

    /* Show title screen */
    show_title_screen();
    play_song(SONG_JINGLE_BELLS);

    /* Main game loop */
    while (1) {
        /* Wait for vblank and update music */
        wait_vblank();
        update_music();
        frame_counter++;

        /* Read controller input */
        read_controller();

        switch (game_state) {
            case STATE_TITLE:
                /* Wait for start button */
                if (button_pressed(BUTTON_START)) {
                    init_game(&hanoi_game);
                    game_state = STATE_GAMEPLAY;
                    stop_music();
                    play_song(SONG_ODE_TO_JOY);
                    /* Set light blue background for gameplay */
                    set_bg_color(COLOR_LIGHT_BLUE);
                    render_game(&hanoi_game);
                }
                break;

            case STATE_GAMEPLAY:
                /* Handle gameplay input */
                if (button_pressed(BUTTON_LEFT)) {
                    if (hanoi_game.selected_tower > 0) {
                        hanoi_game.selected_tower--;
                        render_game(&hanoi_game);
                    }
                }
                else if (button_pressed(BUTTON_RIGHT)) {
                    if (hanoi_game.selected_tower < NUM_TOWERS - 1) {
                        hanoi_game.selected_tower++;
                        render_game(&hanoi_game);
                    }
                }
                else if (button_pressed(BUTTON_A)) {
                    unsigned char should_render = 1;
                    if (hanoi_game.holding_block == 0) {
                        /* Try to pick up a block */
                        pickup_block(&hanoi_game, hanoi_game.selected_tower);
                    } else {
                        /* Try to place the block */
                        if (place_block(&hanoi_game, hanoi_game.selected_tower)) {
                            /* Check for win */
                            win_status = check_win(&hanoi_game);
                            if (win_status == 1) {
                                /* Perfect win */
                                hanoi_game.level++;
                                if (hanoi_game.level > 8) {
                                    game_state = STATE_WIN_GAME;
                                    stop_music();
                                    show_win_screen();
                                    should_render = 0;
                                } else {
                                    game_state = STATE_LEVEL_COMPLETE;
                                    level_complete_timer = 120;  /* 2 seconds at 60 FPS */
                                    show_level_complete();
                                    should_render = 0;
                                }
                            } else if (win_status == 2) {
                                /* Complete but not optimal - lose a life */
                                hanoi_game.lives--;
                                if (hanoi_game.lives == 0) {
                                    game_state = STATE_GAME_OVER;
                                    stop_music();
                                    show_game_over();
                                    should_render = 0;
                                } else {
                                    show_level_failed();
                                    start_level(&hanoi_game);
                                    /* Brief pause then re-render */
                                    for (win_status = 0; win_status < 120; win_status++) {
                                        wait_vblank();
                                    }
                                    render_game(&hanoi_game);
                                    should_render = 0;
                                }
                            }
                        }
                    }
                    if (should_render) {
                        render_game(&hanoi_game);
                    }
                }
                else if (button_pressed(BUTTON_B)) {
                    /* Cancel - put block back */
                    if (hanoi_game.holding_block != 0) {
                        place_block(&hanoi_game, hanoi_game.holding_from);
                        hanoi_game.moves--;  /* Don't count this as a move */
                        render_game(&hanoi_game);
                    }
                }
                break;

            case STATE_LEVEL_COMPLETE:
                /* Auto-proceed after timer, or immediately on Start/A button */
                if (button_pressed(BUTTON_START) || button_pressed(BUTTON_A) || level_complete_timer == 0) {
                    start_level(&hanoi_game);
                    game_state = STATE_GAMEPLAY;
                    render_game(&hanoi_game);
                } else {
                    level_complete_timer--;
                }
                break;

            case STATE_GAME_OVER:
                /* Wait for start to restart */
                if (button_pressed(BUTTON_START)) {
                    game_state = STATE_TITLE;
                    show_title_screen();
                    play_song(SONG_JINGLE_BELLS);
                }
                break;

            case STATE_WIN_GAME:
                /* Wait for start to return to title */
                if (button_pressed(BUTTON_START)) {
                    game_state = STATE_TITLE;
                    show_title_screen();
                    play_song(SONG_JINGLE_BELLS);
                }
                break;
        }
    }
}
