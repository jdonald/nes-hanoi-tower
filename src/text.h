#ifndef TEXT_H
#define TEXT_H

/* ASCII to tile mappings for text rendering */
/* Our CHR ROM has simplified character set starting at tile $10 for '0' */

/* Convert ASCII character to tile number */
unsigned char ascii_to_tile(char c);

/* Write a string to the nametable at given position */
void write_text(unsigned char x, unsigned char y, const char* text);

/* Clear the screen (fill with tile 0) */
void clear_screen(void);

/* Update the screen (copy buffer to PPU) */
void update_screen(void);

#endif /* TEXT_H */
