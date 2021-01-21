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
    if(bcm_info.sensor.ce == 1)
    {   
        td = (uint8_t)(huart3.Instance->RDR);// 读取缓冲区数据
    }
    else
    {
        td = (uint8_t)(huart3.Instance->RDR) & 0x7F;// 读取缓冲区数据
    }
	
    //if(bcm_info.sensor.ce == 1)
    //{
        switch(AirP_RevStep)
        {
            case 1:   // 接收数据状态
            if(td == 0x0D)
            {                
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
                AirP_RevStep = 0;
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
    //}
}

