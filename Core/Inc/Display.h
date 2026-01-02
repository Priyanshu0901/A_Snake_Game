/*
 * Display.h
 *
 *  Created on: 30-Dec-2025
 *      Author: rayv_mini_pc
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "WS2812B.h"
#include "pixel.h"

#define DISPLAY_ROWS 8
#define DISPLAY_COLS 8

#define DEFAULT_BRIGHTNESS 1U << 3

typedef struct{
	PIXEL_t * display_buffer;
	uint8_t size_x,size_y;
	uint8_t brightness;
	WS2812B_t driver;
}DISPLAY_t;

void DISPLAY_ctor(DISPLAY_t * const me, GPIO_TypeDef *port, uint16_t pin);

void DISPLAY_clear(DISPLAY_t * const me);

void DISPLAY_update(DISPLAY_t * const me);

/*
 * Test Functions
 */
void DISPLAY_test(DISPLAY_t * const me);

#endif /* INC_DISPLAY_H_ */
