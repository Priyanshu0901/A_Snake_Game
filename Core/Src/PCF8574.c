/*
 * PCF8574.c
 *
 *  Created on: Jan 31, 2024
 *      Author: Priyanshu Roy
 */

#include "PCF8574.h"
#include "debug_logger.h"

#define I2C_TIMEOUT 100

HAL_StatusTypeDef PCF8574_ctor(PCF8574_t * const me, I2C_HandleTypeDef *i2cHandle,
		uint8_t address) {

	me->i2cHandle = i2cHandle;
	me->address = address;

	me->set_pins.pin_byte[0] = 0x00;

	log_message("PCF8574", LOG_INFO, "Initializing meice at address 0x%02X", address);
	
	// Try expected address first
	HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(i2cHandle, (address << 1), 1, I2C_TIMEOUT);
	
	if (status != HAL_OK) {
		log_message("PCF8574", LOG_WARN, "Device not found at 0x%02X, doing one-time sweep", address);
		
		// One-time sweep through common PCF8574 addresses
		for (uint8_t addr = 0x00; addr < 0x7F; addr++) {
			status = HAL_I2C_IsDeviceReady(i2cHandle, (addr << 1), 1, I2C_TIMEOUT);
			if (status == HAL_OK) {
				me->address = addr;
				log_message("PCF8574", LOG_INFO, "Found meice at 0x%02X", addr);
				break;
			}
			else {
				log_message("PCF8574", LOG_WARN, "meice not found at 0x%02X", addr);
			}
		}
		
		if (status != HAL_OK) {
			log_message("PCF8574", LOG_ERROR, "No PCF8574 meices found");
			return HAL_ERROR;
		}
	}

	// Set safe defaults
	me->set_pins.pin_byte[0] = 0x00; // All pins LOW for safety
	
	status = PCF8574_write(me);
	if (status != HAL_OK) {
		log_message("PCF8574", LOG_ERROR, "Initial write failed with status %d", status);
		return status;
	}

	// Step 4: Verify communication by reading back
	status = PCF8574_read(me);
	if (status != HAL_OK) {
		log_message("PCF8574", LOG_ERROR, "Initial read failed with status %d", status);
		return status;
	}

	log_message("PCF8574", LOG_INFO, "Device initialized successfully at address 0x%02X", me->address);
	return HAL_OK;
}



HAL_StatusTypeDef PCF8574_write(PCF8574_t * const me) {

	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(me->i2cHandle,
			(me->address << 1) | 0, me->set_pins.pin_byte,
			sizeof(me->set_pins.pin_byte), I2C_TIMEOUT);
	if (status != HAL_OK) {
		return status;
	}

	return HAL_OK;
}

HAL_StatusTypeDef PCF8574_read(PCF8574_t * const me) {
	HAL_StatusTypeDef status = HAL_I2C_Master_Receive(me->i2cHandle,
			(me->address << 1) | 1, me->status_pins.pin_byte,
			sizeof(me->status_pins.pin_byte), I2C_TIMEOUT);
	if (status != HAL_OK) {
		return status;
	}

	return HAL_OK;
}
