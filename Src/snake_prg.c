/*
 * snake_prg.c - Snake Game Implementation with Interrupts (with Blue Border)
 *
 * Created on: Sep 13, 2025
 * Author: ahmed
 */

/***************************************************/
/*              Snake Game Implementation          */
/*              Author: Your Name                 */
/*              Version: 1.1 (with blue border)    */
/***************************************************/
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "MGPIO_int.h"
#include "MSTK_int.h"
#include "HTFT_int.h"
#include "MEXTI_Interface.h"
#include "MNVIC_Interface.h"
#include "snake_int.h"
#include "snake_prv.h"
#include <stdlib.h>
#include "FrameBuffer.h"

/***************************************************/
/*              Private Global Variables           */
/***************************************************/

/* Global game state */
static GameState_t game;

/* Button state flags for interrupt handling */
static volatile u8 btnLeftPressed = 0;
static volatile u8 btnRightPressed = 0;
static volatile u8 btnRestartPressed = 0;

/***************************************************/
/*              Interrupt Callback Functions       */
/***************************************************/
void Snake_btnLeftCallback(void) { btnLeftPressed = 1; }
void Snake_btnRightCallback(void) { btnRightPressed = 1; }
void Snake_btnRestartCallback(void) { btnRestartPressed = 1; }

/***************************************************/
/*              Private Function Prototypes        */
/***************************************************/
static void Snake_voidFillScreen(u16 color);
static void Snake_voidDrawPixel(u16 x, u16 y, u16 color);
static void Snake_voidDrawRect(u16 x, u16 y, u16 w, u16 h, u16 color);
static void Snake_voidUpdateDisplay(void);
static void Snake_voidDrawInitialState(void);
static void Snake_voidDrawBlock(u8 x, u8 y, u16 color);
static void Snake_voidGenerateFood(void);
static u8   Snake_u8CheckCollision(u8 x, u8 y);
static void Snake_voidDrawScore(void);
static void Snake_voidGameOver(void);

/* NEW: Draw blue border */
static void Snake_voidDrawBorder(void)
{
    u8 x, y;

    // Top and bottom borders
    for (x = 0; x < GRID_WIDTH; x++) {
        Snake_voidDrawBlock(x, 0, COLOR_BLUE);                     // Top
        Snake_voidDrawBlock(x, GRID_HEIGHT - 1, COLOR_BLUE);       // Bottom
    }

    // Left and right borders
    for (y = 0; y < GRID_HEIGHT; y++) {
        Snake_voidDrawBlock(0, y, COLOR_BLUE);                     // Left
        Snake_voidDrawBlock(GRID_WIDTH - 5, y, COLOR_BLUE);        // Right
    }
}

/***************************************************/
/*              Public Function Implementations    */
/***************************************************/
void Snake_voidInit(void)
{
    /* Configure EXTI for Snake game buttons */
    // LEFT button - EXTI8 (PIN10)
    MEXTI_voidSetInterruptPort(EXTI_Line8, EXTI_PORTA);
    MEXTI_voidSetTrigger(EXTI_Line8, EXTI_Falling);
    MEXTI_voidCallBack(Snake_btnLeftCallback, EXTI_Line8);
    MEXTI_voidEnable(EXTI_Line8);

    // RIGHT button - EXTI9 (PIN9)
    MEXTI_voidSetInterruptPort(EXTI_Line9, EXTI_PORTA);
    MEXTI_voidSetTrigger(EXTI_Line9, EXTI_Falling);
    MEXTI_voidCallBack(Snake_btnRightCallback, EXTI_Line9);
    MEXTI_voidEnable(EXTI_Line9);

    // RESTART button - EXTI10 (PIN8)
    MEXTI_voidSetInterruptPort(EXTI_Line10, EXTI_PORTA);
    MEXTI_voidSetTrigger(EXTI_Line10, EXTI_Falling);
    MEXTI_voidCallBack(Snake_btnRestartCallback, EXTI_Line10);
    MEXTI_voidEnable(EXTI_Line10);

    /* Enable NVIC interrupts */
    MNVIC_voidEnable(40); // EXTI15_10 (covers EXTI10)
    MNVIC_voidEnable(23); // EXTI9_5 (covers EXTI8, EXTI9)

    /* Clear the game state */
    game.length = 3;
    game.direction = DIR_RIGHT;
    game.next_direction = DIR_RIGHT;
    game.score = 0;
    game.game_over = 0;
    game.food_eaten = 0;

    /* Initialize snake in the middle of the screen */
    game.snake[0].x = GRID_WIDTH / 2;
    game.snake[0].y = GRID_HEIGHT / 2;
    game.snake[1].x = game.snake[0].x - 1;
    game.snake[1].y = game.snake[0].y;
    game.snake[2].x = game.snake[0].x - 2;
    game.snake[2].y = game.snake[0].y;

    /* Clear screen */
    Snake_voidFillScreen(COLOR_BLACK);

    /* Draw border */
    Snake_voidDrawBorder();

    /* Generate first food */
    Snake_voidGenerateFood();

    /* Draw initial game state */
    Snake_voidDrawInitialState();
}

