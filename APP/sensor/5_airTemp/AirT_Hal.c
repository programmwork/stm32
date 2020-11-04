/*
;************************************************************************************************************
;*
;*
;*                                 	      	 
;*
;*--------------------------------------------- 文件信息 ----------------------------------------------------
;*
;* 文件名称 :
;* 文件功能 :
;*
;* 补充说明 :
;*-------------------------------------------- 最新版本信息 -------------------------------------------------
;* 修改作者 :
;* 修改日期 :
;* 版本声明 :
;*-------------------------------------------- 历史版本信息 -------------------------------------------------
;* 文件作者 : liujunjie_2008@126.com
;* 创建日期 :
;* 版本声明 :
;*-----------------------------------------------------------------------------------------------------------
;*-----------------------------------------------------------------------------------------------------------
;************************************************************************************************************
*/
#include "sensor_basic.h"

#define PARA_A 				3.9083E-3 	   	            // 温度系数
#define PARA_B 				-5.775E-7 
#define STD_R_VALUE 	100.0f       		            // 标准电阻值

TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart3;

// 通道1配置寄存器的值：偏置禁用、单极性编码、4倍增益、内部基准电压、缓冲使能、通道1采样
//0x1290

// 通道2配置寄存器的值：偏置禁用、单极性编码、4倍增益、内部基准电压、缓冲使能、通道2采样
//0x1291


/********************************************************************************
** 函数名称 ：void Sensor_Init(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************/
unsigned char Sensor_Init(void) 
{

    AD7792_Init();
    
    return 1;																						// 初始化成功
}

/*
********************************************************************************
** 函数名称 ：uint8 Check_RDY(uint8 state, uint32 timeout)
** 函数功能 ：
** 入口参数 ：state 1:未完成         0：完成
**
** 出口参数 ：
********************************************************************************
*/ 
uint8 Check_RDY(uint8 state, uint32 timeout)
{
    uint8 readSTAT = 0;

    while(timeout--)
    {        
        
        AD7792_Red_Reg( AD7792_REG_STAT, &readSTAT, 1 );//把寄存器状态写入readSTAT  
        
        if((readSTAT & AD7792_STAT_NRDY) == state)
        {
            return 1;//success
        }
        
        vTaskDelay(1);

    }
    return 0;//error
}


/*
********************************************************************************
** 函数名称 ：
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/
void hal_sensor_init()
{

    Sensor_Init();
    SampleData_Init(&sensors_data);
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
    //可进行接收处理
    if(((m_tempdata.m_uartrcv[UARTDEV_3].WD + 1) % MAX_UARTRCV_LEN) != m_tempdata.m_uartrcv[UARTDEV_3].RD)
    {
         m_tempdata.m_uartrcv[UARTDEV_3].buff[m_tempdata.m_uartrcv[UARTDEV_3].WD]=(uint8_t)HAL_UART_Receive(&huart3 , (uint8_t *)m_tempdata.m_uartrcv[UARTDEV_3].buff, 1, 0xFFFF);;
         m_tempdata.m_uartrcv[UARTDEV_3].WD = (m_tempdata.m_uartrcv[UARTDEV_3].WD + 1) % MAX_UARTRCV_LEN;
    }
}


/*
********************************************************************************
** 函数名称 ：unsigned char AirTemp_engine(float Result[5])
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：0:数据无效  1:数据有效
********************************************************************************
*/
unsigned char AirTemp_engine(float result[MAX_SENSOR_NUM])
{
    uint8 readAD[2];
    unsigned long result_0, result_1;
    float fp32_1;

    if(Check_RDY(AD7792_STAT_NRDY, 10))
    {
        // AD7792转换第0通道的数据
        AD7792_Set_Cfg( AD7792_CFG_VBIAS_DIS
                      |AD7792_CFG_POR_U
                      |AD7792_CFG_GAIN_4
                      |AD7792_CFG_REF_EXT
                      |AD7792_CFG_BUFFER
                      |AD7792_CFG_SEL_CH0
                      );

        AD7792_Set_Mode(AD7792_MODE_CONV_ONCE
                      |AD7792_MODE_CLK_INT64
                      |AD7792_MODE_RATE_17
                      );
        
        vTaskDelay( 100 );

        if(Check_RDY(0, 50)) //0：转换完成
        {
            
            AD7792_Red_Reg( AD7792_REG_DATA, readAD, 2 );
            
            //通道1的采样值
            result_0 = (readAD[0] << 8) + readAD[1];


            //加检测是否读取成功了，用封号的函数
            //判断成功以后才开始 AD7792转换第1通道的数据
            if(Check_RDY(AD7792_STAT_NRDY, 10))
            {
                AD7792_Set_Cfg( AD7792_CFG_VBIAS_DIS
                              |AD7792_CFG_POR_U
                              |AD7792_CFG_GAIN_4
                              |AD7792_CFG_REF_EXT
                              |AD7792_CFG_BUFFER
                              |AD7792_CFG_SEL_CH1
                              );

                AD7792_Set_Mode(AD7792_MODE_CONV_ONCE
                              |AD7792_MODE_CLK_INT64
                              |AD7792_MODE_RATE_17
                              );

                vTaskDelay( 100 );

                if(Check_RDY(0, 50))
                {
                    AD7792_Red_Reg( AD7792_REG_DATA, readAD, 2 );

                    //通道2的采样值
                    result_1 = (readAD[0] << 8) + readAD[1];       
                

                    // 计算电阻值
                    fp32_1 = (((float)result_0) * STD_R_VALUE) / result_1;

                    // 计算温度值
                    fp32_1 = (float)(((-PARA_A+sqrt(PARA_A*PARA_A-4*PARA_B*(1-(fp32_1)/100)))/(2*PARA_B)));

                    result[0] = fp32_1;

                    return 1;                
                }        
            }
        }
    }
        
    result[0] = INVALID_DATA;
    return 0;
}																											

/*
********************************************************************************
** 函数名称 ：void Reset_Sensor(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/ 
void Reset_Sensor(void)
{
  Sensor_Init();
}



