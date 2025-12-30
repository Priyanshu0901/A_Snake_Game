/*
 * pixel.h
 *
 *  Created on: 30-Dec-2025
 *      Author: rayv_mini_pc
 */

#ifndef INC_PIXEL_H_
#define INC_PIXEL_H_

#define PIXEL_SIZE 3

#pragma pack(1)
typedef struct{
	uint8_t green;
	uint8_t red;
	uint8_t blue;
}PIXEL_GRB_t;

typedef union{
	PIXEL_GRB_t pixels;
	uint8_t pixel_array[PIXEL_SIZE];
}PIXEL_t;

#endif /* INC_PIXEL_H_ */
