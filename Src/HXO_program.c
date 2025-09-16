/* HXO_program.c - XO Game with Interrupt Support */

#include "STD_TYPES.h"
#include "HTFT_int.h"
#include "MSTK_int.h"
#include "MGPIO_int.h"
#include "MEXTI_Interface.h"
#include "MNVIC_Interface.h"
#include "HXO_interface.h"
#include "FrameBuffer.h"
#include "font5x7.h"

#define TFT_W 128
#define TFT_H 160

/* Game state */
typedef enum { PLAYER_X = 0, PLAYER_O } Player_t;
typedef enum { CELL_EMPTY = 0, CELL_X, CELL_O } Cell_t;
typedef enum { GAME_PLAYING = 0, GAME_X_WINS, GAME_O_WINS, GAME_DRAW } GameState_t;

static Cell_t board[3][3];
static Player_t currentPlayer;
static GameState_t gameState;
static u8 cursorRow, cursorCol;

/* Button state flags for interrupt handling */
static volatile u8 btnXPressed = 0;
static volatile u8 btnOPressed = 0;
static volatile u8 btnNextPressed = 0;

/* Colors (RGB565) */
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_GRAY  0x8410
#define COLOR_BLUE  0x001F
#define COLOR_GREEN 0x07E0
#define COLOR_RED   0xF800
#define COLOR_YELLOW 0xFFE0

/* Drawing helpers using GlobalFrameBuffer */
static inline void putPixel(int x, int y, u16 color)
{
    if (x < 0 || x >= TFT_W || y < 0 || y >= TFT_H) return;
    GlobalFrameBuffer[y * TFT_W + x] = color;
}

static void fillRect(int x, int y, int w, int h, u16 color)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    int x2 = x + w;
    int y2 = y + h;
    if (x2 > TFT_W) x2 = TFT_W;
    if (y2 > TFT_H) y2 = TFT_H;
    for (int yy = y; yy < y2; ++yy)
        for (int xx = x; xx < x2; ++xx)
            putPixel(xx, yy, color);
}

static void drawChar(int x, int y, char c, u16 color)
{
    if (c < 32 || c > 127) return;
    const u8 *ch = font5x7[c - 32];
    for (u8 col = 0; col < 5; col++) {
        u8 colbits = ch[col];
        for (u8 row = 0; row < 8; row++) {
            if (colbits & (1 << row)) putPixel(x + col, y + row, color);
        }
    }
}

static void drawString(int x, int y, const char *s, u16 color)
{
    int cx = x;
    while (*s) {
        drawChar(cx, y, *s++, color);
        cx += 8;
    }
}

/* Interrupt callback functions */
void HXO_btnXCallback(void)
{
    btnXPressed = 1;
}

void HXO_btnOCallback(void)
{
    btnOPressed = 1;
}

void HXO_btnNextCallback(void)
{
    btnNextPressed = 1;
}

/* Game logic functions */
static void resetGame(void)
{
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = CELL_EMPTY;
        }
    }
    currentPlayer = PLAYER_X;
    gameState = GAME_PLAYING;
    cursorRow = 0;
    cursorCol = 0;
}

static GameState_t checkWinner(void)
{
    /* Check rows */
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != CELL_EMPTY &&
            board[i][0] == board[i][1] &&
            board[i][1] == board[i][2]) {
            return (board[i][0] == CELL_X) ? GAME_X_WINS : GAME_O_WINS;
        }
    }

    /* Check columns */
    for (int j = 0; j < 3; j++) {
        if (board[0][j] != CELL_EMPTY &&
            board[0][j] == board[1][j] &&
            board[1][j] == board[2][j]) {
            return (board[0][j] == CELL_X) ? GAME_X_WINS : GAME_O_WINS;
        }
    }

    /* Check diagonals */
    if (board[0][0] != CELL_EMPTY &&
        board[0][0] == board[1][1] &&
        board[1][1] == board[2][2]) {
        return (board[0][0] == CELL_X) ? GAME_X_WINS : GAME_O_WINS;
    }

    if (board[0][2] != CELL_EMPTY &&
        board[0][2] == board[1][1] &&
        board[1][1] == board[2][0]) {
        return (board[0][2] == CELL_X) ? GAME_X_WINS : GAME_O_WINS;
    }

    /* Check for draw */
    u8 emptyCells = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == CELL_EMPTY) emptyCells++;
        }
    }

    if (emptyCells == 0) return GAME_DRAW;

    return GAME_PLAYING;
}

