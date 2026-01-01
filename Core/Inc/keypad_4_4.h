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

typedef enum {
	S1 = 0,
	S2 = 1,
	S3 = 2,
	S4 = 3,
	S5 = 4,
	S6 = 5,
	S7 = 6,
	S8 = 7,
	S9 = 8,
	S10 = 9,
	S11 = 10,
	S12 = 11,
	S13 = 12,
	S14 = 13,
	S15 = 14,
	S16 = 15,
	NO_KEY
}keys_e;

typedef struct{
	bool new_key_press;
	keys_e key;
	PCF8574_t driver;
}KEYPAD_t;

void KEYPAD_ctor(KEYPAD_t * const me, I2C_HandleTypeDef *i2cHandle);
void KEYPAD_poll(KEYPAD_t * const me);

#endif /* INC_KEYPAD_4_4_H_ */
