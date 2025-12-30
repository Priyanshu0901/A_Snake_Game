/*
 * WS2812B.h
 *
 *  Created on: 30-Dec-2025
 *      Author: rayv_mini_pc
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#include "stm32f4xx.h"
#include "pixel.h"


typedef enum{
	NOT_INIT = 0,
	OK,
	WRITING,
	RESET_WAIT,
	ERROR_STATE
}WS2812B_driver_state;

typedef struct{
	GPIO_TypeDef* port;
	uint16_t pin;
	WS2812B_driver_state state;
}WS2812B_t;

void WS2812B_ctor(WS2812B_t * const me, GPIO_TypeDef *port, uint16_t pin);
void WS2812B_write(WS2812B_t *const me, PIXEL_t *pixels, uint16_t count);
void WS2812B_reset(WS2812B_t * const me);

#endif /* INC_WS2812B_H_ */
