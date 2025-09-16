/* HMENU_program.c - Menu using shared font5x7 and GlobalFrameBuffer */

#include "STD_TYPES.h"
#include "HTFT_int.h"
#include "MSTK_int.h"
#include "MGPIO_int.h"
#include "HMENU_interface.h"

#include "HMEM_interface.h"
#include "HXO_interface.h"
#include "HMATH_interface.h"
#include "snake_int.h"

#include "FrameBuffer.h"
#include "font5x7.h"

#define TFT_W 128
#define TFT_H 160

/* Button pins */
#define BTN_UP_PORT   PORTA
#define BTN_UP_PIN    PIN8
#define BTN_DOWN_PORT PORTA
#define BTN_DOWN_PIN  PIN9
#define BTN_SEL_PORT  PORTA
#define BTN_SEL_PIN   PIN10

typedef enum { MENU_MEM = 0, MENU_XO, MENU_MATH, MENU_SNAKE, MENU_COUNT } MenuItem_t;
static MenuItem_t selected = MENU_MEM;

/* forward */
static void drawMenu(void);
static u8 readButtonUp(void);
static u8 readButtonDown(void);
static u8 readButtonSelect(void);

/* debounce helper (blocking like earlier) */
static u8 sampleButton(u8 (*readFn)(void))
{
    if (readFn()) {
        MSTK_voidDelayms(10);
        if (readFn()) {
            while (readFn()) {} /* wait release */
            return 1;
        }
    }
    return 0;
}

/* ---------- drawing helpers using GlobalFrameBuffer ---------- */
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

/* Use same glyph drawing as your math code (5x7 font, 8px stride) */
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
        cx += 8; /* same spacing as HMATH code */
    }
}

/* Colors (RGB565) */
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_GRAY  0x8410
#define COLOR_BLUE  0x001F
#define COLOR_GREEN 0x07E0
#define COLOR_RED   0xF800
#define COLOR_YEL   0xFFE0
#define COLOR_ORNG  0xFD20  /* orange-like */

/* ------------ menu implementation ------------ */
void HMenu_voidInit(void)
{
    selected = MENU_MEM;
    drawMenu();
    HTFT_voidDisplay(GlobalFrameBuffer);
}

void HMenu_voidPoll(void)
{
    if (sampleButton(readButtonUp)) {
        if (selected == 0) selected = MENU_COUNT - 1;
        else selected = (MenuItem_t)(selected - 1);
        drawMenu();
        HTFT_voidDisplay(GlobalFrameBuffer);
    }
    if (sampleButton(readButtonDown)) {
        selected = (MenuItem_t)((selected + 1) % MENU_COUNT);
        drawMenu();
        HTFT_voidDisplay(GlobalFrameBuffer);
    }
    if (sampleButton(readButtonSelect)) {
        switch (selected) {
            case MENU_MEM:
                HMEM_voidInit();
                while (1) {
                    HMEM_voidPoll();
                    HTFT_voidDisplay(HMEM_u16GetFrameBuffer());
                    MSTK_voidDelayus(20);
                }
                break;
            case MENU_XO:
                HXO_voidInit();
                while (1) {
                    HXO_voidPoll();
                    HTFT_voidDisplay(HXO_u16GetFrameBuffer());
                    MSTK_voidDelayms(10);
                }
                break;
            case MENU_MATH:
                HMATH_voidInit();
                while (1) {
                    HMATH_voidPoll();
                    HTFT_voidDisplay(HMATH_u16GetFrameBuffer());
                    MSTK_voidDelayus(20);
                }
                break;
            case MENU_SNAKE:
                Snake_voidInit();
                Snake_voidGameLoop();
            default:
                break;
        }
    }
}

/* Draw menu: simple tiles + colored labels */
static void drawMenu(void)
{
    /* background */
    fillRect(0, 0, TFT_W, TFT_H, COLOR_BLACK);

    /* four tiles vertically */
    int tile_h = TFT_H / 4;
    for (int i = 0; i < 4; ++i) {
        int y = i * tile_h + 4;
        int h = tile_h - 8;
        u16 boxColor = COLOR_GRAY;
        u16 textColor = COLOR_WHITE;

        if (i == (int)selected) {
            boxColor = COLOR_WHITE;
            textColor = COLOR_BLACK;
        }
        fillRect(6, y, TFT_W - 12, h, boxColor);

        /* labels with distinct colors when not selected, black when selected */
        switch (i) {
            case 0: drawString(20, y + 8, "MEMORY MATCH",   (i == selected) ? textColor : COLOR_RED);   break;
            case 1: drawString(20, y + 8, "XO",    (i == selected) ? textColor : COLOR_GREEN); break;
            case 2: drawString(20, y + 8, "MATH QUIZ",  (i == selected) ? textColor : COLOR_BLUE);  break;
            case 3: drawString(20, y + 8, "SNAKE", (i == selected) ? textColor : COLOR_ORNG);  break;
        }

        /* selection arrow */
        if (i == (int)selected) {
            drawString(TFT_W - 28, y + 8, ">", COLOR_RED);
        }
    }

    /* top title */
    drawString(8, 4, "GAME HUB", COLOR_GREEN);
}

/* button read wrappers - pressed = 0 (active low) */
static u8 readButtonUp(void)    { return (MGPIO_u8GetPinValue(BTN_UP_PORT, BTN_UP_PIN) == 0); }
static u8 readButtonDown(void)  { return (MGPIO_u8GetPinValue(BTN_DOWN_PORT, BTN_DOWN_PIN) == 0); }
static u8 readButtonSelect(void){ return (MGPIO_u8GetPinValue(BTN_SEL_PORT, BTN_SEL_PIN) == 0); }
