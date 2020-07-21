
#include "stm32l4xx_hal.h"
#include "main.h"

#include "i2c.h"

#include "uart.h"

#define SLAVE_ADDRESS 0x48

I2C_HandleTypeDef I2cHandle1;
I2C_HandleTypeDef I2cHandle2;
I2C_HandleTypeDef I2cHandle1;
/*
From device to device, the I2C communication protocol may vary,
in the example below, the protocol uses repeated start to read data from or
write to the device:
For master read: the sequence is: STA,Addr(W),offset,RE-STA,Addr(r),data...STO
for master write: the sequence is: STA,Addr(W),offset,RE-STA,Addr(w),data...STO
Thus, in state 8, the address is always WRITE. in state 10, the address could
be READ or WRITE depending on the I2C command.
*/

static void I2C_MspInit(I2C_HandleTypeDef *hi2c);
/*****************************************************************************
** Function name:		I2CInit
**
** Descriptions:		Initialize I2C controller
**
** parameters:			I2c mode is either MASTER or SLAVE
** Returned value:		true or false, return false if the I2C
**				interrupt handler was not installed correctly
**
*****************************************************************************/
uint8_t I2CInit( uint8_t num )
{
		I2C_HandleTypeDef *hi2c = NULL;
	
		if(num == 1)
		{
				I2cHandle1.Instance = I2C1;
				hi2c = &I2cHandle1;
		}
		else
		{
				if(num == 2)
				{
			  I2cHandle2.Instance = I2C2;
				hi2c = &I2cHandle2;
				}
		}
		
		if(hi2c == 0)
		{
				return 0;
		}
    
    if(hi2c->Instance == I2C1)
		{
				hi2c->Init.Timing           = 1000;
				hi2c->Init.OwnAddress1      = 0;
				hi2c->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
				hi2c->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
				hi2c->Init.OwnAddress2      = 0;
				hi2c->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
				hi2c->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
		}
		
    /* Init the I2C */
    I2C_MspInit(hi2c);
    HAL_I2C_Init(hi2c);
	
	/**Configure Analogue filter 
	*/
		
  if (HAL_I2CEx_ConfigAnalogFilter(hi2c, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Digital filter 
    */
  if (HAL_I2CEx_ConfigDigitalFilter(hi2c, 0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

	
	return( TRUE );
}


/**
  * @brief Discovery I2C1 MSP Initialization
  * @param hi2c: I2C handle
  * @retval None
  */
static void I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;

  /* IOSV bit MUST be set to access GPIO port G[2:15] */
  //__HAL_RCC_PWR_CLK_ENABLE();
  //HAL_PWREx_EnableVddIO2();

  if (hi2c->Instance == I2C1)
  {
    /*##-1- Configure the Discovery I2C clock source. The clock is derived from the SYSCLK #*/
    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
    RCC_PeriphCLKInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);
     
    //__HAL_RCC_GPIOB_CLK_ENABLE();


   /**I2C1 GPIO Configuration    
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA 
    */
    GPIO_InitStructure.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
   }
}

	
/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();
  
    /**I2C1 GPIO Configuration    
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }

}	
	
/**
  * @brief  Write a value in a register of the device through BUS.
  * @param  Addr: Device address on BUS Bus.
  * @param  Reg: The target register address to write
  * @param  RegSize: The target register size (can be 8BIT or 16BIT)
  * @param  pBuffer: The target register value to be written
  * @param  Length: buffer size to be written
  * @retval None
  */
HAL_StatusTypeDef I2C_WriteBuffer(uint8_t num, uint16_t Addr, uint16_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length)
{
		HAL_StatusTypeDef status = HAL_OK;
		I2C_HandleTypeDef *hi2c = NULL;
	
	
		if(num == 1)
		{
				hi2c = &I2cHandle1;
		}
		else
		{
				if(num == 2)
				{
					hi2c = &I2cHandle2;
				}
		}
		
		if(hi2c == 0)
		{
			return HAL_ERROR;
		}
		
	status = HAL_I2C_Mem_Write(hi2c, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, 1000);

  /* Check the communication status */
  if (status != HAL_OK)
  {
    /* De-initialize the I2C communication BUS */
		HAL_I2C_DeInit(hi2c);

		/* Re- Initiaize the I2C communication BUS */
		I2CInit(num);
  }
  return status;
}

/**
  * @brief  Reads multiple data on the BUS.
  * @param  Addr: I2C Address
  * @param  Reg: Reg Address
  * @param  RegSize : The target register size (can be 8BIT or 16BIT)
  * @param  pBuffer: pointer to read data buffer
  * @param  Length: length of the data
  * @retval 0 if no problems to read multiple data
  */
HAL_StatusTypeDef I2C_ReadBuffer(uint8_t num, uint16_t Addr, uint16_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;
			I2C_HandleTypeDef *hi2c = NULL;
	
		if(num == 1)
		{
			hi2c = &I2cHandle1;
		}
		else
		{
				if(num == 2)
				{
					hi2c = &I2cHandle2;
				}
		}
		
		if(hi2c == 0)
		{
			return HAL_ERROR;
		}
		
		status = HAL_I2C_Mem_Read(hi2c, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, 1000);
	
  /* Check the communication status */
  if (status != HAL_OK)
  {
    /* De-initialize the I2C communication BUS */
		HAL_I2C_DeInit(hi2c);

		/* Re- Initiaize the I2C communication BUS */
		I2CInit(num);
  }
  return status;
}

	
	





