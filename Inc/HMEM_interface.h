/*
 * Created on: Sep 13, 2025
 * Author: Moustfa Youssef
 */
#ifndef HMEM_INTERFACE_H_
#define HMEM_INTERFACE_H_

#include "STD_TYPES.h"

typedef enum {
    HMEM_CARD_FACE_DOWN = 0,
    HMEM_CARD_FACE_UP,
    HMEM_CARD_MATCHED
} HMEM_CardState_t;

typedef enum {
    HMEM_STATE_PLAYING = 0,
    HMEM_STATE_SHOWING_MISMATCH,
    HMEM_STATE_WON
} HMEM_GameState_t;

/* Initialize memory game driver */
void HMEM_voidInit(void);

/* Poll function - call periodically from main loop */
void HMEM_voidPoll(void);

/* Return pointer to internal framebuffer */
const u16* HMEM_u16GetFrameBuffer(void);

/* Reset/start a new game */
void HMEM_voidResetGame(void);

/* Get current game state */
HMEM_GameState_t HMEM_GameStateGet(void);

/* Get number of moves made */
u16 HMEM_u16GetMoves(void);

#endif /* HMEM_INTERFACE_H_ */
