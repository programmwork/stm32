/*
 * defpam.h
 *
 *  Created on: 2016年3月7日
 *      Author: lenovo
 */

#ifndef  SOURCE_PRODUCT_DEFPAM_H_
#define  SOURCE_PRODUCT_DEFPAM_H_

#include "typedef.h"
#include "command.h"
//#include "ds3231.h"
//#include "w25x16.h"
//#include "adc12_a.h"
//#include "hal_AD7792.h"
//#include "uartprocess.h"

#pragma pack(1)//单字节对齐

#define TIMER_0_DELAY_MS 1 //定时器 MS

#define MAX_PKGLEN 400				//数据包长度
#define MAX_UARTRCV_LEN 1024 //串口接收长度
#define UART_RCV_TIMEOUT_COUNTER 8 //接收超时机制，超时时间=TIMER_0_DELAY_MS * UART_RCV_TIMEOUT_COUNTER
#define MAXN 16

/**************************************************/
/*串口工作方式*/
enum{

        COMTYPE_RS232=0,
        COMTYPE_RS485,
        COMTYPE_RS422,
        MAX_UART_COMTYPE,
};
/*串口序号*/
enum {
        UARTDEV_1=0,
        UARTDEV_2,
        UARTDEV_3,
        //UARTDEV_4,
        MAX_UARTNUM,
};

/*串口设备类型*/
enum{
        UART_DEVICE_DEBUG=0,    //接上位机调试口
        UART_DEVICE_SENSOR,     //SENSOR模块
        UART_DEVICE_LORA,       //LORA
	    UART_DEVICE_RS485,      //modbus 设备
	    MAX_UART_DEVICE_TYPE
};

typedef struct _uartinfo{
    U8 comworktype;//串口工作类型，232,485
    U8 uartdevtype;//挂接设备类型
}s_uartinfo_t;

typedef struct _rtcdatetime_t{
    U16 msec;
    U8	sec;
    U8	min;
    U8	hour;
    U8	day;
    U8	month;
    U16	year;
}s_RtcTateTime_t;


#define s_datetime_t s_RtcTateTime_t

typedef struct _baseinfo{
    unsigned char  sn[40];                 //序列号 36字符
    unsigned short id;                     //设备ID
    unsigned char  hard_version[20];       //硬件版本号
}s_baseinfo_t;

#define MAX_SERVER_LINK_NUM 1
#define MAX_SERVER_SENDQUEUE_NUM 4 //server 可以缓存的数据
#define SAVEPAM_STARTFLAG_CHAR 0x55
#define SAVEPAM_ENDFLAG_CHAR 0xAA

//----------------------------------------------------------
//内部段 flash 4块，每块大小为 128byte
typedef struct {        //基础信息
    U8 save_start_flag;//保存标志

    s_baseinfo_t m_baseinfo;
    s_uartinfo_t m_uartinfo[MAX_UARTNUM];

    U8 save_end_flag;//保存标志
}s_defdata_t;
/*
typedef struct {        //校正信息
   U8 save_start_flag;//保存标志

   s_check_info_t m_check_info;

   U8 save_end_flag;//保存标志
}s_check_t;

typedef struct {        //检定信息
   U8 save_start_flag;//保存标志

   s_check_info_t m_calib_info;

   U8 save_end_flag;//保存标志
}s_calib_t;*/

//typedef struct {        //预留
//
//  }s_reserve_t;
//---------------------------------------------------------
typedef struct _uartrcv{
    U8 buff[MAX_UARTRCV_LEN+1];

    U16 WD;
    U16 RD;

    U8 (*process)(U8 uartno,U8 devtype);//回调函数
}s_uartrcv_t;


 //整个flash上，有多少片sector 有数据
typedef struct _rcdFlash{
    U16 recordNum;//总共有多少条数据记录
    struct{
        U16 sectorNo;
        U16 pageNo;
        U16 pieceNo;
        U16 recordnumOfSector;//当前sector有多少个record
    }currentRcd;
}s_rcdFlash_t;

typedef struct _rcdHeadOne{
    //时间年月日,记录报文时间信息
    U8 year;
    U8 mon;
    U8 day;
    U8 hour;
    U8 min;
    U8 sec;

    U16 msgflag;
    U8 msgno;
    U8 sendok;//是否发送成功
}s_rcdHeadOne_t;

