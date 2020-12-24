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


char AirP_TxRxBuffer[AIRP_TX_RX_BUFF_LEN];								// 接收发送缓冲区
unsigned char  AirP_TxRxLength;											// 接收发送数据长度
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
** 函数名称 ：unsigned char AirP_GetProcessingPhase(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：无
*********************************************************************************************************
*/
unsigned char AirP_GetProcessingPhase(void)
{
	return AirP_UartProcessingPhase;
}

/*
*********************************************************************************************************
** 函数名称 ：void ResetProcessingPhase(void)
** 函数功能 ：复位串口处理状态机
** 入口参数 ：
**
** 出口参数 ：无
*********************************************************************************************************
*/
void AirP_ResetProcessingPhase(void)
{

	AirP_RevStep = 0;  
    AirP_UartProcessingPhase = AIRP_USART_PROCESSING_IDEL;
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
        switch(AirP_RevStep)
        {
            case 1:                                                             // 接收数据状态
            if(td == 0x0D)
            {
                AirP_TxRxBuffer[AirP_TxRxIndex] = 0;              // 将接收的数据转换成字符串（不是必须）
                
                AirP_RevStep = 2;
            }
            else
            {
                if(AirP_TxRxIndex >= AIRP_TX_RX_BUFF_LEN)
                {
                    AirP_RevStep = 0;                  // 不接收任何数据
                    
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
                AirP_TxRxLength = AirP_TxRxIndex;                        // 数据帧长度，不包括字符串最后接收的回车换行
                AirP_UartProcessingPhase = AIRP_USART_PROCESSING_FINISH; // 已经接收到正确的ADU数据帧
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

