/*
 * SPLC780D.c
 *
 *  Created on: 16-Jan-2026
 *      Author: rayv_mini_pc
 */
#include "SPLC780D_defs.h"
#include "SPLC780D.h"

inline GPIO_PinState CMD_TO_STATE_SPLC780D_RS(uint32_t cmd) {
	return (cmd & (1 << 9)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

inline GPIO_PinState CMD_TO_STATE_SPLC780D_RW(uint32_t cmd) {
	return (cmd & (1 << 8)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

inline void SPLC780D_Toggle_Latch(SPLC780D_t *const me) {
	HAL_GPIO_WritePin(me->E_Port, me->E_Pin, SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(me->E_Port, me->E_Pin, RESET);
}

void SPLC780D_Write_CMD(SPLC780D_t *const me, uint16_t cmd) {
	me->data_pins.set_pins.pin_byte[0] = (uint8_t) cmd & SPLC780D_CMD_BITMASK;
	PCF8574_write(&(me->data_pins));
	HAL_GPIO_WritePin(me->RS_Port, me->RS_Pin, CMD_TO_STATE_SPLC780D_RS(cmd));
	HAL_GPIO_WritePin(me->RW_Port, me->RW_Pin, CMD_TO_STATE_SPLC780D_RW(cmd));
	SPLC780D_Toggle_Latch(me);
}

void SPLC780D_Clear(SPLC780D_t *const me){
	SPLC780D_Write_CMD(me, SPLC780D_CLEAR_CMD);
	HAL_Delay(2); // VERY IMPORTANT: Clear needs ~2ms
	me->cursor_x = 0;
	me->cursor_y = 0;
}

//Follow Page 10
void SPLC780D_Reset(SPLC780D_t *const me) {

	// 1. First Function Set (Forcing 8-bit mode)
	SPLC780D_Write_CMD(me, SPLC780D_FUNCTION_SET);
	HAL_Delay(5); // Wait more than 4.1ms

	// 2. Second Function Set
	SPLC780D_Write_CMD(me, SPLC780D_FUNCTION_SET);
	HAL_Delay(1); // Wait more than 100us

	// 3. Third Function Set
	SPLC780D_Write_CMD(me, SPLC780D_FUNCTION_SET);

	// 4. Final Function Set (Set rows/font)
	// 0x38 = 8-bit mode, 2-line display, 5x8 font
	SPLC780D_Write_CMD(me, SPLC780D_FUNCTION_SET);

	// 5. Display ON/OFF Control
	SPLC780D_Write_CMD(me, SPLC780D_DISPLAY_CONTROL);

	SPLC780D_Write_CMD(me, SPLC780D_ENTRY_MODE_SET);
}

void SPLC780D_ctor(SPLC780D_t *const me, I2C_HandleTypeDef *i2chandle) {

	me->data_pins.address = SPLC780D_PCF8574_ADDRESS;
	PCF8574_ctor(&(me->data_pins), i2chandle);

	// Wait at least 40ms after VCC rises to 4.5V
	HAL_Delay(50);

	SPLC780D_Reset(me);
}

void SPLC780D_reset_cursor(SPLC780D_t *const me) {
	SPLC780D_Write_CMD(me, SPLC780D_RETURN_HOME);
}

void SPLC780D_move_display_cursor(SPLC780D_t *const me,
		SPLC780D_CURSOR_MOVEMENT_e movement) {
	SPLC780D_Write_CMD(me,
			SPLC780D_DISPLAY_CURSOR_SET | ((movement & 0x03) << 2));
}

void SPLC780D_move_cursor(SPLC780D_t *const me, uint8_t x, uint8_t y) {
	if (x >= CHAR_DISP_COLS || y >= CHAR_DISP_ROWS) {
		log_message("SPLC780D", LOG_ERROR,
				"SPLC780D_move_cursor Out of Bounds");
	}
	uint8_t addr =
			(y == 0) ? (SPLC780D_LINE_1_BASE + x) : (SPLC780D_LINE_2_BASE + x);
	SPLC780D_Write_CMD(me, SPLC780D_DDRAM_SET | (addr & SPLC780D_DDRAM_ADDR_MSK));

	me->cursor_x = x;
	me->cursor_y = y;
}

void SPLC780D_write_char(SPLC780D_t * const me, const char data){
	SPLC780D_Write_CMD(me,SPLC780D_WRITE_DATA_TO_RAM | data);
}