typedef struct _rcdHeadOnePage{
	s_rcdHeadOne_t m_rcdHead_t[25];//一个page 存25条head 信息
}s_rcdHeadOnePage_t;

typedef struct _rcdHeadOneSector{
	s_rcdHeadOnePage_t m_rcdHeadPage_t[16];//一个sector 存16个page--400条报文
}s_rcdHeadOneSector_t;

#define MAX_RECORD_NUM 4096

typedef struct _tempdata
{
    s_RtcTateTime_t m_RtcTateTime;
    s_uartrcv_t m_uartrcv[MAX_UARTNUM];

    struct {
        U8 sample;
    }m_debuguint_t;

    struct{
        U8  flag;//打开串口透传标识
        U8  mainuartno;//串口透传设备号
        U8  slaveuartno;//串口透传设备号
        U8  counter;//如果main串口无指令了，最多透传60秒
    }m_opencomset_t;

    struct{  /*任务标志*/
        U8 msecevent;//1S
        U8 secevent;//1M
        U8 Flag_1s;
        U8 send_flag; //自动发送成功标识
        U8 minevent;//1M
        U8 hourevent;//1M
        U8 autosendevent;//采集事件
        U8 minstorageevent;//存储
        U8 hourstorageevent;//存储
        U8 readdataevent;//历史数据事件  wjj 20150510
        U8 uart_config;
        U8 uart_config2;
        U8 uart_config_counter;
        U8 uart_config_counter2;
    }event;

    U8   currentCmdIndex;
    U8   currentUartIndex;  //0~3
    U8   reset;     //用于计算看门狗复位时间
//----------------------------------------------------------
//down历史数据
    U16 Count_down_history;  //历史数据条数  不能超过60*12条
    U32 time_start;//历史数据起始时间
    U16 time_interval;//读取历史数据间隔
    //U8  Flag_Mindata_send;//分钟数据发送标志，在af回执里面判断是否是发送分钟数据失败
//readdata 读取当前历史数据
    U8  Count_read_history;  //历史数据条数  1条
    U32 time_start_read;//历史数据起始时间
    U16 time_interval_read;//读取历史数据间隔
//----------------------------------------------------------
    U8  SuperadMin;        // 20分钟的操作权限命令
    U8  SuperadMinCnt;     // 计算20分钟
    U32 SysPowerON;        // 系统上电时间
    U8  DebugON;           // 是否打开调试 20min
    U8  DebugONCnt;        // 调试开启时间
    U8  SecDataOut;        // 秒级数据输出
//----------------------------------------------------------
    U8  CheckFlag;         //检定读取标志
    U8  CfcFlag;           //校正读取标志
}s_tempdata_t;

typedef struct{
    unsigned char state_num;                    //显示状态个数
    unsigned char self_test;                    //自检状态
    unsigned char board_temp;                   //板温
    unsigned char exter_power;                  //外部电源
    unsigned char board_volt;                   //板压
    unsigned char board_current;                //板卡电流
    unsigned char exter_volt;                   //外部电压
    unsigned char bat_volt;                     //蓄电池电压

#if 0
    unsigned char weigh_state;                  //盛水桶工作状态
    unsigned char bucket_dev_state;             //翻斗式雨量工作状态检测
    unsigned char bucket_block;                 //雨量筒筒口堵塞监测
    unsigned char bucket_state;                 //雨量筒上翻斗状态监测
    unsigned char count_bucket_state;           //计数翻斗状态监测
#endif

}sensor_state_t;

//设置或读取各要素瞬时值质量控制参数(QM)
typedef struct {
    float               min;                 //下限
    float               max;                 //上限
    float               doubtful;            //存疑门限
    float               err;                 //错误门限
    float               changerate;          //最小应该变化的速率
}qm_t;

//设置或读取各要素采样值质量控制参数(QS)
typedef struct {
    float               min;             //下限
    float               max;             //上限
    float               change;          //允许最大变化值
}qs_t;

