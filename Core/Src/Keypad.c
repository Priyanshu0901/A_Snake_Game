/*
 * Keypad.c
 *
 *  Created on: 01-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "Keypad.h"
#include "debug_logger.h"

void KEYPAD_ctor(KEYPAD_t *const me, I2C_HandleTypeDef *i2cHandle) {
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

	for (int i = 0; i < 4; i++) {
		// Write row mask to PCF8574 (lower 4 bits are rows, upper 4 bits stay 1)
		me->driver.set_pins.pin_byte[0] = row_masks[i] | 0xF0;
		PCF8574_write(&(me->driver));

		// Read back the state
		PCF8574_read(&(me->driver));

		// Check upper 4 bits (Columns) for a 0
		uint8_t col_mask = (me->driver.status_pins.pin_byte[0] >> 4) & 0x0F;
		if (col_mask != 0x0F) {
			uint8_t col_idx = col_to_int(col_mask);

			if (col_idx != 0xFF) {
				keys_e new_key = (keys_e) ((3 - i) + (col_idx * 4));
				me->key = new_key;
				me->new_key_press = true;
				return;
			}
		}
	}
	me->key = NO_KEY;
}
