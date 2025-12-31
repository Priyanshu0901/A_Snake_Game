/*
 * keypad_4_4.h
 *
 *  Created on: 01-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_KEYPAD_4_4_H_
#define INC_KEYPAD_4_4_H_

#include <stdbool.h>
#include "PCF8574.h"

typedef struct{
	bool new_key_press;
	uint8_t key_r,key_c;
	PCF8574_t driver;
}KEYPAD_t;

void KEYPAD_ctor(KEYPAD_t * const me, I2C_HandleTypeDef *i2cHandle);
void KEYPAD_read(KEYPAD_t * const me);

#endif /* INC_KEYPAD_4_4_H_ */
