
#ifndef __UART_h_
#define __UART_h_

#include "FreeRTOS.h"
#include "semphr.h"
//#include "stm32l4xx_hal.h"


#define UART_RD	0X01
#define UART_WR	0X02

#define SWAPSIZE						32

#define SIZE_RCV_BUF 1024
#define SIZE_SEND_BUF 1024
#define STATUS_SUCCESS 1

#define CBD_UART_1 0
#define CBD_UART_2 1


#define MES_SEND_SIZE 1024
#define MES_RCV_SIZE 1024




#define RX_BUF_SIZE 10
typedef struct {
	uint8_t cid;
	uint8_t *data;
	uint8_t *wp;
	uint8_t *rp;
	uint8_t *end;
	uint16_t cnt;
	uint16_t size;

	uint8_t channelSelec;

    SemaphoreHandle_t Mes_queue_Send;
    SemaphoreHandle_t Mes_queue_Rec;
    
	SemaphoreHandle_t SemDev;
	SemaphoreHandle_t SemBuf;
}uart_device_t;

typedef uart_device_t uart;

typedef struct{
	uint8_t cnt;
	uint8_t data[SWAPSIZE];
}dma_buf;

typedef struct{
	uint8_t wr;
	uint8_t rd;
	dma_buf buf[RX_BUF_SIZE];
}uart_dma;


typedef struct{
	uint8_t cid;
	uint8_t	rw;

	uint8_t	pri_r;
	uint8_t	pri_t;

	uint8_t channelSelec_r;
	uint8_t channelSelec_t;

	uint16_t heap_r_size;
	uint16_t heap_t_size;

    uint16_t Mes_queue_Send_size;
    uint16_t Mes_queue_Rec_size;

	uint32_t bps;

}uart_CBD;

enum{
    BPS_1200,
    BPS_2400,
    BPS_4800,
	BPS_9600,
	BPS_19200,
 	BPS_38400,
 	BPS_57600,
 	BPS_115200,
	BPS_MAX
};

typedef struct _BPS_CFG
{
	unsigned char bpsIndex;
	unsigned long bps;

} BPS_CFG;



uint8_t USART_DMA_Restart(uint8_t num);
uint8_t UART_Init(uint8_t num, uint32_t bps, uint8_t databit, uint8_t parity, uint8_t stopbit, uint8_t msp);
uint8_t UartOpen( uart_CBD* CBD );

void UartSend_1( void *pdata );
void UartSend_2( void *pdata );

void UartRecv_1( void *pdata );
void UartRecv_2( void *pdata );


uint16_t UartWrite( uint8_t cid, uint8_t* payload, uint16_t len );
uint16_t UartRead( uint8_t cid, uint8_t* payload, uint16_t len );

void uart_printf( uint8_t cid, char* fmt, ... );
void Hex_dis( uint8_t* data, uint8_t len );

uint16_t UartBufCnt( uint8_t cid );
void UartDmaInfo( uint8_t cid );

uint16_t UartWrite_RAW( uint8_t cid, uint8_t* payload, uint16_t len );
void DMA_IRQHandler(uint8_t num);


#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
