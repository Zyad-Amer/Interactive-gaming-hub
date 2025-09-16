/*
 * Created on: Sep 13, 2025
 * Author: Moustfa Youssef
 */

#ifndef HMEM_CONFIG_H_
#define HMEM_CONFIG_H_

/* Screen / framebuffer config */
#define HMEM_SCREEN_WIDTH   128
#define HMEM_SCREEN_HEIGHT  160
#define IMAGE_SIZE          (HMEM_SCREEN_WIDTH * HMEM_SCREEN_HEIGHT)

/* Game board layout (4x4 grid of cards) */
#define HMEM_GRID_COLS      3
#define HMEM_GRID_ROWS      2
#define HMEM_TOTAL_CARDS    (HMEM_GRID_COLS * HMEM_GRID_ROWS)
#define HMEM_TOTAL_PAIRS    (HMEM_TOTAL_CARDS / 2)

/* Board layout on screen (pixel coordinates) */
#define HMEM_GRID_LEFT      8
#define HMEM_GRID_TOP       8
#define HMEM_GRID_RIGHT     (HMEM_SCREEN_WIDTH - 8)
#define HMEM_GRID_BOTTOM    (HMEM_SCREEN_HEIGHT - 20)

/* Card dimensions */
#define HMEM_CARD_WIDTH     ((HMEM_GRID_RIGHT - HMEM_GRID_LEFT) / HMEM_GRID_COLS)
#define HMEM_CARD_HEIGHT    ((HMEM_GRID_BOTTOM - HMEM_GRID_TOP) / HMEM_GRID_ROWS)
#define HMEM_CARD_MARGIN    2

/* Colors (RGB565) */
#define HMEM_COLOR_BG       0x0000 /* black */
#define HMEM_COLOR_CARD_BACK 0x001F /* blue - face down cards */
#define HMEM_COLOR_CARD_FACE 0xFFFF /* white - face up cards */
#define HMEM_COLOR_CURSOR   0xF800 /* red - cursor */
#define HMEM_COLOR_MATCHED  0x07E0 /* green - matched pairs */
#define HMEM_COLOR_TEXT     0xF800 /* red - symbols */

/* Pins for 3 switches (same as tic-tac-toe) */
#define HMEM_BTN_PORT       PORTA
#define HMEM_BTN_FLIP_PIN   PIN8   /* flip card */
#define HMEM_BTN_NEXT_PIN   PIN9   /* move cursor */
#define HMEM_BTN_RESET_PIN  PIN10  /* reset game */

/* Button active level: 0 for active low, 1 for active high */
#define HMEM_BTN_ACTIVE_LOW 1

/* Debounce and timing (ms) */
#define HMEM_DEBOUNCE_MS    50
#define HMEM_FLIP_DELAY_MS  70  /* how long cards stay flipped when not matching */

#endif /* HMEM_CONFIG_H_ */
