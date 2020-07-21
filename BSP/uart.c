
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "sys_config.h"


#include "uart.h"


#include "stm32l4xx_hal.h"
#define DMA_TSIZE	128



UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

 
uint8_t buf_t[2][SIZE_SEND_BUF];

uint8_t buf_r[2][SIZE_RCV_BUF];

uart uart_send[2];
uart uart_recv[2];

dma_buf swap_buf[2][3];

uart_dma dma_recv[2];





void HAL_UART_MspInit(UART_HandleTypeDef* huart);
uint8_t UART_Init(uint8_t num, uint32_t bps, uint8_t databit, uint8_t parity, uint8_t stopbit, uint8_t msp)
{
	  
				
		UART_HandleTypeDef* huart = 0;
	
		if(num == 1)
		{
				huart1.Instance = USART1;
				huart = &huart1;
}
		else
		{
				if(num == 2)
				{
					huart2.Instance = USART2;
					huart = &huart2;
				}
				else
				{
					if(num == 3)
					{
						huart3.Instance = USART3;
						huart = &huart3;
					}
		    }
		}
		

		if(huart == 0)
		{
			return 0;
		}
	
	
  huart->Init.BaudRate = bps;
	
	if(databit == 7)
		huart->Init.WordLength = UART_WORDLENGTH_7B;
	if(databit == 8)
		huart->Init.WordLength = UART_WORDLENGTH_8B;
	if(databit == 9)
		huart->Init.WordLength = UART_WORDLENGTH_9B;
	
	if(stopbit == 1)
		huart->Init.StopBits = UART_STOPBITS_1;
  if(stopbit == 2)
		huart->Init.StopBits = UART_STOPBITS_2;

	if(parity == 0)
		huart->Init.Parity = UART_PARITY_NONE;
  if(parity == 1)
		huart->Init.Parity = UART_PARITY_EVEN;
	
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	
	
	if(msp == 1)
	{
		HAL_UART_MspInit(huart);
  }
	
	HAL_UART_Init(huart);
	
	return 1;
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct;
	
  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    //__HAL_RCC_GPIOA_CLK_ENABLE();
    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    RCC_PeriphCLKInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
		
	if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
    
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Request = DMA_REQUEST_2;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(huart,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Channel4;
    hdma_usart1_tx.Init.Request = DMA_REQUEST_2;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(huart,hdmatx,hdma_usart1_tx);		
	__HAL_UART_ENABLE_IT(huart, UART_IT_ERR);
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
	HAL_NVIC_SetPriority(USART1_IRQn , 5, 0);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    RCC_PeriphCLKInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
		
	if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
    //__HAL_RCC_GPIOA_CLK_ENABLE();

    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    
     /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Channel6;
    hdma_usart2_rx.Init.Request = DMA_REQUEST_2;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }


    __HAL_LINKDMA(huart,hdmarx,hdma_usart2_rx);

    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Channel7;
    hdma_usart2_tx.Init.Request = DMA_REQUEST_2;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(huart,hdmatx,hdma_usart2_tx);		
	__HAL_UART_ENABLE_IT(huart, UART_IT_ERR);
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
	HAL_NVIC_SetPriority(USART2_IRQn , 5, 0);


  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
   
    else if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();
  
    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    RCC_PeriphCLKInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
		
	if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
    //__HAL_RCC_GPIOC_CLK_ENABLE();
   /**USART3 GPIO Configuration    
    PC4     ------> USART3_TX
    PC5     ------> USART3_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);
    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
    HAL_NVIC_SetPriority(USART3_IRQn , 5, 0);

  }
}


void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(huart->hdmarx);
    HAL_DMA_DeInit(huart->hdmatx);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(huart->hdmarx);
    HAL_DMA_DeInit(huart->hdmatx);

  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
  else if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART3 GPIO Configuration    
    PC4     ------> USART3_TX
    PC5     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4|GPIO_PIN_5);

  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }

}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    //
    if((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET)
    {
        if(huart->Instance == USART1)
        {
            USART_DMA_Restart(1);
        }
        else
        {
            if(huart->Instance == USART2)
            {
                USART_DMA_Restart(2);
            }
						else 
						{
							if(huart->Instance == USART3)
							{
								huart->RxState = HAL_UART_STATE_READY;								
							}
						}
        } 
    }
}

uint8_t USART_DMA_Restart(uint8_t num)
{
    uart_dma* dma = NULL;
    uart* device = NULL;
    UART_HandleTypeDef* huart = 0;

    if(num == 1)
    {
        huart = &huart1;
        dma = &dma_recv[0];
	    device = &uart_recv[0];
    }
    else
    {
        if(num ==2 )
        {
            huart = &huart2;
            dma = &dma_recv[1];
            device = &uart_recv[1];
        }
    }


    //if(__HAL_DMA_GET_COUNTER(huart->hdmarx)!=SWAPSIZE)
    {
        __HAL_DMA_DISABLE(huart->hdmarx);

        // Save data len
        dma->buf[dma->wr].cnt = ( SWAPSIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx));

        dma->wr++;

        dma->wr = dma->wr%RX_BUF_SIZE;
    		
       

    	if( dma->wr==dma->rd )
    	{
    		dma->rd++;

    		dma->rd = dma->rd%RX_BUF_SIZE;
    	}

    	dma->buf[dma->wr].cnt = 0;

        huart->RxState = HAL_UART_STATE_READY;
        HAL_DMA_Abort(huart->hdmarx);
        //
        HAL_UART_Receive_DMA(huart, (uint8_t *)(dma->buf[dma->wr].data), SWAPSIZE);

        xSemaphoreGiveFromISR( device->SemDev, NULL );
    }


    return 1;
}

uint8_t UartOpen(uart_CBD* CBD)
{
	
	if( CBD->rw&UART_WR )
	{
		uart* device = &uart_send[CBD->cid];
		

		device->cid = CBD->cid;

		device->data = buf_t[CBD->cid];

		device->size = sizeof( buf_t[CBD->cid] );
		device->wp = device->data;
		device->rp = device->data;
		device->end = device->data + device->size - 1;

		device->cnt = 0;

		device->SemDev = xSemaphoreCreateBinary();
		if ( device->SemDev==NULL )
		{
			return NULL;
		}

		device->SemBuf = xSemaphoreCreateBinary();
		if ( device->SemBuf==NULL )
		{
			return NULL;
		}

        device->Mes_queue_Send = xQueueCreate( CBD->Mes_queue_Send_size, sizeof(uint8_t) );//创建队列，即在内容中开辟固定大小的区域。
        if ( device->Mes_queue_Send==NULL )
        {
            return NULL;
        }


        /*BEGIN:modify by guozikun 2020.5.20*/
        if(CBD->cid == CBD_UART_1)
        {      
    		xTaskCreate(UartSend_1, "UartSend1", CBD->heap_t_size, (void*)device, CBD->pri_t, NULL);
    		xSemaphoreGive( device->SemBuf );
        }
        else if(CBD->cid == CBD_UART_2)
        {      
    		xTaskCreate(UartSend_2, "UartSend2", CBD->heap_t_size, (void*)device, CBD->pri_t, NULL);
    		xSemaphoreGive( device->SemBuf );
        }
        else
        {
            return NULL;
        }
        /*END:modify by guozikun 2020.5.20*/
	}

	if(CBD->rw&UART_RD)
	{
		uart* device = &uart_recv[CBD->cid];

		uart_dma* dma = &dma_recv[CBD->cid];

		device->cid = CBD->cid;

		device->data = buf_r[CBD->cid];

		device->size = sizeof( buf_r[CBD->cid] );
		device->wp = device->data;
		device->rp = device->data;
		device->end = device->data + device->size - 1;

		device->cnt = 0;



		device->SemDev = xSemaphoreCreateBinary();
		if ( device->SemDev==NULL )
		{
			return NULL;
		}

		device->SemBuf = xSemaphoreCreateBinary();
		if (device->SemBuf==NULL)
		{
			return NULL;
		}

		/*END:modify by guozikun 2020.5.20*/
        if(CBD->cid == CBD_UART_1)
        {        
            HAL_UART_Receive_DMA(&huart1, (dma->buf[dma->wr].data), SWAPSIZE);
            xSemaphoreGive( device->SemBuf );
            xTaskCreate(UartRecv_1, "UartRecv1", CBD->heap_r_size, (void*)device, CBD->pri_r, NULL);
        }
        else if(CBD->cid == CBD_UART_2)
		{        
            HAL_UART_Receive_DMA(&huart2, (dma->buf[dma->wr].data), SWAPSIZE);
            xSemaphoreGive( device->SemBuf );
            xTaskCreate(UartRecv_2, "UartRecv2", CBD->heap_r_size, (void*)device, CBD->pri_r, NULL);
        } 
        else
        {
            return NULL;
        }
        /*END:modify by guozikun 2020.5.20*/
	}

	return STATUS_SUCCESS;
}



