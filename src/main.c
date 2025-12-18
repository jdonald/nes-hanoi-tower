#include "nes.h"
#include "text.h"
#include "input.h"
#include "music.h"
#include "hanoi.h"
#include "sprite.h"
#include "sfx.h"

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
static unsigned char needs_bg_redraw;
static unsigned char needs_hud_redraw;
static unsigned char needs_sprite_rebuild;
static unsigned char needs_nice_overlay;

/* 3x3 "big font" for title screen, using solid BG tile $08 for filled pixels. */
static const unsigned char big_font[][9] = {
    /* Each character is a 3x3 tile bitmap, row-major, 1=filled */
    /* A */ {1,1,1, 1,0,1, 1,1,1},
    /* E */ {1,1,1, 1,1,0, 1,1,1},
    /* F */ {1,1,1, 1,1,0, 1,0,0},
    /* H */ {1,0,1, 1,1,1, 1,0,1},
    /* I */ {1,1,1, 0,1,0, 1,1,1},
    /* N */ {1,0,1, 1,1,1, 1,0,1},
    /* O */ {1,1,1, 1,0,1, 1,1,1},
    /* R */ {1,1,0, 1,1,1, 1,0,1},
    /* T */ {1,1,1, 0,1,0, 0,1,0},
    /* W */ {1,0,1, 1,0,1, 1,1,1}
};

enum {
    BIG_A = 0,
    BIG_E,
    BIG_F,
    BIG_H,
    BIG_I,
    BIG_N,
    BIG_O,
    BIG_R,
    BIG_T,
    BIG_W
};

static void draw_big_char(unsigned char x0, unsigned char y0, unsigned char glyph) {
    unsigned char r, c;
    unsigned int base = 0x2000 + ((unsigned int)y0 * 32) + x0;
    unsigned char big_tile = 0x08; /* Solid tile using palette color 1 */

    for (r = 0; r < 3; r++) {
        PPU_STATUS;
        PPU_ADDR = (unsigned char)((base + (unsigned int)r * 32) >> 8);
        PPU_ADDR = (unsigned char)((base + (unsigned int)r * 32) & 0xFF);
        for (c = 0; c < 3; c++) {
            PPU_DATA = big_font[glyph][r * 3 + c] ? big_tile : 0x00;
        }
    }
}

static void draw_big_word(unsigned char x0, unsigned char y0, const unsigned char* glyphs, unsigned char len) {
    unsigned char idx;
    for (idx = 0; idx < len; idx++) {
        draw_big_char((unsigned char)(x0 + idx * 4), y0, glyphs[idx]);
    }
}

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

    /* Clear attribute table (we'll set it after drawing) */
    for (i = 0; i < 64; i++) {
        PPU_DATA = 0x00;
    }

    /* Draw large title: TOWER / OF / HANOI (3x tiles), centered */
    {
        static const unsigned char tower_glyphs[] = {BIG_T, BIG_O, BIG_W, BIG_E, BIG_R};
        static const unsigned char of_glyphs[] = {BIG_O, BIG_F};
        static const unsigned char hanoi_glyphs[] = {BIG_H, BIG_A, BIG_N, BIG_O, BIG_I};

        /* Word widths in tiles: len*3 + (len-1)*1 */
        unsigned char tower_w = (unsigned char)(5 * 3 + 4);
        unsigned char of_w = (unsigned char)(2 * 3 + 1);
        unsigned char hanoi_w = (unsigned char)(5 * 3 + 4);

        unsigned char tower_x = (unsigned char)((32 - tower_w) / 2);
        unsigned char of_x = (unsigned char)((32 - of_w) / 2);
        unsigned char hanoi_x = (unsigned char)((32 - hanoi_w) / 2);

        draw_big_word(tower_x, 6, tower_glyphs, 5);
        draw_big_word(of_x, 10, of_glyphs, 2);
        draw_big_word(hanoi_x, 14, hanoi_glyphs, 5);
    }

    /* Write "PRESS START" below the title */
    write_text(10, 22, "PRESS START");

    /* Set attribute table to use palette 3 (cyan/blue) for the title screen */
    addr = 0x23C0;
    PPU_STATUS;
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);
    for (i = 0; i < 64; i++) {
        PPU_DATA = 0xFF;
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

    /* Overlay "NICE!" on top of the existing gameplay screen (no clear). */
    addr = 0x2000 + (6 * 32) + 14;
    PPU_STATUS;
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);
    PPU_DATA = 0x4E; /* N */
    PPU_DATA = 0x49; /* I */
    PPU_DATA = 0x43; /* C */
    PPU_DATA = 0x45; /* E */
    PPU_DATA = 0x21; /* ! */

    PPU_STATUS;
    PPU_SCROLL = 0;
    PPU_SCROLL = 0;
}

/* Display failed level screen */
void show_level_failed(void) {
    clear_screen();

    write_text(9, 8, "TOO MANY MOVES");
    write_text(10, 10, "LIFE LOST");
    write_text(7, 14, "PRESS START");

    PPU_CTRL = PPU_CTRL_NMI;
    PPU_MASK = PPU_MASK_SHOW_BG;
    clear_sprites();
    update_sprites();
}

/* Display game over screen */
void show_game_over(void) {
    clear_screen();

    write_text(10, 10, "GAME OVER");
    write_text(7, 14, "PRESS START");

    PPU_CTRL = PPU_CTRL_NMI;
    PPU_MASK = PPU_MASK_SHOW_BG;
    clear_sprites();
    update_sprites();
}

