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

    unsigned char buffer[128] = {0x66, 0x6F, 0x72, 0x6D, 0x20, 0x35, 0x2E, 0x32, 0x20, 0x52,\
                                     0x48, 0x20,0x22, 0x3B, 0x22, 0x20, 0x54, 0x61, 0x20, 0x22, \
                                     0x3B, 0x22, 0x20, 0x54, 0x64, 0x20, 0x22, 0x3B, 0x22, 0x20,\
                                     0x23, 0x72, 0x23, 0x6E, 0x0D, 0x0A};
    if(bcm_info.sensor.ce_H == 1)
    {        
        UART_Init(3, 19200, 8, 0, 1, 1);
        
        AirH_UartProcessingPhase = AIRH_USART_PROCESSING_IDEL;
    }
    else if(bcm_info.sensor.ce_H == 2)
    {
        UART_Init(3, 4800, 8, 1, 1, 1);//4800, 7, E, 1
        //strcpy((char *)buffer,'form 5.2 RH ";" Ta ";" Td ";" #r#n\r\n');
        uartSendStr(UARTDEV_3, (UINT8 *)&buffer, strlen((char *)buffer));
        uartSendStr(UARTDEV_3, (UINT8 *)&buffer, strlen((char *)buffer));
        uartSendStr(UARTDEV_3, (UINT8 *)&buffer, strlen((char *)buffer));
        AirH_UartProcessingPhase = AIRH_USART_PROCESSING_IDEL;  // ���ڿ���
    }

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


    if(bcm_info.sensor.ce_H == 1)
    {
        switch(AirH_RevStep)
        {
            case 1:																// ��������״̬
                if(td == 0x0D)
                {
                    AirH_TxRxBuffer[AirH_TxRxIndex] = 0;    // �����յ�����ת�����ַ��������Ǳ��룩
                    AirH_TxRxLength = AirH_TxRxIndex;      //����֡���ȣ��������ַ��������յĻس����� 

                    AirH_UartProcessingPhase = AIRH_USART_PROCESSING_FINISH; // �Ѿ����յ���ȷ��ADU����֡

                    AirH_RevStep = 0;
                }
                else
                {
                    if(AirH_TxRxIndex >= AIRH_TX_RX_BUFF_LEN)
                    {
                        AirH_RevStep = 0;	              // �������κ�����
                        
                        AirH_UartProcessingPhase = AIRH_USART_PROCESSING_ERR;
                    }
                    else
                    {
                        AirH_TxRxBuffer[AirH_TxRxIndex++] = td;
                    }
                }break;
            
            default: break;
        }   
    }
    else if(bcm_info.sensor.ce_H == 2)
    {
        td = td & 0x7F;
    
        switch(AirH_RevStep)
        {
            case 1:		// ��������״̬
            {
                if(td == 0x0D)  AirH_RevStep = 2;               // �л������ջ��з���״̬
                else
                {
                    if(AirH_TxRxIndex >= AIRH_TX_RX_BUFF_LEN)
                    {
                        AirH_RevStep = 0;                 // �������κ�����
                        AirH_UartProcessingPhase = AIRH_USART_PROCESSING_ERR;
                    }
                    else
                    {
                        AirH_TxRxBuffer[AirH_TxRxIndex++] = td;
                    }
                }
            }break;                

            case 2:	// ���ջ��з���״̬
            {
                if(td == 0x0A)                                          // �ɹ�����һ֡����
                {
                    AirH_TxRxBuffer[AirH_TxRxIndex] = 0;                // �����յ�����ת�����ַ��������Ǳ��룩
                    AirH_TxRxLength = AirH_TxRxIndex;                       // ����֡���ȣ��������ַ��������յĻس�����
                    AirH_UartProcessingPhase = AIRH_USART_PROCESSING_FINISH;    // �Ѿ����յ���ȷ��ADU����֡
                }
                else
                {
                    AirH_UartProcessingPhase = AIRH_USART_PROCESSING_ERR;           // ���յ�����֡���󣬴��ڿ���
                }
                  
                AirH_RevStep = 0;                                             // ��λ״̬��
            }break;

            default: break;
        }
    }
}

