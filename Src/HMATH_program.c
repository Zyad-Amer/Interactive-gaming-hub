/*
 * Created on: Sep 13, 2025
 * Author: Moustfa Youssef
 */
#include "STD_TYPES.h"
#include <stdlib.h>
#include "BIT_MATH.h"
#include "MGPIO_int.h"
#include "MSTK_int.h"
#include "HTFT_int.h"
#include "MEXTI_Interface.h"
#include "MNVIC_Interface.h"

#include "HMATH_config.h"
#include "HMATH_interface.h"
#include "HMATH_private.h"
#include "FrameBuffer.h"

#define MATH_frame GlobalFrameBuffer

/* Game state */
static MATH_Question_t current_question;
static MATH_GameState_t game_state;
static u8 score;
static u8 question_number;
static u8 selected_option;

/* Button state flags for interrupt handling */
static volatile u8 btnAPressed = 0;
static volatile u8 btnBPressed = 0;
static volatile u8 btnCPressed = 0;

/* Timing */
static u32 feedback_start_time = 0;

/* Simple LFSR for random numbers */
static u16 lfsr = 0xBCE1;

/* 5x7 font data for numbers and basic chars */
static const u8 font5x7[96][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // space
    {0x00, 0x00, 0x5f, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7f, 0x14, 0x7f, 0x14}, // #
    {0x24, 0x2a, 0x7f, 0x2a, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1c, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1c, 0x00}, // )
    {0x14, 0x08, 0x3e, 0x08, 0x14}, // *
    {0x08, 0x08, 0x3e, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 0
    {0x00, 0x42, 0x7f, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4b, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7f, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1e}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x59, 0x51, 0x3e}, // @
    {0x7e, 0x11, 0x11, 0x11, 0x7e}, // A
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3e, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7f, 0x41, 0x41, 0x22, 0x1c}, // D
    {0x7f, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7f, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3e, 0x41, 0x49, 0x49, 0x7a}, // G
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, // H
    {0x00, 0x41, 0x7f, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3f, 0x01}, // J
    {0x7f, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7f, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // M
    {0x7f, 0x04, 0x08, 0x10, 0x7f}, // N
    {0x3e, 0x41, 0x41, 0x41, 0x3e}, // O
    {0x7f, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3e, 0x41, 0x51, 0x21, 0x5e}, // Q
    {0x7f, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7f, 0x01, 0x01}, // T
    {0x3f, 0x40, 0x40, 0x40, 0x3f}, // U
    {0x1f, 0x20, 0x40, 0x20, 0x1f}, // V
    {0x3f, 0x40, 0x38, 0x40, 0x3f}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
};

/* Interrupt callback functions */
void HMATH_btnACallback(void)
{
    btnAPressed = 1;
}

void HMATH_btnBCallback(void)
{
    btnBPressed = 1;
}

void HMATH_btnCCallback(void)
{
    btnCPressed = 1;
}

/* ------------ Public implementations ------------- */

void HMATH_voidInit(void)
{
    /* Configure EXTI for Math Quiz buttons */
    // A button - EXTI8
    MEXTI_voidSetInterruptPort(EXTI_Line8, EXTI_PORTA);
    MEXTI_voidSetTrigger(EXTI_Line8, EXTI_Falling);
    MEXTI_voidCallBack(HMATH_btnCCallback, EXTI_Line8);
    MEXTI_voidEnable(EXTI_Line8);

    // B button - EXTI9
    MEXTI_voidSetInterruptPort(EXTI_Line9, EXTI_PORTA);
    MEXTI_voidSetTrigger(EXTI_Line9, EXTI_Falling);
    MEXTI_voidCallBack(HMATH_btnBCallback, EXTI_Line9);
    MEXTI_voidEnable(EXTI_Line9);

    // C button - EXTI10
    MEXTI_voidSetInterruptPort(EXTI_Line10, EXTI_PORTA);
    MEXTI_voidSetTrigger(EXTI_Line10, EXTI_Falling);
    MEXTI_voidCallBack(HMATH_btnACallback, EXTI_Line10);
    MEXTI_voidEnable(EXTI_Line10);

    /* Enable NVIC interrupts */
    MNVIC_voidEnable(40); // EXTI15_10 (covers EXTI10)
    MNVIC_voidEnable(23); // EXTI9_5 (covers EXTI8, EXTI9)
    
    /* Initialize TFT display */
    HTFT_voidInit();
    
    /* Initialize random seed */
    HMATH_voidInitRandom();
    
    /* Reset game */
    HMATH_voidResetGame();
    
    /* Initial display */
    HMATH_voidDisplayQuestion();
    HMATH_voidDisplayScore();
}