void UartSend_1( void *pdata )
{
    uart* device = ( uart* )pdata;

    if( device->cid==CBD_UART_1 )
    {
    	
    }
    else
    {
        // Task stack size must big than 70
        //uart_printf( 0, "Uart %d Send...\r\n", device->cid+1 );

        uart_printf( 0, "Uart Send...\r\n" );
    }

    while(1)
    {
        // Wait block here
        xSemaphoreTake( device->SemBuf, portMAX_DELAY );

        memset(device->data, 0x00, SIZE_SEND_BUF);

        for(device->cnt = 0; device->cnt < SIZE_SEND_BUF; device->cnt++)
        {
            if(pdFALSE == xQueueReceive(device->Mes_queue_Send, (void *)&device->data[device->cnt], 0))//向队列中填充内容
            {
                break;
            }
        }

		if(device->cnt > 0)
		{
			HAL_UART_Transmit_DMA(&huart1, (uint8_t *)device->data, device->cnt);
		}
		else
		{
			xSemaphoreGive( device->SemBuf );

			vTaskDelay( 5 );
		}
	}
}

void UartSend_2( void *pdata )
{
    uart* device = ( uart* )pdata;

    if( device->cid==CBD_UART_2 )
    {
        
    }
    else
    {
        // Task stack size must big than 70
        //uart_printf( 0, "Uart %d Send...\r\n", device->cid+1 );

        uart_printf( 1, "Uart Send...\r\n" );
    }

    while(1)
    {
        // Wait block here
        xSemaphoreTake( device->SemBuf, portMAX_DELAY );

        memset(device->data, 0x00, SIZE_SEND_BUF);

        for(device->cnt = 0; device->cnt < SIZE_SEND_BUF; device->cnt++)
        {
            if(pdFALSE == xQueueReceive(device->Mes_queue_Send, (void *)&device->data[device->cnt], 0))//向队列中填充内容
            {
                break;
            }
        }

        if(device->cnt > 0)
        {
            HAL_UART_Transmit_DMA(&huart2, (uint8_t *)device->data, device->cnt);
        }
        else
        {
            xSemaphoreGive( device->SemBuf );

            vTaskDelay( 5 );
        }
    }
}