void Snake_voidGameLoop(void)
{
    /* Initialize TFT display */
    HTFT_voidInit();

    /* Initialize random seed */
    srand(1234);

    /* Initialize game */
    Snake_voidInit();

    /* Main game loop */
    while (1) {
        if (!game.game_over) {
            Snake_voidUpdate();
            MSTK_voidDelayms(GAME_SPEED);
        } else {
            /* In game over state, wait for restart */
            if (btnRestartPressed) {
                btnRestartPressed = 0; // Clear flag
                Snake_voidInit(); // Restart the game
            }
            MSTK_voidDelayms(20);
        }
    }
}

void Snake_voidUpdate(void)
{
    if (game.game_over) {
        return;
    }

    /* Handle input using interrupt flags */
    Snake_voidHandleInput();

    /* Update direction */
    game.direction = game.next_direction;

    /* Calculate new head position */
    SnakeSegment_t new_head = game.snake[0];

    switch (game.direction) {
        case DIR_UP:    new_head.y--; break;
        case DIR_DOWN:  new_head.y++; break;
        case DIR_LEFT:  new_head.x--; break;
        case DIR_RIGHT: new_head.x++; break;
    }

    /* Check wall collision (with blue border as walls) */
    if (new_head.x == 0 || new_head.x == GRID_WIDTH - 5 ||
        new_head.y == 0 || new_head.y == GRID_HEIGHT - 1) {
        Snake_voidGameOver();
        return;
    }

    /* Check self collision */
    if (Snake_u8CheckCollision(new_head.x, new_head.y)) {
        Snake_voidGameOver();
        return;
    }

    /* Check food collision */
    if (new_head.x == game.food.x && new_head.y == game.food.y) {
        game.food_eaten = 1;
        game.score += 10;
        game.length++;
        Snake_voidGenerateFood();
    }

    /* Move snake body */
    if (!game.food_eaten) {
        /* Remove tail */
        Snake_voidDrawBlock(game.snake[game.length - 1].x,
                            game.snake[game.length - 1].y, COLOR_BLACK);

        /* Shift body */
        u16 i;
        for (i = game.length - 1; i > 0; i--) {
            game.snake[i] = game.snake[i - 1];
        }
    } else {
        /* Shift body without removing tail */
        u16 i;
        for (i = game.length - 1; i > 0; i--) {
            game.snake[i] = game.snake[i - 1];
        }
        game.food_eaten = 0;
    }

    /* Add new head */
    game.snake[0] = new_head;

    /* Draw border again to prevent overwriting */
    Snake_voidDrawBorder();

    /* Draw new head */
    Snake_voidDrawBlock(game.snake[0].x, game.snake[0].y, COLOR_GREEN);

    /* Update score display */
    Snake_voidDrawScore();

    /* Update display */
    Snake_voidUpdateDisplay();
}

void Snake_voidRestart(void) { Snake_voidInit(); }

u16 Snake_u16GetScore(void) { return game.score; }

u8 Snake_u8IsGameOver(void) { return game.game_over; }

