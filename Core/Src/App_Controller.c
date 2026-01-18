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
    if (me->state == new_state) return;  // No-op if already in target state

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
 * @brief Handle input when in PLAYING state
 */
static void handle_input_playing(APP_Controller_t *me, key_action_e action) {
    if (action == ACTION_NONE) return;

    // CONFIRM opens settings menu and pauses game
    if (action == ACTION_CONFIRM) {
        me->previous_state = APP_STATE_PLAYING;
        me->game_was_paused = me->game->game_over;  // Remember if game was already paused
        transition_to_state(me, APP_STATE_MENU);

        // Switch UI to settings page
        CHAR_CANVAS_switch_page(me->ui->canvas, SETTINGS_PAGE);
        me->ui->current_state = MENU_STATE_SETTINGS;
        me->ui->needs_refresh = true;
        return;
    }

#ifndef ALGO
    // All other input goes to the game
    GAME_update(me->game, action);
#endif
}

/**
 * @brief Handle input when in MENU/PAUSED state
 */
static void handle_input_menu(APP_Controller_t *me, key_action_e action) {
    if (action == ACTION_NONE) return;

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

    // Other navigation keys could be used for menu navigation in future
    // For now, just consume them so they don't affect game
}

/**
 * @brief Handle input when in GAME_OVER state
 */
static void handle_input_game_over(APP_Controller_t *me, key_action_e action) {
    if (action == ACTION_NONE) return;

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

void APP_CONTROLLER_ctor(APP_Controller_t *me,
                         GAME_Engine_t *game,
                         APP_UI_t *ui,
                         INPUT_t *input) {
    me->game = game;
    me->ui = ui;
    me->input = input;

    // Start in playing state
    me->state = APP_STATE_PLAYING;
    me->previous_state = APP_STATE_PLAYING;
    me->game_needs_tick = true;
    me->ui_needs_update = true;
    me->game_was_paused = false;

    log_message("APP_CTRL", LOG_INFO, "Initialized in PLAYING state");
}

void APP_CONTROLLER_process_input(APP_Controller_t *me) {
    // CRITICAL: Call INPUT_get_action() ONLY ONCE per cycle
    key_action_e action = INPUT_get_action(me->input);

    // Route input based on current state
    switch (me->state) {
        case APP_STATE_PLAYING:
            handle_input_playing(me, action);
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
        GAME_tick(me->game);

        // Check if game just ended
        if (me->game->game_over) {
            transition_to_state(me, APP_STATE_GAME_OVER);
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

void APP_CONTROLLER_set_state(APP_Controller_t *me, APP_State_e new_state) {
    transition_to_state(me, new_state);
}

APP_State_e APP_CONTROLLER_get_state(APP_Controller_t *me) {
    return me->state;
}
