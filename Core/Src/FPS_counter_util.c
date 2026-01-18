/*
 * FPS_counter_util.c
 *
 *  Created on: 18-Jan-2026
 *      Author: rayv_mini_pc
 */

#include "FPS_counter_util.h"

void FPS_ctor(FPS_Counter_t *me, uint32_t update_interval_ms) {
    me->frame_count = 0;
    me->last_update_tick = 0;
    me->fps = 0;
    me->update_interval = update_interval_ms;
}

uint32_t FPS_tick(FPS_Counter_t *me, uint32_t current_tick) {
    // Increment frame count every time this is called
    me->frame_count++;

    // Calculate elapsed time since last FPS update
    uint32_t elapsed = current_tick - me->last_update_tick;

    // Update FPS calculation if enough time has passed
    if (elapsed >= me->update_interval) {
        // FPS = frames / (elapsed_ms / 1000)
        // Multiply by 1000 first to avoid float division
        me->fps = (me->frame_count * 1000) / elapsed;

        // Reset for next measurement period
        me->frame_count = 0;
        me->last_update_tick = current_tick;
    }

    return me->fps;
}

uint32_t FPS_get(FPS_Counter_t *me) {
    return me->fps;
}

void FPS_reset(FPS_Counter_t *me) {
    me->frame_count = 0;
    me->fps = 0;
    me->last_update_tick = 0;
}