void Snake_voidHandleInput(void)
{
    /* Check button flags set by interrupts */
    /* BUTTON 1: Turn Left (counter-clockwise) */
    if (btnLeftPressed) {
        btnLeftPressed = 0; // Clear flag
        switch (game.direction) {
            case DIR_UP:    game.next_direction = DIR_LEFT;  break;
            case DIR_LEFT:  game.next_direction = DIR_DOWN;  break;
            case DIR_DOWN:  game.next_direction = DIR_RIGHT; break;
            case DIR_RIGHT: game.next_direction = DIR_UP;    break;
        }
    }
    /* BUTTON 2: Turn Right (clockwise) */
    else if (btnRightPressed) {
        btnRightPressed = 0; // Clear flag
        switch (game.direction) {
            case DIR_UP:    game.next_direction = DIR_RIGHT; break;
            case DIR_RIGHT: game.next_direction = DIR_DOWN;  break;
            case DIR_DOWN:  game.next_direction = DIR_LEFT;  break;
            case DIR_LEFT:  game.next_direction = DIR_UP;    break;
        }
    }
    /* BUTTON 3: Restart */
    else if (btnRestartPressed && game.game_over) {
        btnRestartPressed = 0; // Clear flag
        Snake_voidInit(); // Restart the game
    }
}

/***************************************************/
/*              Private Function Implementations   */
/***************************************************/
static void Snake_voidFillScreen(u16 color)
{
    u32 i;
    for (i = 0; i < (DISPLAY_WIDTH * DISPLAY_HEIGHT); i++) {
        GlobalFrameBuffer[i] = color;
    }
    HTFT_voidDisplay(GlobalFrameBuffer);
}

static void Snake_voidDrawPixel(u16 x, u16 y, u16 color)
{
    if (x < DISPLAY_WIDTH && y < DISPLAY_HEIGHT) {
        GlobalFrameBuffer[y * DISPLAY_WIDTH + x] = color;
    }
}

static void Snake_voidDrawRect(u16 x, u16 y, u16 w, u16 h, u16 color)
{
    u16 i, j;
    for (j = y; j < y + h && j < DISPLAY_HEIGHT; j++) {
        for (i = x; i < x + w && i < DISPLAY_WIDTH; i++) {
            Snake_voidDrawPixel(i, j, color);
        }
    }
}

static void Snake_voidUpdateDisplay(void)
{
    HTFT_voidDisplay(GlobalFrameBuffer);
}

static void Snake_voidDrawInitialState(void)
{
    u16 i;

    /* Draw border */
    Snake_voidDrawBorder();

    /* Draw snake */
    for (i = 0; i < game.length; i++) {
        Snake_voidDrawBlock(game.snake[i].x, game.snake[i].y, COLOR_GREEN);
    }

    /* Draw food */
    Snake_voidDrawBlock(game.food.x, game.food.y, COLOR_RED);

    /* Draw score */
    Snake_voidDrawScore();

    /* Update display */
    Snake_voidUpdateDisplay();
}

static void Snake_voidDrawBlock(u8 x, u8 y, u16 color)
{
    u16 pixel_x = x * BLOCK_SIZE;
    u16 pixel_y = y * BLOCK_SIZE;

    /* Draw filled rectangle */
    Snake_voidDrawRect(pixel_x, pixel_y, BLOCK_SIZE - 1, BLOCK_SIZE - 1, color);
}

static void Snake_voidGenerateFood(void)
{
    u8 valid_position = 0;

    while (!valid_position) {
        game.food.x = (rand() % (GRID_WIDTH - 6)) + 1;   // avoid border
        game.food.y = (rand() % (GRID_HEIGHT - 2)) + 1;  // avoid border

        /* Check if food position collides with snake */
        valid_position = !Snake_u8CheckCollision(game.food.x, game.food.y);
    }

    /* Draw the food */
    Snake_voidDrawBlock(game.food.x, game.food.y, COLOR_RED);
}

static u8 Snake_u8CheckCollision(u8 x, u8 y)
{
    u16 i;
    for (i = 0; i < game.length; i++) {
        if (game.snake[i].x == x && game.snake[i].y == y) {
            return 1;
        }
    }
    return 0;
}

