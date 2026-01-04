/*
 * AI.h
 *
 *  Created on: 04-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_AI_H_
#define INC_AI_H_

#include "Game.h"

typedef struct {
	GAME_Engine_t * game_state;
}AI_t;

void AI_ctor(AI_t * const me, GAME_Engine_t * game_state);
void AI_reset(AI_t * const me);
key_action_e AI_get_action(AI_t * const me);

#endif /* INC_AI_H_ */
