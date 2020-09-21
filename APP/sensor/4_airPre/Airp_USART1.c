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


char TxRxBuffer[AirP_TX_RX_BUFF_LEN];								// ���շ��ͻ�����
unsigned char  TxRxLength;											// ���շ������ݳ���


unsigned char AirP_TxRxIndex;

unsigned char UartProcessingPhase;
unsigned char AirP_RevStep;

static  unsigned int AirP_T3IntCounter;

unsigned char T3Timer_start = 0;
volatile unsigned char Flag_DataValid;
unsigned long AirpValue;

/*
*********************************************************************************************************
** �������� ��void USART_Init(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ����
*********************************************************************************************************
*/
void AirH_USART3_Init(void)
{
  uart1Init(9600,8,'N',1);

  if(bcm_info.sensor.ce == 0)
  {
    /* T3��ʱ����ʼ�� */
      Init_Timer1(50);  //��ൽ65

    AirP_T3_STOP_COUNTING();
    
    UartProcessingPhase = USART_PROCESSING_IDEL;  // ���ڿ��� 
  }
  else if(bcm_info.sensor.ce == 3)
  {
    Flag_DataValid = 0;
  }
}

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

/*
*********************************************************************************************************
** �������� ��void USART1_SendAudData(void)
** �������� �������жϷ���AduBuffer�е�����
** ��ڲ��� ��
**
** ���ڲ��� ����
*********************************************************************************************************
*/
unsigned char AirP_USART1_SendBytes(void)
{	
	if(TxRxLength > AirP_TX_RX_BUFF_LEN)  return 0; 
    
  if(UartProcessingPhase != USART_PROCESSING_IDEL)	return 0;		// ����æ
		
  USCI_A_UART_disableInterrupt(USCI_A1_BASE, USCI_A_UART_RECEIVE_INTERRUPT);																			// ��ֹ�����ж�
  UCA1IFG &= ~UCTXIFG;                                     // �����жϱ�־λ����
	
  UCA1IFG &= ~UCRXIFG;                                     // �����жϱ�־λ����
	
	TxRxIndex = 1;                                  // �´η���TxRxBuffer[1]
	
	USCI_A_UART_transmitData(USCI_A1_BASE, TxRxBuffer[0]);					// ��ʼ��������

	USCI_A_UART_enableInterrupt(USCI_A1_BASE, USCI_A_UART_TRANSMIT_INTERRUPT);		// ʹ�ܷ����ж�
	
	UartProcessingPhase = USART_PROCESSING_SENDING;// ���ڷ�������
	
	return 1;                                       // �ɹ�
}

