/*
 * FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * The documentation page for this demo available on http://www.FreeRTOS.org
 * documents the hardware configuration required to run this demo.  It also
 * provides more information on the expected demo application behaviour.
 *
 * main() creates all the demo application tasks, then starts the scheduler.
 * A lot of the created tasks are from the pool of "standard demo" tasks.  The
 * web documentation provides more details of the standard demo tasks, which
 * provide no particular functionality but do provide good examples of how to
 * use the FreeRTOS API.
 *
 * In addition to the standard demo tasks, the following tasks, interrupts and
 * tests are defined and/or created within this file:
 *
 * "LCD" task - The LCD task is a 'gatekeeper' task.  It is the only task that
 * is permitted to access the LCD and therefore ensures access to the LCD is
 * always serialised and there are no mutual exclusion issues.  When a task or
 * an interrupt wants to write to the LCD, it does not access the LCD directly
 * but instead sends the message to the LCD task.  The LCD task then performs
 * the actual LCD output.  This mechanism also allows interrupts to, in effect,
 * write to the LCD by sending messages to the LCD task.
 *
 * The LCD task is also a demonstration of a 'controller' task design pattern.
 * Some tasks do not actually send a string to the LCD task directly, but
 * instead send a command that is interpreted by the LCD task.  In a normal
 * application these commands can be control values or set points, in this
 * simple example the commands just result in messages being displayed on the
 * LCD.
 *
 * "Button Poll" task - This task polls the state of the 'up' key on the
 * joystick input device.  It uses the vTaskDelay() API function to control
 * the poll rate to ensure debouncing is not necessary and that the task does
 * not use all the available CPU processing time.
 *
 * Button Interrupt and run time stats display - The select button on the
 * joystick input device is configured to generate an external interrupt.  The
 * handler for this interrupt sends a message to LCD task, which interprets the
 * message to mean, firstly write a message to the LCD, and secondly, generate
 * a table of run time statistics.  The run time statistics are displayed as a
 * table that contains information on how much processing time each task has
 * been allocated since the application started to execute.  This information
 * is provided both as an absolute time, and as a percentage of the total run
 * time.  The information is displayed in the terminal IO window of the IAR
 * embedded workbench.  The online documentation for this demo shows a screen
 * shot demonstrating where the run time stats can be viewed.
 *
 * Idle Hook - The idle hook is a function that is called on each iteration of
 * the idle task.  In this case it is used to place the processor into a low
 * power mode.  Note however that this application is implemented using standard
 * components, and is therefore not optimised for low power operation.  Lower
 * power consumption would be achieved by converting polling tasks into event
 * driven tasks, and slowing the tick interrupt frequency.
 *
 * "Check" function called from the tick hook - The tick hook is called during
 * each tick interrupt.  It is called from an interrupt context so must execute
 * quickly, not attempt to block, and not call any FreeRTOS API functions that
 * do not end in "FromISR".  In this case the tick hook executes a 'check'
 * function.  This only executes every five seconds.  Its main function is to
 * check that all the standard demo tasks are still operational.  Each time it
 * executes it sends a status code to the LCD task.  The LCD task interprets the
 * code and displays an appropriate message - which will be PASS if no tasks
 * have reported any errors, or a message stating which task has reported an
 * error.
 *
 * "Reg test" tasks - These fill the registers with known values, then check
 * that each register still contains its expected value.  Each task uses
 * different values.  The tasks run with very low priority so get preempted
 * very frequently.  A check variable is incremented on each iteration of the
 * test loop.  A register containing an unexpected value is indicative of an
 * error in the context switching mechanism and will result in a branch to a
 * null loop - which in turn will prevent the check variable from incrementing
 * any further and allow the check task (described a above) to determine that an
 * error has occurred.  The nature of the reg test tasks necessitates that they
 * are written in assembly code.
 *
 * *NOTE 1* vApplicationSetupTimerInterrupt() is called by the kernel to let
 * the application set up a timer to generate the tick interrupt.  In this
 * example a timer A0 is used for this purpose.
 *
*/

#include "main.h"

s_tempdata_t m_tempdata;
s_defdata_t  m_defdata;
//s_check_t    m_check;
//s_calib_t    m_calib;

bcm_info_t bcm_info;

osThreadId UART_PROCESS_Handle;
osThreadId TIME_Handle;
osThreadId EVENT_SAMPLE_Handle;
osThreadId READDATA_Handle;

char sensor_di[5];

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 80000000
  *            HCLK(Hz)                       = 80000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  * @retval None
  */