void HMATH_voidResetGame(void)
{
    score = 0;
    question_number = 1;
    selected_option = 0;
    game_state = MATH_STATE_QUESTION;
    HMATH_voidGenerateQuestion();
}

const u16* HMATH_u16GetFrameBuffer(void)
{
    return GlobalFrameBuffer;
}

u8 HMATH_u8GetScore(void)
{
    return score;
}

u8 HMATH_u8GetQuestionNumber(void)
{
    return question_number;
}

void HMATH_voidPoll(void)
{
    u32 now = HMATH_u32GetMillis();
    
    switch (game_state) {
        case MATH_STATE_QUESTION:
            /* Handle answer selection using interrupt flags */
            if (btnAPressed) {
                btnAPressed = 0; // Clear flag
                selected_option = 0;
                u8 correct = HMATH_u8CheckAnswer(0);
                HMATH_voidShowFeedback(correct, 0);
                if (correct) {
                    score++;
                    game_state = MATH_STATE_CORRECT;
                } else {
                    game_state = MATH_STATE_WRONG;
                }
                feedback_start_time = now;
            }
            else if (btnBPressed) {
                btnBPressed = 0; // Clear flag
                selected_option = 1;
                u8 correct = HMATH_u8CheckAnswer(1);
                HMATH_voidShowFeedback(correct, 1);
                if (correct) {
                    score++;
                    game_state = MATH_STATE_CORRECT;
                } else {
                    game_state = MATH_STATE_WRONG;
                }
                feedback_start_time = now;
            }
            else if (btnCPressed) {
                btnCPressed = 0; // Clear flag
                selected_option = 2;
                u8 correct = HMATH_u8CheckAnswer(2);
                HMATH_voidShowFeedback(correct, 2);
                if (correct) {
                    score++;
                    game_state = MATH_STATE_CORRECT;
                } else {
                    game_state = MATH_STATE_WRONG;
                }
                feedback_start_time = now;
            }
            break;
            
        case MATH_STATE_CORRECT:
        case MATH_STATE_WRONG:
            /* Show feedback for a while, then move to next question */
            if (now - feedback_start_time > MATH_FEEDBACK_DELAY) {
                question_number++;
                HMATH_voidGenerateQuestion();
                HMATH_voidDisplayQuestion();
                HMATH_voidDisplayScore();
                game_state = MATH_STATE_QUESTION;
            }
            break;
            
        default:
            game_state = MATH_STATE_QUESTION;
            break;
    }
}

/* ------------ Private implementations ------------- */

static void HMATH_voidGenerateQuestion(void)
{
    /* Generate random numbers */
    current_question.num1 = HMATH_u16GetRandom(MATH_NUMBER_RANGE + 1);
    current_question.num2 = HMATH_u16GetRandom(MATH_NUMBER_RANGE + 1);
    
    /* Choose random operation */
    current_question.operation = (MATH_Operation_t)HMATH_u16GetRandom(3);
    
    /* Calculate correct answer */
    switch (current_question.operation) {
        case MATH_OP_ADD:
            current_question.correct_answer = current_question.num1 + current_question.num2;
            break;
        case MATH_OP_SUB:
            current_question.correct_answer = current_question.num1 - current_question.num2;
            break;
        case MATH_OP_MUL:
            current_question.correct_answer = current_question.num1 * current_question.num2;
            break;
    }
    
    /* Choose random position for correct answer (0, 1, or 2) */
    current_question.correct_index = HMATH_u16GetRandom(3);
    
    /* Place correct answer */
    current_question.options[current_question.correct_index] = current_question.correct_answer;
    
    /* Generate wrong answers */
    for (u8 i = 0; i < 3; i++) {
        if (i != current_question.correct_index) {
            s16 wrong_answer;
            do {
                /* Generate wrong answer within reasonable range */
                wrong_answer = current_question.correct_answer + (HMATH_u16GetRandom(21) - 10);
            } while (wrong_answer == current_question.correct_answer);
            current_question.options[i] = wrong_answer;
        }
    }
}

