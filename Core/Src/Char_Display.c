/*
 * Char_Display.c
 *
 *  Created on: 17-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "Char_Display.h"
#include "string.h"
#include "stdlib.h"

void CHAR_DISPLAY_ctor(CHAR_DISPLAY_t *const me, SPLC780D_t *display_driver) {
	me->driver = display_driver;
	// Standard 16x2 = 32 bytes.
	// We use calloc to zero it out, but we should fill with spaces (0x20)
	me->buffer = (char*) calloc(CHAR_DISP_COLS * CHAR_DISP_ROWS, sizeof(char));
	CHAR_DISPLAY_clear(me, display_driver);
}

void CHAR_DISPLAY_clear(CHAR_DISPLAY_t *const me, SPLC780D_t *display_driver) {
	// Fill buffer with ASCII spaces (0x20) instead of nulls
	memset(me->buffer, ' ', CHAR_DISP_COLS * CHAR_DISP_ROWS);
	// Physically clear the hardware
	SPLC780D_Clear(display_driver);
}

void CHAR_WRITE_data(CHAR_DISPLAY_t *const me, char *const str, uint8_t x,
		uint8_t y) {
	if (y >= CHAR_DISP_ROWS || x >= CHAR_DISP_COLS)
		return;

	uint8_t i = 0;
	uint16_t buffer_index = (y * CHAR_DISP_COLS) + x;

	while (str[i] != '\0'
			&& (buffer_index + i) < (CHAR_DISP_COLS * CHAR_DISP_ROWS)) {
		me->buffer[buffer_index + i] = str[i];
		i++;
	}
}

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
