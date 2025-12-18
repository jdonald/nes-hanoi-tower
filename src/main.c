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

    /* Background palette 3 */
    PPU_DATA = COLOR_BLACK;
    PPU_DATA = COLOR_BLUE;
    PPU_DATA = COLOR_YELLOW_GREEN;
    PPU_DATA = COLOR_DEEP_BLUE;

    /* Sprite palettes */
    PPU_DATA = COLOR_BLACK;
    PPU_DATA = COLOR_WHITE;
    PPU_DATA = COLOR_RED;
    PPU_DATA = COLOR_BLUE;

    PPU_DATA = COLOR_BLACK;
    PPU_DATA = COLOR_WHITE;
    PPU_DATA = COLOR_RED;
    PPU_DATA = COLOR_BLUE;

    PPU_DATA = COLOR_BLACK;
    PPU_DATA = COLOR_WHITE;
    PPU_DATA = COLOR_RED;
    PPU_DATA = COLOR_BLUE;

    PPU_DATA = COLOR_BLACK;
    PPU_DATA = COLOR_WHITE;
    PPU_DATA = COLOR_RED;
    PPU_DATA = COLOR_BLUE;

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
    /* Set pink background for title screen */
    set_bg_color(COLOR_PINK);

    clear_screen();

    /* Display title */
    write_text(7, 8, "TOWER OF HANOI");
    write_text(8, 12, "PRESS START");

    /* Enable rendering */
    PPU_CTRL = PPU_CTRL_NMI;
    PPU_MASK = PPU_MASK_SHOW_BG;
}

/* Display level complete screen */
void show_level_complete(void) {
    clear_screen();

    write_text(8, 10, "LEVEL COMPLETE");
    write_text(7, 14, "PRESS START");

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
                                } else {
                                    game_state = STATE_LEVEL_COMPLETE;
                                    show_level_complete();
                                }
                            } else if (win_status == 2) {
                                /* Complete but not optimal - lose a life */
                                hanoi_game.lives--;
                                if (hanoi_game.lives == 0) {
                                    game_state = STATE_GAME_OVER;
                                    stop_music();
                                    show_game_over();
                                } else {
                                    show_level_failed();
                                    start_level(&hanoi_game);
                                    /* Brief pause then re-render */
                                    for (win_status = 0; win_status < 120; win_status++) {
                                        wait_vblank();
                                    }
                                    render_game(&hanoi_game);
                                }
                            }
                        }
                    }
                    render_game(&hanoi_game);
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
                /* Wait for start to continue to next level */
                if (button_pressed(BUTTON_START)) {
                    start_level(&hanoi_game);
                    game_state = STATE_GAMEPLAY;
                    render_game(&hanoi_game);
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
