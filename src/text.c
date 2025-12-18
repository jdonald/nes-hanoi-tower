#include "nes.h"
#include "text.h"

/* Simple ASCII to tile conversion */
unsigned char ascii_to_tile(char c) {
    if (c >= '0' && c <= '9') {
        return 0x10 + (c - '0');
    }
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 0x41;  /* Uppercase letters */
    }
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 0x61;  /* Lowercase letters */
    }
    if (c == ' ') {
        return 0x00;  /* Space is blank tile */
    }
    if (c == '!') {
        return 0x21;
    }
    if (c == ':') {
        return 0x3A;
    }
    return 0x00;  /* Default to blank */
}

/* Set background color */
void set_bg_color(unsigned char color) {
    /* Disable rendering */
    PPU_MASK = 0;

    /* Set universal background color (palette address $3F00) */
    PPU_STATUS;
    PPU_ADDR = 0x3F;
    PPU_ADDR = 0x00;
    PPU_DATA = color;

    /* Reset scroll */
    PPU_STATUS;
    PPU_SCROLL = 0;
    PPU_SCROLL = 0;

    /* Re-enable rendering */
    PPU_MASK = PPU_MASK_SHOW_BG;
}

/* Write text to nametable buffer */
void write_text(unsigned char x, unsigned char y, const char* text) {
    unsigned int addr;
    unsigned char i;

    /* Calculate nametable address */
    addr = 0x2000 + (y * 32) + x;

    /* Disable rendering for safe PPU access */
    PPU_MASK = 0;

    /* Set PPU address */
    PPU_STATUS;  /* Reset address latch */
    PPU_ADDR = (unsigned char)(addr >> 8);
    PPU_ADDR = (unsigned char)(addr & 0xFF);

    /* Write characters */
    i = 0;
    while (text[i] != '\0') {
        PPU_DATA = ascii_to_tile(text[i]);
        i++;
    }

    /* Reset scroll */
    PPU_STATUS;
    PPU_SCROLL = 0;
    PPU_SCROLL = 0;

    /* Re-enable rendering */
    PPU_MASK = PPU_MASK_SHOW_BG;
}

/* Clear the screen */
void clear_screen(void) {
    unsigned int i;

    /* Disable rendering for safe PPU access */
    PPU_MASK = 0;

    /* Clear nametable */
    PPU_STATUS;
    PPU_ADDR = 0x20;
    PPU_ADDR = 0x00;

    for (i = 0; i < 960; i++) {
        PPU_DATA = 0x00;
    }

    /* Clear attribute table */
    for (i = 0; i < 64; i++) {
        PPU_DATA = 0x00;
    }

    /* Reset scroll */
    PPU_STATUS;
    PPU_SCROLL = 0;
    PPU_SCROLL = 0;

    /* Re-enable rendering */
    PPU_MASK = PPU_MASK_SHOW_BG;
}

/* Update screen - wait for vblank */
void update_screen(void) {
    /* Wait for vblank to safely update PPU */
    while (!(PPU_STATUS & PPU_STATUS_VBLANK));
}
