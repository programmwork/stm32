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

extern unsigned char TxRxIndex;

extern unsigned char UartProcessingPhase;


/********************************************************************************
** 函数名称 ：void Sensor_Init(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************/


void hal_sensor_init(void)
{
  Sensor_Init();

  SampleData_Init(&sensors_data);
}

/********************************************************************************
** 函数名称 ：void Sensor_Init(void)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************/

unsigned char Sensor_Init(void)
{
  AirH_USART3_Init();
  return 1;
}


/*
********************************************************************************
** 函数名称 ：void AirP_rotronic(char *buf,float *result)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：0:数据无效  1:数据有效
********************************************************************************
*/

void AirP_rotronic(char *buf,float *result)
{
    char *p = NULL, i = 0;
    float temp[3];

    p = strtok(buf, ";");
    while(p)
    {
       if(1 == i)  {temp[0] = atof(p);} //濠?鍨
       if(5 == i)  {temp[1] = atof(p);} //濞撯晛瀹?
       if(10 == i) {temp[2] = atof(p);} //闂囪尙鍋?

       p = strtok(NULL, ";");
       i++;
    }

    *result = temp[0];
    *(++result) = temp[2];
    *(++result) = temp[1];
}


/*
********************************************************************************
** 函数名称 ：unsigned char Airp_engine(float *result)
** 函数功能 ：
** 入口参数 ：
**
** 出口参数 ：0:数据无效  1:数据有效
********************************************************************************
*/

unsigned char Airp_engine(float *result)
{ 
    UINT8 buffer[10];
    UINT32 count = 50;


    unsigned char phase = AirP_USART1_GetProcessingPhase();

    if(phase == USART_PROCESSING_IDEL)                                              // 闁告帇鍊栭弻鍥及椤栨碍鍎婄紒灞炬そ濡拷
    {
        TxRxIndex = 0;
        TxRxLength = 0;
        strcpy((char *)buffer,"01:R\r\n");
        RevStep = 1;
        
        uartSendStr(UARTDEV_1, (UINT8 *)&buffer, sizeof(buffer));

        UartProcessingPhase = USART_PROCESSING_SENDING;

        while(count)
        {                
            if(USART_PROCESSING_FINISH == UartProcessingPhase)
            {
                Airp_ResetProcessingPhase();

                if(TxRxLength != 104)  
                {
                    result[0] = INVALID_DATA;
                    result[1] = INVALID_DATA;
                    result[2] = INVALID_DATA;

                }
                else
                {                    
                    AirP_rotronic(TxRxBuffer, result);
                }

                TxRxIndex = 0;
                TxRxLength = 0;

                break;
            }
            
            if(USART_PROCESSING_ERR == UartProcessingPhase)
            {
                TxRxIndex = 0;
                TxRxLength = 0;

                Airp_ResetProcessingPhase();

                result[0] = INVALID_DATA;
                result[1] = INVALID_DATA;
                result[2] = INVALID_DATA;

                break;
            }
            
            count--;

            vTaskDelay(20);                       
        }

        if(count <= 0)
        {
            TxRxIndex = 0;
            TxRxLength = 0;
	
            Airp_ResetProcessingPhase();

            result[0] = INVALID_DATA;
            result[1] = INVALID_DATA;
            result[2] = INVALID_DATA;
        }
    }
    else           
    {
        TxRxIndex = 0;
        TxRxLength = 0;

        Airp_ResetProcessingPhase();

        result[0] = INVALID_DATA;
        result[1] = INVALID_DATA;
        result[2] = INVALID_DATA;
    }
    
    return 1;
}



/*******************************************************************************
** 闁告垼濮ら弳鐔煎触瀹ュ泦锟? 闁挎稒顒皁id Reset_Sensor(void)
** 闁告垼濮ら弳鐔煎礉閻旇鍘? 闁挎冻鎷?
** 闁稿繈鍎辫ぐ娑㈠矗閸屾稒娈? 闁挎冻鎷?
**
** 闁告垵鎼ぐ娑㈠矗閸屾稒娈? 闁挎冻鎷?
*******************************************************************************/
void Reset_Sensor(void)
{
    Sensor_Init();
}

