/*
;*--------------------------------------------- 闁哄倸娲ｅ▎銏＄┍閳╁啩绱? ----------------------------------------------------
;* 闁哄倸娲ｅ▎銏ゅ触瀹ュ泦锟? : 1111111
;* 闁哄倸娲ｅ▎銏ゅ礉閻旇鍘? :
;* 閻炴稏鍎遍崢鏍嫚鐎涙ɑ顫? :
;*-------------------------------------------- 闁哄牞鎷烽柡鍌涘婢ф寮甸澶哥箚闁诡叏鎷? -------------------------------------------------
;* 濞ｅ浂鍠楅弫鍏兼媴濠婂棴鎷烽敓锟? :
;* 濞ｅ浂鍠楅弫濂稿籍閵夛附鍩? :
;* 闁绘鐗婂﹢鐗堢珶閻楀牊顫? :
;*-------------------------------------------- 闁告ê妫楄ぐ鍫曟偋閸喐鎷卞ǎ鍥ｅ墲娴硷拷 -------------------------------------------------
;* 闁哄倸娲ｅ▎銏℃媴濠婂棴鎷烽敓锟? : liujunjie_2008@126.com
;* 闁告帗绋戠紓鎾诲籍閵夛附鍩? :
;* 闁绘鐗婂﹢鐗堢珶閻楀牊顫? :
;************************************************************************************************************
*/
#include "sensor_basic.h"

TIM_HandleTypeDef htim1;


/********************************************************************************
** 闁告垼濮ら弳鐔煎触瀹ュ泦锟? 闁挎冻鎷?
** 闁告垼濮ら弳鐔煎礉閻旇鍘? 闁挎冻鎷?
** 闁稿繈鍎辫ぐ娑㈠矗閸屾稒娈? 闁挎冻鎷?
**
** 闁告垵鎼ぐ娑㈠矗閸屾稒娈? 闁挎冻鎷?
********************************************************************************
*/
void hal_sensor_init(void)
{
  Sensor_Init();

  SampleData_Init(&sensors_data);
}
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
** 闁告垼濮ら弳鐔煎触瀹ュ泦锟? 闁挎稒顒痭signed char Sensor_Init(void)
** 闁告垼濮ら弳鐔煎礉閻旇鍘? 闁挎稒纰嶉惃鐢稿储鐎ｂ晝鐐婇柟鎵枎濞呮帡宕氬┑鍡╂綏闁告牭鎷?
** 闁稿繈鍎辫ぐ娑㈠矗閸屾稒娈? 闁挎冻鎷?
**
** 闁告垵鎼ぐ娑㈠矗閸屾稒娈? 闁挎冻鎷?
********************************************************************************
*/
unsigned char Sensor_Init(void)
{
  Airp_Init();
  return 1;
}

