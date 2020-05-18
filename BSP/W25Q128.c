
#include "sys_config.h"
#include "bsp_led.h"
#include "uart.h"
#include "spi.h"

#include "W25Q128.h"

void W25Q128_Init( void )
{
	uint8_t tmp = 0;
	uint8_t buf[32] = {0};

	
	
	
	SPIInit(1);

	vTaskDelay( 10 );

	W25Q128_Reset();

	vTaskDelay( 10 );

#if DEBUG
	//W25Q128_WriteEnable();

	W25Q128_Read_Reg( W25Q128_READ_SR1, buf, 1 );
	uart_printf( 0, "SR1: %02X\r\n", buf[0] );

	W25Q128_Read_Reg( W25Q128_READ_SR2, buf, 1 );
	uart_printf( 0, "SR2: %02X\r\n", buf[0] );

	W25Q128_Read_Reg( W25Q128_READ_SR3, buf, 1 );
	uart_printf( 0, "SR3: %02X\r\n", buf[0] );

	W25Q128_Get_DID();

	W25Q128_Get_MID();

	W25Q128_Get_UID();

	// Do not enable the cmd below, just for debug!!!
	//W25Q128_SET_SR( W25Q128_SET_SR1, W25Q128_SR1_BP0|W25Q128_SR1_BP1|W25Q128_SR1_BP2 );

	W25Q128_Data_Read( 0X00, buf, 32 );

	W25Q128_Data_Read( 0X40, buf, 32 );

	W25Q128_Erase_Chip( W25Q128_ERS_SEC, 0X00 );

	for( tmp=0; tmp<32; tmp++ )
	{
		buf[tmp] = tmp;
	}

	W25Q128_Data_Prog( 0X00, buf, 32 );

	W25Q128_Data_Read( 0X00, buf, 32 );

	for( tmp=0; tmp<32; tmp++ )
	{
		buf[tmp] = 0X55;
	}

	W25Q128_Data_Prog( 0X40, buf, 32 );

	W25Q128_Data_Read( 0X40, buf, 32 );

#endif

	return;

}
void W25Q128_Reset(void)
{
	uint8_t buf[2] = {0};

	buf[0] = W25Q128_ENA_RST;
	W25Q128_Send_Buf( buf, 1, 1 );

	vTaskDelay( 5 );

	buf[1] = W25Q128_EXC_RST;
	W25Q128_Send_Buf( buf+1, 1, 1 );

	// If the instruction sequence is successfully accepted,
	// the device will take approximately 30¦ÌS (tRST) to reset.
	// No instruction will be accepted during the reset period.
	//W25Q128_Send_Buf( buf, 2, 1 );

	return;
}

uint8_t W25Q128_Data_Read( uint32_t addr, uint8_t *rbuf, uint16_t len )
{
	uint8_t tmp = 0;

	uint8_t buf[4] = {0};

	while( W25Q128_IS_Busy() )
	{
		vTaskDelay( 10 );

		tmp++;

		if( tmp>=100 )
		{
			uart_printf( 0, "Chip is still busy!\r\n" );

			return pdFAIL;
		}
	}

	buf[0] = W25Q128_DATA_READ;
	buf[1] = 0XFF&(addr>>16);
	buf[2] = 0XFF&(addr>>8);
	buf[3] = 0XFF&(addr>>0);

	W25Q128_Send_Buf( buf, 4, 0 );

	W25Q128_Recv_Buf( rbuf, len );

#if DEBUG
	tmp = 0;

	while( W25Q128_IS_Busy() )
	{
		vTaskDelay( 10 );

		tmp++;

		uart_printf( 0, "Chip is still busy!\r\n" );
	}

	Hex_dis( rbuf, len );
#endif

	return pdTRUE;

}

uint8_t W25Q128_Data_Prog( uint32_t addr, uint8_t *tbuf, uint16_t len )
{
	uint8_t tmp = 0;

	uint8_t buf[4] = {0};

	if( len==256 && (addr&0XFF)!=0 )
	{
		uart_printf( 0, "Addr must page aligned!\r\n" );

		return pdFAIL;
	}

	if( len==0 || len>256 )
	{
		uart_printf( 0, "Len error!\r\n" );

		return pdFAIL;
	}

	while( W25Q128_IS_Busy() )
	{
		vTaskDelay( 10 );

		tmp++;

		if( tmp>=100 )
		{
			uart_printf( 0, "Chip is still busy!\r\n" );

			return pdFAIL;
		}
	}

	W25Q128_WriteEnable();

	buf[0] = W25Q128_DATA_PROG;
	buf[1] = 0XFF&(addr>>16);
	buf[2] = 0XFF&(addr>>8);
	buf[3] = 0XFF&(addr>>0);

	W25Q128_Send_Buf( buf, 4, 0 );

	W25Q128_Send_Buf( tbuf, len, 1 );

	return pdTRUE;
}

