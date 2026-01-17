/*
 * FPS_counter_util.h
 *
 *  Created on: 18-Jan-2026
 *      Author: rayv_mini_pc
 */

#ifndef INC_FPS_COUNTER_UTIL_H_
#define INC_FPS_COUNTER_UTIL_H_

#include <stdint.h>

typedef struct {
    uint32_t frame_count;       // Total frames rendered
    uint32_t last_update_tick;  // Last time FPS was calculated
    uint32_t fps;               // Current FPS value
    uint32_t update_interval;   // How often to recalculate (ms)
} FPS_Counter_t;

/**
 * @brief Initialize FPS counter
 * @param me Pointer to FPS counter instance
 * @param update_interval_ms How often to update FPS value (e.g., 1000 for every second)
 */
void FPS_ctor(FPS_Counter_t *me, uint32_t update_interval_ms);

/**
 * @brief Call this once per frame render
 * @param me Pointer to FPS counter instance
 * @param current_tick Current HAL_GetTick() value
 * @return Current FPS value (updated every update_interval_ms)
 */
uint32_t FPS_tick(FPS_Counter_t *me, uint32_t current_tick);

/**
 * @brief Get current FPS without updating
 * @param me Pointer to FPS counter instance
 * @return Current FPS value
 */
uint32_t FPS_get(FPS_Counter_t *me);

/**
 * @brief Reset the counter
 * @param me Pointer to FPS counter instance
 */
void FPS_reset(FPS_Counter_t *me);

#endif /* INC_FPS_COUNTER_UTIL_H_ */