static void Snake_voidDrawScore(void)
{
    /* Clear score area */
    Snake_voidDrawRect(0, GRID_HEIGHT * BLOCK_SIZE, DISPLAY_WIDTH, 20, COLOR_BLACK);

    /* Draw score as colored blocks */
    u16 score_blocks = game.score / 10;
    u16 i;

    for (i = 0; i < score_blocks && i < 15; i++) {
        Snake_voidDrawRect(5 + i * 8, GRID_HEIGHT * BLOCK_SIZE + 5, 6, 6, COLOR_YELLOW);
    }
}

static void Snake_voidGameOver(void)
{
    game.game_over = 1;

    /* Clear screen */
    Snake_voidFillScreen(COLOR_BLACK);

    /* Draw "GAME OVER" pattern */
    /* (same as your original code) */

    /* G */
    Snake_voidDrawRect(10, 30, 4, 20, COLOR_RED);
    Snake_voidDrawRect(10, 30, 15, 4, COLOR_RED);
    Snake_voidDrawRect(10, 46, 15, 4, COLOR_RED);
    Snake_voidDrawRect(21, 38, 4, 12, COLOR_RED);
    Snake_voidDrawRect(17, 38, 8, 4, COLOR_RED);

    /* A */
    Snake_voidDrawRect(30, 30, 4, 20, COLOR_RED);
    Snake_voidDrawRect(30, 30, 15, 4, COLOR_RED);
    Snake_voidDrawRect(30, 38, 15, 4, COLOR_RED);
    Snake_voidDrawRect(41, 34, 4, 16, COLOR_RED);

    /* M */
    Snake_voidDrawRect(50, 30, 4, 20, COLOR_RED);
    Snake_voidDrawRect(50, 30, 15, 4, COLOR_RED);
    Snake_voidDrawRect(57, 34, 4, 8, COLOR_RED);
    Snake_voidDrawRect(61, 30, 4, 20, COLOR_RED);

    /* E */
    Snake_voidDrawRect(70, 30, 4, 20, COLOR_RED);
    Snake_voidDrawRect(70, 30, 15, 4, COLOR_RED);
    Snake_voidDrawRect(70, 38, 10, 4, COLOR_RED);
    Snake_voidDrawRect(70, 46, 15, 4, COLOR_RED);

    /* O */
    Snake_voidDrawRect(10, 60, 4, 20, COLOR_RED);
    Snake_voidDrawRect(10, 60, 15, 4, COLOR_RED);
    Snake_voidDrawRect(10, 76, 15, 4, COLOR_RED);
    Snake_voidDrawRect(21, 60, 4, 20, COLOR_RED);

    /* V */
    Snake_voidDrawRect(30, 60, 4, 16, COLOR_RED);
    Snake_voidDrawRect(34, 72, 4, 4, COLOR_RED);
    Snake_voidDrawRect(38, 76, 4, 4, COLOR_RED);
    Snake_voidDrawRect(42, 72, 4, 4, COLOR_RED);
    Snake_voidDrawRect(46, 60, 4, 16, COLOR_RED);

    /* E */
    Snake_voidDrawRect(55, 60, 4, 20, COLOR_RED);
    Snake_voidDrawRect(55, 60, 15, 4, COLOR_RED);
    Snake_voidDrawRect(55, 68, 10, 4, COLOR_RED);
    Snake_voidDrawRect(55, 76, 15, 4, COLOR_RED);

    /* R */
    Snake_voidDrawRect(75, 60, 4, 20, COLOR_RED);
    Snake_voidDrawRect(75, 60, 15, 4, COLOR_RED);
    Snake_voidDrawRect(75, 68, 10, 4, COLOR_RED);
    Snake_voidDrawRect(86, 60, 4, 12, COLOR_RED);
    Snake_voidDrawRect(82, 72, 4, 4, COLOR_RED);
    Snake_voidDrawRect(86, 76, 4, 4, COLOR_RED);

    /* Display final score as blocks */
    u16 final_score_blocks = game.score / 10;
    u16 i;
    for (i = 0; i < final_score_blocks && i < 10; i++) {
        Snake_voidDrawRect(20 + i * 8, 90, 6, 6, COLOR_YELLOW);
    }

    Snake_voidUpdateDisplay();
}
