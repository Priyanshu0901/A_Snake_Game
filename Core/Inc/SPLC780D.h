/*
 * SPLC780D.h
 *
 *  Created on: 16-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_SPLC780D_H_
#define INC_SPLC780D_H_

#include "PCF8574.h"

typedef enum {
	CURSOR_LEFT = 0,
	CURSOR_RIGHT,
	DISPLAY_LEFT,
	DISPLAY_RIGHT,
}SPLC780D_CURSOR_MOVEMENT_e;

typedef struct{
	GPIO_TypeDef * RW_Port;
	uint16_t RW_Pin;
	GPIO_TypeDef * RS_Port;
	uint16_t RS_Pin;
	GPIO_TypeDef * E_Port;
	uint16_t E_Pin;
	PCF8574_t data_pins;
}SPLC780D_t;

void SPLC780D_ctor(SPLC780D_t * const me, I2C_HandleTypeDef *i2chandle);

void SPLC780D_reset_cursor(SPLC780D_t * const me);
void SPLC780D_move_cursor(SPLC780D_t * const me,SPLC780D_CURSOR_MOVEMENT_e movement);

#endif /* INC_SPLC780D_H_ */
