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


char AirP_TxRxBuffer[AirP_TX_RX_BUFF_LEN];								// 接收发送缓冲区
unsigned char AirP_TxRxLength;											// 接收发送数据长度
unsigned char AirP_TxRxIndex;

unsigned char AirP_UartProcessingPhase;
unsigned char AirP_RevStep;

static  unsigned int AirP_T3IntCounter;

unsigned char T3Timer_start = 0;
volatile unsigned char Flag_DataValid;
unsigned long AirpValue;

void uart1Init(){}

/**********************************************************************************************************
** 函数名称 ：void USART_Init(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：无
**********************************************************************************************************/
void AirP_USART1_Init(void)
{
    uart1Init(19200,8,'N',1);
    
    if(bcm_info.sensor.ce == 0)
    {
        /* T3定时器初始化 */
        Init_Timer1(50);  //最多到65
        AirP_T3_STOP_COUNTING();
        AirP_UartProcessingPhase = AirP_USART1_PROCESSING_IDEL;  // 串口空闲
    }
    else if(bcm_info.sensor.ce == 3)
    {
        Flag_DataValid = 0;
    }
}

/**********************************************************************************************************
** 函数名称 ：unsigned char USART1_GetProcessingPhase(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：无
**********************************************************************************************************/
unsigned char AirP_USART1_GetProcessingPhase(void)
{
	return AirP_UartProcessingPhase;
}

/**********************************************************************************************************
** 函数名称 ：void USART1_ResetProcessingPhase(void)
** 函数功能 ：复位串口处理状态机
** 入口参数 ：
**
** 出口参数 ：无
**********************************************************************************************************/
void AirP_USART1_ResetProcessingPhase(void)
{
    //USCI_A_UART_disableInterrupt(USCI_A1_BASE, USCI_A_UART_RECEIVE_INTERRUPT);		// 禁止接收中断
    //USCI_A_UART_disableInterrupt(USCI_A1_BASE, USCI_A_UART_TRANSMIT_INTERRUPT);       // 禁止发送中断
    //UCA0IFG &= ~UCTXIFG;                     // 发送中断标志位清零
	AirP_RevStep = 0;
	AirP_UartProcessingPhase = AirP_USART1_PROCESSING_IDEL;
}

/**********************************************************************************************************
** 函数名称 ：void USART1_SendAudData(void)
** 函数功能 ：利用中断发送AduBuffer中的数据
** 入口参数 ：
**
** 出口参数 ：无
**********************************************************************************************************/
unsigned char AirP_USART1_SendBytes(void)
{	
    /*if(AirP_TxRxLength > AirP_TX_RX_BUFF_LEN)  return 0;

    if(AirP_UartProcessingPhase != AirP_USART1_PROCESSING_IDEL)	return 0;		// 串口忙

    USCI_A_UART_disableInterrupt(USCI_A1_BASE, USCI_A_UART_RECEIVE_INTERRUPT);	// 禁止接收中断
    UCA1IFG &= ~UCTXIFG;                                     // 接收中断标志位清零

    UCA1IFG &= ~UCRXIFG;                                     // 发送中断标志位清零

    AirP_TxRxIndex = 1;                                  // 下次发送TxRxBuffer[1]

    USCI_A_UART_transmitData(USCI_A1_BASE, AirP_TxRxBuffer[0]);					// 开始发送数据

    USCI_A_UART_enableInterrupt(USCI_A1_BASE, USCI_A_UART_TRANSMIT_INTERRUPT);		// 使能发送中断

    AirP_UartProcessingPhase = AirP_USART1_PROCESSING_SENDING;// 正在发送数据
    
    return 1;   */                                    // 成功
}

/**********************************************************************************************************
** 函数名称 ：
** 函数功能 ：串口发送中断服务程序
** 入口参数 ：
**
** 出口参数 ：
**********************************************************************************************************/
void USART1_TX(void)
{/*
	if(AirP_TxRxIndex < AirP_TxRxLength)
	{
	    USCI_A_UART_transmitData(USCI_A1_BASE, AirP_TxRxBuffer[AirP_TxRxIndex++]);
	}
	else														// 开始接收数据
	{
		USCI_A_UART_disableInterrupt(USCI_A1_BASE, USCI_A_UART_TRANSMIT_INTERRUPT);
		
		AirP_UartProcessingPhase = AirP_USART1_PROCESSING_RECEIVING;		// 正在接收数据
		
		USCI_A_UART_enableInterrupt(USCI_A1_BASE, USCI_A_UART_RECEIVE_INTERRUPT);    // 使能接收中断
    
        AirP_TxRxIndex = 0;
        AirP_RevStep = 1;
		
		AirP_T3_START_COUNTING();					// 启动接收超时定时器
	}*/
}

/**********************************************************************************************************
** 函数名称 ：
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：
**********************************************************************************************************/
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

/**********************************************************************************************************
** 函数名称 ：
** 函数功能 ：串口接收中断服务程序
** 入口参数 ：
**
** 出口参数 ：
**********************************************************************************************************/
void USART1_RX(void)
{ 	
    unsigned char td;

    //URX1IF = 0;
    td = UARTDEV_1;										      // 读取缓冲区数据
    
    if(bcm_info.sensor.ce == 0)
    {
        AirP_T3_RESTART_COUNTING();						// 重启超时定时器

        switch(AirP_RevStep)
        {
        case 1:																// 接收数据状态
            if(td == 0x0D)
            {
                AirP_T3_STOP_COUNTING();

                AirP_TxRxBuffer[AirP_TxRxIndex] = 0;              // 将接收的数据转换成字符串（不是必须）
                AirP_TxRxLength = AirP_TxRxIndex;                     // 数据帧长度，不包括字符串最后接收的回车换行
                AirP_UartProcessingPhase = AirP_USART1_PROCESSING_FINISH; // 已经接收到正确的ADU数据帧

                AirP_RevStep = 0;
            }
            else
            {
                if(AirP_TxRxIndex >= AirP_TX_RX_BUFF_LEN)
                {
                    AirP_T3_STOP_COUNTING();
                    AirP_RevStep = 0;	              // 不接收任何数据
                    //URX1IE = 0;											// 禁止接收中断
                    //URX1IF = 0;
                    AirP_UartProcessingPhase = AirP_USART1_PROCESSING_ERR;
                }
                else
                {
                    AirP_TxRxBuffer[AirP_TxRxIndex++] = td;
                }
            }
            break;

        default : break;
        }
    }
    else if(bcm_info.sensor.ce == 3)            // 泰科传感器
    {
        static unsigned char RevStep = 0;
        static unsigned char revBuffer[6];

        if(td == 'B')
        {
            RevStep = 1;	  return;
        }
        else if(RevStep == 0) return;

        switch(RevStep)
        {
            case 1:																// 接收'G'
            if(td == 'G')	RevStep = 2;
            else RevStep = 0;
            break;

            case 2:																//
            if(td == ',')	RevStep++;
            else RevStep = 0;
            break;

            case 3:																// 接收003
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

