/*
 * SPLC780D_defs.h
 *
 *  Created on: 16-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_SPLC780D_DEFS_H_
#define INC_SPLC780D_DEFS_H_

#define SPLC780D_PCF8574_ADDRESS 	(PCF8574_DEFAULT_ADDRESS | 0x05)

#define SPLC780D_CMD_BITMASK 		0x00FF

// I / D = 1: Increment, I / D = 0: Decrement.
#define SPLC780D_I_D_SET			(1U << 1)

// S = 1: The display shift, S = 0: The display does not shift.
#define SPLC780D_SHIFT_SET			(0U << 0)

#define SPLC780D_ENTRY_MODE_SET 	(0x0004 | SPLC780D_I_D_SET | SPLC780D_SHIFT_SET)

//D = 1: Display on, D = 0: Display off
#define SPLC780D_DISPLAY_SET 		(1U << 2)

//C = 1: Cursor on, C = 0: Cursor off
#define SPLC780D_CURSOR_SET 		(1U << 1)  // NOTE: For test purposes

//B = 1: Blinks on, B= 0: Blinks off
#define SPLC780D_BLINK_SET			(0U << 0)

#define SPLC780D_DISPLAY_CONTROL 	(0x0008 | SPLC780D_DISPLAY_SET | SPLC780D_CURSOR_SET | SPLC780D_BLINK_SET)

#define SPLC780D_DISPLAY_CURSOR_SET (0x0010)

/*
 *	DL = 1: Data transferred with 8-bit length (DB7 - 0).
 *	DL = 0: Data transferred with 4-bit length (DB7 - 4).
 */
#define SPLC780D_DATA_LENGTH 		(1U << 4)

/*
 * 	N = 0: One-line display.
 *	N = 1: Two-line display.
 */
#define SPLC780D_DISPLAY_LINE 		(1U << 3)
#define SPLC780D_LINE_1_BASE 		(0x0000)
#define SPLC780D_LINE_2_BASE 		(0x0040)
/*
 * 	F = 0: 5 x 8 dots character font.
 *	F = 1: 5 x 10 dots character font.
 *	Note: if SPLC780D_DISPLAY_LINE = 1 then F = x (Don't care)
 */
#define SPLC780D_DISPLAY_FONT 		(0U << 2)

#define SPLC780D_FUNCTION_SET 		(0x0030 | SPLC780D_DATA_LENGTH | SPLC780D_DISPLAY_LINE | SPLC780D_DISPLAY_FONT) // Pg 10

#define SPLC780D_CLEAR_CMD			0x0001
#define SPLC780D_RETURN_HOME 		0x0002

#define SPLC780D_DDRAM_SET 			(1U << 7)
#define	SPLC780D_DDRAM_ADDR_MSK 	(0x007F)

//Set CGRAM Address
#define SPLC780D_CGRAM_SET 			(1U << 6)
#define	SPLC780D_CGRAM_ADDR_MSK 	(0x003F)

#define SPLC780D_WRITE_DATA_TO_RAM	(1U << 9)

#define SPLC780D_READ_DATA_TO_RAM	(SPLC780D_WRITE_DATA_TO_RAM | (1U << 8))

#endif /* INC_SPLC780D_DEFS_H_ */