static void HMATH_voidDisplayQuestion(void)
{
    HMATH_voidClearFrame();
    
    /* Display question number and score at top */
    HMATH_voidDisplayScore();
    
    /* Display the math problem */
    char op_char;
    switch (current_question.operation) {
        case MATH_OP_ADD: op_char = '+'; break;
        case MATH_OP_SUB: op_char = '-'; break;
        case MATH_OP_MUL: op_char = '*'; break;
        default: op_char = '+'; break;
    }
    
    /* Center the question */
    u16 question_x = MATH_SCREEN_WIDTH / 2;
    u16 question_y = MATH_QUESTION_Y;
    
    /* Draw numbers and operator */
    u16 x_pos = 20;
    HMATH_voidDrawNumber(current_question.num1, x_pos, question_y, MATH_COLOR_TEXT);
    x_pos += HMATH_u8GetNumberWidth(current_question.num1) + 8;
    
    HMATH_voidDrawChar(op_char, x_pos, question_y, MATH_COLOR_TEXT);
    x_pos += 13;
    
    HMATH_voidDrawNumber(current_question.num2, x_pos, question_y, MATH_COLOR_TEXT);
    x_pos += HMATH_u8GetNumberWidth(current_question.num2) + 8;
    
    HMATH_voidDrawChar('=', x_pos, question_y, MATH_COLOR_TEXT);
    x_pos += 13;
    
    HMATH_voidDrawChar('?', x_pos, question_y, MATH_COLOR_TEXT);
    
    /* Display answer options */
    for (u8 i = 0; i < 3; i++) {
        u16 option_y = MATH_OPTION_START_Y + (i * MATH_OPTION_SPACING);
        char option_letter = 'A' + i;
        
        HMATH_voidDrawChar(option_letter, MATH_TEXT_MARGIN, option_y, MATH_COLOR_TEXT);
        HMATH_voidDrawChar(')', MATH_TEXT_MARGIN + 8, option_y, MATH_COLOR_TEXT);
        HMATH_voidDrawNumber(current_question.options[i], MATH_TEXT_MARGIN + 20, option_y, MATH_COLOR_TEXT);
    }
}

static void HMATH_voidDisplayScore(void)
{
    /* Display score */
    HMATH_voidDrawText("Q:", 5, MATH_SCORE_Y, MATH_COLOR_TEXT);
    HMATH_voidDrawNumber(question_number, 20, MATH_SCORE_Y, MATH_COLOR_TEXT);
    
    HMATH_voidDrawText("S:", 88, MATH_SCORE_Y, MATH_COLOR_TEXT);
    HMATH_voidDrawNumber(score, 110, MATH_SCORE_Y, MATH_COLOR_TEXT);
}

static u8 HMATH_u8CheckAnswer(u8 selected_option)
{
    return (selected_option == current_question.correct_index) ? 1 : 0;
}

static void HMATH_voidShowFeedback(u8 is_correct, u8 selected_option)
{
    /* Clear area and show feedback */
    HMATH_voidFillRect(0, MATH_OPTION_START_Y + 3 * MATH_OPTION_SPACING, 
                       MATH_SCREEN_WIDTH - 1, MATH_SCREEN_HEIGHT - 1, MATH_COLOR_BG);
    
    u16 feedback_y = MATH_OPTION_START_Y + 3 * MATH_OPTION_SPACING + 10;
    
    if (is_correct) {
        HMATH_voidDrawText("CORRECT!", 30, feedback_y, MATH_COLOR_CORRECT);
    } else {
        HMATH_voidDrawText("WRONG!", 35, feedback_y, MATH_COLOR_WRONG);
        HMATH_voidDrawText("Answer:", 20, feedback_y + 15, MATH_COLOR_TEXT);
        HMATH_voidDrawNumber(current_question.correct_answer, 70, feedback_y + 15, MATH_COLOR_CORRECT);
    }
    
    /* Update display immediately */
    HTFT_voidDisplay(IMAGE_SIZE);
}

