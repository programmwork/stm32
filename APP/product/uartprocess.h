#ifndef __UARTPROCESS_H__
#define __UARTPROCESS_H__
 
#include "main.h"





extern void checkuart(U8 uartno);
void uart_rcv(uint8_t uartno);

void processuartdata(U8 uartno);

void uartSendStr(UINT8 uartno,UINT8 *pucStr,UINT16 len);

U8 uartdev_process(U8 uartno,U8 *buff,U16 len);
U8 uartdev_process_sensor(U8 uartno,U8 *buff,U8 len);
U8 uartdev_process_rs485(U8 uartno,U8 *buff,U8 len);
U8 uartdev_process_lora(U8 uartno,U8 *buff,U8 len);

#endif
