/*
 * i2c.h
 *
 *  Created on: Jun 11, 2017
 *      Author: wizath
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include <stdio.h>

#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_i2c.h"
#include "lpc177x_8x_gpio.h"

#define TCAADDR 			0x71
#define TCARTMADDR 			0x76
#define IIC_SLAVE_ADDR		0x68

typedef struct SI324Info
{
	uint8_t RegIndex;	/* Register Number */
	uint8_t Value;		/* Value to be Written */
} SI324Info;

void i2c_init();
uint8_t i2c_write(uint8_t slaveAddr, uint8_t * buff, uint8_t len);
uint8_t i2c_read(uint8_t slaveAddr, uint8_t * buff, uint8_t len);
void i2c_tcaselect(uint8_t i);
/* Function that probes all available slaves connected to an I2C bus */
void i2c_probe_slaves(void);
void i2c_probe_slaves2(void);
void i2c_setSI5324(void);
void i2c_setSI5324_RTM(void);
void i2c_RTM_PWRON(void);



#endif /* INC_I2C_H_ */
