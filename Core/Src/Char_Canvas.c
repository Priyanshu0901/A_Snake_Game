/*
 * Char_Canvas.c
 *
 *  Created on: 17-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "Char_Canvas.h"
#include <string.h>
#include <stdlib.h>

// Use a private constant for the total screen area
static const uint16_t TOTAL_CELLS = CHAR_DISP_COLS * CHAR_DISP_ROWS;

void CHAR_CANVAS_ctor(CHAR_CANVAS_t * const me, CHAR_DISPLAY_t * display_driver) {
    me->display = display_driver;
    me->has_updated = false;
    me->current_page_idx = MAIN_PAGE;

    // Allocate the main canvas buffer (+1 for a safety null terminator)
    me->canvas_buffer = (char*)calloc(TOTAL_CELLS + 1, sizeof(char));

    if (me->canvas_buffer != NULL) {
        memset(me->canvas_buffer, ' ', TOTAL_CELLS);
    }
}

void CHAR_CANVAS_obj_init(CHAR_CANVAS_t * const me, CHAR_CANVAS_Pages_e page, CHAR_CANVAS_obj_e type, uint8_t x, uint8_t y, uint8_t len) {
    if (type >= MAX_OBJECTS || page >= TOTAL_PAGES) return;

    CHAR_CANVAS_obj_t *obj = &me->pages[page].variable_objs[type];
    obj->x = x;
    obj->y = y;
    obj->length = len;

    // Allocate buffer (+1 for null terminator so string functions work safely)
    obj->buffer = (char*)calloc(len + 1, sizeof(char));
    if (obj->buffer != NULL) {
        memset(obj->buffer, ' ', len);
    }
}

void CHAR_CANVAS_update_obj(CHAR_CANVAS_t * const me, CHAR_CANVAS_obj_e type, const char * str) {
    if (type >= MAX_OBJECTS || me->canvas_buffer == NULL) return;

    CHAR_Page_t *page = &me->pages[me->current_page_idx];
    CHAR_CANVAS_obj_t *obj = &page->variable_objs[type];

    if (obj->buffer == NULL) return;

    // First, fill entire buffer with spaces to clear old content
    memset(obj->buffer, ' ', obj->length);

    // Then copy the new string, but ensure we don't go past obj->length
    // Using a manual loop to handle the string properly
    uint8_t i = 0;
    while (str[i] != '\0' && i < obj->length) {
        obj->buffer[i] = str[i];
        i++;
    }
    // Remaining characters are already spaces from memset
    // No need to pad - memset already did that

    me->has_updated = true;
}

void CHAR_CANVAS_switch_page(CHAR_CANVAS_t * const me, CHAR_CANVAS_Pages_e page) {
    if (page < TOTAL_PAGES) {
        me->current_page_idx = page;
        me->has_updated = true;
    }
}

void CHAR_CANVAS_render(CHAR_CANVAS_t * const me) {
    if (!me->has_updated || me->display == NULL) return;

    CHAR_Page_t *page = &me->pages[me->current_page_idx];

    // Layer 1: Load static background or clear the working canvas
    if (page->static_template != NULL) {
        memcpy(me->canvas_buffer, page->static_template, TOTAL_CELLS);
    } else {
        memset(me->canvas_buffer, ' ', TOTAL_CELLS);
    }

    // Layer 2: Overlay dynamic objects onto the canvas
    for (int i = 0; i < MAX_OBJECTS; i++) {
        CHAR_CANVAS_obj_t *obj = &page->variable_objs[i];
        if (obj->buffer == NULL) continue;

        uint16_t start_pos = (obj->y * CHAR_DISP_COLS) + obj->x;

        for (int j = 0; j < obj->length; j++) {
            if ((start_pos + j) < TOTAL_CELLS) {
                me->canvas_buffer[start_pos + j] = obj->buffer[j];
            }
        }
    }

    // Layer 3: Sync Canvas to Display Buffer
    // Instead of CHAR_WRITE_data (which relies on \0), we do a direct block copy
    // because both buffers are the same size (TOTAL_CELLS)
    memcpy(me->display->buffer, me->canvas_buffer, TOTAL_CELLS);

    // Physically push the display buffer to the SPLC780D hardware
    CHAR_DISPLAY_buffer_flush(me->display);

    me->has_updated = false;
}
