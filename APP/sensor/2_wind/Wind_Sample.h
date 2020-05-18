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
;* 文件作者 : 王建佳
;* 创建日期 : 2015-5-8
;* 版本声明 :
;************************************************************************************************************
*/
#ifndef __WIND_SAMPLING_H_
#define	__WIND_SAMPLING_H_

#include "Wind_Sensor.h"

#define WIND_1_5  0x01
#define WIND_10   0x03

typedef struct _qcflashdata{ 
  float data;
  unsigned char qc;
}qcfalshdata_t;

struct flashdata{
  
  qcfalshdata_t fengsu_3s;//AFA150 3s平均风速数据  
  qcfalshdata_t fengsu_1min_avg;//AFB150 1分钟平均风速数据  
  qcfalshdata_t fengsu_2min_avg;//AFC150 2分钟平均风速数据  
  qcfalshdata_t fengsu_10min_avg;//AFD150 10分钟平均风速数据  
  qcfalshdata_t fengsu_max;//AFA150a  极大风速分钟数据  
  
  
  qcfalshdata_t fengxiang_1min;			      //风向瞬时值数据瞬时风向（1s采样）
  qcfalshdata_t fengxiang_1min_avg;			      //一分钟平均风向
  qcfalshdata_t fengxiang_2min_avg;			      //二分钟平均风向
  qcfalshdata_t fengxiang_10min_avg;			      //十分钟平均风向
  qcfalshdata_t fengxiang_max;			      //分钟内极大风速（瞬时风速）对应风向
};

typedef struct 
{
  unsigned char flag;
  U32 time;
  struct flashdata sensor[1];//存储位置最多的是温度，三支，并有5分钟的平均值，地温只有5个一分钟的平均值
  short boardstemp;
  short boardsvolt;
  long Standard_Deviation[STANDARD_DEV_NUM];
  sensor_state_t state;
}FLASH_Store_MinData_t;

//--------------------变量声明 ---------------------------
extern float data_sample_buf[SAMPLE_DATA_NUM];


//------------------函数声明----------------------------
extern int GetMinData(char *Header_packet,sensors_data_t *sensors_tempdata,unsigned char num_mins);

extern unsigned char Element_MsSample(sensors_data_t *sensors_tempdata);
extern unsigned char Element_SecSample(sensors_data_t *sensors_tempdata);
extern unsigned char Element_MinSample(sensors_data_t *sensors_tempdata);
extern unsigned char Quality_Sec_Ctrl(sensors_data_t *sensors_tempdata,unsigned char ch);
extern unsigned char Quality_Min_Ctrl(sensors_data_t *sensors_tempdata,unsigned char mins);

extern void SampleData_Init(sensors_data_t *sensors_tempdata);
extern void Quality_Init(sensors_data_t *sensors_tempdata);
//unsigned char iQCPS_Code(int sensorNo,float value,qm_t *qcctrl);
unsigned char iQCPM_Code(int sensorNo,float value,qm_t *qcctrl);
unsigned char iQCPS_Code_ws250(int sensorNo,float value,qs_t *qcctrl);
unsigned char iQCPS_Code_ws3s(int sensorNo,float value,qs_t *qcctrl);
unsigned char iQCPS_Code_wd(int sensorNo,float value,qs_t *qcctrl);

static void WindDirectCal(float *winddir_x,float *winddir_y,float *winddir_d);

void GetSampleData(sensors_data_t *sensors_tempdata,float *dat,uint8 ch);

#endif
