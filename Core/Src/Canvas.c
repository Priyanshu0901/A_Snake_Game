/*
 * Canvas.c
 *
 *  Created on: 02-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "Canvas.h"
#include "main.h"
#include <stdlib.h>
#include "string.h"

static const char *tag = "CANVAS";

static inline void reset_cursor(CANVAS_t *const me) {
	me->cursor.x = 0;
	me->cursor.y = 0;
}

static void move_cursor(CANVAS_t *const me, C_COORDINATES_t postion) {
	if (postion.x > me->display->size_x || postion.y > me->display->size_y) {
		log_message(tag, LOG_ERROR, "Position invalid");
		return;
	}
	me->cursor.x = postion.x;
	me->cursor.y = postion.y;
}

void CANVAS_ctor(CANVAS_t *const me, DISPLAY_t *display) {
	me->display = display;
	me->canvas_buffer = (PIXEL_t*) calloc(
			me->display->size_x * me->display->size_y, sizeof(PIXEL_t));
	reset_cursor(me);
}

void CANVAS_clear(CANVAS_t *const me) {
	DISPLAY_clear(me->display);
	memset(me->canvas_buffer, 0,
			me->display->size_x * me->display->size_y * sizeof(PIXEL_t));
	reset_cursor(me);
}

void CANVAS_sync(CANVAS_t *const me) {
	memcpy(me->display->display_buffer, me->canvas_buffer,
			me->display->size_x * me->display->size_y * sizeof(PIXEL_t));
}

void CANVAS_draw_point(CANVAS_t *const me, C_COORDINATES_t postion,
		PIXEL_t color)
{
	move_cursor(me, postion);

	uint32_t index = (me->cursor.y * me->display->size_x) + me->cursor.x;

	me->canvas_buffer[index] = color;
}

void CANVAS_clear_point(CANVAS_t *const me, C_COORDINATES_t postion)
{
	move_cursor(me, postion);

	uint32_t index = (me->cursor.y * me->display->size_x) + me->cursor.x;

	PIXEL_t color;
	color.pixels.green = 0;
	color.pixels.red = 0;
	color.pixels.blue = 0;

	me->canvas_buffer[index] = color;
}

void CANVAS_draw_rectangle(CANVAS_t *const me, C_COORDINATES_t postion,PIXEL_t color, uint8_t length, uint8_t breadth){

}

void CANVAS_clear_rectangle(CANVAS_t *const me, C_COORDINATES_t postion, uint8_t length, uint8_t breadth){

}
