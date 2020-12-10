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

/**********************************************************************************************/
/***********************************串口1驱动程序**********************************************/
/**********************************************************************************************/
#define	SOILMOISTURE_ADU_BUFF_LEN		64

/* 串口处理ADU数据帧的阶段描述 */
#define	SOILMOISTURE_ADU_PROCESSING_IDEL         0
#define	SOILMOISTURE_ADU_PROCESSING_SENDING      1
#define	SOILMOISTURE_ADU_PROCESSING_RECEIVING    2
#define	SOILMOISTURE_ADU_PROCESSING_FINISH       3
#define	SOILMOISTURE_ADU_PROCESSING_ERR          4

char SOILMOISTURE_AduBuffer[SOILMOISTURE_ADU_BUFF_LEN];               // ADU缓冲区
unsigned char SOILMOISTURE_AduLength;                                 // ADU字节长度

unsigned char SOILMOISTURE_TxRxIndex;
unsigned volatile char  SOILMOISTURE_AduProcessingPhase;      //


#define RS485_BIT //BIT4         //P5.4
#define RS485_IO()          //P5SEL &=~ RS485_BIT
#define RS485_IO_OUT()     // P5DIR |=  RS485_BIT
#define RS485_TX_EN()      // P5OUT |=  RS485_BIT
#define RS485_RX_EN()       //P5OUT &=~ RS485_BIT

TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart3;



/*
*********************************************************************************************************
** 函数名称 ：void USART_Init(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：无
*********************************************************************************************************
*/
void SOILMOISTURE_USART1_Init(void)
{

    UART_Init(3, 9600, 8, 'N', 1, 1);

    RS485_IO();
    RS485_IO_OUT();

    SOILMOISTURE_AduProcessingPhase = SOILMOISTURE_ADU_PROCESSING_IDEL;
}




/*
*********************************************************************************************************
** 函数名称 ：unsigned char USART1_GetProcessingPhase(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：无
*********************************************************************************************************
*/
unsigned char SOILMOISTURE_USART1_GetProcessingPhase(void)
{
  return SOILMOISTURE_AduProcessingPhase;
}

/*
*********************************************************************************************************
** 函数名称 ：void USART1_ProcessingPhaseReset(void)
** 函数功能 ：复位串口处理状态机
** 入口参数 ：
**
** 出口参数 ：无
*********************************************************************************************************
*/
void SOILMOISTURE_USART1_ProcessingPhaseReset(void)
{


    SOILMOISTURE_AduProcessingPhase = SOILMOISTURE_ADU_PROCESSING_IDEL;
}

