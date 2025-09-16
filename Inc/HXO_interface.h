#ifndef HXO_INTERFACE_H_
#define HXO_INTERFACE_H_

#include "STD_TYPES.h"

typedef enum {
    HXO_EMPTY = 0,
    HXO_X,
    HXO_O
} HXO_Cell_t;

typedef enum {
    HXO_PLAYER_X = 0,
    HXO_PLAYER_O
} HXO_Player_t;

/* Initialize game driver (buttons config, TFT init not required here but ok) */
void HXO_voidInit(void);

/* Poll function to run game loop step (call periodically from main loop).
   It handles button reads, updates board, updates internal framebuffer.
   After calling HXO_voidPoll(), call HTFT_voidDisplay(HXO_u16GetFrameBuffer());
*/
void HXO_voidPoll(void);

/* Return pointer to internal framebuffer to pass to HTFT_voidDisplay */
const u16* HXO_u16GetFrameBuffer(void);

/* Reset/start a new game */
void HXO_voidResetGame(void);

/* Optional: Get winner state
   Returns HXO_EMPTY if no winner yet, HXO_X/HXO_O if winner, or HXO_EMPTY + draw flag checked by another function */
HXO_Cell_t HXO_CellGetWinner(void);

/* Optional: returns 1 if draw and board full */
u8 HXO_u8IsDraw(void);

#endif /* HXO_INTERFACE_H_ */