void W25Q128_WriteEnable( void )
{
	uint8_t buf[2] = {0};

	buf[0] = W25Q128_ENA_WR;

	// If the instruction sequence is successfully accepted,
	// the device will take approximately 30¦ÌS (tRST) to reset.
	// No instruction will be accepted during the reset period.
	W25Q128_Send_Buf( buf, 1, 1 );

	return;
}

void W25Q128_WriteDisable( void )
{
	uint8_t buf[2] = {0};

	buf[0] = W25Q128_DIS_WR;

	// If the instruction sequence is successfully accepted,
	// the device will take approximately 30¦ÌS (tRST) to reset.
	// No instruction will be accepted during the reset period.
	W25Q128_Send_Buf( buf, 1, 1 );

	return;
}

void W25Q128_WriteVolatile( void )
{
	uint8_t buf[2] = {0};

	buf[0] = W25Q128_ENA_VOL;

	// If the instruction sequence is successfully accepted,
	// the device will take approximately 30¦ÌS (tRST) to reset.
	// No instruction will be accepted during the reset period.
	W25Q128_Send_Buf( buf, 1, 1 );

	return;
}

uint8_t W25Q128_SET_SR( uint8_t cmd, uint8_t val )
{
	uint8_t ret = 0;

	uint8_t buf[2] = {0};

	buf[0] = cmd;
	buf[1] = val;
	//buf[1] = 0;

	W25Q128_WriteEnable();

	W25Q128_WriteVolatile();

	W25Q128_Send_Buf( buf, 2, 1 );

	vTaskDelay( 5 );

	W25Q128_Read_Reg( cmd+4, &ret, 1 );

	uart_printf( 0, "Val: %02X\r\n", ret );

	return ret;
}

void W25Q128_Erase_Chip( uint8_t cmd, uint32_t addr )
{
	uint8_t buf[4] = {0};

	buf[0] = cmd;
	buf[1] = 0XFF&(addr>>16);
	buf[2] = 0XFF&(addr>>8);
	buf[3] = 0XFF&(addr>>0);

	W25Q128_WriteEnable();
	W25Q128_Send_Buf( buf, 4, 1 );

	return;
}

uint8_t W25Q128_IS_Busy( void )
{
	uint8_t ret = pdFAIL;
	uint8_t buf[1] = {0};

	W25Q128_Read_Reg( W25Q128_READ_SR1, buf, 1 );

	if( buf[0]&W25Q128_SR1_BUSY )
	{
		ret = pdTRUE;
	}

	return ret;
}

void W25Q128_Get_DID( void )
{
	uint8_t buf[4] = {0};

	buf[0] = W25Q128_GET_DID;
	buf[1] = 0X00;
	buf[2] = 0X00;
	buf[3] = 0X00;

	W25Q128_Send_Buf( buf, 4, 0 );

	W25Q128_Recv_Buf( buf, 1 );

	uart_printf( 0, "DID: %02X\r\n", buf[0] );

	return;
}

void W25Q128_Get_MID( void )
{
	uint8_t buf[4] = {0};

	buf[0] = W25Q128_GET_MID;
	buf[1] = 0X00;
	buf[2] = 0X00;
	buf[3] = 0X00;

	W25Q128_Send_Buf( buf, 4, 0 );

	W25Q128_Recv_Buf( buf, 2 );

	uart_printf( 0, "MID: %02X %02X\r\n", buf[0], buf[1] );

	return;
}

void W25Q128_Get_UID( void )
{
	uint8_t buf[5] = {0};

	buf[0] = W25Q128_GET_UID;
	buf[1] = 0X00;
	buf[2] = 0X00;
	buf[3] = 0X00;
	buf[4] = 0X00;

	W25Q128_Send_Buf( buf, 5, 0 );

	W25Q128_Recv_Buf( buf, 4 );

	uart_printf( 0, "UID: %02X %02X %02X %02X\r\n", buf[0], buf[1], buf[2], buf[3] );

	return;
}

void W25Q128_Send_Buf( uint8_t *buf, uint16_t len, uint8_t over )
{
	uint16_t i = 0;

	W25Q128_SET_CS_LOW();

	for( i=0; i<len; i++ )
	{
		SPI_Read_Write_Byte( SPI1_BASE0, buf[i] );
	}

	if( over )
	{
		W25Q128_SET_CS_HIGH();
	}

	return;
}

void W25Q128_Recv_Buf( uint8_t *buf, uint16_t len )
{
	uint16_t i = 0;

	for( i=0; i<len; i++ )
	{
		buf[i] = SPI_Read_Write_Byte( SPI1_BASE0, 0X00 );
	}

	W25Q128_SET_CS_HIGH();

	return;
}

void W25Q128_Read_Reg( uint8_t cmd, uint8_t *buf, uint16_t len )
{
	uint16_t i = 0;

	W25Q128_SET_CS_LOW();

	SPI_Read_Write_Byte( SPI1_BASE0, cmd );

	for( i=0; i<len; i++ )
	{
		buf[i] = SPI_Read_Write_Byte( SPI1_BASE0, 0X00 );
	}

	W25Q128_SET_CS_HIGH();

	return;
}



