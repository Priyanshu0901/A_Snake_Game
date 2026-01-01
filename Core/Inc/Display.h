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

#define DISPLAY_BRIGHTNESS 1U << 2

typedef struct{
	PIXEL_t display_buffer[DISPLAY_ROWS][DISPLAY_COLS];
	WS2812B_t driver;
}DISPLAY_t;

void DISPLAY_ctor(DISPLAY_t * const me, GPIO_TypeDef *port, uint16_t pin);

void DISPLAY_update(DISPLAY_t * const me);
/*
 * Test Functions
 */
void DISPLAY_test(DISPLAY_t * const me);

#endif /* INC_DISPLAY_H_ */
