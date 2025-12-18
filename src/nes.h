#ifndef NES_H
#define NES_H

/* NES Hardware Register Definitions */

/* PPU (Picture Processing Unit) Registers */
#define PPU_CTRL   (*(unsigned char*)0x2000)
#define PPU_MASK   (*(unsigned char*)0x2001)
#define PPU_STATUS (*(unsigned char*)0x2002)
#define PPU_SCROLL (*(unsigned char*)0x2005)
#define PPU_ADDR   (*(unsigned char*)0x2006)
#define PPU_DATA   (*(unsigned char*)0x2007)
#define OAM_ADDR   (*(unsigned char*)0x2003)
#define OAM_DATA   (*(unsigned char*)0x2004)
#define OAM_DMA    (*(unsigned char*)0x4014)

/* APU (Audio Processing Unit) and Controller Registers */
#define APU_PULSE1_CTRL (*(unsigned char*)0x4000)
#define APU_PULSE1_SWEEP (*(unsigned char*)0x4001)
#define APU_PULSE1_TIMER_LO (*(unsigned char*)0x4002)
#define APU_PULSE1_TIMER_HI (*(unsigned char*)0x4003)

#define APU_PULSE2_CTRL (*(unsigned char*)0x4004)
#define APU_PULSE2_SWEEP (*(unsigned char*)0x4005)
#define APU_PULSE2_TIMER_LO (*(unsigned char*)0x4006)
#define APU_PULSE2_TIMER_HI (*(unsigned char*)0x4007)

#define APU_TRIANGLE_CTRL (*(unsigned char*)0x4008)
#define APU_TRIANGLE_TIMER_LO (*(unsigned char*)0x400A)
#define APU_TRIANGLE_TIMER_HI (*(unsigned char*)0x400B)

#define APU_NOISE_CTRL (*(unsigned char*)0x400C)
#define APU_NOISE_PERIOD (*(unsigned char*)0x400E)
#define APU_NOISE_LENGTH (*(unsigned char*)0x400F)

#define APU_DMC_CTRL (*(unsigned char*)0x4010)
#define APU_STATUS (*(unsigned char*)0x4015)
#define APU_FRAME_COUNTER (*(unsigned char*)0x4017)

#define CONTROLLER1 (*(unsigned char*)0x4016)
#define CONTROLLER2 (*(unsigned char*)0x4017)

/* PPU Control Register Flags */
#define PPU_CTRL_NMI          0x80
#define PPU_CTRL_SPRITE_SIZE  0x20
#define PPU_CTRL_BG_PATTERN   0x10
#define PPU_CTRL_SPRITE_PATTERN 0x08
#define PPU_CTRL_VRAM_ADD     0x04

/* PPU Mask Register Flags */
#define PPU_MASK_BLUE_EMPHASIS  0x80
#define PPU_MASK_GREEN_EMPHASIS 0x40
#define PPU_MASK_RED_EMPHASIS   0x20
#define PPU_MASK_SHOW_SPRITES   0x10
#define PPU_MASK_SHOW_BG        0x08
#define PPU_MASK_SPRITE_LEFT    0x04
#define PPU_MASK_BG_LEFT        0x02
#define PPU_MASK_GREYSCALE      0x01

/* PPU Status Register Flags */
#define PPU_STATUS_VBLANK    0x80
#define PPU_STATUS_SPRITE0   0x40
#define PPU_STATUS_OVERFLOW  0x20

/* Controller Button Masks */
#define BUTTON_A      0x01
#define BUTTON_B      0x02
#define BUTTON_SELECT 0x04
#define BUTTON_START  0x08
#define BUTTON_UP     0x10
#define BUTTON_DOWN   0x20
#define BUTTON_LEFT   0x40
#define BUTTON_RIGHT  0x80

/* Color Palette Values (NES palette hex codes) */
#define COLOR_BLACK       0x0F
#define COLOR_DARK_GRAY   0x00
#define COLOR_GRAY        0x10
#define COLOR_WHITE       0x30
#define COLOR_LIGHT_BLUE  0x21
#define COLOR_PINK        0x04
#define COLOR_RED         0x16
#define COLOR_MAGENTA     0x24
#define COLOR_ORANGE      0x27
#define COLOR_YELLOW      0x28
#define COLOR_YELLOW_GREEN 0x2A
#define COLOR_GREEN       0x1A
#define COLOR_TEAL        0x1C
#define COLOR_BLUE        0x12
#define COLOR_DEEP_BLUE   0x02

/* Sprite Attributes */
#define SPRITE_PALETTE_0  0x00
#define SPRITE_PALETTE_1  0x01
#define SPRITE_PALETTE_2  0x02
#define SPRITE_PALETTE_3  0x03
#define SPRITE_PRIORITY   0x20
#define SPRITE_FLIP_H     0x40
#define SPRITE_FLIP_V     0x80

/* Helper Macros */
#define WAIT_VBLANK() while (!(PPU_STATUS & PPU_STATUS_VBLANK))

#endif /* NES_H */
