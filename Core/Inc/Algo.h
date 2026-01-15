/*
 * Algo.h
 *
 *  Created on: 04-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_ALGO_H_
#define INC_ALGO_H_

#include "Game.h"

#define SHORTCUT_THRESHOLD      (MAX_SNAKE_LEN / 2)

typedef struct {
	GAME_Engine_t *game_state;
	C_COORDINATES_t ham_path[MAX_SNAKE_LEN];
	uint8_t grid_to_index[DISPLAY_ROWS][DISPLAY_COLS];
} ALGO_t;

void ALGO_ctor(ALGO_t *const me, GAME_Engine_t *game_state);
void ALGO_reset(ALGO_t *const me);
key_action_e ALGO_get_action(ALGO_t *const me);

#endif /* INC_ALGO_H_ */
