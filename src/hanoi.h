#ifndef HANOI_H
#define HANOI_H

#define MAX_BLOCKS 8
#define NUM_TOWERS 3

/* Block colors (smallest to largest) */
extern const unsigned char block_colors[MAX_BLOCKS];

/* Game state structure */
typedef struct {
    unsigned char level;           /* Current level (1-8) */
    unsigned char lives;           /* Remaining lives (0-3) */
    unsigned char num_blocks;      /* Number of blocks for current level */
    unsigned char moves;           /* Current number of moves */
    unsigned char min_moves;       /* Minimum moves required (2^n - 1) */
    unsigned char towers[NUM_TOWERS][MAX_BLOCKS];  /* Tower stacks */
    unsigned char tower_heights[NUM_TOWERS];       /* Height of each tower */
    unsigned char selected_tower;  /* Currently selected tower (0-2) */
    unsigned char holding_block;   /* Block being held (0 = none, 1-8 = block) */
    unsigned char holding_from;    /* Tower block was picked from */
} game_state_t;

/* Initialize game state */
void init_game(game_state_t* game);

/* Start a new level */
void start_level(game_state_t* game);

/* Pick up a block from a tower */
unsigned char pickup_block(game_state_t* game, unsigned char tower);

/* Place a block on a tower */
unsigned char place_block(game_state_t* game, unsigned char tower);

/* Check if level is complete */
unsigned char check_win(game_state_t* game);

/* Render the game */
void render_game(game_state_t* game);

#endif /* HANOI_H */