void SystemClock_Config(void)
{
  /* oscillator and clocks configs */
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;
	 /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the main internal regulator output voltage 
    */
	RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    RCC_PeriphCLKInitStruct.I2c1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);
	
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  //HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

	
void main( void )
{
    // Configure the peripherals used by this demo application.  This includes
    // configuring the joystick input select button to generate interrupts.
	
		
     HAL_Init();
	
		SystemClock_Config();
		MX_DMA_Init();
  	MX_GPIO_Init();

	  sysinit();

    //startupprint();      //开始信息  打印
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);


    //打开串口总中断
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
		
    osThreadDef(UART_PROCESS, uartprocess_task, UART_PROCESS_TASK_PRIORITY, 0, TASK_UART_PROCESS_STACK_SIZE);
		
    osThreadDef(TIME, time_task, TIME_TASK_PRIORITY, 0, TASK_TIME_STACK_SIZE);
		
    //osThreadDef(EVENT_SAMPLE, check_event_sample_task, EVENT_SAMPLE_TASK_PRIORITY, 0, TASK_EVENT_SAMPLE_STACK_SIZE);
	
    osThreadDef(READDATA, readdata_task, READDATA_TASK_PRIORITY, 0, TASK_READDATA_STACK_SIZE);
		
    UART_PROCESS_Handle = osThreadCreate(osThread(UART_PROCESS), NULL);
		
    TIME_Handle = osThreadCreate(osThread(TIME), NULL);
		

	//EVENT_SAMPLE_Handle = osThreadCreate(osThread(EVENT_SAMPLE), NULL);
	
	READDATA_Handle = osThreadCreate(osThread(READDATA), NULL);

	osKernelStart();

	
	
	
	

    //xTaskCreate( uartprocess_task, "UART_PROCESS", TASK_UART_PROCESS_STACK_SIZE, NULL, UART_PROCESS_TASK_PRIORITY, NULL );
    
    //xTaskCreate( time_task, "TIME", TASK_TIME_STACK_SIZE, NULL, TIME_TASK_PRIORITY, NULL );
    
    //xTaskCreate( check_event_sample_task, "EVENT_SAMPLE", TASK_EVENT_SAMPLE_STACK_SIZE, NULL, EVENT_SAMPLE_TASK_PRIORITY, NULL );
    
    //xTaskCreate( readdata_task, "READDATA", TASK_READDATA_STACK_SIZE, NULL, READDATA_TASK_PRIORITY, NULL );
    
        
        
        
    
    //vTaskStartScheduler();



}

void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* MemoryManagement_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
  /* BusFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
  /* UsageFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
  /* SVCall_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
  /* DebugMonitor_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}



/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

}
/** Pinout Configuration
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}
void Datetime_up(void)
{
    //只做一秒内毫秒的增加
    m_tempdata.m_RtcTateTime.msec+=TIMER_0_DELAY_MS;
    if(m_tempdata.m_RtcTateTime.msec>=1000)
    {
        m_tempdata.m_RtcTateTime.msec=0;
        Rtc_UpdateTime(1);
    }
}

void Rtc_UpdateTime(U8 type)
{
    if(type==1)//加一秒
    {
        m_tempdata.m_RtcTateTime.sec++;
        if(m_tempdata.m_RtcTateTime.sec < 60)
            {goto UPDATE_TIME_0;}
        m_tempdata.m_RtcTateTime.sec = 0;
        m_tempdata.m_RtcTateTime.min++;
        if(m_tempdata.m_RtcTateTime.min < 60)
            {goto UPDATE_TIME_0;}
        m_tempdata.m_RtcTateTime.min = 0;

        m_tempdata.m_RtcTateTime.hour++;
        if(m_tempdata.m_RtcTateTime.hour < 24)
            {goto UPDATE_TIME_0;}
        m_tempdata.m_RtcTateTime.hour = 0;
    }
    else if(type==8)//加8小时
    {
        m_tempdata.m_RtcTateTime.hour+=8;
        if(m_tempdata.m_RtcTateTime.hour < 24)
            {goto UPDATE_TIME_0;}
        m_tempdata.m_RtcTateTime.hour -= 24;
    }

  //处理4、6、9、11月份
    if((m_tempdata.m_RtcTateTime.month == 4) || (m_tempdata.m_RtcTateTime.month == 6) || (m_tempdata.m_RtcTateTime.month == 9) || (m_tempdata.m_RtcTateTime.month == 11))
    {
        if(m_tempdata.m_RtcTateTime.day < 30)
        {
            m_tempdata.m_RtcTateTime.day++;
            goto UPDATE_TIME_0;
        }
        else
    {m_tempdata.m_RtcTateTime.day = 1;}
    }
  //处理2月份
    else if(m_tempdata.m_RtcTateTime.month == 2)
    {
        if(((m_tempdata.m_RtcTateTime.year + 2000) & 0x03) == 0)
        {// 闰年
            if(m_tempdata.m_RtcTateTime.day < 29)
            {
                m_tempdata.m_RtcTateTime.day++;
                goto UPDATE_TIME_0;
            }
            else
                {m_tempdata.m_RtcTateTime.day = 1;}
        }
        else
        {// 平年
            if(m_tempdata.m_RtcTateTime.day < 28)
            {
                m_tempdata.m_RtcTateTime.day++;
                goto UPDATE_TIME_0;
            }
            else
                {m_tempdata.m_RtcTateTime.day = 1;}
        }
    }
  //处理1、3、5、7、8、10、12月份
    else
    {
        if(m_tempdata.m_RtcTateTime.day < 31)
        {
            m_tempdata.m_RtcTateTime.day++;
            goto UPDATE_TIME_0;
        }
        else
            {m_tempdata.m_RtcTateTime.day = 1;}
    }

    m_tempdata.m_RtcTateTime.month++;
    if(m_tempdata.m_RtcTateTime.month < 12)
    {
      goto UPDATE_TIME_0;
    }
    m_tempdata.m_RtcTateTime.month = 1;
    m_tempdata.m_RtcTateTime.year++;

UPDATE_TIME_0:
  ;
}





void vApplicationTickHook( void )
{
    static unsigned long ulCounter = 0;

    static const unsigned long ulCheckFrequency = 5000UL / portTICK_PERIOD_MS;

    Datetime_up();
}



void vApplicationIdleHook( void )
{
    return;
}

void vApplicationMallocFailedHook( void )
{
    // Called if a call to pvPortMalloc() fails because there is insufficient
    // free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    // internally by FreeRTOS API functions that create tasks, queues or
    // semaphores.
    taskDISABLE_INTERRUPTS();

    for( ;; );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pxTask;
    ( void ) pcTaskName;

    // Run time stack overflow checking is performed if
    // configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    // function is called if a stack overflow is detected.
    taskDISABLE_INTERRUPTS();

    for( ;; );
}

