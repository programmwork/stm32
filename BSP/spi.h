/*****************************************************************************
 *   ssp.h:  Header file for NXP LPC134x Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.19  ver 1.00    Preliminary version, first Release
 *
******************************************************************************/
#ifndef __SPI_H__
#define __SPI_H__



#define SPI1_BASE0 1
#define SPI2_BASE0 2
#define SPI3_BASE0 3

uint8_t SPIInit( uint8_t num);
void AD7792_SET_CS_LOW( void );
void AD7792_SET_CS_HIGH( void );
void W25Q128_SET_CS_LOW( void );
void W25Q128_SET_CS_HIGH( void );
uint8_t SPI_Read_Write_Byte( uint8_t num, uint8_t data );

#endif  /* __SPI_H__ */

