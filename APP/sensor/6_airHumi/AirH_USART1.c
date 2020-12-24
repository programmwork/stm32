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



char AirH_TxRxBuffer[AIRH_TX_RX_BUFF_LEN];								// 接收发送缓冲区
unsigned char AirH_TxRxLength;											// 接收发送数据长度
unsigned char AirH_TxRxIndex;

unsigned char AirH_UartProcessingPhase;
unsigned char AirH_RevStep;

extern UART_HandleTypeDef huart3;

/**********************************************************************************************************
** 函数名称 ：void AirH_Init(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：无
**********************************************************************************************************/
void AirH_Init(void)
{
    UART_Init(3, 19200, 8, 'N', 1, 1);

    AirH_UartProcessingPhase = AIRH_USART_PROCESSING_IDEL;  
}

/**********************************************************************************************************
** 函数名称 ：unsigned char AirH_GetProcessingPhase(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：无
**********************************************************************************************************/
unsigned char AirH_GetProcessingPhase(void)
{
	return AirH_UartProcessingPhase;
}

/**********************************************************************************************************
** 函数名称 ：void AirH_ResetProcessingPhase(void)
** 函数功能 ：复位串口处理状态机
** 入口参数 ：
**
** 出口参数 ：无
**********************************************************************************************************/
void AirH_ResetProcessingPhase(void)
{
  	AirH_RevStep = 1;
	AirH_UartProcessingPhase = AIRH_USART_PROCESSING_IDEL;
}


/**********************************************************************************************************
** 函数名称 ：
** 函数功能 ：串口接收中断服务程序
** 入口参数 ：
**
** 出口参数 ：
**********************************************************************************************************/
void USART3_RX(void)
{ 	
    unsigned char td;

    td = (uint8_t)(huart3.Instance->RDR);// 读取缓冲区数据

    switch(AirH_RevStep)
    {
        case 1:																// 接收数据状态
            if(td == 0x0D)
            {
                AirH_TxRxBuffer[AirH_TxRxIndex] = 0;    // 将接收的数据转换成字符串（不是必须）
                AirH_TxRxLength = AirH_TxRxIndex;      //数据帧长度，不包括字符串最后接收的回车换行 

                AirH_UartProcessingPhase = AIRP_USART_PROCESSING_FINISH; // 已经接收到正确的ADU数据帧

                AirH_RevStep = 0;
            }
            else
            {
                if(AirH_TxRxIndex >= AIRH_TX_RX_BUFF_LEN)
                {
                    AirH_RevStep = 0;	              // 不接收任何数据
                    
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

