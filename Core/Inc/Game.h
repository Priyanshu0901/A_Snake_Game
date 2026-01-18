/*
 * Game.h
 *
 *  Created on: 02-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_

#include "Canvas.h"
#include "Input.h"  // Only for key_action_e enum type

#define REFRESH_RATE 60
#define INPUT_RATE 30
#define RENDER_RATE 60

// Note: TICK_RATE is now dynamic - set via game->level_tick_rate
// AI mode: 15 Hz (fast)
// Manual mode: 5 Hz (slower for human)

#define MAX_SNAKE_LEN (DISPLAY_COLS * DISPLAY_ROWS)

typedef struct {
	CANVAS_t * canvas;

	// --- Snake State ---
	C_COORDINATES_t body[MAX_SNAKE_LEN];
	uint8_t length;
	key_action_e current_dir;

	// Food State
	C_COORDINATES_t food;
	uint8_t food_color;

	// Game statistics (exposed for UI to read)
	int game_counter;
	int game_won_counter;

	// State flags
	bool game_over;              // Game ended (collision/win)
	bool game_state_has_updated; // Stats changed, UI should refresh

	// Dynamic tick rate (5 for manual, 15 for AI)
	uint8_t level_tick_rate;
}GAME_Engine_t;

/**
 * @brief Initialize the game engine
 * @param me Pointer to GAME_Engine instance
 * @param canvas Pointer to initialized canvas
 * @note Input is no longer passed to game - it's managed externally
 */
void GAME_ctor(GAME_Engine_t * const me, CANVAS_t * canvas);

/**
 * @brief Update game direction based on input action
 * @param me Pointer to GAME_Engine instance
 * @param new_action Input action to process (directional movement)
 * @note This only updates direction, doesn't move the snake
 *       Call GAME_tick() to actually advance the game state
 */
void GAME_update(GAME_Engine_t *const me, key_action_e const new_action);

/**
 * @brief Advance game state by one tick (move snake, check collisions)
 * @param me Pointer to GAME_Engine instance
 * @note Should be called at level_tick_rate (5-15 Hz depending on mode)
 */
void GAME_tick(GAME_Engine_t * const me);

/**
 * @brief Render the current game state to the canvas
 * @param me Pointer to GAME_Engine instance
 * @note Should be called at RENDER_RATE (60 Hz)
 */
void GAME_render(GAME_Engine_t * const me);

/**
 * @brief Reset game to initial state
 * @param me Pointer to GAME_Engine instance
 * @note This is called on collision or win, increments game_counter
 */
void GAME_reset(GAME_Engine_t *const me);

#endif /* INC_GAME_H_ */
