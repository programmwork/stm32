/*
;*--------------------------------------------- 文件信息 ----------------------------------------------------
;* 文件名称 : 
;* 文件功能 : 
;* 补充说明 :
;*-------------------------------------------- 最新版本信息 -------------------------------------------------
;* 修改作者 :
;* 修改日期 :
;* 版本声明 :
;*-------------------------------------------- 历史版本信息 -------------------------------------------------
;* 文件作者 : liujunjie_2008@126.com
;* 创建日期 :
;* 版本声明 :
;************************************************************************************************************
*/
#include "sensor_basic.h"


char TxRxBuffer[TX_RX_BUFF_LEN];								// 接收发送缓冲区
unsigned char  TxRxLength;											// 接收发送数据长度
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
** 函数名称 ：void USART_Init(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：无
*********************************************************************************************************
*/
/*void AirH_USART3_Init(void)
{
  uart1Init(9600,8,'N',1);

  if(bcm_info.sensor.ce == 0)
  {
    //T3定时器初始化 
      Init_Timer1(50);  //最多到65

    AirP_T3_STOP_COUNTING();
    
    UartProcessingPhase = USART_PROCESSING_IDEL;  // 串口空闲 
  }
  else if(bcm_info.sensor.ce == 3)
  {
    Flag_DataValid = 0;
  }
}*/

/*
*********************************************************************************************************
** 函数名称 ：unsigned char USART1_GetProcessingPhase(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：无
*********************************************************************************************************
*/
unsigned char AirP_USART1_GetProcessingPhase(void)
{
	return UartProcessingPhase;
}

/*
*********************************************************************************************************
** 函数名称 ：void USART1_ResetProcessingPhase(void)
** 函数功能 ：复位串口处理状态机
** 入口参数 ：
**
** 出口参数 ：无
*********************************************************************************************************
*/
void AirP_USART1_ResetProcessingPhase(void)
{
    //USCI_A_UART_disableInterrupt(USCI_A1_BASE, USCI_A_UART_RECEIVE_INTERRUPT);				// 禁止接收中断

    //USCI_A_UART_disableInterrupt(USCI_A1_BASE, USCI_A_UART_TRANSMIT_INTERRUPT);         // 禁止发送中断
    //UCA0IFG &= ~UCTXIFG;                     // 发送中断标志位清零
  
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
** 函数名称 ：
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：
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
** 函数名称 ：
** 函数功能 ：串口接收中断服务程序
** 入口参数 ：
**
** 出口参数 ：
*********************************************************************************************************
*/
void USART3_RX(void)
{ 	
	unsigned char td;
	
	//URX1IF = 0;
	
	td = (uint8_t)(huart3.Instance->RDR);										      // 读取缓冲区数据
	
  if(bcm_info.sensor.ce == 0)
  {
    AirP_T3_RESTART_COUNTING();						// 重启超时定时器

    switch(RevStep)
    {
        case 1:                                                             // 接收数据状态
            if(td == 0x0D)
            {
                TxRxBuffer[TxRxIndex] = 0;              // 将接收的数据转换成字符串（不是必须）
                
                RevStep = 2;
            }
            else
            {
                if(TxRxIndex >= TX_RX_BUFF_LEN)
                {
                    RevStep = 0;                  // 不接收任何数据
                    
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
                TxRxLength = TxRxIndex;                        // 数据帧长度，不包括字符串最后接收的回车换行
                UartProcessingPhase = USART_PROCESSING_FINISH; // 已经接收到正确的ADU数据帧
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

