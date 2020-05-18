#ifndef _AIRT_SENSOR_H_
#define _AIRT_SENSOR_H_

#include <time.h>
#include "sysinit.h"

#define myApp_TIMER1_PERIOD_MS            500 //开2个定时器，第1个定时器定时间隔
#define myApp_TIMER2_PERIOD_MS            50  //开2个定时器，第2个定时器定时间隔

//对于温度，地温，土壤水分之类的请将此处修改
#define MAX_SENSOR_NUM                    1 //定义传感器最大接入数量
#define SAMPLE_COUNT                      30//单通道采样循环数
#define SAMPLE_DATA_NUM                   SAMPLE_COUNT * MAX_SENSOR_NUM   //5个通道，每个通道一分钟有30个数
#define STANDARD_DEV_NUM                  2//标准差数量
#define MAX_ELEMENT_NUM                   2

#define INVALID_DATA                      -999//定义无效数据值

//质量控制码
enum QC_CODE_LIST{
         QC_OK=0,           //正确
         QC_DOUBT=1,        //  存疑
         QC_ERROR=2,        //   错误
         QC_NOEXAMINE=3,    // 订正
         QC_MODIFY=4 ,      // 修改
         QC_NOSAME=5,       // 不一致
         QC_RESERVED,       // 预留
         QC_NOMEASURE=7,   // 不观测
         QC_MISSED=8,       //   缺测
         QC_NOCONTROL=9     //未做质量控制
};


#define QC_CODE_LIST unsigned char
typedef struct _qcdata{ 
  float data;
  unsigned char qc;
}qcdata_t;

typedef struct
{
  struct{
    float Vmin;
    float Vmax;
    float Rmax;
  }secdata_ctrl_t;
  
  struct{
    float Vmin;
    float Vmax;
  }mindata_ctrl_t; 
}qc_ctrl_t;

#define MAX_PKG_LEN       512//187字节
#define MAXSENDBUFFLEN    512

typedef struct{
  struct {
    qcdata_t secdata;			      //秒数据   存放2min中最前面一个3s平均值
    qcdata_t mindata;			      //分钟数据 存放1min中最前面一个3s平均值
    
    qcdata_t secdata_add;		      //秒数据累加   或1min的平均值累加
    unsigned char secdata_counter;    //秒数据个数  2min计数
    qcdata_t mindata_add;		      //分钟数据累加  或2min的平均值累加
    unsigned char mindata_counter;	  //分钟数据个数 1min计数

    unsigned char hourdata_counter;   //小时数据个数累加
    unsigned char num;
    float hour_data[61];
    unsigned char hour_data_flag;  
    //unsigned char mindata5_counter;   //5min数据个数累加
    //qcdata_t mindata5[10];
    //float mindata5_add;
  }sensor[MAX_ELEMENT_NUM];
  qc_ctrl_t qualityCtrl[MAX_ELEMENT_NUM];
   
  short boardstemp_data,boardsvolt_data;
    
  uint8  sendpacket[MAX_PKG_LEN];
  uint16 sendpacketlen;
  uint8  historyPacket[MAX_PKG_LEN];
  uint16 historyPacketLen;
}sensors_data_t;

void check_event_sample(void);
void check_event_calc(void);
uint8 startupPrint(void);
void Init_sys_cfg();

#endif
