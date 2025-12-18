#include "nes.h"
#include "sprite.h"

/* OAM buffer - must be page-aligned ($xx00) */
#pragma bss-name(push, "OAM")
sprite_t oam_buffer[64];
#pragma bss-name(pop)

/* Clear all sprites (move them offscreen) */
void clear_sprites(void) {
    unsigned char i;
    for (i = 0; i < 64; i++) {
        oam_buffer[i].y = 0xFF;  /* Offscreen */
        oam_buffer[i].tile = 0x00;
        oam_buffer[i].attributes = 0;
        oam_buffer[i].x = 0xFF;  /* Offscreen */
    }
}

/* Update OAM with DMA transfer */
void update_sprites(void) {
    /* Set OAM address to 0 */
    OAM_ADDR = 0x00;

    /* Trigger DMA from oam_buffer to OAM */
    /* DMA transfers 256 bytes from $xx00 to OAM */
    OAM_DMA = (unsigned char)((unsigned int)oam_buffer >> 8);
}
