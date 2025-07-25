#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>
#include <stm32f401xe.h>

void I2C_Config (void);

void I2C_Start (uint8_t Address, uint8_t direction);

void I2C_Write (uint8_t data);
void I2C_Stop (void);

void I2C_WriteMulti (uint8_t *data, uint8_t size);

void I2C_Read (uint8_t Address, uint8_t *buffer, uint8_t size);

#endif