/*
*********************************************************************************************************
** 函数名称 ：void USART1_SendAudData(void)
** 函数功能 ：利用中断发送SOILMOISTURE_AduBuffer中的数据
** 入口参数 ：
**
** 出口参数 ：无
*********************************************************************************************************
*/
unsigned char SOILMOISTURE_USART1_SendAudData(void)
{
  unsigned char  i, buffer[256] = {0};
  
  if(SOILMOISTURE_AduProcessingPhase != SOILMOISTURE_ADU_PROCESSING_IDEL)  return 0;    // 串口忙

  if((SOILMOISTURE_AduLength < 10) || (SOILMOISTURE_AduLength > 200)) return 0;

  SOILMOISTURE_AduBuffer[SOILMOISTURE_AduLength] = 0x0D;
  SOILMOISTURE_AduBuffer[SOILMOISTURE_AduLength+1] = 0x0A;
  SOILMOISTURE_AduBuffer[SOILMOISTURE_AduLength+2] = 0xFF;
  
  SOILMOISTURE_AduLength = SOILMOISTURE_AduLength + 3;
  
  for(i=0;i<SOILMOISTURE_AduLength;i++)  SOILMOISTURE_AduBuffer[i] |= 0x80;  // 将要发送的Ascii码的最高位置一

  SOILMOISTURE_TxRxIndex = 0;

  buffer[0] = ':' | 0x80;
  memcpy(&buffer[1], &SOILMOISTURE_AduBuffer[0], SOILMOISTURE_AduLength);
  uartSendStr(UARTDEV_3, (UINT8 *)&buffer, SOILMOISTURE_AduLength + 1);

  SOILMOISTURE_AduProcessingPhase = SOILMOISTURE_ADU_PROCESSING_SENDING;    // 正在发送数据
  
  return 1;
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
    static unsigned char revStep;
  
    unsigned char td;
  
    //URX1IF = 0;
  
    td = (uint8_t)(huart3.Instance->RDR);  // 读取缓冲区数据,最高位清零(由7,N,2模式变为8,N,1模式)
    
    if(td == ':')                          // 接收到起始字符":"
    {
        SOILMOISTURE_TxRxIndex = 0;
        revStep = 1;                        // 状态机到 "接收数据状态"
        return;
    }
  
    switch(revStep)
    {
        case 1:                               // 接收数据状态
            if(td == 0x0D)  revStep = 2;        // 切换到接收换行符的状态
            else  
            {
                if(SOILMOISTURE_TxRxIndex >= SOILMOISTURE_ADU_BUFF_LEN)
                {
                    revStep = 0;

                    SOILMOISTURE_AduProcessingPhase = SOILMOISTURE_ADU_PROCESSING_ERR;
                }
                else
                    SOILMOISTURE_AduBuffer[SOILMOISTURE_TxRxIndex++] = td;
            }
        break;

        case 2:                                 // 接收换行符的状态
            if(td == 0x0A)                        // 成功接收一帧数据
            {
                SOILMOISTURE_AduBuffer[SOILMOISTURE_TxRxIndex] = 0;           // 将接收的数据转换成字符（不是必须）
                SOILMOISTURE_AduLength = SOILMOISTURE_TxRxIndex;              // ADU数据帧长度，不包括字符串最后的0
                SOILMOISTURE_AduProcessingPhase = SOILMOISTURE_ADU_PROCESSING_FINISH;    // 已经接收到正确的ADU数据帧
            }
            else
            {
                SOILMOISTURE_AduProcessingPhase = SOILMOISTURE_ADU_PROCESSING_ERR;      // 接收的数据帧错误，串口空闲
            }

            revStep = 0;                          // 复位状态机到等待开始字符":"状态
        break;

        default : break;
    }    
}

/**********************************************************************************************/
/**********************************土壤水分驱动程序********************************************/
/**********************************************************************************************/

static unsigned char SOILMOISTURE_AduBufferBin[25];  

static unsigned char SOILMOISTURE_SensorProcessingPhase;      //

/*
*********************************************************************************************************
** 函数名称 ：unsigned char CalculateLRC( unsigned char* pd, unsigned char len)
** 函数功能 ：计算LRC
** 入口参数 ： 
** 出口参数 ：
*********************************************************************************************************
*/ 
unsigned char SOILMOISTURE_CalculateLRC( unsigned char *pd, unsigned char len)
{
  unsigned char   ucLRC = 0;  
  
  while(len--)
    ucLRC += *pd++;       
  
  ucLRC = (unsigned char) (-((char) ucLRC));
  
  return ucLRC;
}

/*
*********************************************************************************************************
** 函数名称 ：
** 函数功能 ：字符和16进制数转换的相关函数
** 入口参数 ：
**        
** 出口参数 ：
*********************************************************************************************************
*/ 
unsigned char SOILMOISTURE_Char2Bin(char ucCharacter)
{
  if( ( ucCharacter >= '0' ) && ( ucCharacter <= '9' ) )
  {
    return ( unsigned char )( ucCharacter - '0' );
  }
  else if( ( ucCharacter >= 'A' ) && ( ucCharacter <= 'F' ) )
  {
    return ( unsigned char )( ucCharacter - 'A' + 0x0A );
  }
  else
  {
    return 0xFF;
  }
}

