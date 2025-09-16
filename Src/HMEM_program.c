/*
 * Created on: Sep 13, 2025
 * Author: Moustfa Youssef
 */
#include "STD_TYPES.h"
#include <stdlib.h>
#include <string.h>
#include "BIT_MATH.h"
#include "MGPIO_int.h"
#include "MSTK_int.h"
#include "HTFT_int.h"
#include "MEXTI_Interface.h"
#include "MNVIC_Interface.h"

#include "HMEM_config.h"
#include "HMEM_interface.h"
#include "HMEM_private.h"

#include "FrameBuffer.h"

#define HMEM_frame GlobalFrameBuffer

/* Game state variables */
static HMEM_Card_t HMEM_board[HMEM_GRID_ROWS][HMEM_GRID_COLS];
static u8 HMEM_cursor_row, HMEM_cursor_col;
static HMEM_GameState_t HMEM_game_state;
static u16 HMEM_move_count;

/* Flipped cards tracking */
static u8 HMEM_first_flipped_row, HMEM_first_flipped_col;
static u8 HMEM_second_flipped_row, HMEM_second_flipped_col;
static u8 HMEM_flipped_count;
static u32 HMEM_mismatch_start_time;

/* Button state flags for interrupt handling */
static volatile u8 btnFlipPressed = 0;
static volatile u8 btnNextPressed = 0;
static volatile u8 btnResetPressed = 0;

/* Card symbols - simple patterns for 8 different symbols */
static const u8 HMEM_symbols[8] = {1, 2, 3, 4, 5, 6, 7, 8};

/* Interrupt callback functions */
void HMEM_btnFlipCallback(void)
{
    btnFlipPressed = 1;
}

void HMEM_btnNextCallback(void)
{
    btnNextPressed = 1;
}

void HMEM_btnResetCallback(void)
{
    btnResetPressed = 1;
}

