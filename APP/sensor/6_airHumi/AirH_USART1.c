/*
;*--------------------------------------------- �ļ���Ϣ ----------------------------------------------------
;* �ļ����� : 
;* �ļ����� : 
;* ����˵�� :
;*-------------------------------------------- ���°汾��Ϣ -------------------------------------------------
;* �޸����� :
;* �޸����� :
;* �汾���� :
;*-------------------------------------------- ��ʷ�汾��Ϣ -------------------------------------------------
;* �ļ����� : liujunjie_2008@126.com
;* �������� :
;* �汾���� :
;************************************************************************************************************
*/
#include "sensor_basic.h"


char AirH_TxRxBuffer[AirH_TX_RX_BUFF_LEN];								// ���շ��ͻ�����
unsigned char AirH_TxRxLength;											// ���շ������ݳ���
unsigned char AirH_TxRxIndex;

unsigned char UartProcessingPhase;
unsigned char RevStep;

volatile unsigned char Flag_DataValid;
unsigned long AirHValue;

/**********************************************************************************************************
** �������� ��void AirH_Init(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ����
**********************************************************************************************************/
void AirH_Init(void)
{
    UART_Init(UARTDEV_3, 9600, 8, 'N', 1, 1);

    UartProcessingPhase = USART_PROCESSING_IDEL;  
}

/**********************************************************************************************************
** �������� ��unsigned char AirH_USART3_GetProcessingPhase(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ����
**********************************************************************************************************/
unsigned char AirH_USART3_GetProcessingPhase(void)
{
	return UartProcessingPhase;
}

/**********************************************************************************************************
** �������� ��void AirH_USART3_ResetProcessingPhase(void)
** �������� ����λ���ڴ���״̬��
** ��ڲ��� ��
**
** ���ڲ��� ����
**********************************************************************************************************/
void AirH_USART3_ResetProcessingPhase(void)
{
  	RevStep = 0;
	UartProcessingPhase = USART_PROCESSING_IDEL;
}


/**********************************************************************************************************
** �������� ��
** �������� �����ڽ����жϷ������
** ��ڲ��� ��
**
** ���ڲ��� ��
**********************************************************************************************************/
void USART3_RX(void)
{ 	
    unsigned char td;

    //URX1IF = 0;
    td = UCA1RXBUF;										      // ��ȡ����������

    switch(RevStep)
    {
        case 1:																// ��������״̬
            if(td == 0x0D)
            {
                TxRxBuffer[TxRxIndex] = 0;              // �����յ�����ת�����ַ��������Ǳ��룩
                TxRxLength = TxRxIndex;                     // ����֡���ȣ��������ַ��������յĻس�����
                //UartProcessingPhase = USART_PROCESSING_FINISH; // �Ѿ����յ���ȷ��ADU����֡

                RevStep = 2;
            }
            else
            {
                if(TxRxIndex >= AirH_TX_RX_BUFF_LEN)
                {
                    RevStep = 0;	              // �������κ�����
                    
                    UartProcessingPhase = USART_PROCESSING_ERR;
                }
                else
                {
                    TxRxBuffer[TxRxIndex++] = td;
                }
            }
            break;
        case 2:
            if(td == 0x0A)
            {
                RevStep = 1;
                UartProcessingPhase = USART_PROCESSING_FINISH;
            }
            else
            {
                UartProcessingPhase = USART_PROCESSING_ERR;
                RevStep = 0;
            }
        default : break;
        }   

}

