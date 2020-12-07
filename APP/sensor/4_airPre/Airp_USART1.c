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

static  unsigned int T3IntCounter;

unsigned char T3Timer_start = 0;
volatile unsigned char Flag_DataValid;
unsigned long Value;
extern UART_HandleTypeDef huart3;

void Airp_Init(void)
{
    UART_Init(3, 9600, 8, 'N', 1, 1);

    UartProcessingPhase = USART_PROCESSING_IDEL;  
}

/*
*********************************************************************************************************
** �������� ��void USART_Init(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ����
*********************************************************************************************************
*/
/*void AirH_USART3_Init(void)
{
  uart1Init(9600,8,'N',1);

  if(bcm_info.sensor.ce == 0)
  {
    //T3��ʱ����ʼ�� 
      Init_Timer1(50);  //��ൽ65

    AirP_T3_STOP_COUNTING();
    
    UartProcessingPhase = USART_PROCESSING_IDEL;  // ���ڿ��� 
  }
  else if(bcm_info.sensor.ce == 3)
  {
    Flag_DataValid = 0;
  }
}*/

/*
*********************************************************************************************************
** �������� ��unsigned char USART1_GetProcessingPhase(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ����
*********************************************************************************************************
*/
unsigned char AirP_USART1_GetProcessingPhase(void)
{
	return UartProcessingPhase;
}

/*
*********************************************************************************************************
** �������� ��void USART1_ResetProcessingPhase(void)
** �������� ����λ���ڴ���״̬��
** ��ڲ��� ��
**
** ���ڲ��� ����
*********************************************************************************************************
*/
void AirP_USART1_ResetProcessingPhase(void)
{
    //USCI_A_UART_disableInterrupt(USCI_A1_BASE, USCI_A_UART_RECEIVE_INTERRUPT);				// ��ֹ�����ж�

    //USCI_A_UART_disableInterrupt(USCI_A1_BASE, USCI_A_UART_TRANSMIT_INTERRUPT);         // ��ֹ�����ж�
    //UCA0IFG &= ~UCTXIFG;                     // �����жϱ�־λ����
  
	RevStep = 0;
  
  UartProcessingPhase = USART_PROCESSING_IDEL;
}

void AirP_Init(void)
{
    UART_Init(3, 9600, 8, 'N', 1, 1);

    UartProcessingPhase = USART_PROCESSING_IDEL;  
}


/*
*********************************************************************************************************
** �������� ��
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ��
*********************************************************************************************************
*/
unsigned char Airp_USART1_TK_Check(unsigned long *pAirp)
{
  if(Flag_DataValid)
  {
    *pAirp = Value;
    Flag_DataValid = 0;
    return 1;
  }
  
  return 0;
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
    AirP_T3_RESTART_COUNTING();						// ������ʱ��ʱ��

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

