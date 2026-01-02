/*
 * Input.c
 *
 *  Created on: 02-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "Input.h"

void INPUT_ctor(INPUT_t * const me , KEYPAD_t * keypad){
	me->keypad = keypad;
}

