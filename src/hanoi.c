#include "nes.h"
#include "hanoi.h"
#include "text.h"

/* Block colors from smallest to largest */
const unsigned char block_colors[MAX_BLOCKS] = {
    COLOR_MAGENTA,      /* Block 1 (smallest) */
    COLOR_RED,          /* Block 2 */
    COLOR_ORANGE,       /* Block 3 */
    COLOR_YELLOW,       /* Block 4 */
    COLOR_YELLOW_GREEN, /* Block 5 */
    COLOR_GREEN,        /* Block 6 */
    COLOR_TEAL,         /* Block 7 */
    COLOR_DEEP_BLUE     /* Block 8 (largest) */
};

/* Initialize game state */
void init_game(game_state_t* game) {
    unsigned char i, j;

    game->level = 1;
    game->lives = 3;

    /* Clear all towers */
    for (i = 0; i < NUM_TOWERS; i++) {
        game->tower_heights[i] = 0;
        for (j = 0; j < MAX_BLOCKS; j++) {
            game->towers[i][j] = 0;
        }
    }

    game->selected_tower = 0;
    game->holding_block = 0;
    game->holding_from = 0;

    start_level(game);
}

/* Calculate minimum moves for n blocks (2^n - 1) */
static unsigned char calc_min_moves(unsigned char n) {
    unsigned char result = 1;
    unsigned char i;

    for (i = 0; i < n; i++) {
        result *= 2;
    }

    return result - 1;
}

/* Start a new level */
void start_level(game_state_t* game) {
    unsigned char i, j;

    /* Set number of blocks based on level */
    game->num_blocks = game->level;
    if (game->num_blocks > MAX_BLOCKS) {
        game->num_blocks = MAX_BLOCKS;
    }

    /* Calculate minimum moves */
    game->min_moves = calc_min_moves(game->num_blocks);
    game->moves = 0;

    /* Clear all towers */
    for (i = 0; i < NUM_TOWERS; i++) {
        game->tower_heights[i] = 0;
        for (j = 0; j < MAX_BLOCKS; j++) {
            game->towers[i][j] = 0;
        }
    }

    /* Place all blocks on first tower (largest to smallest) */
    for (i = 0; i < game->num_blocks; i++) {
        game->towers[0][i] = game->num_blocks - i;
    }
    game->tower_heights[0] = game->num_blocks;

    game->selected_tower = 0;
    game->holding_block = 0;
}

/* Pick up a block from a tower */
unsigned char pickup_block(game_state_t* game, unsigned char tower) {
    if (tower >= NUM_TOWERS) {
        return 0;  /* Invalid tower */
    }

    if (game->tower_heights[tower] == 0) {
        return 0;  /* No blocks on this tower */
    }

    /* Pick up the top block */
    game->tower_heights[tower]--;
    game->holding_block = game->towers[tower][game->tower_heights[tower]];
    game->towers[tower][game->tower_heights[tower]] = 0;
    game->holding_from = tower;

    return 1;  /* Success */
}

/* Place a block on a tower */
unsigned char place_block(game_state_t* game, unsigned char tower) {
    if (tower >= NUM_TOWERS) {
        return 0;  /* Invalid tower */
    }

    if (game->holding_block == 0) {
        return 0;  /* Not holding a block */
    }

    /* Check if placement is valid (can't place larger block on smaller one) */
    if (game->tower_heights[tower] > 0) {
        unsigned char top_block = game->towers[tower][game->tower_heights[tower] - 1];
        if (game->holding_block > top_block) {
            return 0;  /* Invalid move - larger block on smaller */
        }
    }

    /* Place the block */
    game->towers[tower][game->tower_heights[tower]] = game->holding_block;
    game->tower_heights[tower]++;
    game->holding_block = 0;

    /* Increment move counter only if not returning to original tower */
    if (tower != game->holding_from) {
        game->moves++;
    }

    return 1;  /* Success */
}

/* Check if level is complete */
unsigned char check_win(game_state_t* game) {
    /* Win condition: all blocks on tower 2 (rightmost) */
    if (game->tower_heights[2] == game->num_blocks) {
        /* Check if done in minimum moves */
        if (game->moves == game->min_moves) {
            return 1;  /* Win */
        } else {
            return 2;  /* Complete but not optimal - lose a life */
        }
    }
    return 0;  /* Not complete */
}

/* Render the game to the screen */
void render_game(game_state_t* game) {
    unsigned char tower, block, row, col;
    unsigned char block_width;
    unsigned char tower_x[NUM_TOWERS] = {5, 14, 23};  /* X positions of towers */

    /* Clear screen first */
    clear_screen();

    /* Display level info */
    write_text(2, 1, "LEVEL");
    write_text(8, 1, "1");  /* Will improve number rendering later */

    /* Display lives */
    write_text(14, 1, "LIVES");

    /* Display moves */
    write_text(2, 3, "MOVES");

    /* Draw towers */
    for (tower = 0; tower < NUM_TOWERS; tower++) {
        /* Draw tower pole */
        for (row = 0; row < 10; row++) {
            PPU_STATUS;
            PPU_ADDR = 0x20 + (((row + 10) * 32 + tower_x[tower]) >> 8);
            PPU_ADDR = (((row + 10) * 32 + tower_x[tower]) & 0xFF);
            PPU_DATA = 0x06;  /* Tower pole tile */
        }

        /* Draw tower base */
        PPU_STATUS;
        PPU_ADDR = 0x20 + ((20 * 32 + tower_x[tower]) >> 8);
        PPU_ADDR = ((20 * 32 + tower_x[tower]) & 0xFF);
        PPU_DATA = 0x07;  /* Tower base tile */

        /* Draw blocks on this tower */
        for (block = 0; block < game->tower_heights[tower]; block++) {
            unsigned char block_num = game->towers[tower][block];
            block_width = block_num;
            row = 19 - block;

            /* Draw the block (centered on tower) */
            for (col = 0; col < block_width; col++) {
                PPU_STATUS;
                PPU_ADDR = 0x20 + ((row * 32 + tower_x[tower] - block_width/2 + col) >> 8);
                PPU_ADDR = ((row * 32 + tower_x[tower] - block_width/2 + col) & 0xFF);
                PPU_DATA = 0x01;  /* Solid block tile */
            }
        }
    }

    /* Draw cursor/selector */
    PPU_STATUS;
    PPU_ADDR = 0x20 + ((9 * 32 + tower_x[game->selected_tower]) >> 8);
    PPU_ADDR = ((9 * 32 + tower_x[game->selected_tower]) & 0xFF);
    PPU_DATA = 0x21;  /* Cursor character */

    /* Enable rendering */
    PPU_MASK = PPU_MASK_SHOW_BG | PPU_MASK_SHOW_SPRITES;
}