char SOILMOISTURE_Bin2Char(unsigned char ucByte)
{
  if( ucByte <= 0x09 )
  {
    return ( unsigned char )( '0' + ucByte );
  }
  else if( ( ucByte >= 0x0A ) && ( ucByte <= 0x0F ) )
  {
    return ( unsigned char )( ucByte - 0x0A + 'A' );
  }
  else
  {
    return 0xFF;
  }    
}

unsigned char SOILMOISTURE_CharArray2BinArray(char *paa,unsigned char *pha,unsigned char aalen)
{
  unsigned char i;
  unsigned char td_1,td_2;
  
  if((aalen & 0x01) || (aalen < 2))  return 0;      // Ascii码数组错误
  
  for(i=0;i<aalen;i+=2)
  {
    td_1 = SOILMOISTURE_Char2Bin(paa[i]);
    if(td_1 == 0xff)  return 0;
    
    td_2 = SOILMOISTURE_Char2Bin(paa[i+1]);
    if(td_2 == 0xff)  return 0;
    
    pha[i>>1] = (td_1<<4) | td_2;  
  }
  return 1;
}

unsigned char SOILMOISTURE_BinArray2CharArray(unsigned char *binA,char *charA,unsigned char len)
{
  unsigned char i,j;
  
  for(i=0;i<len;i++)
  {
    j = i << 1;
    
    charA[j++] = SOILMOISTURE_Bin2Char(binA[i] >> 4);
    
    charA[j] = SOILMOISTURE_Bin2Char(binA[i] & 0x0F);    
  }
  
  return 1;
}

