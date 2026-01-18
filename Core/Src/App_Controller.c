/*
 * App_Controller.c
 *
 *  Created on: 18-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "App_Controller.h"
#include "debug_logger.h"
#include <stdio.h>

/* ========================================================================
 * PRIVATE HELPER FUNCTIONS
 * ======================================================================== */

/**
 * @brief Handle state transition logic and side effects
 */
static void transition_to_state(APP_Controller_t *me, APP_State_e new_state) {
	if (me->state == new_state)
		return;  // No-op if already in target state

	APP_State_e old_state = me->state;
	me->state = new_state;

	// Log state transitions
	log_message("APP_CTRL", LOG_INFO, "State: %d -> %d", old_state, new_state);

	// Handle state entry logic
	switch (new_state) {
	case APP_STATE_MENU:
		// Entering settings menu
		me->game_needs_tick = false;
		me->ui_needs_update = true;
		break;

	case APP_STATE_PLAYING:
		// Start/resume game
		me->game_needs_tick = true;
		me->ui_needs_update = true;

		// If returning from settings, ensure game isn't in game_over state
		if (old_state == APP_STATE_MENU || old_state == APP_STATE_PAUSED) {
			if (me->game->game_over && !me->game_was_paused) {
				// Game was over when we entered menu, don't auto-resume
				me->game_needs_tick = false;
			} else {
				me->game->game_over = false;  // Unpause
			}
		}
		break;

	case APP_STATE_PAUSED:
		// Game frozen while in settings
		me->game_needs_tick = false;
		me->ui_needs_update = true;
		break;

	case APP_STATE_GAME_OVER:
		// Game ended
		me->game_needs_tick = false;
		me->ui_needs_update = true;
		me->game->game_over = true;
		break;
	}
}

/**
 * @brief Handle input when in PLAYING state (MANUAL mode only)
 */
static void handle_input_playing_manual(APP_Controller_t *me,
		key_action_e action) {
	if (action == ACTION_NONE)
		return;

	// CONFIRM opens settings menu and pauses game
	if (action == ACTION_CONFIRM) {
		me->previous_state = APP_STATE_PLAYING;
		me->game_was_paused = me->game->game_over; // Remember if game was already paused
		transition_to_state(me, APP_STATE_MENU);

		// Switch UI to settings page
		CHAR_CANVAS_switch_page(me->ui->canvas, SETTINGS_PAGE);
		me->ui->current_state = MENU_STATE_SETTINGS;
		me->ui->needs_refresh = true;
		return;
	}

	// All other input goes to the game
	GAME_update(me->game, action);
}

/**
 * @brief Handle input when in PLAYING state (AI mode)
 * AI doesn't need directional input, but can still respond to menu
 */
static void handle_input_playing_ai(APP_Controller_t *me, key_action_e action) {
	if (action == ACTION_NONE)
		return;

	// CONFIRM opens settings menu and pauses game
	if (action == ACTION_CONFIRM) {
		me->previous_state = APP_STATE_PLAYING;
		me->game_was_paused = me->game->game_over;
		transition_to_state(me, APP_STATE_MENU);

		CHAR_CANVAS_switch_page(me->ui->canvas, SETTINGS_PAGE);
		me->ui->current_state = MENU_STATE_SETTINGS;
		me->ui->needs_refresh = true;
		return;
	}

	// Note: Directional keys are ignored in AI mode
	// AI makes its own decisions at tick rate
}

/**
 * @brief Handle input when in MENU/PAUSED state
 */
static void handle_input_menu(APP_Controller_t *me, key_action_e action) {
	if (action == ACTION_NONE)
		return;

	// UP/DOWN to toggle play mode in settings
	if (action == ACTION_UP || action == ACTION_DOWN) {
		APP_CONTROLLER_toggle_play_mode(me);
		me->ui->needs_refresh = true;
		return;
	}

	// CONFIRM exits settings and returns to game
	if (action == ACTION_CONFIRM) {
		// Switch UI back to main page
		CHAR_CANVAS_switch_page(me->ui->canvas, MAIN_PAGE);
		me->ui->current_state = MENU_STATE_MAIN;
		me->ui->needs_refresh = true;

		// Return to previous state (usually PLAYING)
		transition_to_state(me, me->previous_state);
		return;
	}

	// Future: LEFT/RIGHT for other settings
}

/**
 * @brief Handle input when in GAME_OVER state
 */
static void handle_input_game_over(APP_Controller_t *me, key_action_e action) {
	if (action == ACTION_NONE)
		return;

	// CONFIRM opens settings
	if (action == ACTION_CONFIRM) {
		me->previous_state = APP_STATE_GAME_OVER;
		transition_to_state(me, APP_STATE_MENU);

		CHAR_CANVAS_switch_page(me->ui->canvas, SETTINGS_PAGE);
		me->ui->current_state = MENU_STATE_SETTINGS;
		me->ui->needs_refresh = true;
		return;
	}

	// Any other key restarts the game
	GAME_reset(me->game);
	transition_to_state(me, APP_STATE_PLAYING);
}

