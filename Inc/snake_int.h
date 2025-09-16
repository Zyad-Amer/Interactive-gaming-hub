/*
 * snake_int.h
 *
 *  Created on: Sep 13, 2025
 *      Author: ahmed
 */

#ifndef SNAKE_INT_H_
#define SNAKE_INT_H_


/***************************************************/
/*              Public Functions                   */
/***************************************************/

/**
 * @brief Initialize the Snake game
 * @param None
 * @return None
 * @note Must be called before any other Snake functions
 */
void Snake_voidInit(void);

/**
 * @brief Main game loop - call continuously in main
 * @param None
 * @return None
 * @note This function contains infinite loop
 */
void Snake_voidGameLoop(void);

/**
 * @brief Update game state (single frame update)
 * @param None
 * @return None
 * @note Alternative to GameLoop for manual control
 */
void Snake_voidUpdate(void);

/**
 * @brief Restart the game
 * @param None
 * @return None
 * @note Resets game state and starts over
 */
void Snake_voidRestart(void);

/**
 * @brief Get current game score
 * @param None
 * @return u16 Current score
 */
u16 Snake_u16GetScore(void);

/**
 * @brief Check if game is over
 * @param None
 * @return u8 1 if game over, 0 if still playing
 */
u8 Snake_u8IsGameOver(void);

/**
 * @brief Handle input from buttons
 * @param None
 * @return None
 * @note Called automatically in update, can be called manually
 */
void Snake_voidHandleInput(void);

#endif /* SNAKE_INT_H */
