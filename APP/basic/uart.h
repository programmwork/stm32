#ifndef __UART_H__
#define __UART_H__
 
#include "typedef.h"
#include "defpam.h"
#include <stdarg.h>

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
	U16 UBR0;
	U16 UBR1;
	U16 UMCTL;
} BPS_CFG;
typedef struct _BPS2_CFG
{
	U8 	bpsIndex;
	U32 bps;

} BPS2_CFG;

/*
#define UART_POWER_BIT BIT4         //P1.4
#define UART_POWER_FUNC_IO()     P1SEL &=~ UART_POWER_BIT
#define UART_POWER_OUT()         P1DIR |=  UART_POWER_BIT
#define UART_POWER_ON()          P1OUT |=  UART_POWER_BIT
#define UART_POWER_OFF()         P1OUT &=~ UART_POWER_BIT
*/

void rs485_ctrl_init(UINT8 uartno);

void uartInit(UINT8 uartno,s_uartinfo_t *uartinfo);
void uart0Init(U32 baud,UINT8 databit,UINT8 parity,UINT8 stopbit);
void uart1Init(U32 baud,UINT8 databit,UINT8 parity,UINT8 stopbit);
void uart2Init(U32 baud,UINT8 databit,UINT8 parity,UINT8 stopbit);
void uart3Init(U32 baud,UINT8 databit,UINT8 parity,UINT8 stopbit);


void uartSendStr(UINT8 uartno,UINT8 *pucStr,UINT16 len);
 

#endif
