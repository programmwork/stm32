/*******************************************************************************
    Filename: sys_config.h

    Copyright 2019 BY Zhouq

This is the system config header file for PRIORITY, TASK_STACK size.
***************************************************************************/
#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H

#include "FreeRTOS.h"
#include "task.h"

#define DEBUG						0

#define COM_RECV_PRIORITY			( tskIDLE_PRIORITY + 4 )  
#define COM_SEND_PRIORITY			( tskIDLE_PRIORITY + 5 )
#define UART_PROCESS_TASK_PRIORITY	( tskIDLE_PRIORITY + 6 )
#define TIME_TASK_PRIORITY			( tskIDLE_PRIORITY + 1 )
#define EVENT_SAMPLE_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 )
#define READDATA_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )


#define TASK_LED_STACK_SIZE             200
#define TASK_SEND_STACK_SIZE            80
#define TASK_RECV_STACK_SIZE            120
#define TASK_UART_PROCESS_STACK_SIZE    200
#define TASK_TIME_STACK_SIZE			200
#define TASK_EVENT_SAMPLE_STACK_SIZE    300
#define TASK_READDATA_STACK_SIZE        600

#endif
