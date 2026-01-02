/*
 * Canvas.h
 *
 *  Created on: 02-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_CANVAS_H_
#define INC_CANVAS_H_

#include "Display.h"

typedef struct {
	uint8_t x,y;
}C_COORDINATES_t;

typedef struct {
	DISPLAY_t * display;
	C_COORDINATES_t cursor;
	PIXEL_t * canvas_buffer;
}CANVAS_t;

void CANVAS_ctor(CANVAS_t * const me, DISPLAY_t * display);

void CANVAS_clear(CANVAS_t * const me);
void CANVAS_sync(CANVAS_t * const me);

void CANVAS_draw_point(CANVAS_t *const me, C_COORDINATES_t postion, PIXEL_t color);
void CANVAS_clear_point(CANVAS_t *const me, C_COORDINATES_t postion);

void CANVAS_draw_rectangle(CANVAS_t *const me, C_COORDINATES_t postion,PIXEL_t color, uint8_t length, uint8_t breadth);
void CANVAS_clear_rectangle(CANVAS_t *const me, C_COORDINATES_t postion, uint8_t length, uint8_t breadth);

#endif /* INC_CANVAS_H_ */
