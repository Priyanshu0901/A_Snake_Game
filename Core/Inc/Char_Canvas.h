/*
 * Char_Canvas.h
 *
 *  Created on: 17-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_CHAR_CANVAS_H_
#define INC_CHAR_CANVAS_H_

#include "Char_Display.h"
#include <stdbool.h>

// Specific UI objects/fields we want to track
typedef enum {
    CURRENT_GAME_NUM = 0,
    TOTAL_GAME_WINS,
    SNAKE_LEN,
	GAME_FPS,
    MAX_OBJECTS
} CHAR_CANVAS_obj_e;

// Define different menu screens
typedef enum {
    MAIN_PAGE = 0,
    SETTINGS_PAGE,
    TOTAL_PAGES
} CHAR_CANVAS_Pages_e;

// A UI Widget: Stores position and a small string buffer
typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t length;
    char *buffer;
} CHAR_CANVAS_obj_t;

// A Page: A collection of UI Widgets and a static background
typedef struct {
    CHAR_CANVAS_obj_t variable_objs[MAX_OBJECTS];
    const char *static_template; // Point to a 40x2 string in Flash
} CHAR_Page_t;

// The Master Canvas Controller
typedef struct {
    char *canvas_buffer;           // Working memory (40x2)
    CHAR_DISPLAY_t *display;       // Link to the hardware buffer
    CHAR_Page_t pages[TOTAL_PAGES];
    CHAR_CANVAS_Pages_e current_page_idx;
    bool has_updated;
} CHAR_CANVAS_t;

/* Public API */
void CHAR_CANVAS_ctor(CHAR_CANVAS_t * const me, CHAR_DISPLAY_t * display_driver);
void CHAR_CANVAS_obj_init(CHAR_CANVAS_t * const me, CHAR_CANVAS_Pages_e page, CHAR_CANVAS_obj_e type, uint8_t x, uint8_t y, uint8_t len);
void CHAR_CANVAS_update_obj(CHAR_CANVAS_t * const me, CHAR_CANVAS_obj_e type, const char * str);
void CHAR_CANVAS_switch_page(CHAR_CANVAS_t * const me, CHAR_CANVAS_Pages_e page);
void CHAR_CANVAS_render(CHAR_CANVAS_t * const me);

#endif /* INC_CHAR_CANVAS_H_ */
