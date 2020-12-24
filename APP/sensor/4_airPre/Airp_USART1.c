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


char AirP_TxRxBuffer[AIRP_TX_RX_BUFF_LEN];								// ���շ��ͻ�����
unsigned char  AirP_TxRxLength;											// ���շ������ݳ���
unsigned char AirP_TxRxIndex;

unsigned char AirP_UartProcessingPhase;
unsigned char AirP_RevStep;

extern UART_HandleTypeDef huart3;

void AirP_Init(void)
{
    UART_Init(3, 9600, 8, 'N', 1, 1);
    AirP_UartProcessingPhase = AIRP_USART_PROCESSING_IDEL;  
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
	return AirP_UartProcessingPhase;
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

	AirP_RevStep = 0;  
    AirP_UartProcessingPhase = AIRP_USART_PROCESSING_IDEL;
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
        switch(AirP_RevStep)
        {
            case 1:                                                             // ��������״̬
            if(td == 0x0D)
            {
                AirP_TxRxBuffer[AirP_TxRxIndex] = 0;              // �����յ�����ת�����ַ��������Ǳ��룩
                
                AirP_RevStep = 2;
            }
            else
            {
                if(AirP_TxRxIndex >= AIRP_TX_RX_BUFF_LEN)
                {
                    AirP_RevStep = 0;                  // �������κ�����
                    
                    AirP_UartProcessingPhase = AIRP_USART_PROCESSING_ERR;
                }
                else
                {
                    AirP_TxRxBuffer[AirP_TxRxIndex++] = td;
                }
            }
            break;
        case 2:
            if(td == 0x0A)
            {
                AirP_RevStep = 1;
                AirP_TxRxLength = AirP_TxRxIndex;                        // ����֡���ȣ��������ַ��������յĻس�����
                AirP_UartProcessingPhase = AIRP_USART_PROCESSING_FINISH; // �Ѿ����յ���ȷ��ADU����֡
            }
            else
            {
                AirP_UartProcessingPhase = AIRP_USART_PROCESSING_ERR;
                AirP_RevStep = 0;
            }
        default : break;
        }   
    }
}

