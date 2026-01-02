/*
 * Input.h
 *
 *  Created on: 02-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_INPUT_H_
#define INC_INPUT_H_

#include "Keypad.h"

typedef struct {
    KEYPAD_t * keypad;
} INPUT_t;

void INPUT_ctor(INPUT_t * const me , KEYPAD_t * const keypad);

#endif /* INC_INPUT_H_ */
