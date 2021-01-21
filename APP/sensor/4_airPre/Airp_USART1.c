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
    unsigned char buffer[128] = {0x66, 0x6F, 0x72, 0x6D, 0x20, 0x34, 0x2E, 0x31, 0x20, 0x50,\
                                 0x20, 0x22, 0x2C, 0x22, 0x20, 0x33, 0x2E, 0x31, 0x20, 0x54,\
                                 0x20, 0x22, 0x2C, 0x22, 0x20, 0x35, 0x2E, 0x30, 0x20, 0x53,\
                                 0x54, 0x41, 0x54, 0x20, 0x23, 0x72, 0x23, 0x6E, 0x0D, 0x0A};
    unsigned char buffer_echo[16] = {0x65, 0x63, 0x68, 0x6F, 0x20, 0x6F, 0x66, 0x66, 0x0D, 0x0A};
    unsigned int count = 100000;
    
    if(bcm_info.sensor.ce == 1)
    {
        UART_Init(3, 9600, 8, 0, 1, 1);
        
        AirP_UartProcessingPhase = AIRP_USART_PROCESSING_IDEL;  

    }
    else if(bcm_info.sensor.ce == 2)
    {
        UART_Init(3, 4800, 8, 1, 1, 1);//4800, 7, E, 1

        //strcpy((char *)buffer,'echo off');
        uartSendStr(UARTDEV_3, (UINT8 *)&buffer_echo, strlen((char *)buffer_echo));
        uartSendStr(UARTDEV_3, (UINT8 *)&buffer_echo, strlen((char *)buffer_echo));
        uartSendStr(UARTDEV_3, (UINT8 *)&buffer_echo, strlen((char *)buffer_echo));


        while(count--)
        {
        
        }
        //strcpy((char *)buffer,'form 4.1 P "," 3.1 T "," 5.0 STAT #r#n');
        uartSendStr(UARTDEV_3, (UINT8 *)&buffer, strlen((char *)buffer));

        AirP_UartProcessingPhase = AIRP_USART_PROCESSING_IDEL;
    }

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
    if(bcm_info.sensor.ce == 1)
    {   
        td = (uint8_t)(huart3.Instance->RDR);// ��ȡ����������
    }
    else
    {
        td = (uint8_t)(huart3.Instance->RDR) & 0x7F;// ��ȡ����������
    }
	
    //if(bcm_info.sensor.ce == 1)
    //{
        switch(AirP_RevStep)
        {
            case 1:   // ��������״̬
            if(td == 0x0D)
            {                
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
                AirP_RevStep = 0;
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
    //}
}

