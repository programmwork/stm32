
#ifndef _WIND_SENSOR_H
#define _WIND_SENSOR_H

#include <time.h>
#include "sysinit.h"

#define myApp_TIMER1_PERIOD_MS            500 //开2个定时器，第1个定时器定时间隔
#define myApp_TIMER2_PERIOD_MS            50  //开2个定时器，第2个定时器定时间隔

//对于温度，地温，土壤水分之类的请将此处修改
#define MAX_SENSOR_NUM                    8 //定义传感器最大接入数量
#define SAMPLE_COUNT                      30//单通道采样循环数
//#define SAMPLE_DATA_NUM                   SAMPLE_COUNT * MAX_SENSOR_NUM   //5个通道，每个通道一分钟有30个数
#define SAMPLE_DATA_NUM 300   //风速240个，风向60个
#define STANDARD_DEV_NUM                  2//标准差数量
#define MAX_ELEMENT_NUM                   1

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
    //*****************************以下风速

    qcdata_t fengsu_MomentaryValue;			      //风速瞬时值数据
    qcdata_t fengsusecdatalast3s[12];	      //过去3s内12个采样数据
    qcdata_t fengsu_3s;			      //瞬时风速(3秒平均)～～～～～～～
	
    qcdata_t fensu_2min_3s;			      //秒数据   存放2min中最前面一个3s平均值
    qcdata_t fensu_1min_3s;			      //分钟数据 存放1min中最前面一个3s平均值
    
    qcdata_t avg1minsum;		      //秒数据累加   或1min的平均值累加
    unsigned char fengsu_min1_counter;	      //分钟数据个数 1min计数
    qcdata_t fengsu_1min_avg;			      //一分钟平均风速～～～～～	
    
	qcdata_t avg2minsum;		      //分钟数据累加  或2min的平均值累加
    unsigned char fengsu_min2_counter;            //秒数据个数  2min计数
    qcdata_t fengsu_2min_avg;			      //二分钟平均风速～～～～～

    qcdata_t fengsumindatalast9min[10];	      //过去9分钟的分钟数据
    qcdata_t fengsu_10min_avg;			      //十分钟平均风速~~~~~~
	
    qcdata_t fengsu_max;			      //分钟内极大风速~~~~

//*****************************以下风向
    qcdata_t fengxiang_MomentaryValue;			      //风向瞬时值数据瞬时风向（1s采样）
    qcdata_t fengxiang_1min;                  //一分钟整点瞬时风向
    qcdata_t fengxiang_1min_avg;			      //一分钟平均风向
    qcdata_t fengxiang_2min_avg;			      //二分钟平均风向
    qcdata_t fengxiang_10min_avg;			      //十分钟平均风向
    qcdata_t fengxiang_max;			      //分钟内极大风速（瞬时风速）对应风向
    qcdata_t fengxiangmindatalast9min_x[10];	      //过去10分钟的x分量平均值数据 
    qcdata_t fengxiangmindatalast9min_y[10];	      //过去10分钟的y分量平均值数据 
	
    qcdata_t avg2m_x;			      	//2min的x平均值
    qcdata_t avg2m_y;	      			//2min的y平均值
    qcdata_t avg2m_x_sum;			      	//2min的x平均值累加
    qcdata_t avg2m_y_sum;	      			//2min的y平均值累加
    unsigned char fengxiang_min2_counter;            //秒数据个数  2min计数
	
    qcdata_t avg1m_x;			      	//1min的x平均值
    qcdata_t avg1m_y;	     			//1min的y平均值
    qcdata_t avg1m_x_sum;			      	//1min的x平均值累加
    qcdata_t avg1m_y_sum;	     			//1min的y平均值累加
    unsigned char fengxiang_min1_counter;	      //分钟数据个数 1min计数

    struct {
        unsigned char hourdata_counter;   //小时数据个数累加
        unsigned char num;
        float hour_data[61];
        unsigned char hour_data_flag;
    }changerate[4];

  }sensor[1];
    qc_ctrl_t qualityCtrl[MAX_SENSOR_NUM+1];//对于风向风速需要二个不同的质量控制
   
  short boardstemp_data,boardsvolt_data;
    
  uint8  sendpacket[MAX_PKG_LEN];
  uint16 sendpacketlen;
  uint8  historyPacket[MAX_PKG_LEN];
  uint16 historyPacketLen;
}sensors_data_t;


void Init_sys_cfg(void);

#endif
