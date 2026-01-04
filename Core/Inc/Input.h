/*
 * Input.h
 *
 *  Created on: 02-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_INPUT_H_
#define INC_INPUT_H_

#include "Keypad.h"

typedef enum {
    ACTION_UP = 0,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_CONFIRM,
    ACTION_NONE
} key_action_e;

typedef struct {
    KEYPAD_t * keypad;
    keys_e last_raw_key;
    bool key_processed;
} INPUT_t;

void INPUT_ctor(INPUT_t * const me , KEYPAD_t * const keypad);
key_action_e INPUT_get_action(INPUT_t *const me);

#endif /* INC_INPUT_H_ */