/*
********************************************************************************
** 函数名称 ：
** 函数功能 ：设置传感器(1-5号)有效
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/ 
static unsigned char SOILMOISTURE_TRSF_SetValid(void)
{
    UINT32 count = 50;
    unsigned char phase = 0;
  
    phase = SOILMOISTURE_USART1_GetProcessingPhase();
  
    if(phase == SOILMOISTURE_ADU_PROCESSING_IDEL)                        // 判断是否空闲
    {
        strcpy(SOILMOISTURE_AduBuffer,"01100001000408001F000000000000C3");

        SOILMOISTURE_AduLength = sizeof("01100001000408001F000000000000C3") - 1;

        SOILMOISTURE_USART1_SendAudData(); // 发送ADU数据帧

        while(count)
        {
            if(phase == SOILMOISTURE_ADU_PROCESSING_FINISH)                      // 判断是否接收完成
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();

                if(SOILMOISTURE_AduLength != 14) return 2;
                if(memcmp("011000010004EA",SOILMOISTURE_AduBuffer,14)) return 2;    // 直接比较    // 直接比较

                return 1;                                                           // 设置成功
            }

            if(phase == SOILMOISTURE_ADU_PROCESSING_ERR)                          // 判断是否出错
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();        
                return 2;                                                           // 出错
            }
            
            count--;

            vTaskDelay(20);        
        }

        return 0;                                                             // 忙
    }
}

/*
********************************************************************************
** 函数名称 ：
** 函数功能 ：启动测量
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/ 
static unsigned char SOILMOISTURE_TRSF_StartMeasuring(void)
{
    UINT32 count = 50;

    unsigned char phase = SOILMOISTURE_USART1_GetProcessingPhase();

    if(phase == SOILMOISTURE_ADU_PROCESSING_IDEL)                        // 判断是否空闲
    {
        strcpy(SOILMOISTURE_AduBuffer,"010600000001F8");

        SOILMOISTURE_AduLength = sizeof("010600000001F8") - 1;

        SOILMOISTURE_USART1_SendAudData();     // 发送ADU数据帧

        while(count)
        {

            if(phase == SOILMOISTURE_ADU_PROCESSING_FINISH)                      // 判断是否接收完成
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();

                if(SOILMOISTURE_AduLength != 14) return 2;

                if(memcmp("010600000001F8",SOILMOISTURE_AduBuffer,14)) return 2;    // 直接比较

                return 1;                                              // 设置成功
            }

            if(phase == SOILMOISTURE_ADU_PROCESSING_ERR)   // 判断是否出错
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();        
                return 2;     // 出错
            }
            
            count--;
            
            vTaskDelay(20);         
        }     
    }

    return 0;                                                // 忙
}

/*
********************************************************************************
** 函数名称 ：
** 函数功能 ：检测是否测量完成
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/ 
static unsigned char SOILMOISTURE_TRSF_CheckMeasuringOK(void)
{
    UINT32 count = 50;

    unsigned char phase = SOILMOISTURE_USART1_GetProcessingPhase();

    if(phase == SOILMOISTURE_ADU_PROCESSING_IDEL)                        // 判断是否空闲
    {
        strcpy(SOILMOISTURE_AduBuffer,"010400010001F9");

        SOILMOISTURE_AduLength = sizeof("010400010001F9") - 1;

        SOILMOISTURE_USART1_SendAudData();                                 // 发送ADU数据帧

        while(count)
        {

            if(phase == SOILMOISTURE_ADU_PROCESSING_FINISH)                      // 判断是否接收完成
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();

                if(!memcmp("0104020002F7",SOILMOISTURE_AduBuffer,12)) return 1;    // 转换完成，且没有错误

                if(!memcmp("0104020003F6",SOILMOISTURE_AduBuffer,12)) return 2;    // 转换完成，但有错误

                if(!memcmp("0104020000F9",SOILMOISTURE_AduBuffer,12)) return 3;    // 没有操作

                return 0;
                // if(!memcmp("0104020001F8",SOILMOISTURE_AduBuffer,12)) return 0;    // 正在测量中

                //  return 2;                                                          // 应答错误        
            }

            if(phase == SOILMOISTURE_ADU_PROCESSING_ERR)                         // 判断是否出错
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();        
                return 2;                                                          // 出错
            }
            
            count--;

            vTaskDelay(20);  
        }
    }

    return 0;                                                            // 忙
}


#define PARA_A  0.1957
#define PARA_B  0.404
#define PARA_C  0.02852
/*
********************************************************************************
** 函数名称 ：
** 函数功能 ：读取传感器（1-5号）测量结果结果
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/ 
static unsigned char SOILMOISTURE_TRSF_ReadMeasuringResult(float *pResult)
{
    UINT32 count = 50;

    unsigned char phase = SOILMOISTURE_USART1_GetProcessingPhase();
    union 
    {
        float fp32_1;
        unsigned char int8u_1[4];
    }union_1;


    if(phase == SOILMOISTURE_ADU_PROCESSING_IDEL)                        // 判断是否空闲
     {
        strcpy(SOILMOISTURE_AduBuffer,"01040100000AF0");

        SOILMOISTURE_AduLength = sizeof("01040100000AF0") - 1;

        SOILMOISTURE_USART1_SendAudData();                                  // 发送ADU数据帧

        while(count)
        {
            if(phase == SOILMOISTURE_ADU_PROCESSING_FINISH)                      // 判断是否接收完成
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();

                if(SOILMOISTURE_AduLength != 48) 
                {
                    pResult[0] = INVALID_DATA;
                    pResult[1] = INVALID_DATA;
                    pResult[2] = INVALID_DATA;
                    pResult[3] = INVALID_DATA;
                    pResult[4] = INVALID_DATA;
                    
                    return 2;
                }
                SOILMOISTURE_CharArray2BinArray(SOILMOISTURE_AduBuffer,SOILMOISTURE_AduBufferBin,48);

                if(SOILMOISTURE_CalculateLRC(SOILMOISTURE_AduBufferBin, 24) == 0)  // 数据校验
                {
                    union_1.int8u_1[3] = SOILMOISTURE_AduBufferBin[5];
                    union_1.int8u_1[2] = SOILMOISTURE_AduBufferBin[6];
                    union_1.int8u_1[1] = SOILMOISTURE_AduBufferBin[3];
                    union_1.int8u_1[0] = SOILMOISTURE_AduBufferBin[4];
                    // 将传感器输出的归一化频率转换为体积含水量
                    // if(union_1.fp32_1 < PARA_C) pResult[0] = 0;
                    //   else 
                    //   pResult[0] = pow(((union_1.fp32_1 - PARA_C) / PARA_A), (1 / PARA_B));  

                     pResult[0] = union_1.fp32_1;  

                    union_1.int8u_1[3] = SOILMOISTURE_AduBufferBin[9];
                    union_1.int8u_1[2] = SOILMOISTURE_AduBufferBin[10];
                    union_1.int8u_1[1] = SOILMOISTURE_AduBufferBin[7];
                    union_1.int8u_1[0] = SOILMOISTURE_AduBufferBin[8];
                    // 将传感器输出的归一化频率转换为体积含水量
                    //  if(union_1.fp32_1 < PARA_C) pResult[1] = 0;
                    ///  else 
                    //    pResult[1] = pow(((union_1.fp32_1 - PARA_C) / PARA_A), (1 / PARA_B)); 


                    pResult[1] = union_1.fp32_1;


                    union_1.int8u_1[3] = SOILMOISTURE_AduBufferBin[13];
                    union_1.int8u_1[2] = SOILMOISTURE_AduBufferBin[14];
                    union_1.int8u_1[1] = SOILMOISTURE_AduBufferBin[11];
                    union_1.int8u_1[0] = SOILMOISTURE_AduBufferBin[12];
                    // 将传感器输出的归一化频率转换为体积含水量
                    //  if(union_1.fp32_1 < PARA_C) pResult[2] = 0;
                    // else 
                    //   pResult[2] = pow(((union_1.fp32_1 - PARA_C) / PARA_A), (1 / PARA_B)); 


                    pResult[2] = union_1.fp32_1;

                    union_1.int8u_1[3] = SOILMOISTURE_AduBufferBin[17];
                    union_1.int8u_1[2] = SOILMOISTURE_AduBufferBin[18];
                    union_1.int8u_1[1] = SOILMOISTURE_AduBufferBin[15];
                    union_1.int8u_1[0] = SOILMOISTURE_AduBufferBin[16];
                    // 将传感器输出的归一化频率转换为体积含水量
                    // if(union_1.fp32_1 < PARA_C) pResult[3] = 0;
                    // else 
                    //   pResult[3] = pow(((union_1.fp32_1 - PARA_C) / PARA_A), (1 / PARA_B)); 
                    pResult[3] = union_1.fp32_1;


                    union_1.int8u_1[3] = SOILMOISTURE_AduBufferBin[21];
                    union_1.int8u_1[2] = SOILMOISTURE_AduBufferBin[22];
                    union_1.int8u_1[1] = SOILMOISTURE_AduBufferBin[19];
                    union_1.int8u_1[0] = SOILMOISTURE_AduBufferBin[20];
                    // 将传感器输出的归一化频率转换为体积含水量
                    // if(union_1.fp32_1 < PARA_C) pResult[4] = 0;
                    // else 
                    //   pResult[4] = pow(((union_1.fp32_1 - PARA_C) / PARA_A), (1 / PARA_B)); 
                    pResult[4] = union_1.fp32_1;
                    
                    return 1;                                            // 设置成功
                }
                pResult[0] = INVALID_DATA;
                pResult[1] = INVALID_DATA;
                pResult[2] = INVALID_DATA;
                pResult[3] = INVALID_DATA;
                pResult[4] = INVALID_DATA;

                return 2;                                              // 校验失败
            }

            if(phase == SOILMOISTURE_ADU_PROCESSING_ERR)                          // 判断是否出错
            {                
                SOILMOISTURE_USART1_ProcessingPhaseReset(); 
                
                pResult[0] = INVALID_DATA;
                pResult[1] = INVALID_DATA;
                pResult[2] = INVALID_DATA;
                pResult[3] = INVALID_DATA;
                pResult[4] = INVALID_DATA;
                
                return 2;// 出错
            }
            
            count--;

            vTaskDelay(20); 
        }
    }
    return 0;                                                // 忙
}

/*
********************************************************************************
** 函数名称 ：void TuRangShuiFen_Init(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/ 
unsigned char Sensor_Init(void)
{
    SOILMOISTURE_USART1_Init();                // 串口初始化

    SOILMOISTURE_SensorProcessingPhase = 0;
    return 1;
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
void hal_sensor_init(void)
{
  Sensor_Init();

  SampleData_Init(&sensors_data);
}

/*
********************************************************************************
** 函数名称 ：unsigned char SoilMoisture_engine(float *pResult)
** 函数功能 ：土壤水分传感器巡检函数
** 入口参数 ：Result[5]:指向浮点数组的指针，有效数据将被存放在此指针指向的数组中
**                      返回数据为小数0.x的形式，如0.34代表34%土壤水分
** 出口参数 ：0：忙  1：数据有效 2：出错
********************************************************************************
*/ 
unsigned char SoilMoisture_engine(float Result[5]) 
{
    unsigned char result;
  
    /* 设置传感器有效 */
    if(SOILMOISTURE_SensorProcessingPhase == 0) 
    {
        result = SOILMOISTURE_TRSF_SetValid();
        if(result == 1)                        // 设置成功
        {
            SOILMOISTURE_SensorProcessingPhase = 1;
        }
        else
        {
            Result[0] == -991;
        }    
    }
  
  /* 检查传感器是否空闲 */
    if(SOILMOISTURE_SensorProcessingPhase == 1) 
    {
        result = SOILMOISTURE_TRSF_CheckMeasuringOK();
        if((result == 1) || (result == 3))
        {
          SOILMOISTURE_SensorProcessingPhase = 2;          // 进行下一步骤的操作
        }
        else if(result == 2)
        {
          SOILMOISTURE_SensorProcessingPhase = 0;          // 转换出错，重新开始使能传感器
          Result[0] = -992;
        }
        else 
        {
            Result[0] = -993;
        }  
    }
  
  /* 启动转换 */
    if(SOILMOISTURE_SensorProcessingPhase == 2) 
    {
        result = SOILMOISTURE_TRSF_StartMeasuring();
        if(result == 1)
        {
          SOILMOISTURE_SensorProcessingPhase = 3;          // 进行下一步骤的操作
        }
        else if(result == 2)                               // 应答出错，重新开始使能传感器
        {
          SOILMOISTURE_SensorProcessingPhase = 0;
          Result[0] = -994;
        }
        else 
        {
            Result[0] = -995;
        }  
    }
  
  /* 等待转换完成 */
    if(SOILMOISTURE_SensorProcessingPhase == 3) 
    {
        result = SOILMOISTURE_TRSF_CheckMeasuringOK();
        if(result == 1)
        {
            SOILMOISTURE_SensorProcessingPhase = 4;          // 进行下一步骤的操作
        }
        else if(result == 2)
        {
            SOILMOISTURE_SensorProcessingPhase = 0;          // 转换完成，但出错
            Result[0] = -996;
        }
        else if(result == 3)
        {
            SOILMOISTURE_SensorProcessingPhase = 1;          // 没有操作
            Result[0] = -997;
        }
        else 
        {
            Result[0] = -998;
        }
    }
  
  /* 读取转换结果 */
    if(SOILMOISTURE_SensorProcessingPhase == 4) 
    {
        result = SOILMOISTURE_TRSF_ReadMeasuringResult(Result);
        if(result == 1)
        {
            SOILMOISTURE_SensorProcessingPhase = 1; 
        }
        else if(result == 2)
        {
            SOILMOISTURE_SensorProcessingPhase = 0; 
            Result[0] = -999;
        }
       
        //return result;
    }
  
  return 1;
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

