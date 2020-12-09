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


char TxRxBuffer[TX_RX_BUFF_LEN];								// ���շ��ͻ�����
unsigned char  TxRxLength;											// ���շ������ݳ���
unsigned char TxRxIndex;

unsigned char UartProcessingPhase;
unsigned char RevStep;

extern UART_HandleTypeDef huart3;

void AirP_Init(void)
{
    UART_Init(3, 9600, 8, 'N', 1, 1);
    UartProcessingPhase = USART_PROCESSING_IDEL;  
}

/*
*********************************************************************************************************
** �������� ��unsigned char AirP_GetProcessingPhase(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ����
*********************************************************************************************************
*/
unsigned char AirP_GetProcessingPhase(void)
{
	return UartProcessingPhase;
}

/*
*********************************************************************************************************
** �������� ��void ResetProcessingPhase(void)
** �������� ����λ���ڴ���״̬��
** ��ڲ��� ��
**
** ���ڲ��� ����
*********************************************************************************************************
*/
void AirP_ResetProcessingPhase(void)
{

	RevStep = 0;  
    UartProcessingPhase = USART_PROCESSING_IDEL;
}

/*
*********************************************************************************************************
** �������� ��
** �������� �����ڽ����жϷ������
** ��ڲ��� ��
**
** ���ڲ��� ��
*********************************************************************************************************
*/
void USART3_RX(void)
{ 	
	unsigned char td;
	
	//URX1IF = 0;
	
	td = (uint8_t)(huart3.Instance->RDR);										      // ��ȡ����������
	
    if(bcm_info.sensor.ce == 0)
    {
        switch(RevStep)
        {
            case 1:                                                             // ��������״̬
            if(td == 0x0D)
            {
                TxRxBuffer[TxRxIndex] = 0;              // �����յ�����ת�����ַ��������Ǳ��룩
                
                RevStep = 2;
            }
            else
            {
                if(TxRxIndex >= TX_RX_BUFF_LEN)
                {
                    RevStep = 0;                  // �������κ�����
                    
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
                TxRxLength = TxRxIndex;                        // ����֡���ȣ��������ַ��������յĻس�����
                UartProcessingPhase = USART_PROCESSING_FINISH; // �Ѿ����յ���ȷ��ADU����֡
            }
            else
            {
                UartProcessingPhase = USART_PROCESSING_ERR;
                RevStep = 0;
            }
        default : break;
        }   
    }
}