void UartRecv_1( void *pdata )
{
	uint8_t tmp = 0;

	uint8_t cnt = 0;

	uart* device = ( uart* )pdata;

	uart_dma* dma = &dma_recv[device->cid];

	uint8_t* buf = NULL;

	uint8_t stat = 0;

	while( 1 )
	{
	    xSemaphoreTake( device->SemDev, portMAX_DELAY );
        xSemaphoreTake( device->SemBuf, portMAX_DELAY );

		if( dma->rd!=dma->wr )
		{
			cnt = dma->buf[dma->rd].cnt;

			if( cnt>0 )
			{
				buf = dma->buf[dma->rd].data;

				// Copy all data to the buf
				if( 1 )
				{
					for( tmp=0; tmp<cnt; tmp++ )
					{
						device->wp[0] = buf[tmp];

						device->wp++;

						// if the in point come to the end, jump to the start point
						if( device->wp>device->end )
						{
							device->wp = device->data;
						}
					}

					if( (device->cnt + cnt) > device->size )
					{
						device->cnt = device->size;

						device->rp = device->wp;
					}
					else
					{
						device->cnt += cnt;
					}
				}
			}

			dma->rd++;

			dma->rd = dma->rd%RX_BUF_SIZE;
            xSemaphoreGive( device->SemBuf );
		}

		
		vTaskDelay( 5 );
	}
}

