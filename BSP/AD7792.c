
#include "bsp_led.h"
#include "uart.h"
#include "spi.h"
#include "AD7792.h"
#include "sys_config.h"


void AD7792_Init( void )
{
	uint8_t buf[2] = {0};

	
	
	
	SPIInit(2);//

	vTaskDelay( 10 );

	AD7792_Reset();

	vTaskDelay( 10 );
	
	AD7792_Set_IO(0x0b);

	// 80
	AD7792_Red_Reg( AD7792_REG_STAT, buf, 1 );
	uart_printf( 0, "STAT: %02X\r\n", buf[0] );

	// 4A
	AD7792_Red_Reg( AD7792_REG_ID, buf, 1 );
	uart_printf( 0, "  ID: %02X\r\n", buf[0] );

#if DEBUG
	// 00 0A
	AD7792_Red_Reg( AD7792_REG_MODE, buf, 2 );
	uart_printf( 0, "MODE: %02X %02X\r\n", buf[0], buf[1] );

	// 07 10
	AD7792_Red_Reg( AD7792_REG_CFG, buf, 2 );
	uart_printf( 0, " CFG: %02X %02X\r\n", buf[0], buf[1] );

	vTaskDelay( 10 );

	// 00 00
	AD7792_Red_Reg( AD7792_REG_DATA, buf, 2 );
	uart_printf( 0, "DATA: %02X %02X\r\n", buf[0], buf[1] );

	// 00
	AD7792_Red_Reg( AD7792_REG_IO, buf, 1 );
	uart_printf( 0, "  IO: %02X\r\n", buf[0] );

	// 00
	AD7792_Red_Reg( AD7792_REG_OFFS, buf, 2 );
	uart_printf( 0, "OFFS: %02X %02X\r\n", buf[0], buf[1] );

	AD7792_Red_Reg( AD7792_REG_FSCA, buf, 2 );
	uart_printf( 0, "FSCA: %02X %02X\r\n", buf[0], buf[1] );

	vTaskDelay( 10 );

	AD7792_SingleConversion_Demo();
#endif
	return;
}

void AD7792_Reset(void)
{
	uint8_t i = 0;

	AD7792_SET_CS_LOW();

	for( i=0; i<4; i++ )
	{
		SPI_Read_Write_Byte( SPI2_BASE0, 0XFF );
	}

	AD7792_SET_CS_HIGH();

	return;
}

// Just for ADC use demo
void AD7792_SingleConversion_Demo( void )
{
	uint8_t buf[2] = {0};

	// Modefy params according to the sensor range
	// VBIAS disabled
	//  Por: Unipolar
	// Gain: 128
	//  Ref: Internal
	//  Buf: Enable
	//  Chn: 1
	AD7792_Set_Cfg(	AD7792_CFG_VBIAS_DIS
					|AD7792_CFG_POR_U
					|AD7792_CFG_GAIN_128
					|AD7792_CFG_REF_IN
					|AD7792_CFG_BUFFER
					|AD7792_CFG_SEL_CH1
					);

	AD7792_Set_Mode(AD7792_MODE_CONV_ONCE
					|AD7792_MODE_CLK_INT64
					|AD7792_MODE_RATE_17
					);

	AD7792_Red_Reg( AD7792_REG_STAT, buf, 1 );
	uart_printf( 0, "STAT: %02X\r\n", buf[0] );

	while( buf[0]&AD7792_STAT_NRDY )
	{
		vTaskDelay( 5 );
		AD7792_Red_Reg( AD7792_REG_STAT, buf, 1 );
		uart_printf( 0, "STAT: %02X\r\n", buf[0] );
	}

	AD7792_Red_Reg( AD7792_REG_DATA, buf, 2 );
	uart_printf( 0, "DATA: %02X %02X\r\n", buf[0], buf[1] );

	return;
}

void AD7792_Set_Mode( uint16_t mode )
{
	uint8_t buf[2] = {0};

	buf[0] = mode>>8;
	buf[1] = mode&0XFF;

	AD7792_Set_Reg( AD7792_REG_MODE, buf, 2 );

	vTaskDelay( 10 );

	AD7792_Red_Reg( AD7792_REG_MODE, buf, 2 );
	uart_printf( 0, "MODE: %02X %02X\r\n", buf[0], buf[1] );

	return;
}

void AD7792_Set_Cfg( uint16_t cfg )
{
	uint8_t buf[2] = {0};

	buf[0] = cfg>>8;
	buf[1] = cfg&0XFF;

	AD7792_Set_Reg( AD7792_REG_CFG, buf, 2 );

	vTaskDelay( 10 );

	AD7792_Red_Reg( AD7792_REG_CFG, buf, 2 );
	uart_printf( 0, " CFG: %02X %02X\r\n", buf[0], buf[1] );

	return;
}

void AD7792_Set_IO( uint8_t ctl )
{
	uint8_t buf[2] = {0};

	AD7792_Set_Reg( AD7792_REG_IO, &ctl, 1 );

	vTaskDelay( 10 );

	AD7792_Red_Reg( AD7792_REG_IO, buf, 1 );
	uart_printf( 0, "  IO: %02X\r\n", buf[0] );

	return;
}

void AD7792_Inter_Cali( uint16_t cali )
{
	uint8_t buf[2] = {0};

	AD7792_Red_Reg( AD7792_REG_MODE, buf, 2 );
	uart_printf( 0, "MODE: %02X %02X\r\n", buf[0], buf[1] );


	buf[0] &= 0X1F;
	buf[0] |= (cali>>8);

	AD7792_Set_Reg( AD7792_REG_MODE, buf, 2 );

	vTaskDelay( 10 );

	AD7792_Red_Reg( AD7792_REG_MODE, buf, 2 );
	uart_printf( 0, "MODE: %02X %02X\r\n", buf[0], buf[1] );

	AD7792_Red_Reg( AD7792_REG_OFFS, buf, 2 );
	uart_printf( 0, "OFFS: %02X %02X\r\n", buf[0], buf[1] );

	AD7792_Red_Reg( AD7792_REG_FSCA, buf, 2 );
	uart_printf( 0, "FSCA: %02X %02X\r\n", buf[0], buf[1] );

	return;
}


void AD7792_Sel_Reg( uint8_t read, uint8_t reg )
{
	uint8_t val = 0;

	val = read|( reg<<3 );

	SPI_Read_Write_Byte( SPI2_BASE0, val );

	return;
}

void AD7792_Red_Reg( uint8_t reg, uint8_t *buf, uint8_t len )
{
	uint8_t i = 0;

	AD7792_SET_CS_LOW();

	AD7792_Sel_Reg( AD7792_READ, reg );

	for( i=0; i<len; i++ )
	{
		buf[i] = SPI_Read_Write_Byte( SPI2_BASE0, 0X00 );
	}

	AD7792_SET_CS_HIGH();

	return;
}

void AD7792_Set_Reg( uint8_t reg, uint8_t *buf, uint8_t len )
{
	uint8_t i = 0;

	AD7792_SET_CS_LOW();

	AD7792_Sel_Reg( AD7792_WRTIE, reg );

	for( i=0; i<len; i++ )
	{
		SPI_Read_Write_Byte( SPI2_BASE0, buf[i] );
	}

	AD7792_SET_CS_HIGH();

	return;
}



