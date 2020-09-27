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
#define STD_R_VALUE 	100.0       		            // 标准电阻值

TIM_HandleTypeDef htim1;


static unsigned char DiWenChlNumber;							// 第几通道的地温
//static unsigned char AD7792ChlNumber;							// AD7792第几通道


// 通道1配置寄存器的值：偏置禁用、单极性编码、4倍增益、内部基准电压、缓冲使能、通道1采样
//0x1290

// 通道2配置寄存器的值：偏置禁用、单极性编码、4倍增益、内部基准电压、缓冲使能、通道2采样
//0x1291

// 通道管脚配置
#define SWITCH0_LOW()                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
#define SWITCH0_HIGH()               HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

#define SWITCH1_LOW()                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
#define SWITCH1_HIGH()               HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

#define SWITCH2_LOW()                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
#define SWITCH2_HIGH()               HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
/********************************************************************************
** 函数名称 ：void Sensor_Init(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/ 
unsigned char Sensor_Init(void) 
{	
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

    /*Configure GPIO pin : PA4 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
    
    AD7792_Init();
    return 1;																						// 初始化成功
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
** 函数名称 ：unsigned char infraredTemp_engine(float Result[5]) 
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：0:数据无效  1:数据有效
********************************************************************************
*/ 
unsigned char EarthTemp_engine(float result[MAX_SENSOR_NUM])
{
    unsigned long result_0;
    unsigned char i = 0, ChlNumber = 0;
    unsigned long result_1;
    uint8 count = 50, readAD[2], readSTAT = 0;
  
    float fp32_1;

    ChlNumber = bcm_info.sensor.SensorNum;

    if(ChlNumber > MAX_SENSOR_NUM)
        ChlNumber = MAX_SENSOR_NUM;

    for(i = 0;i < ChlNumber; i++)
    {
        if(i == 0)		     {SWITCH0_LOW();  SWITCH1_LOW();  SWITCH2_LOW();}
        else if(i == 1)  	 {SWITCH0_HIGH(); SWITCH1_LOW();  SWITCH2_LOW();}
        else if(i == 2)   	 {SWITCH0_LOW();  SWITCH1_HIGH(); SWITCH2_LOW();}
        else if(i == 3)   	 {SWITCH0_HIGH(); SWITCH1_HIGH(); SWITCH2_LOW();}
        else if(i == 4)      {SWITCH0_LOW();  SWITCH1_LOW();  SWITCH2_HIGH();}
        else if(i == 5)      {SWITCH0_HIGH(); SWITCH1_LOW();  SWITCH2_HIGH();}
        else if(i == 6)      {SWITCH0_LOW();  SWITCH1_HIGH(); SWITCH2_HIGH();}
        else if(i == 7)      {SWITCH0_HIGH(); SWITCH1_HIGH(); SWITCH2_HIGH();}

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
                          |AD7792_MODE_RATE_19
                          );
            vTaskDelay( 100 );
            if(Check_RDY(0, 10))
            {            
                AD7792_Red_Reg( AD7792_REG_DATA, readAD, 2 );
                
                //通道1的采样值
                result_0 = (readAD[0] << 8) + readAD[1];
                
                if(Check_RDY(AD7792_STAT_NRDY, 10))
                {
                    // AD7792转换第1通道的数据
                    AD7792_Set_Cfg( AD7792_CFG_VBIAS_DIS
                              |AD7792_CFG_POR_U
                              |AD7792_CFG_GAIN_4
                              |AD7792_CFG_REF_EXT
                              |AD7792_CFG_BUFFER
                              |AD7792_CFG_SEL_CH1
                              );

                    AD7792_Set_Mode(AD7792_MODE_CONV_ONCE
                              |AD7792_MODE_CLK_INT64
                              |AD7792_MODE_RATE_19
                              );

                    vTaskDelay( 100 );

                    if(Check_RDY(0, 10))
                    {
                        AD7792_Red_Reg( AD7792_REG_DATA, readAD, 2 );

                        //通道2的采样值
                        result_1 = (readAD[0] << 8) + readAD[1];

                         // 计算电阻值
                        fp32_1 = (((float)result_0) * STD_R_VALUE) / result_1;
                        
                        // 计算温度值
                        fp32_1 = (float)(((-PARA_A+sqrt(PARA_A*PARA_A-4*PARA_B*(1-(fp32_1)/100)))/(2*PARA_B)));

                        result[i] = fp32_1;
                    }
                }
            }
        }        
        result[i] = INVALID_DATA;
    }
    return 1;																												// 数据无效
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


