/*
 * Created on: Sep 13, 2025
 * Author: Moustfa Youssef
 */
#ifndef HMATH_PRIVATE_H_
#define HMATH_PRIVATE_H_

#include "HMATH_config.h"
#include "HMATH_interface.h"

/* Question structure */
typedef struct {
    u8 num1;
    u8 num2;
    MATH_Operation_t operation;
    s16 correct_answer;
    s16 options[3];
    u8 correct_index;  /* 0, 1, or 2 */
} MATH_Question_t;

/* Drawing functions */
static void HMATH_voidClearFrame(void);
static void HMATH_voidDrawText(const char* text, u16 x, u16 y, u16 color);
static void HMATH_voidDrawChar(char c, u16 x, u16 y, u16 color);
static void HMATH_voidDrawNumber(s16 number, u16 x, u16 y, u16 color);
static void HMATH_voidFillRect(u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
static void HMATH_voidSetPixel(u16 x, u16 y, u16 color);

/* Game logic functions */
static void HMATH_voidGenerateQuestion(void);
static void HMATH_voidDisplayQuestion(void);
static void HMATH_voidDisplayScore(void);
static u8 HMATH_u8CheckAnswer(u8 selected_option);
static void HMATH_voidShowFeedback(u8 is_correct, u8 selected_option);
static u32 HMATH_u32GetMillis(void);

/* Random number generation */
static u16 HMATH_u16GetRandom(u16 max);
static void HMATH_voidInitRandom(void);

/* Helper functions */
static u8 HMATH_u8GetTextWidth(const char* text);
static u8 HMATH_u8GetNumberWidth(s16 number);

#endif /* HMATH_PRIVATE_H_ */
