/*
 * AI.c
 *
 *  Created on: 04-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "AI.h"
#include "stdlib.h"

#include "Display.h"

void AI_ctor(AI_t * const me, GAME_Engine_t * game_state){
	me->game_state = game_state;
	AI_reset(me);
}

void AI_reset(AI_t * const me){
	log_message("AI", LOG_INFO, "RESET called");


	log_message("AI", LOG_INFO, "Ready to play");
	me->game_state->game_over = false;
}

key_action_e AI_get_action(AI_t * const me)
{
	return (key_action_e)rand() % ACTION_CONFIRM;
}
