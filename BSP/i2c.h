
/*****************************************************************************
 *   i2c.h:  Header file for NXP LPC13xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Preliminary version, first Release
 *
******************************************************************************/
#ifndef __I2C_H
#define __I2C_H



#include "stm32l4xx_hal.h"

#define I2C1BASE 1
#define I2C2BASE 2




extern void I2C_IRQHandler( void );
extern uint8_t I2CInit( uint8_t num );
extern HAL_StatusTypeDef I2C_WriteBuffer(uint8_t num, uint16_t Addr, uint16_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length);
extern HAL_StatusTypeDef I2C_ReadBuffer(uint8_t num, uint16_t Addr, uint16_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length);
extern 




#endif /* end __I2C_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