void UartRecv_2( void *pdata )
{
	uint8_t tmp = 0;

	uint8_t cnt = 0;

	uart* device = ( uart* )pdata;

	uart_dma* dma = &dma_recv[device->cid];

	uint8_t* buf = NULL;

	uint8_t stat = 0;

	while( 1 )
	{	
		xSemaphoreTake( device->SemDev, portMAX_DELAY );
        xSemaphoreTake( device->SemBuf, portMAX_DELAY );

		if( dma->rd!=dma->wr )
		{
			cnt = dma->buf[dma->rd].cnt;

			if( cnt>0 )
			{
				buf = dma->buf[dma->rd].data;

				// Copy all data to the buf
				if( 1 )
				{
					for( tmp=0; tmp<cnt; tmp++ )
					{
						device->wp[0] = buf[tmp];

						device->wp++;

						// if the in point come to the end, jump to the start point
						if( device->wp>device->end )
						{
							device->wp = device->data;
						}
					}

					if( (device->cnt + cnt) > device->size )
					{
						device->cnt = device->size;

						device->rp = device->wp;
					}
					else
					{
						device->cnt += cnt;
					}
				}
			}

			dma->rd++;

			dma->rd = dma->rd%RX_BUF_SIZE;
            xSemaphoreGive( device->SemBuf );
		}	
		vTaskDelay( 5 );
	}
}


uint16_t UartWrite( uint8_t cid, uint8_t* payload, uint16_t len )
{
	uint16_t i=0;

	uart* device = &uart_send[cid];

	//xSemaphoreTake( device->SemBuf, portMAX_DELAY );
    
	for(i = 0; i < len; i++)
    {
        if(pdTRUE == xQueueSend( device->Mes_queue_Send, ( void* )payload, 20 ))//向队列中填充内容
        {
            payload++;
        }
		else
		{
			break;
		}
    } 
    

	//xSemaphoreGive( device->SemBuf );

	return i;
}