/*
*********************************************************************************************************
** �������� ��
** �������� �����ڷ����жϷ������
** ��ڲ��� ��
**
** ���ڲ��� ��
*********************************************************************************************************
*/
void USART3_TX(void)
{
	if(TxRxIndex < TxRxLength)
	{
	    USCI_A_UART_transmitData(USCI_A1_BASE, TxRxBuffer[TxRxIndex++]);
	}
	else														// ��ʼ��������
	{
		USCI_A_UART_disableInterrupt(USCI_A1_BASE, USCI_A_UART_TRANSMIT_INTERRUPT);
		
		UartProcessingPhase = USART_PROCESSING_RECEIVING;		// ���ڽ�������
		
		USCI_A_UART_enableInterrupt(USCI_A1_BASE, USCI_A_UART_RECEIVE_INTERRUPT);    // ʹ�ܽ����ж�
    
    TxRxIndex = 0;
    RevStep = 1;
		
		AirP_T3_START_COUNTING();					// �������ճ�ʱ��ʱ��
	}
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
    *pAirp = AirpValue;
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
	
	td = UCA1RXBUF;										      // ��ȡ����������
	
  if(bcm_info.sensor.ce == 0)
  {
    AirP_T3_RESTART_COUNTING();						// ������ʱ��ʱ��
    
    switch(RevStep)
    {
    case 1:																// ��������״̬
      if(td == 0x0D)	RevStep = 2;				// �л������ջ��з���״̬
      else	
      {
        if(TxRxIndex >= AirP_TX_RX_BUFF_LEN)
        {
          AirP_T3_STOP_COUNTING();
          RevStep = 0;	              // �������κ�����
          //URX1IE = 0;											// ��ֹ�����ж�
          //URX1IF = 0;
          UartProcessingPhase = USART_PROCESSING_ERR;
        }
        else
        {
            TxRxBuffer[TxRxIndex++] = td;
        }
      }
      break;
      
    case 2:																// ���ջ��з���״̬		
      AirP_T3_STOP_COUNTING();
      
      if(td == 0x0A)											// �ɹ�����һ֡����
      {
        TxRxBuffer[TxRxIndex] = 0;				// �����յ�����ת�����ַ��������Ǳ��룩
        TxRxLength = TxRxIndex;						// ����֡���ȣ��������ַ��������յĻس�����
        UartProcessingPhase = USART_PROCESSING_FINISH;	// �Ѿ����յ���ȷ��ADU����֡
      }
      else
      {
        UartProcessingPhase = USART_PROCESSING_ERR;			// ���յ�����֡���󣬴��ڿ���
      }
      
      //URX1IE = 0;													    // ��ֹ�����ж�
      //URX1IF = 0;
      RevStep = 0;												// ��λ״̬��
      break;
      
    default : break;
    }	  
  }
  else if(bcm_info.sensor.ce == 3)            // ̩�ƴ�����
  {
    static unsigned char RevStep = 0;
    static unsigned char revBuffer[6];
        
    if(td == 'B')	
    {
      RevStep = 1;	
      return;
    }
    else if(RevStep == 0) return;
    
    
    switch(RevStep)
    {
    case 1:																// ����'G'
      if(td == 'G')	RevStep = 2;	
      else RevStep = 0;
      break;
      
    case 2:																// 
      if(td == ',')	RevStep++;	
      else RevStep = 0;
      break;
      
    case 3:																// ����003
      if(td == '0')	RevStep++;	
      else RevStep = 0;
      break;
      
    case 4:																
      if(td == '0')	RevStep++;	
      else RevStep = 0;
      break;
      
    case 5:														
      if(td == '1')	RevStep++;	
      else RevStep = 0;
      break;
      
    case 6:																// 
      if(td == ',')	RevStep++;	
      else RevStep = 0;
      break;
      
    case 7:																// 
      if((td >'9') || (td < '0')) 
        RevStep = 0;
      else
      {
        revBuffer[0] = td;
        RevStep++; 	    
      }
      break;
      
    case 8:																// 
      if((td >'9') || (td < '0')) 
        RevStep = 0;
      else
      {
        revBuffer[1] = td;
        RevStep++; 	    
      }
      break;
      
    case 9:																// 
      if((td >'9') || (td < '0')) 
        RevStep = 0;
      else
      {
        revBuffer[2] = td;
        RevStep++; 	    
      }
      break;
      
    case 10:																// 
      if((td >'9') || (td < '0')) 
        RevStep = 0;
      else
      {
        revBuffer[3] = td;
        RevStep++; 	    
      }
      break;
      
    case 11:																// 
      if((td >'9') || (td < '0')) 
        RevStep = 0;
      else
      {
        revBuffer[4] = td;
        revBuffer[5] = 0;
        AirpValue = atol((char *)revBuffer);
        RevStep++; 	    
      }
      break;

    case 12:																// 
      if(td == ',')	RevStep++;	
      else RevStep = 0;
      break;
      
    case 13:																// 
      if(td == 'E')	RevStep++;	
      else RevStep = 0;
      break;
      
    case 14:																// 
      if(td == 'D')	RevStep++;	
      else RevStep = 0;
      break;
      
    case 15:																// 
      if(td == '\r')	RevStep++;	
      else RevStep = 0;
      break;
      
    case 16:																// 
      if(td == '\n')	
      {
        Flag_DataValid = 1;
      }
      
      RevStep = 0;
      break;
      
    default : break;
    }	
  }
}

