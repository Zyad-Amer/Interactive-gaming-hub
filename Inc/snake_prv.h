/*
 * snake_prv.h
 *
 *  Created on: Sep 13, 2025
 *      Author: ahmed
 */

#ifndef SNAKE_PRV_H_
#define SNAKE_PRV_H_



/***************************************************/
/*              Private Definitions                */
/***************************************************/

/* Game Configuration */
#define GRID_WIDTH          20
#define GRID_HEIGHT         15
#define BLOCK_SIZE          8
#define MAX_SNAKE_LENGTH    (GRID_WIDTH * GRID_HEIGHT)
u32 GAME_SPEED =100 ;    /* ms delay between moves */

/* Display Configuration */
#define DISPLAY_WIDTH       128
#define DISPLAY_HEIGHT      160

/* Colors in RGB565 format */
#define COLOR_BLACK         0x0000
#define COLOR_GREEN         0x07E0
#define COLOR_RED           0xF800
#define COLOR_WHITE         0xFFFF
#define COLOR_BLUE          0x001F
#define COLOR_YELLOW        0xFFE0

/* Button debounce delay */
#define BUTTON_DEBOUNCE_MS  50
#define RESTART_DEBOUNCE_MS 100

/***************************************************/
/*              Private Enumerations               */
/***************************************************/

/* Direction enumeration */
typedef enum {
    DIR_UP = 0,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction_t;

/***************************************************/
/*              Private Structures                 */
/***************************************************/

/* Snake segment structure */
typedef struct {
    u8 x;
    u8 y;
} SnakeSegment_t;

/* Game state structure */
typedef struct {
    SnakeSegment_t snake[MAX_SNAKE_LENGTH];
    u16 length;
    Direction_t direction;
    Direction_t next_direction;
    SnakeSegment_t food;
    u16 score;
    u8 game_over;
    u8 food_eaten;
} GameState_t;

/***************************************************/
/*              Private Function Prototypes        */
/***************************************************/

/**
 * @brief Generate food at random valid position
 */
static void Snake_voidGenerateFood(void);

/**
 * @brief Draw a single block at grid position
 * @param x Grid X coordinate
 * @param y Grid Y coordinate
 * @param color Block color
 */
static void Snake_voidDrawBlock(u8 x, u8 y, u16 color);

/**
 * @brief Draw current score on display
 */
static void Snake_voidDrawScore(void);

/**
 * @brief Check collision with snake body
 * @param x X coordinate to check
 * @param y Y coordinate to check
 * @return u8 1 if collision, 0 if free
 */
static u8 Snake_u8CheckCollision(u8 x, u8 y);

/**
 * @brief Handle game over state
 */
static void Snake_voidGameOver(void);

/**
 * @brief Fill entire screen with specified color
 * @param color Color to fill screen with
 */
static void Snake_voidFillScreen(u16 color);

/**
 * @brief Draw a single pixel on display buffer
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Pixel color
 */
static void Snake_voidDrawPixel(u16 x, u16 y, u16 color);

/**
 * @brief Draw filled rectangle
 * @param x X coordinate
 * @param y Y coordinate
 * @param w Width
 * @param h Height
 * @param color Rectangle color
 */
static void Snake_voidDrawRect(u16 x, u16 y, u16 w, u16 h, u16 color);

/**
 * @brief Update display with current buffer
 */
static void Snake_voidUpdateDisplay(void);

/**
 * @brief Draw initial game state
 */
static void Snake_voidDrawInitialState(void);

#endif /* SNAKE_PRV_H */

