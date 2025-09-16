#ifndef HXO_CONFIG_H_
#define HXO_CONFIG_H_

/* Screen / framebuffer config - change if your TFT differs */
#define HXO_SCREEN_WIDTH   128
#define HXO_SCREEN_HEIGHT  160
#define IMAGE_SIZE         (HXO_SCREEN_WIDTH * HXO_SCREEN_HEIGHT)

/* Board layout on screen (pixel coordinates)
   We center a 3x3 grid inside the screen. Adjust margins if desired. */
#define HXO_GRID_LEFT      10
#define HXO_GRID_TOP       10
#define HXO_GRID_RIGHT     (HXO_SCREEN_WIDTH - 10)
#define HXO_GRID_BOTTOM    (HXO_SCREEN_HEIGHT - 10)

/* line thickness in pixels */
#define HXO_GRID_LINE_THICK 3

/* Colors (RGB565) - change if you like */
#define HXO_COLOR_BG       0x0000 /* black */
#define HXO_COLOR_GRID     0xFFFF /* white */
#define HXO_COLOR_CURSOR   0xF800 /* red */
#define HXO_COLOR_X        0x07E0 /* green */
#define HXO_COLOR_O        0x001F /* blue */
#define HXO_COLOR_TEXT     0xFFFF

/* Pins for 3 switches (map to your actual pins) */
#define HXO_BTN_PORT       PORTA
#define HXO_BTN_X_PIN      PIN8   /* set your pin */
#define HXO_BTN_O_PIN      PIN9    /* set your pin */
#define HXO_BTN_NEXT_PIN   PIN10    /* set your pin */

/* Button active level: 0 for active low, 1 for active high */
#define HXO_BTN_ACTIVE_LOW 1

/* Debounce and repeat timing (ms) */
#define HXO_DEBOUNCE_MS    40
#define HXO_REPEAT_MS      200

#endif /* HXO_CONFIG_H_ */