/* ========================================================================
 * PUBLIC API IMPLEMENTATION
 * ======================================================================== */

void APP_CONTROLLER_ctor(APP_Controller_t *me, GAME_Engine_t *game,
		APP_UI_t *ui, INPUT_t *input, ALGO_t *ai_player) {
	me->game = game;
	me->ui = ui;
	me->input = input;
	me->ai_player = ai_player;  // AI always available

	// Start in AI mode by default (can be changed via UI)
	me->play_mode = PLAY_MODE_AI;
	me->game->level_tick_rate = 15;  // AI mode tick rate

	// Start in playing state
	me->state = APP_STATE_PLAYING;
	me->previous_state = APP_STATE_PLAYING;
	me->game_needs_tick = true;
	me->ui_needs_update = true;
	me->game_was_paused = false;

	log_message("APP_CTRL", LOG_INFO, "Initialized in PLAYING state, mode=%d",
			me->play_mode);
}

void APP_CONTROLLER_process_input(APP_Controller_t *me) {
	// CRITICAL: Call INPUT_get_action() ONLY ONCE per cycle
	key_action_e action = INPUT_get_action(me->input);

	// Route input based on current state AND play mode
	switch (me->state) {
	case APP_STATE_PLAYING:
		if (me->play_mode == PLAY_MODE_MANUAL) {
			handle_input_playing_manual(me, action);
		} else {
			handle_input_playing_ai(me, action);
		}
		break;

	case APP_STATE_MENU:
	case APP_STATE_PAUSED:
		handle_input_menu(me, action);
		break;

	case APP_STATE_GAME_OVER:
		handle_input_game_over(me, action);
		break;
	}
}

void APP_CONTROLLER_update(APP_Controller_t *me) {
	// Only tick the game if we're in PLAYING state
	if (me->state == APP_STATE_PLAYING && me->game_needs_tick) {

		// AI makes its decision HERE at tick rate (5-15 Hz), not at input rate (30 Hz)
		if (me->play_mode == PLAY_MODE_AI && me->ai_player != NULL) {
			key_action_e ai_action = ALGO_get_action(me->ai_player);
			GAME_update(me->game, ai_action);
		}

		// Tick the game (move snake, check collisions)
		GAME_tick(me->game);

		// Check if game just ended
		if (me->game->game_over) {
			transition_to_state(me, APP_STATE_GAME_OVER);

			// Reset AI path when game ends
			if (me->play_mode == PLAY_MODE_AI && me->ai_player != NULL) {
				ALGO_reset(me->ai_player);
			}
		}

		// Check if game state changed (stats need updating)
		if (me->game->game_state_has_updated) {
			me->ui_needs_update = true;
		}
	}
}

void APP_CONTROLLER_render(APP_Controller_t *me) {
	// Always render the game (visible in background)
	GAME_render(me->game);

	// Update UI if stats changed
	if (me->ui_needs_update && me->game->game_state_has_updated) {
		char buffer[10];

		snprintf(buffer, sizeof(buffer), "%u", me->game->game_counter);
		APP_UI_update_value(me->ui, CURRENT_GAME_NUM, buffer);

		snprintf(buffer, sizeof(buffer), "%u", me->game->game_won_counter);
		APP_UI_update_value(me->ui, TOTAL_GAME_WINS, buffer);

		snprintf(buffer, sizeof(buffer), "%u", me->game->length);
		APP_UI_update_value(me->ui, SNAKE_LEN, buffer);

		me->game->game_state_has_updated = false;
		me->ui_needs_update = false;
	}
}

void APP_CONTROLLER_toggle_play_mode(APP_Controller_t *me) {
	if (me->play_mode == PLAY_MODE_MANUAL) {
		// Switch to AI mode
		me->play_mode = PLAY_MODE_AI;
		me->game->level_tick_rate = 15;  // Faster for AI
		log_message("APP_CTRL", LOG_INFO, "Switched to AI mode");

		// Reset AI when switching to AI mode
		if (me->ai_player != NULL) {
			ALGO_reset(me->ai_player);
		}

		// Update UI to show "AI"
		APP_UI_update_value(me->ui, PLAY_MODE_DISPLAY, "AI    ");

	} else {
		// Switch to Manual mode
		me->play_mode = PLAY_MODE_MANUAL;
		me->game->level_tick_rate = 5;  // Slower for human
		log_message("APP_CTRL", LOG_INFO, "Switched to MANUAL mode");

		// Update UI to show "MANUAL"
		APP_UI_update_value(me->ui, PLAY_MODE_DISPLAY, "MANUAL");
	}
}

APP_PlayMode_e APP_CONTROLLER_get_play_mode(APP_Controller_t *me) {
	return me->play_mode;
}

void APP_CONTROLLER_set_state(APP_Controller_t *me, APP_State_e new_state) {
	transition_to_state(me, new_state);
}

APP_State_e APP_CONTROLLER_get_state(APP_Controller_t *me) {
	return me->state;
}