static void HMATH_voidClearFrame(void)
{
    for (u32 i = 0; i < IMAGE_SIZE; i++) {
        MATH_frame[i] = MATH_COLOR_BG;
    }
}

static void HMATH_voidDrawText(const char* text, u16 x, u16 y, u16 color)
{
    u16 char_x = x;
    u8 i = 0;
    
    while (text[i] != '\0') {
        HMATH_voidDrawChar(text[i], char_x, y, color);
        char_x += 8;
        i++;
    }
}

static void HMATH_voidDrawChar(char c, u16 x, u16 y, u16 color)
{
    if (c < 32 || c > 126) return; /* Out of range */
    
    const u8* char_data = font5x7[c - 32];
    
    for (u8 col = 0; col < 5; col++) {
        for (u8 row = 0; row < 8; row++) {
            if (char_data[col] & (1 << row)) {
                HMATH_voidSetPixel(x + col, y + row, color);
            }
        }
    }
}

static void HMATH_voidDrawNumber(s16 number, u16 x, u16 y, u16 color)
{
    if (number == 0) {
        HMATH_voidDrawChar('0', x, y, color);
        return;
    }
    
    char num_str[8];
    u8 i = 0;
    u8 is_negative = 0;
    
    if (number < 0) {
        is_negative = 1;
        number = -number;
    }
    
    /* Convert to string (reverse order) */
    while (number > 0) {
        num_str[i++] = '0' + (number % 10);
        number /= 10;
    }
    
    u16 draw_x = x;
    
    /* Draw negative sign if needed */
    if (is_negative) {
        HMATH_voidDrawChar('-', draw_x, y, color);
        draw_x += 8;
    }
    
    /* Draw digits in correct order */
    for (s8 j = i - 1; j >= 0; j--) {
        HMATH_voidDrawChar(num_str[j], draw_x, y, color);
        draw_x += 8;
    }
}

static void HMATH_voidFillRect(u16 x0, u16 y0, u16 x1, u16 y1, u16 color)
{
    if (x1 < x0) { u16 t = x0; x0 = x1; x1 = t; }
    if (y1 < y0) { u16 t = y0; y0 = y1; y1 = t; }
    if (x0 >= MATH_SCREEN_WIDTH) return;
    if (y0 >= MATH_SCREEN_HEIGHT) return;
    if (x1 >= MATH_SCREEN_WIDTH) x1 = MATH_SCREEN_WIDTH - 1;
    if (y1 >= MATH_SCREEN_HEIGHT) y1 = MATH_SCREEN_HEIGHT - 1;

    for (u16 y = y0; y <= y1; y++) {
        u32 base = (u32)y * MATH_SCREEN_WIDTH;
        for (u16 x = x0; x <= x1; x++) {
            MATH_frame[base + x] = color;
        }
    }
}

static void HMATH_voidSetPixel(u16 x, u16 y, u16 color)
{
    if (x >= MATH_SCREEN_WIDTH || y >= MATH_SCREEN_HEIGHT) return;
    MATH_frame[(u32)y * MATH_SCREEN_WIDTH + x] = color;
}

static u32 HMATH_u32GetMillis(void)
{
    /* Simple millisecond counter using static variable */
    static u32 t = 0;
    t += 20; /* Assume poll called every ~20ms */
    return t;
}

static u16 HMATH_u16GetRandom(u16 max)
{
    /* Simple Linear Feedback Shift Register for pseudo-random numbers */
    u16 bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
    lfsr = (lfsr >> 1) | (bit << 15);
    return lfsr % max;
}

static void HMATH_voidInitRandom(void)
{
    /* Initialize with a non-zero seed */
    lfsr = 0xBCE1;
}

static u8 HMATH_u8GetTextWidth(const char* text)
{
    u8 i = 0;
    while (text[i] != '\0') i++;
    return i * 8; /* Each character is 8 pixels wide including spacing */
}

static u8 HMATH_u8GetNumberWidth(s16 number)
{
    if (number == 0) return 8;
    
    u8 digits = 0;
    u8 is_negative = 0;
    
    if (number < 0) {
        is_negative = 1;
        number = -number;
    }
    
    while (number > 0) {
        digits++;
        number /= 10;
    }
    
    return (digits + is_negative) * 8;
}
