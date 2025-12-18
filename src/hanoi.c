#include "nes.h"
#include "hanoi.h"
#include "text.h"
#include "sprite.h"

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
    unsigned int addr;
    unsigned char tower_x[NUM_TOWERS] = {5, 14, 23};  /* X positions of towers */
    unsigned char i;

    /* Disable rendering for all PPU writes */
    PPU_MASK = 0;

    /* Clear nametable */
    PPU_STATUS;
    PPU_ADDR = 0x20;
    PPU_ADDR = 0x00;
    for (i = 0; i < 240; i++) {  /* Clear 240 bytes at a time for speed */
        PPU_DATA = 0x00;
        PPU_DATA = 0x00;
        PPU_DATA = 0x00;
        PPU_DATA = 0x00;
    }

    /* Clear attribute table */
    for (i = 0; i < 64; i++) {
        PPU_DATA = 0x00;
    }

    /* Write "LEVEL" text at (2, 1) */
    addr = 0x2000 + (1 * 32) + 2;
    PPU_STATUS;
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);
    PPU_DATA = 0x4C; /* L */
    PPU_DATA = 0x45; /* E */
    PPU_DATA = 0x56; /* V */
    PPU_DATA = 0x45; /* E */
    PPU_DATA = 0x4C; /* L */

    /* Write level number */
    addr = 0x2000 + (1 * 32) + 8;
    PPU_STATUS;
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);
    PPU_DATA = 0x10 + game->level; /* Level number tile */

    /* Write "LIVES" text at (14, 1) */
    addr = 0x2000 + (1 * 32) + 14;
    PPU_STATUS;
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);
    PPU_DATA = 0x4C; /* L */
    PPU_DATA = 0x49; /* I */
    PPU_DATA = 0x56; /* V */
    PPU_DATA = 0x45; /* E */
    PPU_DATA = 0x53; /* S */

    /* Write lives count */
    addr = 0x2000 + (1 * 32) + 20;
    PPU_STATUS;
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);
    PPU_DATA = 0x10 + game->lives; /* Lives number tile */

    /* Write "MOVES" text at (2, 3) */
    addr = 0x2000 + (3 * 32) + 2;
    PPU_STATUS;
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);
    PPU_DATA = 0x4D; /* M */
    PPU_DATA = 0x4F; /* O */
    PPU_DATA = 0x56; /* V */
    PPU_DATA = 0x45; /* E */
    PPU_DATA = 0x53; /* S */

    /* Write moves count (0-255) next to "MOVES" at (8, 3), padded to 3 chars */
    {
        unsigned char moves = game->moves;
        unsigned char hundreds = moves / 100;
        unsigned char tens = (moves % 100) / 10;
        unsigned char ones = moves % 10;

        addr = 0x2000 + (3 * 32) + 8;
        PPU_STATUS;
        PPU_ADDR = (unsigned char)(addr >> 8);
        PPU_ADDR = (unsigned char)(addr & 0xFF);

        PPU_DATA = hundreds ? (0x10 + hundreds) : 0x00;
        PPU_DATA = (hundreds || tens) ? (0x10 + tens) : 0x00;
        PPU_DATA = 0x10 + ones;
    }

    /* Draw towers */
    for (tower = 0; tower < NUM_TOWERS; tower++) {
        /* Draw tower pole */
        for (row = 0; row < 10; row++) {
            addr = 0x2000 + ((row + 10) * 32) + tower_x[tower];
            PPU_STATUS;
            PPU_ADDR = (unsigned char)(addr >> 8);
            PPU_ADDR = (unsigned char)(addr & 0xFF);
            PPU_DATA = 0x06;  /* Tower pole tile */
        }

        /* Draw tower base */
        addr = 0x2000 + (20 * 32) + tower_x[tower];
        PPU_STATUS;
        PPU_ADDR = (unsigned char)(addr >> 8);
        PPU_ADDR = (unsigned char)(addr & 0xFF);
        PPU_DATA = 0x07;  /* Tower base tile */

        /* Draw blocks on this tower */
        for (block = 0; block < game->tower_heights[tower]; block++) {
            unsigned char block_num = game->towers[tower][block];
            block_width = block_num;
            row = 19 - block;

            /* Draw the block (centered on tower) */
            for (col = 0; col < block_width; col++) {
                addr = 0x2000 + (row * 32) + tower_x[tower] - block_width/2 + col;
                PPU_STATUS;
                PPU_ADDR = (unsigned char)(addr >> 8);
                PPU_ADDR = (unsigned char)(addr & 0xFF);
                PPU_DATA = 0x01;  /* Solid block tile */
            }
        }
    }

    /* Draw cursor/selector */
    addr = 0x2000 + (9 * 32) + tower_x[game->selected_tower];
    PPU_STATUS;
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);
    PPU_DATA = 0x21;  /* Cursor character */

    /* Set attribute table for block colors */
    /* Each attribute byte controls a 4x4 tile area (2x2 attribute areas) */
    /* Bits 0-1: top-left, 2-3: top-right, 4-5: bottom-left, 6-7: bottom-right */
    addr = 0x23C0;  /* Start of attribute table */
    PPU_STATUS;
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);

    /* First 2 rows (16 bytes) use palette 0 for text */
    for (i = 0; i < 16; i++) {
        PPU_DATA = 0x00;  /* Palette 0 (white text on background) */
    }

    /* Remaining rows use palette 2 for blocks (has magenta, orange, teal) */
    for (i = 16; i < 64; i++) {
        PPU_DATA = 0xAA;  /* Palette 2 for all quadrants (10 10 10 10 in binary) */
    }

    /* Reset scroll and enable rendering */
    PPU_STATUS;
    PPU_SCROLL = 0;
    PPU_SCROLL = 0;
    PPU_MASK = PPU_MASK_SHOW_BG;
}
