/*
 * Char_Display.h
 *
 *  Created on: 17-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_CHAR_DISPLAY_H_
#define INC_CHAR_DISPLAY_H_

#include "SPLC780D.h"

typedef struct{
	char * buffer;
	SPLC780D_t * driver;
}CHAR_DISPLAY_t;

void CHAR_DISPLAY_ctor(CHAR_DISPLAY_t * const me, SPLC780D_t * display_driver);
void CHAR_DISPLAY_clear(CHAR_DISPLAY_t * const me, SPLC780D_t * display_driver);

void CHAR_WRITE_data(CHAR_DISPLAY_t * const me,char * const str,uint8_t x,uint8_t y);

void CHAR_DISPLAY_buffer_flush(CHAR_DISPLAY_t * const me);
#endif /* INC_CHAR_DISPLAY_H_ */
