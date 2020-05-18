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


static unsigned char DiWenChlNumber;							// 第几通道的地温
static unsigned char AD7792ChlNumber;							// AD7792第几通道


// 通道1配置寄存器的值：偏置禁用、单极性编码、4倍增益、内部基准电压、缓冲使能、通道1采样
#define DI_WEN_CHL0_CFG_REG_VAL 		0x1290

// 通道2配置寄存器的值：偏置禁用、单极性编码、4倍增益、内部基准电压、缓冲使能、通道2采样
#define DI_WEN_CHL1_CFG_REG_VAL 		0x1291

// 通道管脚配置
/*#define SWITCH0_BIT   BIT4 //P9.4
#define SWITCH1_BIT   BIT5 //P9.5
#define SWITCH2_BIT   BIT6 //P9.6

#define SWITCH0_FUNC_IO()            P9SEL &= ~SWITCH0_BIT
#define SWITCH0_PIN_OUT()            P9DIR |=  SWITCH0_BIT
#define SWITCH0_LOW()                P9OUT &= ~SWITCH0_BIT
#define SWITCH0_HIGH()               P9OUT |=  SWITCH0_BIT

#define SWITCH1_FUNC_IO()            P9SEL &= ~SWITCH1_BIT
#define SWITCH1_PIN_OUT()            P9DIR |=  SWITCH1_BIT
#define SWITCH1_LOW()                P9OUT &= ~SWITCH1_BIT
#define SWITCH1_HIGH()               P9OUT |=  SWITCH1_BIT

#define SWITCH2_FUNC_IO()            P9SEL &= ~SWITCH2_BIT
#define SWITCH2_PIN_OUT()            P9DIR |=  SWITCH2_BIT
#define SWITCH2_LOW()                P9OUT &= ~SWITCH2_BIT
#define SWITCH2_HIGH()               P9OUT |=  SWITCH2_BIT*?
/*
********************************************************************************
** 函数名称 ：void Sensor_Init(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/ 
unsigned char Sensor_Init(void) 
{		/*
    SWITCH0_FUNC_IO();
    SWITCH0_PIN_OUT();

    SWITCH1_FUNC_IO();
    SWITCH1_PIN_OUT();

    SWITCH2_FUNC_IO();
    SWITCH2_PIN_OUT();*/

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
void USART1_RX(void)
{

}
/*
********************************************************************************
** 函数名称 ：unsigned char EarthTemp_engine(float Result[5]) 
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：0:数据无效  1:数据有效
********************************************************************************
*/ 

unsigned char EarthTemp_engine(float result[MAX_SENSOR_NUM])
{
  static float earthTemp_1[MAX_SENSOR_NUM];        // 存放前四个通道的温度数据
  static unsigned long result_0;
  unsigned char i = 0, ChlNumber = 0;
  unsigned long result_1;
  
  float fp32_1;

  ChlNumber = bcm_info.sensor.SensorNum;

  if(ChlNumber > MAX_SENSOR_NUM)
      ChlNumber = MAX_SENSOR_NUM;
/*
  if(DiWenChlNumber == 0)		    {SWITCH0_LOW();  SWITCH1_LOW();  SWITCH2_LOW();}
  else if(DiWenChlNumber == 1)  	{SWITCH0_HIGH(); SWITCH1_LOW();  SWITCH2_LOW();}
  else if(DiWenChlNumber == 2)  	{SWITCH0_LOW();  SWITCH1_HIGH(); SWITCH2_LOW();}
  else if(DiWenChlNumber == 3)  	{SWITCH0_HIGH(); SWITCH1_HIGH(); SWITCH2_LOW();}
  else if(DiWenChlNumber == 4)  	{SWITCH0_LOW();  SWITCH1_LOW();  SWITCH2_HIGH();}
  else if(DiWenChlNumber == 5)      {SWITCH0_HIGH(); SWITCH1_LOW();  SWITCH2_HIGH();}
  else if(DiWenChlNumber == 6)      {SWITCH0_LOW();  SWITCH1_HIGH();  SWITCH2_HIGH();}
  else if(DiWenChlNumber == 7)      {SWITCH0_HIGH();  SWITCH1_HIGH();  SWITCH2_HIGH();}

  // AD7792转换第1通道的数据
  if(AD7792ChlNumber == 0)																					
  {
    if(AD7792_SingleConversion(DI_WEN_CHL0_CFG_REG_VAL,&result_0))	// 数据有效
      AD7792ChlNumber = 1;   
    else return 0;
  }
  
  
  // AD7792转换第2通道的数据
  if(AD7792_SingleConversion(DI_WEN_CHL1_CFG_REG_VAL,&result_1))	  // 数据有效
  {
    AD7792ChlNumber = 0;		      
    
    // 计算电阻值
    fp32_1 = (((float)result_0) * STD_R_VALUE) / result_1;
    
    // 计算温度值
    fp32_1 = (float)(((-PARA_A+sqrt(PARA_A*PARA_A-4*PARA_B*(1-(fp32_1)/100)))/(2*PARA_B)));
    
    if(DiWenChlNumber < (ChlNumber-1))
    {
      earthTemp_1[DiWenChlNumber] = fp32_1;
      DiWenChlNumber++;
    }
    else 
    {
        for(i = 0;i < ChlNumber;i++)
        {
            if(i < (ChlNumber - 1))
            {
                result[i] = earthTemp_1[i];
            }
            else
            {
                result[i] = fp32_1;
            }
        }

        DiWenChlNumber = 0;
      
      return 1;																						
    }		
  }
  */
  return 0;																													// 数据无效
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


