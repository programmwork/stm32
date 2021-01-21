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
        AirH_UartProcessingPhase = AIRH_USART_PROCESSING_IDEL;  // 串口空闲
    }

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


    if(bcm_info.sensor.ce_H == 1)
    {
        switch(AirH_RevStep)
        {
            case 1:																// 接收数据状态
                if(td == 0x0D)
                {
                    AirH_TxRxBuffer[AirH_TxRxIndex] = 0;    // 将接收的数据转换成字符串（不是必须）
                    AirH_TxRxLength = AirH_TxRxIndex;      //数据帧长度，不包括字符串最后接收的回车换行 

                    AirH_UartProcessingPhase = AIRH_USART_PROCESSING_FINISH; // 已经接收到正确的ADU数据帧

                    AirH_RevStep = 0;
                }
                else
                {
                    if(AirH_TxRxIndex >= AIRH_TX_RX_BUFF_LEN)
                    {
                        AirH_RevStep = 0;	              // 不接收任何数据
                        
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
            case 1:		// 接收数据状态
            {
                if(td == 0x0D)  AirH_RevStep = 2;               // 切换到接收换行符的状态
                else
                {
                    if(AirH_TxRxIndex >= AIRH_TX_RX_BUFF_LEN)
                    {
                        AirH_RevStep = 0;                 // 不接收任何数据
                        AirH_UartProcessingPhase = AIRH_USART_PROCESSING_ERR;
                    }
                    else
                    {
                        AirH_TxRxBuffer[AirH_TxRxIndex++] = td;
                    }
                }
            }break;                

            case 2:	// 接收换行符的状态
            {
                if(td == 0x0A)                                          // 成功接收一帧数据
                {
                    AirH_TxRxBuffer[AirH_TxRxIndex] = 0;                // 将接收的数据转换成字符串（不是必须）
                    AirH_TxRxLength = AirH_TxRxIndex;                       // 数据帧长度，不包括字符串最后接收的回车换行
                    AirH_UartProcessingPhase = AIRH_USART_PROCESSING_FINISH;    // 已经接收到正确的ADU数据帧
                }
                else
                {
                    AirH_UartProcessingPhase = AIRH_USART_PROCESSING_ERR;           // 接收的数据帧错误，串口空闲
                }
                  
                AirH_RevStep = 0;                                             // 复位状态机
            }break;

            default: break;
        }
    }
}

