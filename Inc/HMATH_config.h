/*
 * Created on: Sep 13, 2025
 * Author: Moustfa Youssef
 */
#ifndef HMATH_CONFIG_H_
#define HMATH_CONFIG_H_

/* Screen configuration */
#define MATH_SCREEN_WIDTH   128
#define MATH_SCREEN_HEIGHT  160
#define IMAGE_SIZE          (MATH_SCREEN_WIDTH * MATH_SCREEN_HEIGHT)

/* Colors (RGB565) */
#define MATH_COLOR_BG       0x0000 /* black */
#define MATH_COLOR_TEXT     0xFFFF /* white */
#define MATH_COLOR_CORRECT  0x07E0 /* green */
#define MATH_COLOR_WRONG    0xF800 /* red */
#define MATH_COLOR_CURSOR   0x001F /* blue */
#define MATH_COLOR_SELECTED 0xFFE0 /* yellow */

/* Button configuration - use same pins as tic-tac-toe */
#define MATH_BTN_PORT       PORTA
#define MATH_BTN_A_PIN      PIN8  /* Answer A */
#define MATH_BTN_B_PIN      PIN9   /* Answer B */
#define MATH_BTN_C_PIN      PIN10  /* Answer C */

/* Button active level: 0 for active low, 1 for active high */
#define MATH_BTN_ACTIVE_LOW 1

/* Debounce timing (ms) */
#define MATH_DEBOUNCE_MS    40

/* Game configuration */
#define MATH_MAX_QUESTIONS  10
#define MATH_TOTAL_QUESTIONS 255  /* Unlimited questions */
#define MATH_NUMBER_RANGE   20    /* Numbers from 0 to 20 */
#define MATH_FEEDBACK_DELAY 55  /* ms to show correct/wrong */

/* Text positioning */
#define MATH_QUESTION_Y     35
#define MATH_OPTION_START_Y 60
#define MATH_OPTION_SPACING 25
#define MATH_SCORE_Y        10
#define MATH_TEXT_MARGIN    5

#endif /* HMATH_CONFIG_H_ */