//多项式方程标校系数 a*X^2 + b*X + c
typedef struct {
    float               a;
    float               b;
    float               c;
}cr_t;
//湿度校准
typedef struct {
      float               HUMI_DATA_12;       // 12%RH时的值
      float               HUMI_DATA_35;       // 35%RH时的值
      float               HUMI_DATA_55;       // 55%RH时的值
      float               HUMI_DATA_75;       // 75%RH时的值
      float               HUMI_DATA_90;       // 90%RH时的值
}cr_hum_t;

typedef struct{//校正 检定 相关信息
    uint8      number;        //传感器号
    cr_t       cr;            //校正系数（a,b,c）
    //检验信息
}s_check_cr_t;

typedef struct{//校正 检定 相关信息
    uint8       flag;           //存储标志
    uint16      year;           //校验时间
    uint8       mon;
    uint8       day;
    uint8       term;           //校验周期
    uint32      number;         //校验员
    uint8       len;            //校正时=系数组数（a,b,c）， 检定时=字符串长度
    //检验信息
}s_check_info_t;                                     //检验信息


//串口参数
typedef struct {
    unsigned long int baudrate;         //波特率
    unsigned char     datasbit;         //数据位
    unsigned char     parity;           //校验位
    unsigned char     stopbits;         //停止位
    unsigned char     res;              //预留
}se_t;







/*device info struct*/
typedef struct {
    unsigned char         flag;                      //保存标志，用于判断FLASH是否保存数据

//第一个结构体common放共用的参数
    struct {
        unsigned char         version;                //版本号
        unsigned char         qz[6];                     //区站号

        struct {                                      //经度
            unsigned char     degree;        //度
            unsigned char     min;           //分
            unsigned char     sec;           //秒
        }Lat;
        struct {                                      //纬度
            unsigned char     degree;        //度
            unsigned char     min;           //分
            unsigned char     sec;           //秒
        }Long;
        long         High;                   //海拔

        unsigned short        st;                     //设备服务类型
        short                 work_temp_max;          //工作温度上限
        short                 work_temp_min;          //工作温度下限
        short                 boardsvolt_max;         //工作电压上限
        short                 boardsvolt_min;         //工作电压下限

        unsigned char         de;                     //标准偏差
        unsigned short        fi;                     //帧标识  3位数字
                 short        at;                     //板温度，按扩大10倍存储
        unsigned short        vv;                     //电池电压，按扩大10倍存储
        //网络参数
        unsigned int          pid;                    //PANID  5
        unsigned char         cha;                    //工作频段 2
        unsigned char         typ;                    //节点类型 1
        unsigned short        sa;                     //短地
        unsigned char         echo;                   //设置命令回响

        se_t se[2];
        
        unsigned short        stdev;                  //数据标准差值计算间隔
        unsigned short        ftd;                    //设置或读取数字传感器主动模式下的数据发送时间间隔
        unsigned short        ft;                     //设置或读取数字传感器主动模式下的数据发送时间

        unsigned char         sw;                     //设置或读取握手机制方式
        unsigned char         mo;                     //设置或读取传感器的发送数据方式
        unsigned char         sensortype;             //传感器类型：温湿度，地温。。。
        unsigned char         time_to_autofilldata;   //自动补传数据时间
		unsigned char         res[3];                 //预留
    }common;

    //第二个结构体sensor放本要素专用参数
    struct{
        unsigned char         data_num;                  //观测要素数量
        unsigned char         status_num;                //状态变量数量
        unsigned char         qc_en;
        qm_t                  qm[10];                     //设置或读取各要素瞬时值质量控制参数(QM)
        qs_t                  qs[10];                     //设置或读取各要素采样值质量控制参数(QS)

        unsigned char         ce;            // 气压传感器厂家
        unsigned char         ce_H;          // 湿度传感器厂家
        unsigned char         res[3];        // 预留
        unsigned char         apunit;        // 气压参数配置
        cr_t                  cr[8];         // 标校系数
        cr_hum_t              crHum[3];      // 湿度做三个点校准  高温  常温  低温

        unsigned char         SensorNum;        // 传感器个数
        char                  str_name[20][8];  // 接入传感器配置
    }sensor;

} bcm_info_t;

#pragma pack()

#endif
