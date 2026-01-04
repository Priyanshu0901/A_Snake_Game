/*
 * Display.c
 *
 *  Created on: 30-Dec-2025
 *      Author: rayv_mini_pc
 */

#include "Display.h"
#include <string.h>
#include <stdlib.h>

void scale_brightness(DISPLAY_t *const me) {
    uint32_t total_pixels = me->size_x * me->size_y;

    for (uint32_t i = 0; i < total_pixels; i++) {

        uint16_t r = me->display_buffer[i].pixels.red;
        me->display_buffer[i].pixels.red = (uint8_t)((r * me->brightness) >> 8);

        uint16_t g = me->display_buffer[i].pixels.green;
        me->display_buffer[i].pixels.green = (uint8_t)((g * me->brightness) >> 8);

        uint16_t b = me->display_buffer[i].pixels.blue;
        me->display_buffer[i].pixels.blue = (uint8_t)((b * me->brightness) >> 8);
    }
}

void DISPLAY_ctor(DISPLAY_t *const me, GPIO_TypeDef *port, uint16_t pin) {
	WS2812B_ctor(&(me->driver), port, pin);
	me->size_x = DISPLAY_COLS;
	me->size_y = DISPLAY_ROWS;
	me->brightness = DEFAULT_BRIGHTNESS;
	me->display_buffer = (PIXEL_t*) calloc(me->size_x * me->size_y,
			sizeof(PIXEL_t));
}

void DISPLAY_clear(DISPLAY_t *const me) {
	memset(me->display_buffer, 0, me->size_x * me->size_y * sizeof(PIXEL_t));
}

void DISPLAY_update(DISPLAY_t *const me) {
	// Post-processing (Optional: Brightness scaling)
	scale_brightness(me);
	// Physical Mapping (Handle zig-zag wiring if necessary)

	WS2812B_write(&(me->driver), me->display_buffer, me->size_y * me->size_x);
}

///**
// * @param hue: 0-255 color position
// * @param brightness: 0-255 (128 for half brightness)
// */
//static void get_rainbow_color(uint8_t hue, uint8_t *r,
//		uint8_t *g, uint8_t *b) {
//	uint8_t rs, gs, bs;
//
//	// 1. Generate the pure "Full Power" Rainbow
//	if (hue < 85) {
//		rs = 255 - (hue * 3);
//		gs = hue * 3;
//		bs = 0;
//	} else if (hue < 170) {
//		hue -= 85;
//		rs = 0;
//		gs = 255 - (hue * 3);
//		bs = hue * 3;
//	} else {
//		hue -= 170;
//		rs = hue * 3;
//		gs = 0;
//		bs = 255 - (hue * 3);
//	}
//}
//
//void test_breathing_rainbow_update(DISPLAY_t *const me) {
//	static uint8_t hue_offset = 0;
//
//	for (uint8_t i = 0; i < me->size_y; i++) {
//		for (uint8_t j = 0; j < me->size_x; j++) {
//			uint8_t r, g, b;
//			get_rainbow_color(hue_offset + (i * 10) + (j * 10), &r, &g, &b);
//			int index = (i * me->size_x) + j;
//			me->display_buffer[index].pixels.green = g;
//			me->display_buffer[index].pixels.red = r;
//			me->display_buffer[index].pixels.blue = b;
//		}
//	}
//	hue_offset += 2; // Speed of rainbow movement
//}
//
//void identify_layout(DISPLAY_t *const me) {
//	for (uint16_t i = 0; i < (me->size_y * me->size_x); i++) {
//		// Clear buffer
//		memset(me->display_buffer, 0,
//				me->size_x * me->size_y * sizeof(PIXEL_t));
//
//		// Treat the 2D buffer as 1D to see how the LEDs are physically mapped
//		PIXEL_t *flat_buffer = me->display_buffer;
//
//		flat_buffer[i].pixels.red = 50; // Dim red
//
//		// Write the whole strip
//		WS2812B_write(&me->driver, flat_buffer, me->size_y * me->size_x);
//
//		HAL_Delay(50);
//	}
//}

//void DISPLAY_test(DISPLAY_t *const me) {
//	identify_layout(me);
//	test_breathing_rainbow_update(me);
//}

