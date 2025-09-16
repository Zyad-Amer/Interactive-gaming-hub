/*
 * Created on: Sep 13, 2025
 * Author: Moustfa Youssef
 */
#ifndef HMEM_PRIVATE_H_
#define HMEM_PRIVATE_H_

#include "HMEM_config.h"
#include "HMEM_interface.h"

/* Card structure */
typedef struct {
    u8 symbol;                    /* symbol ID (0-7 for 8 different symbols) */
    HMEM_CardState_t state;       /* face down, face up, or matched */
} HMEM_Card_t;

/* Helper functions */
static void HMEM_voidDrawFrame(void);
static void HMEM_voidDrawCard(u8 row, u8 col);
static void HMEM_voidDrawCursor(u8 row, u8 col);
static void HMEM_voidDrawSymbol(u16 cx, u16 cy, u8 symbol, u16 color);
static void HMEM_voidFillRect(u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
static void HMEM_voidSetPixel(u16 x, u16 y, u16 color);
static void HMEM_voidDrawLine(int x0, int y0, int x1, int y1, u16 color);
static void HMEM_voidShuffleCards(void);
static u8 HMEM_u8CheckWin(void);
static u32 HMEM_u32GetMillis(void);

/* Card geometry helpers */
static void HMEM_voidGetCardBounds(u8 row, u8 col, u16 *x0, u16 *y0, u16 *x1, u16 *y1);
static void HMEM_voidGetCardCenter(u8 row, u8 col, u16 *cx, u16 *cy);

#endif /* HMEM_PRIVATE_H_ */
