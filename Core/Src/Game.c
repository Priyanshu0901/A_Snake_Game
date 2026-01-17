/*
 * Game.c
 *
 *  Created on: 02-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "Game.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int game_counter = 0, game_won_counter = 0;

#define FOOD_LUT_SIZE 20
static PIXEL_t food_color_lut[FOOD_LUT_SIZE];

#define SNAKE_LUT_SIZE MAX_SNAKE_LEN // Larger size = smoother rainbow
static PIXEL_t snake_color_lut[SNAKE_LUT_SIZE];

void generate_food_color_lut() {
	// 1. Set start and end points
	food_color_lut[0].pixels.red = 255;
	food_color_lut[0].pixels.green = 255;
	food_color_lut[0].pixels.blue = 255;

	food_color_lut[FOOD_LUT_SIZE - 1].pixels.red = 50;
	food_color_lut[FOOD_LUT_SIZE - 1].pixels.green = 50;
	food_color_lut[FOOD_LUT_SIZE - 1].pixels.blue = 50;

	// 2. Calculate steps using floats to maintain precision
	float r_start = (float) food_color_lut[0].pixels.red;
	float g_start = (float) food_color_lut[0].pixels.green;
	float b_start = (float) food_color_lut[0].pixels.blue;

	float r_end = (float) food_color_lut[FOOD_LUT_SIZE - 1].pixels.red;
	float g_end = (float) food_color_lut[FOOD_LUT_SIZE - 1].pixels.green;
	float b_end = (float) food_color_lut[FOOD_LUT_SIZE - 1].pixels.blue;

	float dr = (r_end - r_start) / (FOOD_LUT_SIZE - 1);
	float dg = (g_end - g_start) / (FOOD_LUT_SIZE - 1);
	float db = (b_end - b_start) / (FOOD_LUT_SIZE - 1);

	// 3. Fill the table (start from index 1)
	for (int i = 1; i < FOOD_LUT_SIZE - 1; i++) {
		food_color_lut[i].pixels.red = (uint8_t) (r_start + (dr * i));
		food_color_lut[i].pixels.green = (uint8_t) (g_start + (dg * i));
		food_color_lut[i].pixels.blue = (uint8_t) (b_start + (db * i));
	}
}

void generate_snake_rainbow_lut() {
	for (int i = 0; i < SNAKE_LUT_SIZE; i++) {
		float hue = (float) i / SNAKE_LUT_SIZE;
		float r, g, b;

		// Standard Hue-to-RGB conversion
		float h = hue * 6.0f;
		float x = (1.0f - fabsf(fmodf(h, 2.0f) - 1.0f));

		if (h < 1) {
			r = 1;
			g = x;
			b = 0;
		} else if (h < 2) {
			r = x;
			g = 1;
			b = 0;
		} else if (h < 3) {
			r = 0;
			g = 1;
			b = x;
		} else if (h < 4) {
			r = 0;
			g = x;
			b = 1;
		} else if (h < 5) {
			r = x;
			g = 0;
			b = 1;
		} else {
			r = 1;
			g = 0;
			b = x;
		}

		snake_color_lut[i].pixels.red = (uint8_t) (r * 255);
		snake_color_lut[i].pixels.green = (uint8_t) (g * 255);
		snake_color_lut[i].pixels.blue = (uint8_t) (b * 255);
	}
}

void GAME_ctor(GAME_Engine_t *const me, CANVAS_t *canvas, INPUT_t *input,
		APP_UI_t* ui) {
	me->canvas = canvas;
	me->input = input;
	me->UI = ui;
	generate_food_color_lut();
	generate_snake_rainbow_lut();
	GAME_reset(me);
}

inline static PIXEL_t get_food_color(GAME_Engine_t *me) {
	if (FOOD_LUT_SIZE <= me->food_color)
		me->food_color = 0;
	return food_color_lut[me->food_color++];
}

void move_snake(GAME_Engine_t *me) {
	// 1. Shift the body: Start from the tail, move each segment to the position of the one before it
	for (int i = me->length - 1; i > 0; i--) {
		me->body[i] = me->body[i - 1];
	}

	// 2. Move the Head based on current direction
	switch (me->current_dir) {
	case ACTION_UP:
		me->body[0].y--;
		break;
	case ACTION_DOWN:
		me->body[0].y++;
		break;
	case ACTION_LEFT:
		me->body[0].x--;
		break;
	case ACTION_RIGHT:
		me->body[0].x++;
		break;
	default:
		break;
	}
}

void spawn_food(GAME_Engine_t *const me) {
	if (MAX_SNAKE_LEN <= me->length) {
		game_won_counter++;
		char buffer[10];

		// Update game number
		snprintf(buffer, sizeof(buffer), "%u", game_won_counter);
		APP_UI_update_value(me->UI, TOTAL_GAME_WINS, buffer);
		// Refresh display
		APP_UI_refresh(me->UI);
		GAME_reset(me);
		return;
	}

	bool on_snake;
	me->food_color = 0;
	do {
		on_snake = false;
		me->food.x = rand() % DISPLAY_COLS;
		me->food.y = rand() % DISPLAY_ROWS;

		// Make sure food didn't spawn on the snake body
		for (int i = 0; i < me->length; i++) {
			if (me->food.x == me->body[i].x && me->food.y == me->body[i].y) {
				on_snake = true;
				break;
			}
		}
	} while (on_snake); // Keep trying until we find a clear spot
}

void check_collisions(GAME_Engine_t *const me) {
	C_COORDINATES_t head = me->body[0];

	// 1. Wall Collision (Checking >= 8 handles wrap-around for uint8_t)
	if (head.x >= me->canvas->display->size_x
			|| head.y >= me->canvas->display->size_y) {
		GAME_reset(me);
		return;
	}

	// 2. Self Collision (Start checking from index 1 to length)
	for (int i = 1; i < me->length; i++) {
		if (head.x == me->body[i].x && head.y == me->body[i].y) {
			GAME_reset(me);
			return;
		}
	}

	// 3. Food Collision
	if (head.x == me->food.x && head.y == me->food.y) {
		if (me->length < MAX_SNAKE_LEN) {
			me->body[me->length] = me->body[me->length - 1];
			me->length++; // Grow the snake
			char buffer[10];
			snprintf(buffer, sizeof(buffer), "%u", me->length);
			APP_UI_update_value(me->UI, SNAKE_LEN, buffer);
			APP_UI_refresh(me->UI);
		}
		spawn_food(me); // Place new food
	}
}

void GAME_update(GAME_Engine_t *const me, key_action_e const new_action) {

#ifndef ALGO
	if(me->game_over && new_action != ACTION_NONE){
		me->game_over = false;
	}
#endif
	// Filter input to prevent 180-degree turns
	if (new_action == ACTION_UP && me->current_dir != ACTION_DOWN)
		me->current_dir = ACTION_UP;
	if (new_action == ACTION_DOWN && me->current_dir != ACTION_UP)
		me->current_dir = ACTION_DOWN;
	if (new_action == ACTION_LEFT && me->current_dir != ACTION_RIGHT)
		me->current_dir = ACTION_LEFT;
	if (new_action == ACTION_RIGHT && me->current_dir != ACTION_LEFT)
		me->current_dir = ACTION_RIGHT;

}

void GAME_tick(GAME_Engine_t *const me) {
	move_snake(me);
	check_collisions(me);
}

void GAME_render(GAME_Engine_t *const me) {
	CANVAS_clear(me->canvas);

	// 1. Draw Food
	CANVAS_draw_point(me->canvas, me->food, get_food_color(me));

	// 2. Draw Snake
	for (int i = 0; i < me->length; i++) {
		CANVAS_draw_point(me->canvas, me->body[i], snake_color_lut[i]);
	}
}

void GAME_reset(GAME_Engine_t *const me) {
	memset(me->body, 0, MAX_SNAKE_LEN * sizeof(C_COORDINATES_t));
	me->game_over = true;
	// Start in the center
	me->body[0].x = rand() % DISPLAY_COLS;
	me->body[0].y = rand() % DISPLAY_ROWS;
	me->length = 1;

	game_counter++;
	char buffer[10];

	// Update game number
	snprintf(buffer, sizeof(buffer), "%u", game_counter);
	APP_UI_update_value(me->UI, CURRENT_GAME_NUM, buffer);
	// Refresh display
	APP_UI_refresh(me->UI);

	me->current_dir = ACTION_NONE; // Wait for player input to start

	log_message("GAME", LOG_INFO, "%d/%d", game_won_counter, game_counter);

	spawn_food(me);
}