static void makeMove(Player_t player)
{
    if (gameState != GAME_PLAYING) return;
    if (board[cursorRow][cursorCol] != CELL_EMPTY) return;

    board[cursorRow][cursorCol] = (player == PLAYER_X) ? CELL_X : CELL_O;
    gameState = checkWinner();

    if (gameState == GAME_PLAYING) {
        currentPlayer = (currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;
    }
}

static void drawBoard(void)
{
    /* Clear screen */
    fillRect(0, 0, TFT_W, TFT_H, COLOR_BLACK);

    /* Draw title */
    drawString(45, 5, "TIC-TAC-TOE", COLOR_WHITE);

    /* Board area starts at y=25 */
    int boardStartY = 25;
    int cellSize = 35;
    int boardSize = cellSize * 3;
    int boardStartX = (TFT_W - boardSize) / 2;

    /* Draw grid lines */
    for (int i = 0; i <= 3; i++) {
        /* Vertical lines */
        int x = boardStartX + i * cellSize;
        for (int y = boardStartY; y < boardStartY + boardSize; y++) {
            putPixel(x, y, COLOR_WHITE);
        }
        /* Horizontal lines */
        int y = boardStartY + i * cellSize;
        for (int x2 = boardStartX; x2 < boardStartX + boardSize; x2++) {
            putPixel(x2, y, COLOR_WHITE);
        }
    }

    /* Draw X's and O's */
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int cellX = boardStartX + j * cellSize + cellSize/2 - 4;
            int cellY = boardStartY + i * cellSize + cellSize/2 - 4;

            if (board[i][j] == CELL_X) {
                drawString(cellX, cellY, "X", COLOR_RED);
            } else if (board[i][j] == CELL_O) {
                drawString(cellX, cellY, "O", COLOR_BLUE);
            }
        }
    }

    /* Draw cursor */
    if (gameState == GAME_PLAYING) {
        int cursorX = boardStartX + cursorCol * cellSize + 2;
        int cursorY = boardStartY + cursorRow * cellSize + 2;
        fillRect(cursorX, cursorY, cellSize - 4, cellSize - 4, COLOR_YELLOW);

        /* Redraw cell content over cursor if exists */
        int cellX = boardStartX + cursorCol * cellSize + cellSize/2 - 4;
        int cellY = boardStartY + cursorRow * cellSize + cellSize/2 - 4;

        if (board[cursorRow][cursorCol] == CELL_X) {
            drawString(cellX, cellY, "X", COLOR_RED);
        } else if (board[cursorRow][cursorCol] == CELL_O) {
            drawString(cellX, cellY, "O", COLOR_BLUE);
        }
    }

    /* Draw current player indicator */
    if (gameState == GAME_PLAYING) {
        char playerStr[20];
        if (currentPlayer == PLAYER_X) {
            drawString(10, 130, "Player X Turn", COLOR_RED);
        } else {
            drawString(10, 130, "Player O Turn", COLOR_BLUE);
        }
    }

    /* Draw game over message */
    if (gameState != GAME_PLAYING) {
        switch (gameState) {
            case GAME_X_WINS:
                drawString(30, 130, "X WINS!", COLOR_RED);
                break;
            case GAME_O_WINS:
                drawString(30, 130, "O WINS!", COLOR_BLUE);
                break;
            case GAME_DRAW:
                drawString(35, 130, "DRAW!", COLOR_YELLOW);
                break;
            default:
                break;
        }
        drawString(10, 145, "Press NEXT to restart", COLOR_WHITE);
    }

    /* Draw controls */
    drawString(10, 150, "X:Place O:Place NEXT:Move", COLOR_GRAY);
}

void HXO_voidInit(void)
{
    /* Configure EXTI for XO game buttons */
    // X button - EXTI11
    MEXTI_voidSetInterruptPort(EXTI_Line8, EXTI_PORTA);
    MEXTI_voidSetTrigger(EXTI_Line8, EXTI_Falling);
    MEXTI_voidCallBack(HXO_btnXCallback, EXTI_Line8);
    MEXTI_voidEnable(EXTI_Line8);

    // O button - EXTI12
    MEXTI_voidSetInterruptPort(EXTI_Line9, EXTI_PORTA);
    MEXTI_voidSetTrigger(EXTI_Line9, EXTI_Falling);
    MEXTI_voidCallBack(HXO_btnOCallback, EXTI_Line9);
    MEXTI_voidEnable(EXTI_Line9);

    // NEXT button - EXTI13
    MEXTI_voidSetInterruptPort(EXTI_Line10, EXTI_PORTA);
    MEXTI_voidSetTrigger(EXTI_Line10, EXTI_Falling);
    MEXTI_voidCallBack(HXO_btnNextCallback, EXTI_Line10);
    MEXTI_voidEnable(EXTI_Line10);

    /* Enable NVIC interrupts for EXTI lines */
    MNVIC_voidEnable(40); // EXTI15_10 (covers EXTI11, EXTI12, EXTI13)
    MNVIC_voidEnable(23);

    /* Initialize game */
    resetGame();
    drawBoard();
}

void HXO_voidPoll(void)
{
    /* Check button flags set by interrupts */
    if (btnXPressed) {
        btnXPressed = 0; // Clear flag

        if (gameState == GAME_PLAYING) {
            makeMove(PLAYER_X);
            drawBoard();
        }
    }

    if (btnOPressed) {
        btnOPressed = 0; // Clear flag

        if (gameState == GAME_PLAYING) {
            makeMove(PLAYER_O);
            drawBoard();
        }
    }

    if (btnNextPressed) {
        btnNextPressed = 0; // Clear flag

        if (gameState == GAME_PLAYING) {
            /* Move cursor to next position */
            do {
                cursorCol++;
                if (cursorCol >= 3) {
                    cursorCol = 0;
                    cursorRow++;
                    if (cursorRow >= 3) {
                        cursorRow = 0;
                    }
                }
            } while (board[cursorRow][cursorCol] != CELL_EMPTY);

            drawBoard();
        } else {
            /* Restart game */
            resetGame();
            drawBoard();
        }
    }
}

const u16* HXO_u16GetFrameBuffer(void)
{
    return GlobalFrameBuffer;
}