/* Forward declarations of private helpers */
static u32 HMEM_u32GetMillis(void);
static void HMEM_voidShuffleCards(void);
static u8 HMEM_u8CheckWin(void);
static void HMEM_voidDrawFrame(void);
static void HMEM_voidDrawCard(u8 row, u8 col);
static void HMEM_voidDrawCursor(u8 row, u8 col);
static void HMEM_voidDrawSymbol(u16 cx, u16 cy, u8 symbol, u16 color);
static void HMEM_voidGetCardBounds(u8 row, u8 col, u16 *x0, u16 *y0, u16 *x1, u16 *y1);
static void HMEM_voidGetCardCenter(u8 row, u8 col, u16 *cx, u16 *cy);
static void HMEM_voidFillRect(u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
static void HMEM_voidSetPixel(u16 x, u16 y, u16 color);
static void HMEM_voidDrawLine(int x0, int y0, int x1, int y1, u16 color);

/* ------------ Public implementations ------------- */

void HMEM_voidInit(void)
{
    /* Configure EXTI for Memory game buttons */
    // FLIP button - EXTI8
    MEXTI_voidSetInterruptPort(EXTI_Line8, EXTI_PORTA);
    MEXTI_voidSetTrigger(EXTI_Line8, EXTI_Falling);
    MEXTI_voidCallBack(HMEM_btnFlipCallback, EXTI_Line8);
    MEXTI_voidEnable(EXTI_Line8);

    // NEXT button - EXTI9
    MEXTI_voidSetInterruptPort(EXTI_Line9, EXTI_PORTA);
    MEXTI_voidSetTrigger(EXTI_Line9, EXTI_Falling);
    MEXTI_voidCallBack(HMEM_btnNextCallback, EXTI_Line9);
    MEXTI_voidEnable(EXTI_Line9);

    // RESET button - EXTI10
    MEXTI_voidSetInterruptPort(EXTI_Line10, EXTI_PORTA);
    MEXTI_voidSetTrigger(EXTI_Line10, EXTI_Falling);
    MEXTI_voidCallBack(HMEM_btnResetCallback, EXTI_Line10);
    MEXTI_voidEnable(EXTI_Line10);

    /* Enable NVIC interrupts */
    MNVIC_voidEnable(40); // EXTI15_10 (covers EXTI10)
    MNVIC_voidEnable(23); // EXTI9_5 (covers EXTI8, EXTI9)

    /* Initialize display */
    HTFT_voidInit();
    
    /* Start new game */
    HMEM_voidResetGame();
    
    /* Initial render */
    HMEM_voidDrawFrame();
    HTFT_voidDisplay(HMEM_frame);
}

void HMEM_voidResetGame(void)
{
    /* Initialize game state */
    HMEM_cursor_row = 0;
    HMEM_cursor_col = 0;
    HMEM_game_state = HMEM_STATE_PLAYING;
    HMEM_move_count = 0;
    HMEM_flipped_count = 0;

    /* Clear framebuffer */
    memset(HMEM_frame, 0, sizeof(HMEM_frame));

    /* Initialize board with pairs of symbols */
    /* We assume HMEM_TOTAL_CARDS == HMEM_GRID_ROWS * HMEM_GRID_COLS and even */
    u16 symbol_index = 0;
    for (u8 r = 0; r < HMEM_GRID_ROWS; ++r) {
        for (u8 c = 0; c < HMEM_GRID_COLS; ++c) {
            /* Each symbol should appear exactly twice; use symbols array cyclically if needed */
            u8 sym = HMEM_symbols[(symbol_index / 2) % (sizeof(HMEM_symbols)/sizeof(HMEM_symbols[0]))];
            HMEM_board[r][c].symbol = sym;
            HMEM_board[r][c].state = HMEM_CARD_FACE_DOWN;
            ++symbol_index;
        }
    }
    
    /* Shuffle the cards */
    HMEM_voidShuffleCards();
}

const u16* HMEM_u16GetFrameBuffer(void)
{
    return GlobalFrameBuffer;
}

HMEM_GameState_t HMEM_GameStateGet(void)
{
    return HMEM_game_state;
}

u16 HMEM_u16GetMoves(void)
{
    return HMEM_move_count;
}

void HMEM_voidPoll(void)
{
    u32 now = HMEM_u32GetMillis();

    /* Handle game state transitions */
    if (HMEM_game_state == HMEM_STATE_SHOWING_MISMATCH) {
        if (now - HMEM_mismatch_start_time >= HMEM_FLIP_DELAY_MS) {
            /* Flip mismatched cards back face down */
            HMEM_board[HMEM_first_flipped_row][HMEM_first_flipped_col].state = HMEM_CARD_FACE_DOWN;
            HMEM_board[HMEM_second_flipped_row][HMEM_second_flipped_col].state = HMEM_CARD_FACE_DOWN;
            HMEM_flipped_count = 0;
            HMEM_game_state = HMEM_STATE_PLAYING;
            
            /* Redraw the frame */
            HMEM_voidDrawFrame();
            HTFT_voidDisplay(HMEM_frame);
        }
        return; /* Don't process buttons during mismatch display */
    }

    /* Check button flags set by interrupts */
    if (btnResetPressed) {
        btnResetPressed = 0; // Clear flag
        HMEM_voidResetGame();
        HMEM_voidDrawFrame();
        HTFT_voidDisplay(HMEM_frame);
    }

    if (btnNextPressed) {
        btnNextPressed = 0; // Clear flag
        
        /* Move to next position */
        HMEM_cursor_col++;
        if (HMEM_cursor_col >= HMEM_GRID_COLS) {
            HMEM_cursor_col = 0;
            HMEM_cursor_row++;
            if (HMEM_cursor_row >= HMEM_GRID_ROWS) {
                HMEM_cursor_row = 0;
            }
        }
        
        /* Redraw frame with new cursor position */
        HMEM_voidDrawFrame();
        HTFT_voidDisplay(HMEM_frame);
    }

    if (btnFlipPressed && HMEM_game_state == HMEM_STATE_PLAYING) {
        btnFlipPressed = 0; // Clear flag
        
        HMEM_Card_t* current_card = &HMEM_board[HMEM_cursor_row][HMEM_cursor_col];
        
        /* Only flip face-down cards */
        if (current_card->state == HMEM_CARD_FACE_DOWN) {
            current_card->state = HMEM_CARD_FACE_UP;
            
            if (HMEM_flipped_count == 0) {
                /* First card flipped */
                HMEM_first_flipped_row = HMEM_cursor_row;
                HMEM_first_flipped_col = HMEM_cursor_col;
                HMEM_flipped_count = 1;
            } else if (HMEM_flipped_count == 1) {
                /* Second card flipped */
                HMEM_second_flipped_row = HMEM_cursor_row;
                HMEM_second_flipped_col = HMEM_cursor_col;
                HMEM_flipped_count = 2;
                HMEM_move_count++;
                
                /* Check for match */
                HMEM_Card_t* first_card = &HMEM_board[HMEM_first_flipped_row][HMEM_first_flipped_col];
                HMEM_Card_t* second_card = &HMEM_board[HMEM_second_flipped_row][HMEM_second_flipped_col];
                
                if (first_card->symbol == second_card->symbol) {
                    /* Match found */
                    first_card->state = HMEM_CARD_MATCHED;
                    second_card->state = HMEM_CARD_MATCHED;
                    HMEM_flipped_count = 0;
                    
                    /* Check for win condition */
                    if (HMEM_u8CheckWin()) {
                        HMEM_game_state = HMEM_STATE_WON;
                    }
                } else {
                    /* No match - start mismatch display timer */
                    HMEM_game_state = HMEM_STATE_SHOWING_MISMATCH;
                    HMEM_mismatch_start_time = now;
                }
            }
            
            /* Redraw frame */
            HMEM_voidDrawFrame();
            HTFT_voidDisplay(HMEM_frame);
        }
    }

    /* Handle win state */
    if (HMEM_game_state == HMEM_STATE_WON) {
        /* Flash screen green briefly */
        static u32 win_flash_start = 0;
        if (win_flash_start == 0) {
            win_flash_start = now;
            HMEM_voidFillRect(0, 0, HMEM_SCREEN_WIDTH-1, HMEM_SCREEN_HEIGHT-1, HMEM_COLOR_MATCHED);
            HTFT_voidDisplay(HMEM_frame);
        } else if (now - win_flash_start >= 2000) {
            /* Auto-reset after 2 seconds */
            win_flash_start = 0;
            HMEM_voidResetGame();
            HMEM_voidDrawFrame();
            HTFT_voidDisplay(HMEM_frame);
        }
    }
}

/* ------------ Private implementations ------------- */

static u32 HMEM_u32GetMillis(void)
{
    /* Simple incrementing counter - in real implementation use proper millisecond timer */
    static u32 t = 0;
    t += 20; /* Assuming poll called every 20ms */
    return t;
}

static void HMEM_voidShuffleCards(void)
{
    /* Simple shuffle algorithm - Fisher-Yates */
    for (int i = HMEM_TOTAL_CARDS - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        
        /* Convert linear indices to row,col */
        u8 i_row = i / HMEM_GRID_COLS;
        u8 i_col = i % HMEM_GRID_COLS;
        u8 j_row = j / HMEM_GRID_COLS;
        u8 j_col = j % HMEM_GRID_COLS;
        
        /* Swap symbols */
        u8 temp_symbol = HMEM_board[i_row][i_col].symbol;
        HMEM_board[i_row][i_col].symbol = HMEM_board[j_row][j_col].symbol;
        HMEM_board[j_row][j_col].symbol = temp_symbol;

        /* Also swap states to keep consistency (should be face down anyway) */
        u8 tmp_state = HMEM_board[i_row][i_col].state;
        HMEM_board[i_row][i_col].state = HMEM_board[j_row][j_col].state;
        HMEM_board[j_row][j_col].state = tmp_state;
    }
}

static u8 HMEM_u8CheckWin(void)
{
    /* Check if all cards are matched */
    for (u8 r = 0; r < HMEM_GRID_ROWS; ++r) {
        for (u8 c = 0; c < HMEM_GRID_COLS; ++c) {
            if (HMEM_board[r][c].state != HMEM_CARD_MATCHED) {
                return 0;
            }
        }
    }
    return 1;
}

static void HMEM_voidDrawFrame(void)
{
    /* Clear background */
    HMEM_voidFillRect(0, 0, HMEM_SCREEN_WIDTH-1, HMEM_SCREEN_HEIGHT-1, HMEM_COLOR_BG);
    
    /* Draw all cards */
    for (u8 r = 0; r < HMEM_GRID_ROWS; ++r) {
        for (u8 c = 0; c < HMEM_GRID_COLS; ++c) {
            HMEM_voidDrawCard(r, c);
        }
    }
    
    /* Draw cursor */
    HMEM_voidDrawCursor(HMEM_cursor_row, HMEM_cursor_col);
    
    /* Draw move counter at bottom */
    /* Simple text display - just draw some pixels to represent the number */
    u16 moves_display_y = HMEM_SCREEN_HEIGHT - 12;
    u16 max_marks = 20;
    u16 marks = (HMEM_move_count < max_marks) ? HMEM_move_count : max_marks;
    for (u8 i = 0; i < marks; ++i) {
        HMEM_voidSetPixel(10 + i * 2, moves_display_y, HMEM_COLOR_TEXT);
    }
}

static void HMEM_voidDrawCard(u8 row, u8 col)
{
    u16 x0, y0, x1, y1, cx, cy;
    HMEM_voidGetCardBounds(row, col, &x0, &y0, &x1, &y1);
    HMEM_voidGetCardCenter(row, col, &cx, &cy);
    
    HMEM_Card_t* card = &HMEM_board[row][col];
    
    switch (card->state) {
        case HMEM_CARD_FACE_DOWN:
            /* Draw blue face-down card */
            HMEM_voidFillRect(x0, y0, x1, y1, HMEM_COLOR_CARD_BACK);
            break;
            
        case HMEM_CARD_FACE_UP:
            /* Draw white card with symbol */
            HMEM_voidFillRect(x0, y0, x1, y1, HMEM_COLOR_CARD_FACE);
            HMEM_voidDrawSymbol(cx, cy, card->symbol, HMEM_COLOR_TEXT);
            break;
            
        case HMEM_CARD_MATCHED:
            /* Draw green matched card with symbol */
            HMEM_voidFillRect(x0, y0, x1, y1, HMEM_COLOR_MATCHED);
            HMEM_voidDrawSymbol(cx, cy, card->symbol, HMEM_COLOR_BG);
            break;

        default:
            /* Safety fallback */
            HMEM_voidFillRect(x0, y0, x1, y1, HMEM_COLOR_CARD_BACK);
            break;
    }
}

static void HMEM_voidDrawCursor(u8 row, u8 col)
{
    u16 x0, y0, x1, y1;
    HMEM_voidGetCardBounds(row, col, &x0, &y0, &x1, &y1);
    
    /* Draw red border around selected card */
    HMEM_voidDrawLine((int)x0-1, (int)y0-1, (int)x1+1, (int)y0-1, HMEM_COLOR_CURSOR);
    HMEM_voidDrawLine((int)x1+1, (int)y0-1, (int)x1+1, (int)y1+1, HMEM_COLOR_CURSOR);
    HMEM_voidDrawLine((int)x1+1, (int)y1+1, (int)x0-1, (int)y1+1, HMEM_COLOR_CURSOR);
    HMEM_voidDrawLine((int)x0-1, (int)y1+1, (int)x0-1, (int)y0-1, HMEM_COLOR_CURSOR);
}

static void HMEM_voidDrawSymbol(u16 cx, u16 cy, u8 symbol, u16 color)
{
    /* Draw different simple symbols based on symbol ID */
    u8 size = 8;
    
    switch (symbol) {
        case 1: /* Circle */
            for (u8 i = 0; i < 8; ++i) {
                HMEM_voidSetPixel(cx + (i-4), cy - 3, color);
                HMEM_voidSetPixel(cx + (i-4), cy + 3, color);
            }
            for (u8 i = 0; i < 6; ++i) {
                HMEM_voidSetPixel(cx - 3, cy + (i-3), color);
                HMEM_voidSetPixel(cx + 3, cy + (i-3), color);
            }
            break;
            
        case 2: /* Cross */
            HMEM_voidDrawLine(cx-(size/2), cy, cx+(size/2), cy, color);
            HMEM_voidDrawLine(cx, cy-(size/2), cx, cy+(size/2), color);
            break;
            
        case 3: /* Diamond */
            HMEM_voidDrawLine(cx, cy-(size/2), cx+(size/2), cy, color);
            HMEM_voidDrawLine(cx+(size/2), cy, cx, cy+(size/2), color);
            HMEM_voidDrawLine(cx, cy+(size/2), cx-(size/2), cy, color);
            HMEM_voidDrawLine(cx-(size/2), cy, cx, cy-(size/2), color);
            break;
            
        case 4: /* Square */
            HMEM_voidDrawLine(cx-(size/2), cy-(size/2), cx+(size/2), cy-(size/2), color);
            HMEM_voidDrawLine(cx+(size/2), cy-(size/2), cx+(size/2), cy+(size/2), color);
            HMEM_voidDrawLine(cx+(size/2), cy+(size/2), cx-(size/2), cy+(size/2), color);
            HMEM_voidDrawLine(cx-(size/2), cy+(size/2), cx-(size/2), cy-(size/2), color);
            break;
            
        case 5: /* Triangle */
            HMEM_voidDrawLine(cx, cy-(size/2), cx+(size/2), cy+(size/2), color);
            HMEM_voidDrawLine(cx+(size/2), cy+(size/2), cx-(size/2), cy+(size/2), color);
            HMEM_voidDrawLine(cx-(size/2), cy+(size/2), cx, cy-(size/2), color);
            break;
            
        case 6: /* Star */
            HMEM_voidDrawLine(cx, cy-(size/2), cx, cy+(size/2), color);
            HMEM_voidDrawLine(cx-(size/2), cy, cx+(size/2), cy, color);
            HMEM_voidDrawLine(cx-(size/3), cy-(size/3), cx+(size/3), cy+(size/3), color);
            HMEM_voidDrawLine(cx+(size/3), cy-(size/3), cx-(size/3), cy+(size/3), color);
            break;
            
        case 7: /* Heart (simplified) */
            HMEM_voidDrawLine(cx-2, cy-2, cx, cy, color);
            HMEM_voidDrawLine(cx+2, cy-2, cx, cy, color);
            HMEM_voidDrawLine(cx, cy, cx, cy+3, color);
            HMEM_voidSetPixel(cx-1, cy-3, color);
            HMEM_voidSetPixel(cx+1, cy-3, color);
            break;
            
        case 8: /* Lightning */
            HMEM_voidDrawLine(cx-2, cy-4, cx, cy, color);
            HMEM_voidDrawLine(cx, cy, cx-1, cy, color);
            HMEM_voidDrawLine(cx-1, cy, cx+2, cy+4, color);
            break;
            
        default:
            /* Fallback - just a dot */
            HMEM_voidSetPixel(cx, cy, color);
            break;
    }
}

static void HMEM_voidGetCardBounds(u8 row, u8 col, u16 *x0, u16 *y0, u16 *x1, u16 *y1)
{
    *x0 = HMEM_GRID_LEFT + col * HMEM_CARD_WIDTH + HMEM_CARD_MARGIN;
    *y0 = HMEM_GRID_TOP + row * HMEM_CARD_HEIGHT + HMEM_CARD_MARGIN;
    *x1 = *x0 + HMEM_CARD_WIDTH - 2 * HMEM_CARD_MARGIN - 1;
    *y1 = *y0 + HMEM_CARD_HEIGHT - 2 * HMEM_CARD_MARGIN - 1;
}

static void HMEM_voidGetCardCenter(u8 row, u8 col, u16 *cx, u16 *cy)
{
    u16 x0, y0, x1, y1;
    HMEM_voidGetCardBounds(row, col, &x0, &y0, &x1, &y1);
    *cx = (x0 + x1) / 2;
    *cy = (y0 + y1) / 2;
}

static void HMEM_voidFillRect(u16 x0, u16 y0, u16 x1, u16 y1, u16 color)
{
    if (x1 < x0) { u16 t = x0; x0 = x1; x1 = t; }
    if (y1 < y0) { u16 t = y0; y0 = y1; y1 = t; }
    if (x0 >= HMEM_SCREEN_WIDTH) return;
    if (y0 >= HMEM_SCREEN_HEIGHT) return;
    if (x1 >= HMEM_SCREEN_WIDTH) x1 = HMEM_SCREEN_WIDTH-1;
    if (y1 >= HMEM_SCREEN_HEIGHT) y1 = HMEM_SCREEN_HEIGHT-1;

    for (u16 y = y0; y <= y1; ++y) {
        u32 base = (u32)y * HMEM_SCREEN_WIDTH;
        for (u16 x = x0; x <= x1; ++x) {
            HMEM_frame[base + x] = color;
        }
    }
}

static void HMEM_voidSetPixel(u16 x, u16 y, u16 color)
{
    if (x >= HMEM_SCREEN_WIDTH || y >= HMEM_SCREEN_HEIGHT) return;
    HMEM_frame[(u32)y * HMEM_SCREEN_WIDTH + x] = color;
}

static void HMEM_voidDrawLine(int x0, int y0, int x1, int y1, u16 color)
{
    /* Bresenham line algorithm (integer) */
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    
    while (1) {
        if ((u32)x0 < HMEM_SCREEN_WIDTH && (u32)y0 < HMEM_SCREEN_HEIGHT && x0 >= 0 && y0 >= 0) {
            HMEM_voidSetPixel((u16)x0, (u16)y0, color);
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

