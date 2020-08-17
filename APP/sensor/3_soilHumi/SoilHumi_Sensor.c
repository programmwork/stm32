//**************************************************************************************************/
//  Filename:       ClimiteSensor.c
//  Revised:        $Date: 2009-12-30 12:32:18 -0800 (Wed, 30 Dec 2009) $
//  Revision:       $Revision: 21417 $
//
//  Description:    Sample application for a Climite sensor utilizing the Climite API.

//**************************************************************************************************/

#include "sensor_basic.h"

extern unsigned char DS3231_ReadTime(s_RtcTateTime_t *prtc);
extern unsigned char DS3231_SetTime(s_RtcTateTime_t *prtc);

uint32 ChannelList[16] = {0x00000800,0x00001000,0x00002000,0x00004000,0x00008000,0x00010000,0x00020000,0x00040000,
0x00080000,0x00100000,0x00200000,0x00400000,0x00800000,0x01000000,0x02000000,0x04000000};

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
    bcm_info.sensor.qm[0].min        = 0;          //下限
    bcm_info.sensor.qm[0].max        = 100;           //上限
    bcm_info.sensor.qm[0].doubtful   = 10;            //存疑门限
    bcm_info.sensor.qm[0].err        = 20;           //错误门限
    bcm_info.sensor.qm[0].changerate = 0;            //最小应该变化的速率
    //20cm
    bcm_info.sensor.qm[1].min        = 0;          //下限
    bcm_info.sensor.qm[1].max        = 100;           //上限
    bcm_info.sensor.qm[1].doubtful   = 10;            //存疑门限
    bcm_info.sensor.qm[1].err        = 20;           //错误门限
    bcm_info.sensor.qm[1].changerate = 0;            //最小应该变化的速率
    //30cm
    bcm_info.sensor.qm[2].min        = 0;          //下限
    bcm_info.sensor.qm[2].max        = 100;           //上限
    bcm_info.sensor.qm[2].doubtful   = 10;            //存疑门限
    bcm_info.sensor.qm[2].err        = 20;            //错误门限
    bcm_info.sensor.qm[2].changerate = 0;            //最小应该变化的速率
    //40cm
    bcm_info.sensor.qm[3].min        = 0;          //下限
    bcm_info.sensor.qm[3].max        = 100;           //上限
    bcm_info.sensor.qm[3].doubtful   = 10;            //存疑门限
    bcm_info.sensor.qm[3].err        = 20;            //错误门限
    bcm_info.sensor.qm[3].changerate = 0;            //最小应该变化的速率
    //50cm
    bcm_info.sensor.qm[4].min        = 0;          //下限
    bcm_info.sensor.qm[4].max        = 100;           //上限
    bcm_info.sensor.qm[4].doubtful   = 10;            //存疑门限
    bcm_info.sensor.qm[4].err        = 20;           //错误门限
    bcm_info.sensor.qm[4].changerate = 0;            //最小应该变化的速率
    
    //设置或读取各要素采样值质量控制参数(QS)
    //10cm
    bcm_info.sensor.qs[0].min    = 0;       //下限
    bcm_info.sensor.qs[0].max    = 100;        //上限
    bcm_info.sensor.qs[0].change = 20;          //允许最大变化值
	//20cm
    bcm_info.sensor.qs[1].min    = 0;       //下限
    bcm_info.sensor.qs[1].max    = 100;        //上限
    bcm_info.sensor.qs[1].change = 20;          //允许最大变化值
    //30cm
    bcm_info.sensor.qs[2].min        = 0;          //下限
    bcm_info.sensor.qs[2].max        = 100;           //上限
    bcm_info.sensor.qs[2].change     = 20;            //允许最大变化值
    //40cm
    bcm_info.sensor.qs[3].min        = 0;          //下限
    bcm_info.sensor.qs[3].max        = 100;           //上限
    bcm_info.sensor.qs[3].change     = 20;            //允许最大变化值
    //50cm
    bcm_info.sensor.qs[4].min        = 0;          //下限
    bcm_info.sensor.qs[4].max        = 100;           //上限
    bcm_info.sensor.qs[4].change     = 20;            //允许最大变化值

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
    bcm_info.sensor.data_num  = 5;                  //观测要素数量
    bcm_info.sensor.status_num = 0;                //状态变量数量
	bcm_info.sensor.ce = 0;               // 气压传感器类型  0-华云   3-泰科
    bcm_info.sensor.apunit = 1;           // 气压参数单位配置 单位默认为百帕
    bcm_info.sensor.SensorNum = 5;        // 传感器个数

    memset(&bcm_info.sensor.str_name,0,10*8);
    memset(&sensor_state,0,sizeof(sensor_state_t));  //板卡状态初始化

}

void check_event_sample(void)
{
    //查询是否到采样周期//秒级事件处理
    if(m_tempdata.event.secevent==true)
    {
        m_tempdata.event.secevent=false;
    
        //每30分钟 的第50秒读硬时钟给软时钟校时
        if((m_tempdata.m_RtcTateTime.min%30 == 0)&&(m_tempdata.m_RtcTateTime.sec == 50))
        {
            s_RtcTateTime_t time_struct_hardrtc_temp;
            if(DS3231_ReadTime(&time_struct_hardrtc_temp)==1)  //时间校验正确才给软时钟校时
            {
                m_tempdata.m_RtcTateTime = time_struct_hardrtc_temp;
            }
        }
    }
	
	if(1 == Element_SecSample(&sensors_data))	//采集成功
	{
		//   flag_check_event = 0;//本采样周期内禁止采样
	}
}
/*

check_event_calc()
check_event_storage()
对实时性要求不高，可放到 MY_TEMP_EVT 事件里完成

*/
void check_event_calc(void)
{
    uint8 i = 0;

    Element_MinSample(&sensors_data);

    memset(&sensors_data.sendpacket,0,MAX_PKG_LEN);
    sensors_data.sendpacketlen=GetMinData((char *)sensors_data.sendpacket,&sensors_data,1);
    if(Flag_DataEmulationMode == 1)//需要输出采样数据包
    {
      for(i = 1;i<MAX_SENSOR_NUM;i++)
        GetSampleData(&sensors_data,data_sample_buf_history,i);
    } 
}

