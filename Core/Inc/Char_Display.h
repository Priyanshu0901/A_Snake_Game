/*
 * Char_Display.h
 *
 *  Created on: 17-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_CHAR_DISPLAY_H_
#define INC_CHAR_DISPLAY_H_

#include "SPLC780D.h"
#include <stdbool.h>

// Enable/disable dirty tracking optimization
// Set to 1 for optimized updates (only changed characters)
// Set to 0 for full buffer updates (simpler but slower)
#define CHAR_DISPLAY_USE_DIRTY_TRACKING 1

typedef struct{
	char * buffer;
	SPLC780D_t * driver;

#if CHAR_DISPLAY_USE_DIRTY_TRACKING
	char * shadow_buffer;      // Previous state for comparison
	bool * dirty_flags;        // Per-character dirty flags
	bool full_refresh_needed;  // Force complete redraw
#endif
} CHAR_DISPLAY_t;

void CHAR_DISPLAY_ctor(CHAR_DISPLAY_t * const me, SPLC780D_t * display_driver);
void CHAR_DISPLAY_clear(CHAR_DISPLAY_t * const me, SPLC780D_t * display_driver);

void CHAR_WRITE_data(CHAR_DISPLAY_t * const me, char * const str, uint8_t x, uint8_t y);

void CHAR_DISPLAY_buffer_flush(CHAR_DISPLAY_t * const me);

#if CHAR_DISPLAY_USE_DIRTY_TRACKING
void CHAR_DISPLAY_force_refresh(CHAR_DISPLAY_t * const me);
#endif

#endif /* INC_CHAR_DISPLAY_H_ */
