//**************************************************************************************************/
//  Filename:       ClimiteSensor.c
//  Revised:        $Date: 2009-12-30 12:32:18 -0800 (Wed, 30 Dec 2009) $
//  Revision:       $Revision: 21417 $
//
//  Description:    Sample application for a Climite sensor utilizing the Climite API.

//**************************************************************************************************/

#include "sensor_basic.h"


void Init_sys_cfg()
{
    unsigned char i;

    memset(&bcm_info,0,sizeof(bcm_info_t));
    //通讯参数
    bcm_info.common.se[0].baudrate = 19200;
    bcm_info.common.se[0].datasbit = 8;
    bcm_info.common.se[0].parity   ='N';
    bcm_info.common.se[0].stopbits = 1;

    bcm_info.common.se[1].baudrate = 19200;
    bcm_info.common.se[1].datasbit = 8;
    bcm_info.common.se[1].parity   ='N';
    bcm_info.common.se[1].stopbits = 1;


    bcm_info.common.version = 1;            //版本号
    bcm_info.common.qz = 85794;             //区站号
    bcm_info.common.st = 1;                 //服务类型

    //经度 纬度 海拔
    bcm_info.common.Lat .degree = 32;
    bcm_info.common.Lat .min    = 14;
    bcm_info.common.Lat .sec    = 20;
    bcm_info.common.Long.degree = 116;
    bcm_info.common.Long.min    = 34;
    bcm_info.common.Long.sec    = 18;
    bcm_info.common.High        = 1000;

    bcm_info.common.work_temp_max  = 800;        //工作温度上限
    bcm_info.common.work_temp_min  = -500;       //工作温度下限
    bcm_info.common.boardsvolt_max = 138;        //工作电压上限
    bcm_info.common.boardsvolt_min = 90;         //工作电压下限

    bcm_info.common.de = 0;                     //标准偏差
    bcm_info.common.fi = 1;                     //帧标识  3位数字
    bcm_info.common.at = 0;                     //板温度，按扩大10倍存储
    bcm_info.common.vv = 0;                     //电池电压，按扩大10倍存储

    bcm_info.common.stdev = 1;                  //数据标准差值计算间隔
    bcm_info.common.ftd   = 1;                  //设置或读取数字传感器主动模式下的数据发送时间间隔
    bcm_info.common.ft    = 30;                 //设置或读取数字传感器主动模式下的数据发送时间

    bcm_info.common.sw                   = 0;   //设置或读取握手机制方式
    bcm_info.common.mo                   = 1;   //设置或读取传感器的发送数据方式
    bcm_info.common.sensortype           = 0;   //传感器类型：温湿度，地温。。。
    bcm_info.common.time_to_autofilldata = 0;   //自动补传数据时间
    strncpy((char *)bcm_info.common.res, "000",3);      //预留

    //设置或读取各要素瞬时值质量控制参数(QM)
    //气压
    bcm_info.sensor.qm[0].min        = 400;          //下限
    bcm_info.sensor.qm[0].max        = 1100;           //上限
    bcm_info.sensor.qm[0].doubtful   = 0.5;            //存疑门限
    bcm_info.sensor.qm[0].err        = 2;           //错误门限
    bcm_info.sensor.qm[0].changerate = 0.1;            //最小应该变化的速率
  
    //设置或读取各要素采样值质量控制参数(QS)
    //气压
    bcm_info.sensor.qs[0].min    = 400;       //下限
    bcm_info.sensor.qs[0].max    = 1100;      //上限
    bcm_info.sensor.qs[0].change = 0.3;       //允许最大变化值

	//修正系数
    for(i = 0; i < 6; i++)	
    {
       bcm_info.sensor.cr[i].a = 0;
       bcm_info.sensor.cr[i].b = 1;
       bcm_info.sensor.cr[i].c = 0;
    }

    bcm_info.flag   = 0;                  //保存标志 FLASH 存参数

    strncpy((char *)bcm_info.sensor.res, "000",3);       //预留

    //传感器第二个结构体sensor放本要素专用参数
    bcm_info.sensor.data_num  = 2;                  //观测要素数量
    bcm_info.sensor.status_num = 0;                //状态变量数量
    bcm_info.sensor.ce = 2;               // 气压传感器类型  1-210 2-baro-1
    bcm_info.sensor.ce_H = 1;             // 湿度传感器类型  1-罗卓妮克          2-155
    bcm_info.sensor.apunit = 1;           // 气压参数单位配置 单位默认为百帕
    bcm_info.sensor.SensorNum = 1;        // 传感器个数

    memset(&bcm_info.sensor.str_name,0,10*8);
    memset(&sensor_state,0,sizeof(sensor_state_t));  //板卡状态初始化

}

