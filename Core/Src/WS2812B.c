/*
 * WS2812B.c
 *
 *  Created on: 30-Dec-2025
 *      Author: rayv_mini_pc
 */

#include "WS2812B.h"

// At 100 MHz,each clock cycle is exactly 10 nanoseconds.
#define T0H_10NS 40 // 0 code ,high voltage time // 0.4us ±150ns -> 400 ±150ns  /10
#define T0L_10NS 85 // 0 code , low voltage time  // 0.85us ±150ns -> 850 ±150ns /10

#define T1H_10NS 80 // 1 code ,high voltage time // 0.8us ±150ns -> 800 ±150ns /10
#define T1L_10NS 45// 1 code ,low voltage time // 0.45us ±150ns -> 450 ±150ns /10

#define RES_10NS 50000 // Low Voltage Above 50μs // 50000 but it has to return and get the new buffer so

#define ITTERATION_FACTOR 3

void WS2812B_ctor(WS2812B_t *const me, GPIO_TypeDef *port, uint16_t pin) {
	me->port = port;
	me->pin = pin;
	me->state = OK;
	WS2812B_reset(me);
}

// Force the compiler to place the code directly inside the loop to save time
static inline void delay_cycles(uint32_t cycles) {
	while (cycles--) {
		__NOP(); // One No-Operation instruction
	}
}

// Use 'inline' to eliminate function call overhead
static inline void write_bit(WS2812B_t *const me, uint32_t high_cycles,
		uint32_t low_cycles) {
	me->port->BSRR = me->pin;                // SET HIGH
	delay_cycles(high_cycles);               // Precise high time
	me->port->BSRR = (uint32_t) me->pin << 16; // SET LOW
	delay_cycles(low_cycles);                // Precise low time
}

void WS2812B_reset(WS2812B_t *const me) {
	me->state = RESET_WAIT;
	me->port->BSRR = (uint32_t) me->pin << 16;
	delay_cycles(RES_10NS / ITTERATION_FACTOR);
	me->state = OK;
}

void WS2812B_write(WS2812B_t *const me, PIXEL_t *pixels, uint16_t count) {
	me->state = WRITING;

	    uint32_t primask = __get_PRIMASK();
	    __disable_irq(); // Keep interrupts off for the WHOLE strip

	    for (uint16_t p = 0; p < count; p++) {
	        // Access the GRB bytes inside the union
	        for (int i = 0; i < PIXEL_SIZE; i++) {
	            uint8_t byte = pixels[p].pixel_array[i];

	            // Unrolling this loop or keeping it tight is key
	            for (int j = 7; j >= 0; j--) {
	                if ((byte >> j) & 0x01) {
	                    write_bit(me, T1H_10NS / ITTERATION_FACTOR, T1L_10NS / ITTERATION_FACTOR);
	                } else {
	                    write_bit(me, T0H_10NS / ITTERATION_FACTOR, T0L_10NS / ITTERATION_FACTOR);
	                }
	            }
	        }
	    }

	    __set_PRIMASK(primask); // Re-enable interrupts only after ALL pixels are sent
	    WS2812B_reset(me);      // Force the latch
	    me->state = OK;
}
