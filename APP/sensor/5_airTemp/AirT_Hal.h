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
#ifndef __AIRT_HAL_H_
#define	__AIRT_HAL_H_

#include "AirT_Sensor.h"

extern void hal_sensor_init(void);
extern unsigned char Sensor_Init(void);
extern unsigned char AirTemp_engine(float result[MAX_SENSOR_NUM]);
extern void Reset_Sensor(void);
extern void USART3_RX(void);
#endif
