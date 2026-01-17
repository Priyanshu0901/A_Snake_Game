/*
 * App_UI.c
 *
 *  Created on: 18-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "App_UI.h"
#include <string.h>

// Static page templates stored in flash memory
// Template for main page (40x2 = 80 characters)
static const char MAIN_PAGE_TEMPLATE[CHAR_DISP_COLS * CHAR_DISP_ROWS] =
    "Game:     Wins:     Snake Len:          "
    "                                        ";

// Template for settings page (40x2 = 80 characters)
static const char SETTINGS_PAGE_TEMPLATE[CHAR_DISP_COLS * CHAR_DISP_ROWS] =
    "        SETTINGS MENU                   "
    "Sound: ON    Brightness: 75%            ";

void APP_UI_ctor(APP_UI_t * const me, CHAR_CANVAS_t * canvas) {
    me->canvas = canvas;
    me->current_state = MENU_STATE_MAIN;
    me->selection_index = 0;
    me->needs_refresh = true;
}

void APP_UI_setup_pages(APP_UI_t * const me) {
    // Setup MAIN_PAGE
    me->canvas->pages[MAIN_PAGE].static_template = MAIN_PAGE_TEMPLATE;

    // Initialize objects on the main page
    // "Game: XXX" - 3 digits starting at position 6, row 0
    CHAR_CANVAS_obj_init(me->canvas, MAIN_PAGE, CURRENT_GAME_NUM, 6, 0, 3);

    // "Wins: XXX" - 3 digits starting at position 16, row 0
    CHAR_CANVAS_obj_init(me->canvas, MAIN_PAGE, TOTAL_GAME_WINS, 16, 0, 3);

    // "Snake Len: XXX" - 3 digits starting at position 35, row 0
    CHAR_CANVAS_obj_init(me->canvas, MAIN_PAGE, SNAKE_LEN, 35, 0, 3);

    // Setup SETTINGS_PAGE
    me->canvas->pages[SETTINGS_PAGE].static_template = SETTINGS_PAGE_TEMPLATE;

    // Initialize default values
    APP_UI_update_value(me, CURRENT_GAME_NUM, "0");
    APP_UI_update_value(me, TOTAL_GAME_WINS, "0");
    APP_UI_update_value(me, SNAKE_LEN, "3");

    // Force initial render
    me->needs_refresh = true;
}

void APP_UI_process_input(APP_UI_t * const me, key_action_e input) {
    // Current implementation: single page, no input changes
    // This can be extended for multi-page navigation

    switch (me->current_state) {
        case MENU_STATE_MAIN:
            if (input == ACTION_CONFIRM) {
                // Future: transition to settings
                // CHAR_CANVAS_switch_page(me->canvas, SETTINGS_PAGE);
                // me->current_state = MENU_STATE_SETTINGS;
                // me->needs_refresh = true;
            }
            break;

        case MENU_STATE_SETTINGS:
            if (input == ACTION_CONFIRM) {
                // Future: return to main
                // CHAR_CANVAS_switch_page(me->canvas, MAIN_PAGE);
                // me->current_state = MENU_STATE_MAIN;
                // me->needs_refresh = true;
            }
            break;

        default:
            break;
    }
}

void APP_UI_update_value(APP_UI_t * const me, CHAR_CANVAS_obj_e obj_type, const char * value) {
    CHAR_CANVAS_update_obj(me->canvas, obj_type, value);
    me->needs_refresh = true;
}

void APP_UI_refresh(APP_UI_t * const me) {
    if (me->needs_refresh) {
        CHAR_CANVAS_render(me->canvas);
        me->needs_refresh = false;
    }
}

void APP_UI_force_refresh(APP_UI_t * const me) {
    me->needs_refresh = true;
    me->canvas->has_updated = true;
    CHAR_CANVAS_render(me->canvas);
    me->needs_refresh = false;
}
