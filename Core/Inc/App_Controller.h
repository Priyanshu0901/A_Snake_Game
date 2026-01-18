/*
 * App_Controller.h
 *
 *  Created on: 18-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_APP_CONTROLLER_H_
#define INC_APP_CONTROLLER_H_

#include "Game.h"
#include "App_UI.h"
#include "Input.h"
#include "Algo.h"  // Always include - no more #ifdef
#include <stdbool.h>

/**
 * @brief Application-wide states
 *
 * State transitions:
 * MENU → PLAYING (user exits settings)
 * PLAYING → PAUSED (user presses confirm/settings)
 * PAUSED → PLAYING (user exits settings)
 * PLAYING → GAME_OVER (collision detected)
 * GAME_OVER → PLAYING (any key press to restart)
 */
typedef enum {
    APP_STATE_MENU,        // User in settings menu, game paused
    APP_STATE_PLAYING,     // Game actively running, input goes to game
    APP_STATE_PAUSED,      // Game frozen while in settings
    APP_STATE_GAME_OVER    // Game ended, waiting for restart
} APP_State_e;

/**
 * @brief Play mode selection (runtime switchable)
 */
typedef enum {
    PLAY_MODE_MANUAL = 0,  // Human controls the snake
    PLAY_MODE_AI           // AI controls the snake
} APP_PlayMode_e;

/**
 * @brief Top-level application controller
 *
 * Responsibilities:
 * - Route input to appropriate subsystem (Game vs UI vs Mode Toggle)
 * - Manage application state transitions
 * - Coordinate updates between Game and UI
 * - Handle AI/Manual mode switching (runtime toggle via UI)
 * - Prepare for future Audio integration
 */
typedef struct {
    APP_State_e state;
    APP_State_e previous_state;  // For returning from settings
    APP_PlayMode_e play_mode;    // Manual or AI control (runtime switchable)

    // Subsystem references
    GAME_Engine_t *game;
    APP_UI_t *ui;
    INPUT_t *input;
    ALGO_t *ai_player;           // AI player (always available)

    // Control flags
    bool game_needs_tick;        // Should GAME_tick() be called this frame?
    bool ui_needs_update;        // Should UI stats be refreshed?

    // Internal state tracking
    bool game_was_paused;        // Track if we manually paused the game
} APP_Controller_t;

/**
 * @brief Initialize the application controller
 * @param me Pointer to APP_Controller instance
 * @param game Pointer to initialized game engine
 * @param ui Pointer to initialized UI controller
 * @param input Pointer to initialized input handler
 * @param ai_player Pointer to initialized AI player
 */
void APP_CONTROLLER_ctor(APP_Controller_t *me,
                         GAME_Engine_t *game,
                         APP_UI_t *ui,
                         INPUT_t *input,
                         ALGO_t *ai_player);

/**
 * @brief Process input and route to appropriate subsystem
 * @param me Pointer to APP_Controller instance
 * @note This should be called ONCE per input cycle (e.g., 30Hz)
 *       It internally calls INPUT_get_action() once and routes accordingly
 */
void APP_CONTROLLER_process_input(APP_Controller_t *me);

/**
 * @brief Update game logic and UI state
 * @param me Pointer to APP_Controller instance
 * @note This should be called at the game tick rate (e.g., 5-15Hz)
 *       Only calls GAME_tick() when in PLAYING state
 *       AI decisions are made HERE at tick rate, not input rate
 */
void APP_CONTROLLER_update(APP_Controller_t *me);

/**
 * @brief Render game and UI
 * @param me Pointer to APP_Controller instance
 * @note This should be called at render rate (e.g., 60Hz)
 */
void APP_CONTROLLER_render(APP_Controller_t *me);

/**
 * @brief Toggle between Manual and AI play modes
 * @param me Pointer to APP_Controller instance
 * @note Can be called during gameplay to switch modes on-the-fly
 *       Triggered by UI menu selection
 */
void APP_CONTROLLER_toggle_play_mode(APP_Controller_t *me);

/**
 * @brief Get current play mode
 * @param me Pointer to APP_Controller instance
 * @return Current play mode (MANUAL or AI)
 */
APP_PlayMode_e APP_CONTROLLER_get_play_mode(APP_Controller_t *me);

/**
 * @brief Force transition to a specific state
 * @param me Pointer to APP_Controller instance
 * @param new_state Target state
 * @note Use sparingly - prefer letting the state machine handle transitions
 */
void APP_CONTROLLER_set_state(APP_Controller_t *me, APP_State_e new_state);

/**
 * @brief Get current application state
 * @param me Pointer to APP_Controller instance
 * @return Current state
 */
APP_State_e APP_CONTROLLER_get_state(APP_Controller_t *me);

#endif /* INC_APP_CONTROLLER_H_ */
