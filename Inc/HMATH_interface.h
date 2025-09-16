/*
 * Created on: Sep 13, 2025
 * Author: Moustfa Youssef
 */
#ifndef HMATH_INTERFACE_H_
#define HMATH_INTERFACE_H_

#include "STD_TYPES.h"

typedef enum {
    MATH_OP_ADD = 0,
    MATH_OP_SUB,
    MATH_OP_MUL
} MATH_Operation_t;

typedef enum {
    MATH_STATE_QUESTION = 0,
    MATH_STATE_CORRECT,
    MATH_STATE_WRONG,
    MATH_STATE_NEXT_QUESTION
} MATH_GameState_t;

/* Initialize math quiz game */
void HMATH_voidInit(void);

/* Poll function to run game loop - call periodically from main loop */
void HMATH_voidPoll(void);

/* Return pointer to internal framebuffer to pass to HTFT_voidDisplay */
const u16* HMATH_u16GetFrameBuffer(void);

/* Get current score */
u8 HMATH_u8GetScore(void);

/* Get current question number */
u8 HMATH_u8GetQuestionNumber(void);

/* Reset game to start over */
void HMATH_voidResetGame(void);

#endif /* HMATH_INTERFACE_H_ */
