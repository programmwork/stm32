/*
 * main.h
 *
 *  Created on: 2016Äê3ÔÂ7ÈÕ
 *      Author: lenovo
 */

#ifndef _SOURCE_PRODUCT_MAIN_H_
#define _SOURCE_PRODUCT_MAIN_H_

#include "flash.h"
#include "typedef.h"
#include "command.h"
#include "uart.h"
#include "defpam.h"
#include "uartprocess.h"
#include "sysinit.h"
#include "cmsis_os.h"
#include "stm32l4xx_hal.h"
#include "sys_config.h"
#include "AD7792.h"
#include "W25Q128.h"
#include "ds3231.h"
#include "adc12_a.h"




#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sensor_basic.h"

// Hardware includes.


#include "FreeRTOS.h"


extern s_tempdata_t m_tempdata;
extern s_defdata_t  m_defdata;
//extern s_check_t    m_check;
//extern s_calib_t    m_calib;
extern bcm_info_t   bcm_info;

extern char sensor_di[5];

void _Error_Handler(char *file, int line);
void Datetime_up(void);
void Rtc_UpdateTime(U8 type);

static void MX_DMA_Init(void);




static void MX_GPIO_Init(void);
void harddog_init(void);
void harddog_feed(void);

#endif
