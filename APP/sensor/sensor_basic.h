/*
 * sensor_basic.h
 *
 *  Created on: 2018年1月24日
 *      Author: HYSD
 */

#ifndef _SOURCE_BASIC_H_
#define _SOURCE_BASIC_H_

#include <time.h>
#include <math.h>


#include "main.h"

#include "sensor_process.h"
 
#define SENSOR 2

#if (SENSOR == 1)   //地温
#include "EarthTemp_hal.h"
#include "EarthTemp_Sample.h"
#include "EarthTemp_Sensor.h"
#define HARD_VER  "ST_V02_1814"     //硬件版本
#define SOFT_VER  "ST_V102_20180917" //软件版本,此处必须统一为3字节长度
#define DI "YSGT"
#endif

#if (SENSOR == 2)   //风
#include "Wind_hal.h"
#include "Wind_Sample.h"
#include "Wind_Sensor.h"
#define HARD_VER  "WD_V10_17C12"     //硬件版本
#define SOFT_VER  "WD_V101_20180917" //软件版本,此处必须统一为3字节长度
#define DI "YWPD"
#endif

#if (SENSOR == 3)   //土壤水分
#include "SoilHumi_hal.h"
#include "SoilHumi_Sample.h"
#include "SoilHumi_Sensor.h"
#define HARD_VER  "SH_V10_17C12"     //硬件版本
#define SOFT_VER  "SH_V101_20180917" //软件版本,此处必须统一为3字节长度
#define DI "YSMS"
#endif

#if (SENSOR == 4)   //气压
#include "AirP_Sample.h"
#include "AirP_Hal.h"
#include "AirP_USART1.h"
#include "AirP_Sensor.h"
#define HARD_VER  "AP_V10_17C13"     //硬件版本
#define SOFT_VER  "AP_V101_20180917" //软件版本,此处必须统一为3字节长度
#define DI "YTPS"
#endif

#if (SENSOR == 5)   //温度
#include "AirT_Hal.h"
#include "AirT_Sample.h"
#include "AirT_Sensor.h"
#define HARD_VER  "AT_V10_17C12"     //硬件版本
#define SOFT_VER  "AT_V102_20180917" //软件版本,此处必须统一为3字节长度
#define DI "YTMP"
#endif

#if (SENSOR == 6)   //湿度
#include "AirH_Hal.h"
#include "AirH_Sample.h"
#include "AirH_USART1.h"
#include "AirH_Sensor.h"
#define HARD_VER  "AH_V10_17C13"     //硬件版本
#define SOFT_VER  "AH_V101_20180917" //软件版本,此处必须统一为3字节长度
#define DI "YHMS"
#endif

#define WIND_VELOCITY_1_M          1
#define WIND_VELOCITY_2_M          2
#define WIND_VELOCITY_10_M         3
#define WIND_DIRECTION_1_M         5
#define WIND_DIRECTION_2_M         6
#define WIND_DIRECTION_10_M        7
#define WIND_VELOCITY_3_S          0
#define WIND_DIRECTION_0_M         4

#define WIND_VELOCITY_250_S        0
#define WIND_DIRECTION_1_S         1
//---------------------------------------------flash information-----------------------------
#define     FLASH_BLOCK_NUM       (uint32)256  //4M = 64 block， 16M = 256 block
#define     FLASH_SECTOR_NUM      (uint32)(16 * FLASH_BLOCK_NUM)
#define     FLASH_PAGE_NUM        (uint32)(16 * FLASH_SECTOR_NUM)

#define     FLASH_CONFIG_START    (uint32)(0 * 16 )    //第0个Sector用作参数存储
#define     FLASH_CFC_START       (uint32)(2 * 16 )  //第2-4 Sector用作校验信息存储
#define     FLASH_CFC_BACK        (uint32)(5 * 16 )   //第5-7 Sector用作校验信息备份
#define     FLASH_CHECK_START     (uint32)(8 * 16 )   //第8-10 Sector用作检定信息存储
#define     FLASH_CHECK_BACK      (uint32)(11 * 16)   //第11-13 Sector用作检定信息备份
#define     FLASH_SN      (uint32)(16 * 16)   //第11-13 Sector用作检定信息备份
#define     FLASH_DATA_START      (uint32)(17 * 16)   //第16Sector开始用作历史数据存储

#define     Flash_PAGE_NUMBER   (uint32)(FLASH_PAGE_NUM - FLASH_DATA_START)      // 芯片的page数量，  4M = 64block * 16sector * 16page
#define     Flash_PAGE_SIZE     (uint32)256ul                  // 每个page的字节数

//---------------------------------------------------------------------------------------

#define CHECK_QC_IS_OK(pQCArray,index)  (pQCArray[index / 8] & (1u << (index % 8)))


extern sensors_data_t sensors_data;
extern sensor_state_t sensor_state;
extern uint8 Flag_nodata;
extern float board_temp_max;
extern float board_temp_min;
extern float board_volt_max;
extern float board_volt_min;

extern FLASH_Store_MinData_t  FLASH_Store_MinData;//存储结构钵
extern FLASH_Store_MinData_t  FLASH_Read_MinData;//读数据结构体
extern uint32  check_offset;   //检定读取偏移地址
extern uint32  cfc_offset;     //校正都取偏移地址

void Convert_Sec_to_Time(s_RtcTateTime_t *rtc_temp, time_t sec_data);
void Convert_Time_to_min(UINT32 *data);
void check_event_storage(void);
void check_event_autosend(void);
unsigned char Readdata_event(void);

extern unsigned char BanWenBanYa_engine(float *pT, float *pV);
extern unsigned char read_record_min(unsigned long timep, unsigned char dat[], unsigned char len);
extern unsigned char save_record_min(unsigned long timep, unsigned char dat[], unsigned char len);
extern unsigned char show_history_mindata(void);
extern unsigned char read_history_mindata(void);

extern void S32ToStrAlignRight(signed long dat, unsigned char charNum, char  *str_out_1);
extern void Data_Copy(qcfalshdata_t *FLASH_Store, qcfalshdata_t *FLASH_Read, qcdata_t *sensor, qcfalshdata_t *Mindata_temp, unsigned char mode, unsigned char multiple);
extern void Pack_Data(qcfalshdata_t *Mindata_temp, unsigned int *len, char *Header_packet, unsigned char data_len);
extern void Pack_Datal(long *Mindata_temp, unsigned int *len, char *Header_packet, unsigned char data_len);
extern float StandardDeviation( float *pData, unsigned char *pQC, unsigned int len);

extern void sample_data_read(unsigned int index, char *dat ,unsigned int data_num);
extern void sample_data_save(char *dat ,uint16 data_num);

extern unsigned char read_cfc_data(void);
extern unsigned char read_check_data(void);
extern unsigned char save_cr(uint8 num, s_check_info_t *check_info, char *data);

#endif /* _SENSOR_BASIC_H_ */
