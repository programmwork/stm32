#ifndef _WIND_SENSOR_H
#define _WIND_SENSOR_H
#include "main.h"


#define INVALID_BUF 0X00

//ȫ�ּĴ�����ַ
#define WK2XXX_GENA     0x00 //ȫ�ֿ��ƼĴ���
#define WK2XXX_GRST     0x01 //ȫ���Ӵ��ڸ�λ�Ĵ���
#define WK2XXX_GMUT     0x02 //ȫ�������ڿ��ƼĴ���      
#define WK2XXX_GIER     0x10 //ȫ���жϼĴ���
#define WK2XXX_GIFR     0x11 //ȫ���жϱ�־�Ĵ���
#define WK2XXX_GPDIR    0x21 //ȫ�� GPIO ����Ĵ���
#define WK2XXX_GPDAT    0x31 //ȫ�� GPIO ���ݼĴ���
///////////////////////////////////////////////

//�Ӵ��ڿ��ƼĴ���
#define WK2XXX_SPAGE   0x0B //�Ӵ���ҳ���ƼĴ���
#define WK2XXX_SCR  0x04    // SCR�Ӵ��ڿ��ƼĴ���
#define WK2XXX_LCR  0x05    //ȫ���Ӵ��ڸ�λ�Ĵ���
#define WK2XXX_FCR   0x56   // �Ӵ���FIFO���ƼĴ���      
#define WK2XXX_SIER 0x07    // SIER�Ӵ����ж�ʹ�ܼĴ���
#define WK2XXX_SIFR 0x08    //ȫ���жϱ�־�Ĵ���
#define WK2XXX_TFCNT  0x09  //ȫ�� GPIO ����Ĵ���
#define WK2XXX_RFCNT 0x0A   //ȫ�ֿ��ƼĴ���
#define WK2XXX_FSR 0x0B     //ȫ���Ӵ��ڸ�λ�Ĵ���
#define WK2XXX_LSR  0x0C    //ȫ�������ڿ��ƼĴ���      
#define WK2XXX_FDAT 0x0D    //ȫ���жϼĴ���
#define WK2XXX_FWCR 0x0E    //ȫ���жϱ�־�Ĵ���
#define WK2XXX_RS485 0x0F   //ȫ�� GPIO ����Ĵ���
#define WK2XXX_BAUD1 0x04   //ȫ�� GPIO ���ݼĴ���
#define WK2XXX_BAUD0 0x05   //ȫ�ֿ��ƼĴ���
#define WK2XXX_PRES 0x06    //ȫ���Ӵ��ڸ�λ�Ĵ���
#define WK2XXX_RFTL  0x07   //ȫ�������ڿ��ƼĴ���      
#define WK2XXX_TFTL 0x08    //ȫ���жϼĴ���
#define WK2XXX_FWTH 0x09    //ȫ���жϱ�־�Ĵ���
#define WK2XXX_FWTL 0x0A    //ȫ�� GPIO ����Ĵ���
#define WK2XXX_XON1 0x0B    //ȫ�� GPIO ���ݼĴ���
#define WK2XXX_XOFF1  0x0C  //ȫ�� GPIO ���ݼĴ���
#define WK2XXX_SADR 0x0D    //ȫ�� GPIO ���ݼĴ���
#define WK2XXX_SAEN 0x0E    //ȫ�� GPIO ���ݼĴ���
#define WK2XXX_RTSDLY 0x0F  //ȫ�� GPIO ���ݼĴ���

//�Ӵ���ʱ��ʹ��λ
#define WK2XXX_UT1EN 0x8F  //ʹ���Ӵ��� 1 ��ʱ��
#define WK2XXX_UT2EN 0x4F  //ʹ���Ӵ��� 2 ��ʱ��
#define WK2XXX_UT3EN 0x2F  //ʹ���Ӵ��� 3 ��ʱ��
#define WK2XXX_UT4EN 0x1F  //ʹ���Ӵ��� 4 ��ʱ��

//�Ӵ��������λλ
#define WK2XXX_UT1RST 0x8F  //ʹ���Ӵ��� 1 �������λ
#define WK2XXX_UT2RST 0x4F  //ʹ���Ӵ��� 2 �������λ
#define WK2XXX_UT3RST 0x2F  //ʹ���Ӵ��� 3 �������λ
#define WK2XXX_UT4RST 0x1F  //ʹ���Ӵ��� 4 �������λ

//
#define WK2XXX_GIER 0x8F  //ʹ���Ӵ��� 1 ��ʱ��
#define WK2XXX_GIER 0x4F  //ʹ���Ӵ��� 2 ��ʱ��
#define WK2XXX_GIER 0x2F  //ʹ���Ӵ��� 3 ��ʱ��
#define WK2XXX_GIER 0x1F  //ʹ���Ӵ��� 4 ��ʱ��

#define WK2XXX_TXEN 0x01  //�Ӵ��ڷ���ʹ��λ
#define WK2XXX_RXEN 0x02  //�Ӵ��ڽ���ʹ��λ

#define WK2204_FDAT 0x0D  //�Ӵ��� FIFO ���ݼĴ���
#define WK2204_FSR  0x07  //�Ӵ��� FIFO ״̬�Ĵ���
 
#define WK2XXX_RFTRIG_IEN 0x01 //���� FIFO �����ж�ʹ��λRXOVT_IEN
#define WK2XXX_RXOUT_IEN 0x02 //���� FIFO ��ʱ�ж�ʹ��λ
 
#define MAXLEN 32            //���ջ���
#define FIFO_EXIST (1<<4)     //FIFO������
#define FIFO_NOTEXIST 0      //FIFO������
#define PORT_1 0
#define PORT_2 1
#define PORT_3 2
#define PORT_4 3
#define ERR    999

#define success 1
#define fail    0





extern UART_HandleTypeDef huart3;

/* ���ڴ���ADU����֡�Ľ׶����� */
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




