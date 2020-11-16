#ifndef _WIND_SENSOR_H
#define _WIND_SENSOR_H
#include "main.h"


#define INVALID_BUF 0X00

//全局寄存器地址
#define WK2XXX_GENA     0x00 //全局控制寄存器
#define WK2XXX_GRST     0x01 //全局子串口复位寄存器
#define WK2XXX_GMUT     0x02 //全局主串口控制寄存器      
#define WK2XXX_GIER     0x10 //全局中断寄存器
#define WK2XXX_GIFR     0x11 //全局中断标志寄存器
#define WK2XXX_GPDIR    0x21 //全局 GPIO 方向寄存器
#define WK2XXX_GPDAT    0x31 //全局 GPIO 数据寄存器
///////////////////////////////////////////////

//子串口控制寄存器
#define WK2XXX_SPAGE   0x0B //子串口页控制寄存器
#define WK2XXX_SCR  0x04    // SCR子串口控制寄存器
#define WK2XXX_LCR  0x05    //全局子串口复位寄存器
#define WK2XXX_FCR   0x56   // 子串口FIFO控制寄存器      
#define WK2XXX_SIER 0x07    // SIER子串口中断使能寄存器
#define WK2XXX_SIFR 0x08    //全局中断标志寄存器
#define WK2XXX_TFCNT  0x09  //全局 GPIO 方向寄存器
#define WK2XXX_RFCNT 0x0A   //全局控制寄存器
#define WK2XXX_FSR 0x0B     //全局子串口复位寄存器
#define WK2XXX_LSR  0x0C    //全局主串口控制寄存器      
#define WK2XXX_FDAT 0x0D    //全局中断寄存器
#define WK2XXX_FWCR 0x0E    //全局中断标志寄存器
#define WK2XXX_RS485 0x0F   //全局 GPIO 方向寄存器
#define WK2XXX_BAUD1 0x04   //全局 GPIO 数据寄存器
#define WK2XXX_BAUD0 0x05   //全局控制寄存器
#define WK2XXX_PRES 0x06    //全局子串口复位寄存器
#define WK2XXX_RFTL  0x07   //全局主串口控制寄存器      
#define WK2XXX_TFTL 0x08    //全局中断寄存器
#define WK2XXX_FWTH 0x09    //全局中断标志寄存器
#define WK2XXX_FWTL 0x0A    //全局 GPIO 方向寄存器
#define WK2XXX_XON1 0x0B    //全局 GPIO 数据寄存器
#define WK2XXX_XOFF1  0x0C  //全局 GPIO 数据寄存器
#define WK2XXX_SADR 0x0D    //全局 GPIO 数据寄存器
#define WK2XXX_SAEN 0x0E    //全局 GPIO 数据寄存器
#define WK2XXX_RTSDLY 0x0F  //全局 GPIO 数据寄存器

//子串口时钟使能位
#define WK2XXX_UT1EN 0x8F  //使能子串口 1 的时钟
#define WK2XXX_UT2EN 0x4F  //使能子串口 2 的时钟
#define WK2XXX_UT3EN 0x2F  //使能子串口 3 的时钟
#define WK2XXX_UT4EN 0x1F  //使能子串口 4 的时钟

//子串口软件复位位
#define WK2XXX_UT1RST 0x8F  //使能子串口 1 的软件复位
#define WK2XXX_UT2RST 0x4F  //使能子串口 2 的软件复位
#define WK2XXX_UT3RST 0x2F  //使能子串口 3 的软件复位
#define WK2XXX_UT4RST 0x1F  //使能子串口 4 的软件复位

//
#define WK2XXX_GIER 0x8F  //使能子串口 1 的时钟
#define WK2XXX_GIER 0x4F  //使能子串口 2 的时钟
#define WK2XXX_GIER 0x2F  //使能子串口 3 的时钟
#define WK2XXX_GIER 0x1F  //使能子串口 4 的时钟

#define WK2XXX_TXEN 0x01  //子串口发送使能位
#define WK2XXX_RXEN 0x02  //子串口接收使能位

#define WK2204_FDAT 0x0D  //子串口 FIFO 数据寄存器
#define WK2204_FSR  0x07  //子串口 FIFO 状态寄存器
 
#define WK2XXX_RFTRIG_IEN 0x01 //接收 FIFO 触点中断使能位RXOVT_IEN
#define WK2XXX_RXOUT_IEN 0x02 //接收 FIFO 超时中断使能位
 
#define MAXLEN 32            //接收缓存
#define FIFO_EXIST (1<<4)     //FIFO有数据
#define FIFO_NOTEXIST 0      //FIFO有数据
#define PORT_1 0
#define PORT_2 1
#define PORT_3 2
#define PORT_4 3
#define ERR    999

#define success 1
#define fail    0





extern UART_HandleTypeDef huart3;

/* 串口处理ADU数据帧的阶段描述 */
#define	USART_PROCESSING_IDEL					0
#define	USART_PROCESSING_SENDING			  1
#define	USART_PROCESSING_RECEIVING		  2
#define	USART_PROCESSING_FINISH				3
#define	USART_PROCESSING_ERR					  4


extern unsigned char UartProcessingPhase;
extern unsigned char TxRxBuffer;

void WK2204Init(UINT8 port);
void BaudAdaptive();
void WK2204WriteReg(unsigned char port,unsigned char reg,unsigned char dat);
unsigned char WK2204ReadReg(unsigned char port,unsigned char reg, unsigned char *data);
void WK2204WriteFIFO(unsigned char port,unsigned char *send_da,unsigned char num);
unsigned char WK2204ReadFIFO(unsigned char port,unsigned char num);
void WK2204SetBaud(UINT8 port,int baud);
void WK2204_RX(void);

typedef struct m_U3rc
{
    uint8_t buff[MAXLEN];

    uint16_t WD;
    uint16_t RD;
}s_U3rc;





#endif




