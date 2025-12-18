#ifndef SPRITE_H
#define SPRITE_H

/* Sprite OAM structure */
typedef struct {
    unsigned char y;          /* Y position */
    unsigned char tile;       /* Tile number */
    unsigned char attributes; /* Attributes (palette, flip, priority) */
    unsigned char x;          /* X position */
} sprite_t;

/* OAM buffer (64 sprites max) */
extern sprite_t oam_buffer[64];

/* Clear all sprites */
void clear_sprites(void);

/* Update OAM (call during vblank) */
void update_sprites(void);

#endif /* SPRITE_H */