uint16_t UartRead( uint8_t cid, uint8_t* payload, uint16_t len )
{
	uint16_t cnt=0;

	uart* device = &uart_recv[cid];

	if( device==NULL )
	{
		return 0;
	}

	if( device->SemBuf==NULL )
	{
		return 0;
	}

	xSemaphoreTake( device->SemBuf, portMAX_DELAY );

	if( device->cnt>0 )
	{
		if( device->cnt>len )
		{
			cnt = len;
		}
		else
		{
			cnt = device->cnt;
		}

		device->cnt -= cnt;

		if( ( device->end-device->rp +1 )>= cnt )
		{
			memcpy( payload, device->rp, cnt );

			device->rp += cnt;

			if( device->rp>device->end )
			{
				device->rp = device->data;
			}
		}
		else
		{
			len =  device->end-device->rp +1 ;
			memcpy( payload, device->rp, len );

			memcpy( payload+len, device->data, cnt-len );

			device->rp = device->data + cnt - len;
		}
	}

	xSemaphoreGive( device->SemBuf );

	return cnt;
}



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	  uart* device = NULL;
    
 
    //
    huart->gState = HAL_UART_STATE_READY;

    if(huart->Instance == USART1)
    {
        device = &uart_send[0];        
    }
    else
    {
        if(huart->Instance == USART2)
        {
            device = &uart_send[1];        
        }
        else
        {
            if(huart->Instance == USART3)
            {
                return;
            }
        }
    }
    
    xSemaphoreGiveFromISR( device->SemBuf, NULL );  
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uart* device = NULL;

    uart_dma* dma = NULL;

    
    if(huart->Instance == USART1)
	{
        dma = &dma_recv[0];
	    device = &uart_recv[0];
	}
    else
    {
        if(huart->Instance == USART2)
        {
            dma = &dma_recv[1];
    	    device = &uart_recv[1];
        }
        else
        {
            if(huart->Instance == USART3)
            {
                //USART3_RX();
                return;
            }
        }
    }
    		
      // Save data len
    dma->buf[dma->wr].cnt = SWAPSIZE;

    dma->wr++;

    dma->wr = dma->wr%RX_BUF_SIZE;

    if( dma->wr==dma->rd )
    {
		dma->rd++;
		dma->rd = dma->rd%RX_BUF_SIZE;
    }

    dma->buf[dma->wr].cnt = 0;

    huart->RxState = HAL_UART_STATE_READY;
    HAL_UART_Receive_DMA(huart, (uint8_t *)(dma->buf[dma->wr].data), SWAPSIZE);

    xSemaphoreGiveFromISR( device->SemDev, NULL );
}


	
void DMA_IRQHandler(uint8_t num)
{
    uint32_t flag_it, rx_flag, tx_flag;
	  uart* device = NULL;

    if(num == 1)
    {
  		  flag_it = hdma_usart1_tx.DmaBaseAddress->ISR;
				tx_flag = flag_it & DMA_FLAG_TC4;
        
        while(flag_it != 0)
				{
            HAL_DMA_IRQHandler(&hdma_usart1_tx);
            flag_it = (0x0f << 12) & hdma_usart1_tx.DmaBaseAddress->ISR;
        }

				//
				if(tx_flag == DMA_FLAG_TC4)
				{
					huart1.gState = HAL_UART_STATE_READY;
					device = &uart_send[0];
					xSemaphoreGiveFromISR( device->SemBuf, NULL );
				}
		}
    else if(num == 2 )
    {
  	    flag_it = hdma_usart1_rx.DmaBaseAddress->ISR;
	      rx_flag = flag_it & DMA_FLAG_TC5;  


		    while(flag_it != 0)
		    {
				    HAL_DMA_IRQHandler(&hdma_usart1_rx);
            flag_it = (0x0f << 16) & hdma_usart1_rx.DmaBaseAddress->ISR;
		    }
		
				if(rx_flag != 0)
				{
						uart_dma* dma = &dma_recv[0];
						device = &uart_recv[0];

						// Save data len
						dma->buf[dma->wr].cnt = SWAPSIZE;

						dma->wr++;

						dma->wr = dma->wr%RX_BUF_SIZE;

						if( dma->wr==dma->rd )
						{
							  dma->rd++;

							  dma->rd = dma->rd%RX_BUF_SIZE;
						}

						dma->buf[dma->wr].cnt = 0;
 
						huart1.RxState = HAL_UART_STATE_READY;
						HAL_UART_Receive_DMA(&huart1, (uint8_t *)(dma->buf[dma->wr].data), SWAPSIZE);
					}
				}

	//DMA_clearInterrupt( device->channelSelec );

	//xSemaphoreGiveFromISR( device->SemDev, NULL );

	return;
}

void uart_printf( uint8_t cid, char* fmt, ... )
{
#if UART_DEBUG

	va_list ap;

	char *buf = pvPortMalloc( 128 );

	if( buf )
	{
		memset( buf, 0, 128 );
	}

	va_start( ap, fmt );

	vsprintf( buf, fmt, ap );

	va_end( ap );

	UartWrite( cid, (uint8_t*)buf, strlen(buf) );

	vPortFree( buf );

	return;

#endif

}

void Hex_dis( uint8_t* data, uint8_t len )
{
	uint8_t i = 0;

	for( i=0; i<len; i++ )
	{
		uart_printf( 0, "%02X ", data[i] );

		if( i%16==15 )
		{
			uart_printf( 0, "\r\n" );
		}
	}

	uart_printf( 0, "\r\n" );

	return;
}

