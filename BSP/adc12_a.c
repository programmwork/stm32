/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//******************************************************************************
//!
//!   ADC12_A_A - Sample A0 Input, AVcc Ref, LED ON if A0 > 0.5*AVcc
//!   MSP430F552x Demo
//!
//!   Description: A single sample is made on A0 with reference to AVcc.
//!   Software sets ADC12SC to start sample and conversion - ADC12SC
//!   automatically cleared at EOC. ADC12_A_A internal oscillator times sample (16x)
//!   and conversion. In Mainloop MSP430 waits in LPM0 to save power until ADC12_A_A
//!   conversion complete, ADC12_A_ISR will force exit from LPM0 in Mainloop on
//!   reti. If A0 > 0.5*AVcc, P1.0 set, else reset.
//!
//!                 MSP430F552x
//!             -----------------
//!         /|\|                 |
//!          | |                 |
//!          --|RST       P6.0/A0|<- Vin
//!            |             P1.0|--> LED
//!            |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - ADC12_A_A peripheral
//! - GPIO Port peripheral
//! - A0
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - ADC12_A_VECTOR
//!
//******************************************************************************

#include "sysinit.h"
#include "main.h"
//#include "driverlib.h"dvvfdvfdv

uint16_t adc_result = 0, adc_getResult_flag = 0;
ADC_HandleTypeDef hadc1;


void Init_ADC(void)
{
    
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc1.Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
       _Error_Handler(__FILE__, __LINE__);
    }
  
}
/**
* @brief ADC MSP Initialization
* This function configures the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct;  //INIT ADC1 CLOCK
	
  if(hadc->Instance==ADC1)
  {
		
		RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    RCC_PeriphCLKInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
		
		if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration    
    PA1     ------> ADC1_IN6 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }

}

/**
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC_CLK_DISABLE();
  
    /**ADC1 GPIO Configuration    
    PA1     ------> ADC1_IN6 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }

}
void ADC_Power_ON(void)
{
/*   BOARDV_POWER_FUNC_IO();
    BOARDV_POWER_OUT();
    BOARDV_POWER_ON();
    delay_ms(100);*/
}
void ADC_Power_OFF(void)
{
//    BOARDV_POWER_OFF();
}
uint8_t Check_ADC(uint16_t *date)
{
    uint16_t i = 0;

    i = 10;
    while(i--)
    {
        if(HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK)
        {
            *date = HAL_ADC_GetValue(&hadc1);
            break;
        }
    }

    adc_getResult_flag = 0;

    if(i > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void Start_ADC(uint8_t channel)
{
    uint32_t adc_channel = 0;
    ADC_ChannelConfTypeDef sConfig = {0};

    switch (channel)
    {
        case 0:
        {
            adc_channel = ADC_CHANNEL_0;
            break;
        }
        
        case 1:
        {
            adc_channel = ADC_CHANNEL_1;
            break;
        }
         
        case 2:
        {
            adc_channel = ADC_CHANNEL_2;
            break;
        }

        case 3:
        {
            adc_channel = ADC_CHANNEL_3;
            break;
        }

        case 4:
        {
            adc_channel = ADC_CHANNEL_4;
            break;
        }

        case 5:
        {
            adc_channel = ADC_CHANNEL_5;
            break;
        }

        case 6:
        {
            adc_channel = ADC_CHANNEL_6;
            break;
        }

        case 7:
        {
            adc_channel = ADC_CHANNEL_7;
            break;
        }

        case 8:
        {
            adc_channel = ADC_CHANNEL_8;
            break;
        }

        case 9:
        {
            adc_channel = ADC_CHANNEL_9;
            break;
        }

        case 10:
        {
            adc_channel = ADC_CHANNEL_10;
            break;
        }

        case 11:
        {
            adc_channel = ADC_CHANNEL_11;
            break;
        }

        case 12:
        {
            adc_channel = ADC_CHANNEL_12;
            break;
        }

        case 13:
        {
            adc_channel = ADC_CHANNEL_13;
            break;
        }

        case 14:
        {
            adc_channel = ADC_CHANNEL_14;
            break;
        }

        case 15:
        {
            adc_channel = ADC_CHANNEL_15;
            break;
        }

        case 16:
        {
            adc_channel = ADC_CHANNEL_16;
            break;
        }

        case 17:
        {
            adc_channel = ADC_CHANNEL_17;
            break;
        }

        case 18:
        {
            adc_channel = ADC_CHANNEL_18;
            break;
        }
        
        default:
            break;
         
    }

    sConfig.Channel = adc_channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    HAL_ADC_Start(&hadc1);
    HAL_ADCEx_Calibration_Start(&hadc1 ,ADC_SINGLE_ENDED);//ADУ׼

    return;
}
//----------------------------------------------------------
//
//
//-----------------------------------------------------------
void Init_Wind_ADC(void)
{
    
}
void ADC_Wind_Power_ON(void)
{
    
}
void ADC_Wind_Power_OFF(void)
{
    
}
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}



