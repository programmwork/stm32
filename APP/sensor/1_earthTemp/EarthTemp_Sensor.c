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
    //10cm
    bcm_info.sensor.qm[0].min        = -90;          //下限
    bcm_info.sensor.qm[0].max        = 90;           //上限
    bcm_info.sensor.qm[0].doubtful   = 5;            //存疑门限
    bcm_info.sensor.qm[0].err        = 10;           //错误门限
    bcm_info.sensor.qm[0].changerate = 0;            //最小应该变化的速率
    //20cm
    bcm_info.sensor.qm[1].min        = -90;          //下限
    bcm_info.sensor.qm[1].max        = 90;           //上限
    bcm_info.sensor.qm[1].doubtful   = 5;            //存疑门限
    bcm_info.sensor.qm[1].err        = 10;           //错误门限
    bcm_info.sensor.qm[1].changerate = 0;            //最小应该变化的速率
    //30cm
    bcm_info.sensor.qm[2].min        = -80;          //下限
    bcm_info.sensor.qm[2].max        = 80;           //上限
    bcm_info.sensor.qm[2].doubtful   = 2;            //存疑门限
    bcm_info.sensor.qm[2].err        = 5;            //错误门限
    bcm_info.sensor.qm[2].changerate = 0;            //最小应该变化的速率
    //40cm
    bcm_info.sensor.qm[3].min        = -70;          //下限
    bcm_info.sensor.qm[3].max        = 70;           //上限
    bcm_info.sensor.qm[3].doubtful   = 1;            //存疑门限
    bcm_info.sensor.qm[3].err        = 5;            //错误门限
    bcm_info.sensor.qm[3].changerate = 0;            //最小应该变化的速率
    //50cm
    bcm_info.sensor.qm[4].min        = -60;          //下限
    bcm_info.sensor.qm[4].max        = 60;           //上限
    bcm_info.sensor.qm[4].doubtful   = 1;            //存疑门限
    bcm_info.sensor.qm[4].err        = 3;           //错误门限
    bcm_info.sensor.qm[4].changerate = 0;            //最小应该变化的速率
    //50cm
    bcm_info.sensor.qm[5].min        = -60;          //下限
    bcm_info.sensor.qm[5].max        = 60;           //上限
    bcm_info.sensor.qm[5].doubtful   = 1;            //存疑门限
    bcm_info.sensor.qm[5].err        = 3;           //错误门限
    bcm_info.sensor.qm[5].changerate = 0;            //最小应该变化的速率

    //50cm
    bcm_info.sensor.qm[6].min        = -60;          //下限
    bcm_info.sensor.qm[6].max        = 60;           //上限
    bcm_info.sensor.qm[6].doubtful   = 1;            //存疑门限
    bcm_info.sensor.qm[6].err        = 3;           //错误门限
    bcm_info.sensor.qm[6].changerate = 0;            //最小应该变化的速率

    //50cm
    bcm_info.sensor.qm[7].min        = -60;          //下限
    bcm_info.sensor.qm[7].max        = 60;           //上限
    bcm_info.sensor.qm[7].doubtful   = 1;            //存疑门限
    bcm_info.sensor.qm[7].err        = 3;           //错误门限
    bcm_info.sensor.qm[7].changerate = 0;            //最小应该变化的速率

    //设置或读取各要素采样值质量控制参数(QS)
    //10 20 30 40 50cm
    bcm_info.sensor.qs[0].min    = -100;       //下限
    bcm_info.sensor.qs[0].max    = 100;        //上限
    bcm_info.sensor.qs[0].change = 2;          //允许最大变化值
    bcm_info.sensor.qs[1].min    = -100;       //下限
    bcm_info.sensor.qs[1].max    = 100;        //上限
    bcm_info.sensor.qs[1].change = 2;          //允许最大变化值
    bcm_info.sensor.qs[2].min    = -100;       //下限
    bcm_info.sensor.qs[2].max    = 100;        //上限
    bcm_info.sensor.qs[2].change = 2;          //允许最大变化值
    bcm_info.sensor.qs[3].min    = -100;       //下限
    bcm_info.sensor.qs[3].max    = 100;        //上限
    bcm_info.sensor.qs[3].change = 2;          //允许最大变化值
    bcm_info.sensor.qs[4].min    = -100;       //下限
    bcm_info.sensor.qs[4].max    = 100;        //上限
    bcm_info.sensor.qs[4].change = 2;          //允许最大变化值
    bcm_info.sensor.qs[5].min    = -100;       //下限
    bcm_info.sensor.qs[5].max    = 100;        //上限
    bcm_info.sensor.qs[5].change = 2;          //允许最大变化值
    bcm_info.sensor.qs[6].min    = -100;       //下限
    bcm_info.sensor.qs[6].max    = 100;        //上限
    bcm_info.sensor.qs[6].change = 2;          //允许最大变化值
    bcm_info.sensor.qs[7].min    = -100;       //下限
    bcm_info.sensor.qs[7].max    = 100;        //上限
    bcm_info.sensor.qs[7].change = 2;          //允许最大变化值

	//修正系数
    for(i = 0; i < 8; i++)
    {
       bcm_info.sensor.cr[i].a = 0;
       bcm_info.sensor.cr[i].b = 1;
       bcm_info.sensor.cr[i].c = 0;
    }
	
    bcm_info.flag   = 0;                  //保存标志 FLASH 存参数

    strncpy((char *)bcm_info.sensor.res, "000",3);       //预留

    strncpy((char *)bcm_info.sensor.str_name[0], "AB10", 4);
    strncpy((char *)bcm_info.sensor.str_name[1], "AB20", 4);
    strncpy((char *)bcm_info.sensor.str_name[2], "AB30", 4);
    strncpy((char *)bcm_info.sensor.str_name[3], "AB40", 4);
    strncpy((char *)bcm_info.sensor.str_name[4], "AB50", 4);
    strncpy((char *)bcm_info.sensor.str_name[5], "AB60", 4);
    strncpy((char *)bcm_info.sensor.str_name[6], "AB70", 4);
    strncpy((char *)bcm_info.sensor.str_name[7], "AB80", 4);

    strncpy((char *)bcm_info.sensor.str_name[8], "ABl10", 5);
    strncpy((char *)bcm_info.sensor.str_name[9], "ABl20", 5);
    strncpy((char *)bcm_info.sensor.str_name[10], "ABl30", 5);
    strncpy((char *)bcm_info.sensor.str_name[11], "ABl40", 5);
    strncpy((char *)bcm_info.sensor.str_name[12], "ABl50", 5);
    strncpy((char *)bcm_info.sensor.str_name[13], "ABl60", 5);
    strncpy((char *)bcm_info.sensor.str_name[14], "ABl70", 5);
    strncpy((char *)bcm_info.sensor.str_name[15], "ABl80", 5);


    //传感器第二个结构体sensor放本要素专用参数
    bcm_info.sensor.data_num  = 10;                //观测要素数量
    bcm_info.sensor.status_num = 0;                //状态变量数量
	bcm_info.sensor.ce = 0;               // 气压传感器类型  0-华云   3-泰科
    bcm_info.sensor.apunit = 1;           // 气压参数单位配置 单位默认为百帕
    bcm_info.sensor.SensorNum = 5;        // 传感器个数

    memset(&sensor_state,0,sizeof(sensor_state_t));  //板卡状态初始化

}


