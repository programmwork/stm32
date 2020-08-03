/*
;*--------------------------------------------- 文件信息 ----------------------------------------------------
;* 文件名称 : 
;* 文件功能 : 
;* 补充说明 :
;*-------------------------------------------- 最新版本信息 -------------------------------------------------
;* 修改作者 :
;* 修改日期 :
;* 版本声明 :
;*-------------------------------------------- 历史版本信息 -------------------------------------------------
;* 文件作者 :
;* 创建日期 :
;* 版本声明 :
;************************************************************************************************************
*/
#ifndef __EARTH_TEMP_SAMPLING_H_
#define	__EARTH_TEMP_SAMPLING_H_

#include "EarthTemp_Sensor.h"

typedef struct _qcflashdata{ 
  float data;
  unsigned char qc;
}qcfalshdata_t;

struct flashdata{
  qcfalshdata_t mindata;//分钟数据  
};

typedef struct 
{
  unsigned char flag;
  U32 time;
  struct flashdata sensor[MAX_SENSOR_NUM];//存储位置最多的是温度，三支，并有5分钟的平均值，地温只有5个一分钟的平均值
  short boardstemp;
  short boardsvolt;
  long Standard_Deviation[STANDARD_DEV_NUM];
  sensor_state_t state;
}FLASH_Store_MinData_t;

//--------------------变量声明 ---------------------------
extern unsigned char Num_sample;
extern unsigned char sec_flag;
extern float data_sample_buf_history[SAMPLE_DATA_NUM]; //采样数据保存缓冲区

//------------------函数声明----------------------------
extern int GetMinData(char *Header_packet,sensors_data_t *sensors_tempdata,unsigned char num_mins);

extern unsigned char Element_MsSample(sensors_data_t *sensors_tempdata);
extern unsigned char Element_SecSample(sensors_data_t *sensors_tempdata);
extern unsigned char Element_MinSample(sensors_data_t *sensors_tempdata);

static unsigned char Quality_Sec_Ctrl(sensors_data_t *sensors_tempdata);
static unsigned char Quality_Min_Ctrl(sensors_data_t *sensors_tempdata,unsigned char ch);

extern void SampleData_Init(sensors_data_t *sensors_tempdata);
extern void Quality_Init(sensors_data_t *sensors_tempdata);

void GetSampleData(sensors_data_t *sensors_tempdata,float *dat,uint8 ch);

unsigned char iQCPS_Code(int sensorNo,float value,qs_t *qcctrl);    //秒数据质量控制函数
unsigned char iQCPM_Code(int sensorNo,float value,qm_t *qcctrl);    //分钟数据质量控制函数
#endif
