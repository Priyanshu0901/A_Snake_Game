/*
 * PCF8574.h
 *
 *  Created on: Jan 31, 2024
 *      Author: Priyanshu Roy
 */

#ifndef INC_PCF8574_H_
#define INC_PCF8574_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

/*
 * DEFINES
 */

/* Default I2C address for the PCF8574 */
#define PCF8574_DEFAULT_ADDRESS 0x20


/*
 * PIN CONTROL MACROS
 */
#define PCF8574_SET_PIN(dev, pin, state) \
    do { \
        if (state) { \
            (dev)->set_pins.pin_byte[0] |= (1 << (pin)); \
        } else { \
            (dev)->set_pins.pin_byte[0] &= ~(1 << (pin)); \
        } \
    } while(0)

#define PCF8574_GET_PIN(dev, pin) \
    (((dev)->status_pins.pin_byte[0] >> (pin)) & 0x01)

#pragma pack(1)
typedef struct port {
	uint8_t port_0 :1;
	uint8_t port_1 :1;
	uint8_t port_2 :1;
	uint8_t port_3 :1;
	uint8_t port_4 :1;
	uint8_t port_5 :1;
	uint8_t port_6 :1;
	uint8_t port_7 :1;
} port_pins_t;

typedef union port_config {
	port_pins_t pins;
	uint8_t pin_byte[1];
} port_config_t;

typedef struct {
	I2C_HandleTypeDef *i2cHandle;
	uint8_t address;
	port_config_t set_pins;
	port_config_t status_pins;
} PCF8574_t;


HAL_StatusTypeDef PCF8574_ctor(PCF8574_t * const dev, I2C_HandleTypeDef *i2cHandle,
		uint8_t address);

HAL_StatusTypeDef PCF8574_write(PCF8574_t * const dev);
HAL_StatusTypeDef PCF8574_read(PCF8574_t * const dev);

#endif /* INC_PCF8574_H_ */
