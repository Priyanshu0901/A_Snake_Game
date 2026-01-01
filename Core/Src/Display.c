/*
 * Display.c
 *
 *  Created on: 30-Dec-2025
 *      Author: rayv_mini_pc
 */

#include "Display.h"
#include <string.h>

void DISPLAY_ctor(DISPLAY_t * const me, GPIO_TypeDef *port, uint16_t pin)
{
	WS2812B_ctor(&(me->driver), port, pin);
}

// Example of a better Update Architecture
void DISPLAY_update(DISPLAY_t * const me) {
    // Post-processing (Optional: Brightness scaling)

    // Physical Mapping (Handle zig-zag wiring if necessary)
    // Flush to Hardware
	WS2812B_write(&(me->driver), &me->display_buffer[0][0], DISPLAY_ROWS * DISPLAY_COLS);
}

// Max raw color value (keep at 255 for full resolution)
#define COLOR_WHEEL_RANGE 10
/**
 * @param hue: 0-255 color position
 * @param brightness: 0-255 (128 for half brightness)
 */
static void get_rainbow_color(uint8_t hue, uint8_t brightness, uint8_t *r, uint8_t *g, uint8_t *b) {
    uint8_t rs, gs, bs;

    // 1. Generate the pure "Full Power" Rainbow
    if (hue < 85) {
        rs = 255 - (hue * 3);
        gs = hue * 3;
        bs = 0;
    } else if (hue < 170) {
        hue -= 85;
        rs = 0;
        gs = 255 - (hue * 3);
        bs = hue * 3;
    } else {
        hue -= 170;
        rs = hue * 3;
        gs = 0;
        bs = 255 - (hue * 3);
    }

    // 2. Apply Brightness Scaling
    // (Color * brightness) / 256
    *r = (uint8_t)((rs * brightness) >> 8);
    *g = (uint8_t)((gs * brightness) >> 8);
    *b = (uint8_t)((bs * brightness) >> 8);
}


void test_breathing_rainbow_update(DISPLAY_t * const me)
{
    static uint8_t hue_offset = 0;

    for(uint8_t i = 0; i < DISPLAY_ROWS; i++){
        for(uint8_t j = 0; j < DISPLAY_COLS; j++){
            uint8_t r, g, b;
            get_rainbow_color(hue_offset + (i * 10) + (j * 10),COLOR_WHEEL_RANGE, &r, &g, &b);

            me->display_buffer[i][j].pixels.green = g;
            me->display_buffer[i][j].pixels.red   = r;
            me->display_buffer[i][j].pixels.blue  = b;
        }
    }
    hue_offset += 2; // Speed of rainbow movement
}

void identify_layout(DISPLAY_t * const me) {
    for (uint16_t i = 0; i < (DISPLAY_ROWS * DISPLAY_COLS); i++) {
        // Clear buffer
        memset(me->display_buffer, 0, sizeof(me->display_buffer));

        // Treat the 2D buffer as 1D to see how the LEDs are physically mapped
        PIXEL_t *flat_buffer = &me->display_buffer[0][0];

        flat_buffer[i].pixels.red = 50; // Dim red

        // Write the whole strip
        WS2812B_write(&me->driver, flat_buffer, DISPLAY_ROWS * DISPLAY_COLS);

        HAL_Delay(50);
    }
}

void DISPLAY_test(DISPLAY_t * const me)
{
//	identify_layout(me);
    test_breathing_rainbow_update(me);
}


