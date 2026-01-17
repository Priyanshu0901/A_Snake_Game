/*
 * App_UI.h
 *
 *  Created on: 18-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_APP_UI_H_
#define INC_APP_UI_H_

#include "Char_Canvas.h"
#include <stdint.h>
#include <stdbool.h>
#include "Input.h"

// Menu states for navigation (extensible)
typedef enum {
    MENU_STATE_MAIN = 0,
    MENU_STATE_SETTINGS,
    MENU_STATE_MAX
} APP_UI_MenuState_e;

// Application UI Controller
typedef struct {
    CHAR_CANVAS_t *canvas;           // Reference to canvas layer
    APP_UI_MenuState_e current_state;
    uint8_t selection_index;         // For menu item selection (future use)
    bool needs_refresh;              // Flag to trigger render
} APP_UI_t;

/* Public API */

/**
 * @brief Initialize the UI controller
 * @param me Pointer to APP_UI instance
 * @param canvas Pointer to initialized canvas
 */
void APP_UI_ctor(APP_UI_t * const me, CHAR_CANVAS_t * canvas);

/**
 * @brief Setup the UI pages and templates
 * @param me Pointer to APP_UI instance
 * @note This should be called after ctor to initialize page layouts
 */
void APP_UI_setup_pages(APP_UI_t * const me);

/**
 * @brief Process user input and update menu state
 * @param me Pointer to APP_UI instance
 * @param input Input event to process
 */
void APP_UI_process_input(APP_UI_t * const me, key_action_e input);

/**
 * @brief Update a specific UI object with new data
 * @param me Pointer to APP_UI instance
 * @param obj_type Object type to update
 * @param value String value to display
 */
void APP_UI_update_value(APP_UI_t * const me, CHAR_CANVAS_obj_e obj_type, const char * value);

/**
 * @brief Refresh the display if needed
 * @param me Pointer to APP_UI instance
 */
void APP_UI_refresh(APP_UI_t * const me);

/**
 * @brief Force a full refresh of the current page
 * @param me Pointer to APP_UI instance
 */
void APP_UI_force_refresh(APP_UI_t * const me);

#endif /* INC_APP_UI_H_ */
