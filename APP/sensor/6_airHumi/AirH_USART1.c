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



char AirH_TxRxBuffer[AIRH_TX_RX_BUFF_LEN];								// ���շ��ͻ�����
unsigned char AirH_TxRxLength;											// ���շ������ݳ���
unsigned char AirH_TxRxIndex;

unsigned char AirH_UartProcessingPhase;
unsigned char AirH_RevStep;

extern UART_HandleTypeDef huart3;

/**********************************************************************************************************
** �������� ��void AirH_Init(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ����
**********************************************************************************************************/
void AirH_Init(void)
{
    UART_Init(3, 19200, 8, 'N', 1, 1);

    AirH_UartProcessingPhase = AIRH_USART_PROCESSING_IDEL;  
}

/**********************************************************************************************************
** �������� ��unsigned char AirH_GetProcessingPhase(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ����
**********************************************************************************************************/
unsigned char AirH_GetProcessingPhase(void)
{
	return AirH_UartProcessingPhase;
}

/**********************************************************************************************************
** �������� ��void AirH_ResetProcessingPhase(void)
** �������� ����λ���ڴ���״̬��
** ��ڲ��� ��
**
** ���ڲ��� ����
**********************************************************************************************************/
void AirH_ResetProcessingPhase(void)
{
  	AirH_RevStep = 1;
	AirH_UartProcessingPhase = AIRH_USART_PROCESSING_IDEL;
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

    td = (uint8_t)(huart3.Instance->RDR);// ��ȡ����������

    switch(AirH_RevStep)
    {
        case 1:																// ��������״̬
            if(td == 0x0D)
            {
                AirH_TxRxBuffer[AirH_TxRxIndex] = 0;    // �����յ�����ת�����ַ��������Ǳ��룩
                AirH_TxRxLength = AirH_TxRxIndex;      //����֡���ȣ��������ַ��������յĻس����� 

                AirH_UartProcessingPhase = AIRP_USART_PROCESSING_FINISH; // �Ѿ����յ���ȷ��ADU����֡

                AirH_RevStep = 0;
            }
            else
            {
                if(AirH_TxRxIndex >= AIRH_TX_RX_BUFF_LEN)
                {
                    AirH_RevStep = 0;	              // �������κ�����
                    
                    AirH_UartProcessingPhase = AIRP_USART_PROCESSING_ERR;
                }
                else
                {
                    AirH_TxRxBuffer[AirH_TxRxIndex++] = td;
                }
            }
            break;
        
        default : break;
    }   

}

