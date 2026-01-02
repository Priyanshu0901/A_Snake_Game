/*
 * Game.c
 *
 *  Created on: 02-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "Game.h"
#include <stdlib.h>

void GAME_ctor(GAME_Engine_t *const me, CANVAS_t *canvas, INPUT_t *input) {
	me->canvas = canvas;
	me->input = input;
}

void GAME_update(GAME_Engine_t *const me) {

}

void GAME_render(GAME_Engine_t *const me) {
	C_COORDINATES_t rand_coordinate_1, rand_coordinate_2;
	PIXEL_t rand_pixel;
	rand_coordinate_1.x = rand() % 8;
	rand_coordinate_1.y = rand() % 8;

	rand_coordinate_2.x = rand() % 8;
	rand_coordinate_2.y = rand() % 8;

	rand_pixel.pixels.green = rand() % 256;
	rand_pixel.pixels.red = rand() % 256;
	rand_pixel.pixels.blue = rand() % 256;

//	log_message("GAME_render", LOG_INFO, "x: %d y: %d, color %0x%0x%0x",
//			rand_coordinate_1.x, rand_coordinate_1.y, rand_pixel.pixels.red,
//			rand_pixel.pixels.green, rand_pixel.pixels.blue);

	CANVAS_draw_point(me->canvas, rand_coordinate_1, rand_pixel);
	CANVAS_sync(me->canvas);
	CANVAS_clear_point(me->canvas, rand_coordinate_2);
}
