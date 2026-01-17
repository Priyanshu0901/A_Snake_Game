/*
 * Keypad.c
 *
 *  Created on: 01-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "Keypad.h"
#include "debug_logger.h"

#define KEYPAD_PCF8574_ADDRESS (PCF8574_DEFAULT_ADDRESS | 0x02)

void KEYPAD_ctor(KEYPAD_t *const me, I2C_HandleTypeDef *i2cHandle) {
	me->driver.address = KEYPAD_PCF8574_ADDRESS;
	PCF8574_ctor(&(me->driver), i2cHandle);
	me->key = NO_KEY;
	me->new_key_press = false;
}

static inline uint8_t col_to_int(uint8_t mask) {
	switch (mask) {
	case 0x0E:
		return 0;
	case 0x0D:
		return 1;
	case 0x0B:
		return 2;
	case 0x07:
		return 3;
	default:
		return 0xFF; // Error or no press
	}
}

void KEYPAD_poll(KEYPAD_t *const me) {
    static uint8_t row_masks[] = { 0xFE, 0xFD, 0xFB, 0xF7 };
    keys_e detected_key = NO_KEY; // Use a local variable

    for (int i = 0; i < 4; i++) {
        me->driver.set_pins.pin_byte[0] = row_masks[i] | 0xF0;
        PCF8574_write(&(me->driver));
        PCF8574_read(&(me->driver));

        uint8_t col_mask = (me->driver.status_pins.pin_byte[0] >> 4) & 0x0F;
        if (col_mask != 0x0F) {
            uint8_t col_idx = col_to_int(col_mask);
            if (col_idx != 0xFF) {
                detected_key = (keys_e) ((3 - i) + (col_idx * 4));
                break; // Exit the loop immediately once a key is found
            }
        }
    }

    if (detected_key != me->key) {
        if (detected_key != NO_KEY) {
            me->new_key_press = true;
        }
        me->key = detected_key;
    }
}