/*********************************************************************************
** 闁告垼濮ら弳鐔煎触瀹ュ泦锟? 闁挎稒顒痭signed char Airp_engine(float *result)
** 闁告垼濮ら弳鐔煎礉閻旇鍘? 闁挎稒淇洪浼村矗閺嶃劎姣滈柛妯侯儐閺嗙喖骞戦敓锟?
** 闁稿繈鍎辫ぐ娑㈠矗閸屾稒娈? 闁挎冻鎷?
**
** 闁告垵鎼ぐ娑㈠矗閸屾稒娈? 闁挎冻鎷?0闁挎稒纰嶉婊堝捶閵婎煈鍤㈤柛娆愮墧閼碉拷 1闁挎稒淇洪浼村矗閺嶃劌鐏囬柛鏃撴嫹  2闁挎稒淇洪浼村矗閺嵮冩瘔闂佸尅鎷?
*********************************************************************************/
unsigned char Airp_engine(float *result)
{// 闁告瑦鐗曢敓鍊熷鐎垫牠宕堕敓锟?0-5,0:PTB301,1:PTB210,2:PTB330,3:闁告绨肩花顖氣枖閹殿噮娼?
    uint8 count = 50;
    unsigned char buffer[10];

    //获取当前信号状态
    unsigned char phase = AirP_USART1_GetProcessingPhase();
    if(phase == USART_PROCESSING_IDEL)// 闁告帇鍊栭弻鍥及椤栨碍鍎婄紒灞炬そ濡拷
    {
        if(bcm_info.sensor.ce == 0)//PTB301
        {
            TxRxIndex = 0;
            TxRxLength = 0;
            strcpy((char *)buffer, "01:R\r\n");
            RevStep = 1;
            
            uartSendStr(UARTDEV_3, (UINT8 *)&buffer, sizeof(buffer));

            UartProcessingPhase = USART_PROCESSING_SENDING;
            while(count)
            {                
                if(USART_PROCESSING_FINISH == UartProcessingPhase)
                {
                    AirP_USART1_ResetProcessingPhase();

                    if((TxRxLength != 9) && (TxRxLength != 10)) //长度待定 
                    {
                        result[0] = INVALID_DATA;
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

                    AirP_USART1_ResetProcessingPhase();

                    result[0] = INVALID_DATA;

                    break;
                }
                
                count--;

                vTaskDelay(20);                       
            }
            if(count <= 0)
            {
                TxRxIndex = 0;
                TxRxLength = 0;

                AirP_USART1_ResetProcessingPhase();

                result[0] = INVALID_DATA;
            }
            else           
            {
                TxRxIndex = 0;
                TxRxLength = 0;

                AirP_USART1_ResetProcessingPhase();

                result[0] = INVALID_DATA;
            }
        
            return 1;
        }
        
        else if(bcm_info.sensor.ce == 3)//泰科传感器
        {
            unsigned long Airp;
            if(AirH_USART3_TK_Check(&Airp))  //閺嶏繝鐛欓幋鎰�?
            {
                *result = (float)Airp / 10.0;
                return 1;
            }
        }
    }

}


/*    unsigned char buffer[10];

  if(bcm_info.sensor.ce == 0)//PTB301
  {
    unsigned char phase = AirP_USART1_GetProcessingPhase();
    
    if(phase == USART_PROCESSING_IDEL)												// 闁告帇鍊栭弻鍥及椤栨碍鍎婄紒灞炬そ濡拷
    {
        TxRxIndex = 0;
        TxRxLength = 0;
        strcpy((char *)buffer,"01:R\r\n");
        RevStep = 1;
        uartSendStr(UARTDEV_3, (UINT8 *)&buffer, sizeof("01:R\r\n") - 1);

        UartProcessingPhase = USART_PROCESSING_SENDING;
        AirP_T3_START_COUNTING();

        return 0;																							        // 闊浄鎷?
    }
    
    if(phase == USART_PROCESSING_FINISH)											// 闁告帇鍊栭弻鍥及椤栨碍鍎婇柟鎭掑劜閺佸湱锟界懓鏈崹锟?
    {    
      AirP_USART1_ResetProcessingPhase();
      
      if((TxRxLength != 9) && (TxRxLength != 10))
      {
          TxRxIndex = 0;
          TxRxLength = 0;
          return 2;
      }
      else
      {
          TxRxIndex = 0;
          TxRxLength = 0;
      }
      
      if(memcmp("01:",TxRxBuffer,3)) return 2;		              // 婵絾妫佺欢婵嬪礈瀹ュ嫮鐟忓☉鎿冧簻閻⊙囨嚍閿燂拷
      
      *result = atof(&TxRxBuffer[3]);
      
      if((phase == USART_PROCESSING_IDEL) && (Num_sample < (SAMPLE_COUNT-1)))                                         // 闁告帇鍊栭弻鍥及椤栨碍鍎婄紒灞炬そ濡拷
      {
          TxRxIndex = 0;
          TxRxLength = 0;
          strcpy((char *)buffer,"01:R\r\n");
          RevStep = 1;
          uartSendStr(UARTDEV_3, (UINT8 *)&buffer, sizeof("01:R\r\n") - 1);

          UartProcessingPhase = USART_PROCESSING_SENDING;
          AirP_T3_START_COUNTING();                                                                                                // 闊浄鎷?
      }

      return 1;																							    // 閻犱礁澧介悿鍡涘箣閹邦剙顫?
    }
    
    if(phase == USART_PROCESSING_ERR)											// 闁告帇鍊栭弻鍥及椤栨碍鍎婇柛鎴炴そ閺侊拷
    {
        TxRxIndex = 0;
        TxRxLength = 0;

      AirP_USART1_ResetProcessingPhase();
      return 2;																							// 闁告垶妞介弫锟?
    }
    return 0;		
  }
  else if(bcm_info.sensor.ce == 3)//闁告绨肩花顖氣枖閹殿噮娼?
  {//ljj婵烇綀顕ф慨鐐烘晬瀹?鍐闁搞儳鍋涢敓浠嬫？鐠愮喓鎷犵拠鎻掔悼闁告帗澹嗗▓鎴炲閻樺啿濡抽柛锝冨妼閿熻棄銈稿▍锟?10闁挎稑鑻ぐ鍥ㄧ▔閿熻姤鎷呭鍛瘓闁轰緤鎷?
    unsigned long Airp;
    
    if(Airp_USART1_TK_Check(&Airp))  //闁哄稄绻濋悰娆撳箣閹邦剙顫?
    {
      *result = (float)Airp / 10.0;
      return 1;
    }
    else return 0;
  }
  else
  {
    return 0;
  }
  
  // 闊浄鎷?
}*/

/*******************************************************************************
** 闁告垼濮ら弳鐔煎触瀹ュ泦锟? 闁挎稒顒皁id Reset_Sensor(void)
** 闁告垼濮ら弳鐔煎礉閻旇鍘? 闁挎冻鎷?
** 闁稿繈鍎辫ぐ娑㈠矗閸屾稒娈? 闁挎冻鎷?
**
** 闁告垵鎼ぐ娑㈠矗閸屾稒娈? 闁挎冻鎷?
*******************************************************************************/
void Reset_Sensor(void)
{
    AirP_Init();
}

void Init_Timer1(unsigned short delayMS)
{

    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;

    htim1.Instance = TIM1;//时钟源
    htim1.Init.Prescaler = 799 ;//分频系数
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;//触发模式
    htim1.Init.Period = 20 * delayMS;//重装值
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    HAL_TIM_Base_Start_IT(&htim1);

}


