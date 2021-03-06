#ifndef __AD7792_H
#define __AD7792_H

#include "main.h"


#define AD7792_READ			0X40
#define AD7792_WRTIE		0X00

#define AD7792_REG_STAT		0X00
#define AD7792_REG_MODE		0X01
#define AD7792_REG_CFG		0X02
#define AD7792_REG_DATA		0X03
#define AD7792_REG_ID		0X04
#define AD7792_REG_IO		0X05
#define AD7792_REG_OFFS		0X06
#define AD7792_REG_FSCA		0X07

#define AD7792_STAT_NRDY	0X80

#define AD7792_MODE_CONV_CONT	(0X00<<13)
#define AD7792_MODE_CONV_ONCE	(0X01<<13)
#define AD7792_MODE_CONV_IDLE	(0X02<<13)
#define AD7792_MODE_CONV_PWDN	(0X03<<13)
#define AD7792_MODE_CONV_ZCAL	(0X04<<13)
#define AD7792_MODE_CONV_FCAL	(0X05<<13)

#define AD7792_MODE_CLK_INT64	(0X00<<5)
#define AD7792_MODE_CLK_INTOUT	(0X01<<5)
#define AD7792_MODE_CLK_EXT64	(0X02<<5)
#define AD7792_MODE_CLK_EXT		(0X03<<5)

#define AD7792_MODE_RATE_470	0X01
#define AD7792_MODE_RATE_242	0X02
#define AD7792_MODE_RATE_123	0X03
#define AD7792_MODE_RATE_62		0X04
#define AD7792_MODE_RATE_50		0X05
#define AD7792_MODE_RATE_39		0X06
#define AD7792_MODE_RATE_33		0X07
#define AD7792_MODE_RATE_19		0X08
#define AD7792_MODE_RATE_17		0X09
#define AD7792_MODE_RATE_16		0X0A
#define AD7792_MODE_RATE_12		0X0B
#define AD7792_MODE_RATE_10		0X0C
#define AD7792_MODE_RATE_8		0X0D
#define AD7792_MODE_RATE_6		0X0E
#define AD7792_MODE_RATE_4		0X0F

#define AD7792_CFG_VBIAS_DIS	(0X00<<14)
#define AD7792_CFG_VBIAS_CH1	(0X01<<14)
#define AD7792_CFG_VBIAS_CH2	(0X02<<14)

#define AD7792_CFG_GAIN_1		(0X00<<8)
#define AD7792_CFG_GAIN_2		(0X01<<8)
#define AD7792_CFG_GAIN_4		(0X02<<8)
#define AD7792_CFG_GAIN_8		(0X03<<8)
#define AD7792_CFG_GAIN_16		(0X04<<8)
#define AD7792_CFG_GAIN_32		(0X05<<8)
#define AD7792_CFG_GAIN_64		(0X06<<8)
#define AD7792_CFG_GAIN_128		(0X07<<8)

#define AD7792_CFG_POR_U		(0X01<<12)
#define AD7792_CFG_POR_B		(0X00<<12)

#define AD7792_CFG_REF_IN		(0X00<<7)
#define AD7792_CFG_REF_EXT		(0X01<<7)

#define AD7792_CFG_BUFFER		(0X01<<4)

#define AD7792_CFG_SEL_CH0		0X00
#define AD7792_CFG_SEL_CH1		0X01
#define AD7792_CFG_SEL_CH2		0X02
#define AD7792_CFG_SEL_TMP		0X06
#define AD7792_CFG_SEL_VDD		0X07

#define AD7792_IO_IEXC_GEN		(0X00<<2)
#define AD7792_IO_IEXC_CRO		(0X01<<2)
#define AD7792_IO_IEXC_OUT1		(0X02<<2)
#define AD7792_IO_IEXC_OUT2		(0X03<<2)

#define AD7792_IO_IEXC_DIS		0X00
#define AD7792_IO_IEXC_10U		0X01
#define AD7792_IO_IEXC_210U		0X02
#define AD7792_IO_IEXC_1M		0X03


void AD7792_Init( void );
void AD7792_Reset(void);

void AD7792_SingleConversion_Demo( void );
void AD7792_Set_Mode( uint16_t mode );
void AD7792_Set_Cfg( uint16_t cfg );
void AD7792_Set_Cfg( uint16_t cfg );
void AD7792_Set_IO( uint8_t ctl );
void AD7792_Inter_Cali( uint16_t cali );

void AD7792_Sel_Reg( uint8_t read, uint8_t reg );
void AD7792_Red_Reg( uint8_t reg, uint8_t *buf, uint8_t len );
void AD7792_Set_Reg( uint8_t reg, uint8_t *buf, uint8_t len );



#endif


