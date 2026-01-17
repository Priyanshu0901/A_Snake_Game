/*
 * Game.h
 *
 *  Created on: 02-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_

#include "Input.h"
#include "Canvas.h"
#include "App_UI.h"

#define REFRESH_RATE 60
#define INPUT_RATE 30
#define RENDER_RATE 60
#ifdef ALGO
	#define TICK_RATE 15
#else
	#define TICK_RATE 5
#endif

#define MAX_SNAKE_LEN (DISPLAY_COLS * DISPLAY_ROWS)

typedef struct {
	CANVAS_t * canvas;
	INPUT_t * input;
	APP_UI_t * UI;

	// --- Snake State ---
	C_COORDINATES_t body[MAX_SNAKE_LEN];
	uint8_t length;
	key_action_e current_dir;

	C_COORDINATES_t food;
	uint8_t food_color;

	bool game_over;

}GAME_Engine_t;

void GAME_ctor(GAME_Engine_t * const me, CANVAS_t * canvas, INPUT_t * input, APP_UI_t * ui);

void GAME_update(GAME_Engine_t *const me, key_action_e const new_action);
void GAME_tick(GAME_Engine_t * const me);
void GAME_render(GAME_Engine_t * const me);

void GAME_reset(GAME_Engine_t *const me);

#endif /* INC_GAME_H_ */
