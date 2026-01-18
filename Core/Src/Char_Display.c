/*
 * Char_Display.c
 *
 *  Created on: 17-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "Char_Display.h"
#include "string.h"
#include "stdlib.h"

static const uint16_t TOTAL_BUFFER_SIZE = CHAR_DISP_COLS * CHAR_DISP_ROWS;

void CHAR_DISPLAY_ctor(CHAR_DISPLAY_t *const me, SPLC780D_t *display_driver) {
	me->driver = display_driver;

	// Allocate main display buffer
	me->buffer = (char*) calloc(TOTAL_BUFFER_SIZE, sizeof(char));

#if CHAR_DISPLAY_USE_DIRTY_TRACKING
	// Allocate shadow buffer for tracking changes
	me->shadow_buffer = (char*) calloc(TOTAL_BUFFER_SIZE, sizeof(char));

	// Allocate dirty flags array
	me->dirty_flags = (bool*) calloc(TOTAL_BUFFER_SIZE, sizeof(bool));

	me->full_refresh_needed = true;

	// Initialize shadow buffer with a different value to force initial update
	if (me->shadow_buffer != NULL) {
		memset(me->shadow_buffer, 0xFF, TOTAL_BUFFER_SIZE);
	}
#endif

	CHAR_DISPLAY_clear(me, display_driver);
}

void CHAR_DISPLAY_clear(CHAR_DISPLAY_t *const me, SPLC780D_t *display_driver) {
	// Fill buffer with ASCII spaces (0x20) instead of nulls
	memset(me->buffer, ' ', TOTAL_BUFFER_SIZE);

#if CHAR_DISPLAY_USE_DIRTY_TRACKING
	// Mark all positions as dirty
	if (me->dirty_flags != NULL) {
		memset(me->dirty_flags, true, TOTAL_BUFFER_SIZE);
	}
	me->full_refresh_needed = true;
#endif

	// Physically clear the hardware
	SPLC780D_Clear(display_driver);
}

void CHAR_WRITE_data(CHAR_DISPLAY_t *const me, char *const str, uint8_t x, uint8_t y) {
	if (y >= CHAR_DISP_ROWS || x >= CHAR_DISP_COLS)
		return;

	uint8_t i = 0;
	uint16_t buffer_index = (y * CHAR_DISP_COLS) + x;

	while (str[i] != '\0' && (buffer_index + i) < TOTAL_BUFFER_SIZE) {
		me->buffer[buffer_index + i] = str[i];

#if CHAR_DISPLAY_USE_DIRTY_TRACKING
		// Mark this character as dirty if it changed
		if (me->dirty_flags != NULL && me->shadow_buffer != NULL) {
			if (me->shadow_buffer[buffer_index + i] != str[i]) {
				me->dirty_flags[buffer_index + i] = true;
			}
		}
#endif

		i++;
	}
}

#if CHAR_DISPLAY_USE_DIRTY_TRACKING

void CHAR_DISPLAY_buffer_flush(CHAR_DISPLAY_t *const me) {
	// If no dirty tracking structures, fall back to full update
	if (me->dirty_flags == NULL || me->shadow_buffer == NULL) {
		// Fallback: full buffer update
		for (uint8_t row = 0; row < CHAR_DISP_ROWS; row++) {
			SPLC780D_move_cursor(me->driver, 0, row);
			for (uint8_t col = 0; col < CHAR_DISP_COLS; col++) {
				char c = me->buffer[(row * CHAR_DISP_COLS) + col];
				SPLC780D_write_char(me->driver, c);
			}
		}
		return;
	}

	// Optimized dirty tracking mode
	if (me->full_refresh_needed) {
		// Full refresh: update everything and reset tracking
		for (uint8_t row = 0; row < CHAR_DISP_ROWS; row++) {
			SPLC780D_move_cursor(me->driver, 0, row);
			for (uint8_t col = 0; col < CHAR_DISP_COLS; col++) {
				uint16_t idx = (row * CHAR_DISP_COLS) + col;
				char c = me->buffer[idx];
				SPLC780D_write_char(me->driver, c);
				me->shadow_buffer[idx] = c;
				me->dirty_flags[idx] = false;
			}
		}
		me->full_refresh_needed = false;
	} else {
		// Incremental update: only update changed characters
		for (uint8_t row = 0; row < CHAR_DISP_ROWS; row++) {
			bool cursor_positioned = false;

			for (uint8_t col = 0; col < CHAR_DISP_COLS; col++) {
				uint16_t idx = (row * CHAR_DISP_COLS) + col;

				// Check if this character has changed
				if (me->buffer[idx] != me->shadow_buffer[idx]) {
					// Position cursor only when needed
					if (!cursor_positioned) {
						SPLC780D_move_cursor(me->driver, col, row);
						cursor_positioned = true;
					}

					// Write the changed character
					char c = me->buffer[idx];
					SPLC780D_write_char(me->driver, c);
					me->shadow_buffer[idx] = c;
					me->dirty_flags[idx] = false;
				} else {
					// Character unchanged, cursor needs repositioning for next write
					cursor_positioned = false;
				}
			}
		}
	}
}

void CHAR_DISPLAY_force_refresh(CHAR_DISPLAY_t *const me) {
	me->full_refresh_needed = true;
	CHAR_DISPLAY_buffer_flush(me);
}

#else // !CHAR_DISPLAY_USE_DIRTY_TRACKING

// Original implementation without dirty tracking
void CHAR_DISPLAY_buffer_flush(CHAR_DISPLAY_t *const me) {
	for (uint8_t row = 0; row < CHAR_DISP_ROWS; row++) {
		// 1. Position the hardware cursor at start of line
		SPLC780D_move_cursor(me->driver, 0, row);

		// 2. Send the row of characters
		for (uint8_t col = 0; col < CHAR_DISP_COLS; col++) {
			char c = me->buffer[(row * CHAR_DISP_COLS) + col];
			SPLC780D_write_char(me->driver, c);
		}
	}
}

#endif // CHAR_DISPLAY_USE_DIRTY_TRACKING