/* Display win screen */
void show_win_screen(void) {
    clear_screen();

    write_text(9, 8, "YOU WIN");
    write_text(6, 12, "ALL LEVELS COMPLETE");
    write_text(7, 16, "PRESS START");

    PPU_CTRL = PPU_CTRL_NMI;
    PPU_MASK = PPU_MASK_SHOW_BG;
    clear_sprites();
    update_sprites();
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
    init_sfx();

    /* Initialize game state */
    game_state = STATE_TITLE;
    frame_counter = 0;
    needs_bg_redraw = 0;
    needs_hud_redraw = 0;
    needs_sprite_rebuild = 0;
    needs_nice_overlay = 0;

    /* Show title screen */
    show_title_screen();
    play_song(SONG_JINGLE_BELLS);
    clear_sprites();
    update_sprites();

    /* Main game loop */
    while (1) {
        /* Wait for vblank and perform PPU updates first to avoid visible artifacts */
        wait_vblank();

        if (game_state == STATE_GAMEPLAY || game_state == STATE_LEVEL_COMPLETE) {
            if (needs_bg_redraw) {
                render_game_background(&hanoi_game);
                needs_bg_redraw = 0;
                needs_hud_redraw = 0;
                needs_sprite_rebuild = 1;
            }
            if (needs_hud_redraw) {
                render_game_hud(&hanoi_game);
                needs_hud_redraw = 0;
            }
            if (needs_nice_overlay) {
                show_level_complete();
                needs_nice_overlay = 0;
            }
            if (needs_sprite_rebuild) {
                build_game_sprites(&hanoi_game, (game_state == STATE_GAMEPLAY));
                needs_sprite_rebuild = 0;
            }
            update_sprites();
        }

        /* Update audio once per frame */
        update_music();
        update_sfx();
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
                    needs_bg_redraw = 1;
                }
                break;

            case STATE_GAMEPLAY:
                /* Handle gameplay input */
                if (button_pressed(BUTTON_LEFT)) {
                    if (hanoi_game.selected_tower > 0) {
                        hanoi_game.selected_tower--;
                        needs_sprite_rebuild = 1;
                    }
                }
                else if (button_pressed(BUTTON_RIGHT)) {
                    if (hanoi_game.selected_tower < NUM_TOWERS - 1) {
                        hanoi_game.selected_tower++;
                        needs_sprite_rebuild = 1;
                    }
                }
                else if (button_pressed(BUTTON_SELECT)) {
                    /* Give up on this level: lose a life and restart immediately */
                    play_sfx_fail();
                    if (hanoi_game.lives > 0) {
                        hanoi_game.lives--;
                    }
                    if (hanoi_game.lives == 0) {
                        game_state = STATE_GAME_OVER;
                        stop_music();
                        show_game_over();
                    } else {
                        start_level(&hanoi_game);
                        set_bg_color(COLOR_LIGHT_BLUE);
                        needs_bg_redraw = 1;
                    }
                }
                else if (button_pressed(BUTTON_A)) {
                    unsigned char should_render = 1;
                    if (hanoi_game.holding_block == 0) {
                        /* Try to pick up a block */
                        pickup_block(&hanoi_game, hanoi_game.selected_tower);
                        needs_sprite_rebuild = 1;
                    } else {
                        /* Try to place the block */
                        if (place_block(&hanoi_game, hanoi_game.selected_tower)) {
                            needs_hud_redraw = 1;
                            needs_sprite_rebuild = 1;
                            /* Check for win */
                            win_status = check_win(&hanoi_game);
                            if (win_status == 1) {
                                /* Perfect win */
                                play_sfx_success();
                                if (hanoi_game.level >= 8) {
                                    game_state = STATE_WIN_GAME;
                                    stop_music();
                                    show_win_screen();
                                    should_render = 0;
                                } else {
                                    game_state = STATE_LEVEL_COMPLETE;
                                    level_complete_timer = 120;  /* 2 seconds at 60 FPS */
                                    needs_nice_overlay = 1;
                                    needs_sprite_rebuild = 1; /* hide cursor during overlay */
                                    should_render = 0;
                                }
                            } else if (win_status == 2) {
                                /* Complete but not optimal - lose a life */
                                play_sfx_fail();
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
                                    game_state = STATE_GAMEPLAY;
                                    set_bg_color(COLOR_LIGHT_BLUE);
                                    needs_bg_redraw = 1;
                                    should_render = 0;
                                }
                            }
                        }
                    }
                    if (should_render) {
                        /* No full redraw needed; sprites/HUD are updated during vblank. */
                    }
                }
                else if (button_pressed(BUTTON_B)) {
                    /* Cancel - put block back */
                    if (hanoi_game.holding_block != 0) {
                        place_block(&hanoi_game, hanoi_game.holding_from);
                        hanoi_game.moves--;  /* Don't count this as a move */
                        needs_hud_redraw = 1;
                        needs_sprite_rebuild = 1;
                    }
                }
                break;

            case STATE_LEVEL_COMPLETE:
                /* Auto-proceed after timer, or immediately on Start/A button */
                if (button_pressed(BUTTON_START) || button_pressed(BUTTON_A) || level_complete_timer == 0) {
                    hanoi_game.level++;
                    start_level(&hanoi_game);
                    game_state = STATE_GAMEPLAY;
                    needs_bg_redraw = 1;
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
                    clear_sprites();
                    update_sprites();
                }
                break;

            case STATE_WIN_GAME:
                /* Wait for start to return to title */
                if (button_pressed(BUTTON_START)) {
                    game_state = STATE_TITLE;
                    show_title_screen();
                    play_song(SONG_JINGLE_BELLS);
                    clear_sprites();
                    update_sprites();
                }
                break;
        }
    }
}
