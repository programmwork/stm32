/*****************************************************************************
 *   2019.03.11 By Zhouq
 *
*****************************************************************************/
#include "stm32l4xx_hal.h"



#include "uart.h"

#include "spi.h"






SPI_HandleTypeDef SpiHandle1;
SPI_HandleTypeDef SpiHandle2;
SPI_HandleTypeDef SpiHandle3;

static void SPIx_MspInit(SPI_HandleTypeDef *hspi);
/*****************************************************************************
** Function name:		SPIInit
**
** Descriptions:		SPIInit port initialization routine
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
uint8_t SPIInit( uint8_t num )
{	
		SPI_HandleTypeDef *hspi = NULL;

		
		if(num == 1)
		{
			SpiHandle1.Instance = SPI1;
			hspi = &SpiHandle1;
		}
		else
		{
				if(num == 2)
				{
					SpiHandle2.Instance = SPI2;
					hspi = &SpiHandle2;
				}
				else
				{
					if(num == 3)
					{
						SpiHandle3.Instance =	SPI3;
						hspi = &SpiHandle3;
					}
				}
		}
		
		if(hspi == 0)
		{
			return 0;
		}

    if(num == 1)
			hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
		if(num == 2)
			hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
		if(num == 3)
			hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
		
    hspi->Init.Direction = SPI_DIRECTION_2LINES;
		hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi->Init.CRCPolynomial = 7;
    hspi->Init.DataSize = SPI_DATASIZE_8BIT;
    hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi->Init.NSS = SPI_NSS_SOFT;
    hspi->Init.TIMode = SPI_TIMODE_DISABLE;
    hspi->Init.Mode = SPI_MODE_MASTER;
		hspi->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
		hspi->Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	
//3?и║??бе
	
    SPIx_MspInit(hspi);
    HAL_SPI_Init(hspi);
  

}








static void SPIx_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef   GPIO_InitStructure;

  
	if(hspi->Instance == SPI1)
	{
		/* Enable SPIx clock  */
		__HAL_RCC_SPI1_CLK_ENABLE();

		/* enable SPIx gpio clock */
		  //__HAL_RCC_GPIOA_CLK_ENABLE();

		
 /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    GPIO_InitStructure.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = GPIO_PIN_8;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else if(hspi->Instance == SPI2)
	{
		/* Enable SPIx clock  */
		__HAL_RCC_SPI2_CLK_ENABLE();
		
   /**SPI2 GPIO Configuration    
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI 
    */
    GPIO_InitStructure.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	GPIO_InitStructure.Pin = GPIO_PIN_12;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
   }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{

  if(hspi->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
  else if(hspi->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspDeInit 0 */

  /* USER CODE END SPI2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI2_CLK_DISABLE();
  
    /**SPI2 GPIO Configuration    
    PB13     ------> SPI2_MISO
    PB14     ------> SPI2_MOSI
    PB15     ------> SPI2_SCK 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

  /* USER CODE BEGIN SPI2_MspDeInit 1 */

  /* USER CODE END SPI2_MspDeInit 1 */
  }

}



/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval none.
  */
uint8_t SPI_Read_Write_Byte( uint8_t num, uint8_t data )
{
		uint8_t receivedbyte;
		SPI_HandleTypeDef *hspi = NULL;
	
		if(num == 1)
		{
			hspi = &SpiHandle1;
		}
		else
		{
				if(num == 2)
				{
					hspi = &SpiHandle2;
				}
				else
				{
					if(num == 3)
					{
						hspi = &SpiHandle3;
					}
				}
		}
		
		if(hspi == 0)
		{
			return 0;
		}
	
  /* Enable the SPI */
  __HAL_SPI_ENABLE(hspi);
  /* check TXE flag */
  while ((hspi->Instance->SR & SPI_FLAG_TXE) != SPI_FLAG_TXE);

  /* Write the data */
  *((__IO uint8_t *)&hspi->Instance->DR) = data;

  while ((hspi->Instance->SR & SPI_FLAG_RXNE) != SPI_FLAG_RXNE);
  receivedbyte = *((__IO uint8_t *)&hspi->Instance->DR);

  /* Wait BSY flag */
  while ((hspi->Instance->SR & SPI_FLAG_FTLVL) != SPI_FTLVL_EMPTY);
  while ((hspi->Instance->SR & SPI_FLAG_BSY) == SPI_FLAG_BSY);

  /* disable the SPI */
  __HAL_SPI_DISABLE(hspi);

  return receivedbyte;
}

void AD7792_SET_CS_LOW( void )
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

	//__delay_cycles( 500 );
}

void AD7792_SET_CS_HIGH( void )  //spi2-nss
{
	//__delay_cycles( 500 );

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}

void W25Q128_SET_CS_LOW( void )   //spi1-nss
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

	//__delay_cycles( 500 );
}

void W25Q128_SET_CS_HIGH( void )
{
	//__delay_cycles( 500 );

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
}







/******************************************************************************
**                            End Of File
******************************************************************************/

