/************************************************************************************************************
;*--------------------------------------------- 文件信息 ----------------------------------------------------
;* 文件名称 : 智能型传感器设备通信命令
;* 文件功能 :
;* 补充说明 :
;*-------------------------------------------- 最新版本信息 -------------------------------------------------
;* 修改作者 : LY
;* 修改日期 : 2017-12-28
;* 版本声明 : 添加私有命令
;*-------------------------------------------- 历史版本信息 -------------------------------------------------
;* 文件作者 : LY
;* 创建日期 : 2017-12-7
;* 版本声明 :
***********************************************************************************************************/

#ifndef __COMMAND_H__
#define __COMMAND_H__
 
#include "typedef.h"
#include "defpam.h"


typedef enum
{
    /* command index*/     
    SETCOM,            //1　设置或读取设备的通讯参数
    AUTOCHECK,         //2　设备自检
    HELP,              //3　帮助命令
    QZ,                //4　设置或读取设备的区站号
    ST,                //5　设置或读取设备的服务类型
    DI,                //6　读取设备标识位
    ID,                //7　设置或读取设备
    LAT,               //8　设置或读取气象观测站的纬度
    LONG,              //9　设置或读取气象观测站的经度
    DATE,              //10　设置或读取设备日期
    TIME,              //11　设置或读取设备时间
    DATETIME,          //12　设置或读取设备日期与时间
    FTD,               //13　设置或读取设备主动模式下的发送时间间隔
    STDEV,             //14　设置或读取设备数据标准偏差值计算的时间间隔
    FAT,               //15　设置或读取设备主动模式下数据发送时间
    SETCOMWAY,         //16　设置握手机制方式
    SS,                //17　读取设备各工作参数值
    STAT,              //18　读取设备工作状态数据
    AT,                //19　设置或读取设备正常工作温度范围
    VV,                //20　设置或读取仪器正常工作电压范围
    SN,                //21　设置或读取设备序列号
    QCPS,              //22　设置或读取采样值质量控制参数
    QCPM,              //23　设置或读取瞬时气象值质量控制参数
    CR,                //24　设置或读取设备的校正、检定信息
    READDATA,          //25　实时读取数据
    DOWN,              //26　历史数据下载
    RESET27,             //27　重启设备
    ASL,               //28　设置或读取气象观测站的海拔
    SUPERADMIN,        //29　操作权限指令
    UNIT,              //30  气压单位修改命令

//------------------- 私有命令 ---------------------------
    DEBUGON,           //1 调试模式开
    DEBUGOFF,          //2 调试模式关
    VERSION,           //3 版本号读取
    RESTORE,           //4 恢复出厂设置
    STARTTIME,         //5 设备开始运行时间
    SECDOUTON,         //6 输出秒级数据
    SECDOUTOFF,
    ERASECR,
    CFC,               //7 标定传感器系数
    WINDH,             //8 风拨码开关设置
    STSENSOR,          //9 低温传感器数量及深度配置
    SHSENSOR,          //10 土壤水分传感器数量及深度配置
    HARDVERCFG,         //硬件版本号写入

	MAX_COMMAND_NUM
} cmd_index_t;


typedef struct { 
	struct{
         char  *name;
	     int   (*action)( char *buf, char *rbuf);
	      }dispatch[MAX_COMMAND_NUM];
} bcm_control_t;



extern volatile unsigned char Flag_DataEmulationMode;  // X1允许产生模拟数据标志位
extern volatile unsigned char EmulationDataType;// X4 传感器当前的模拟值数据类型
extern volatile float EmulationData;//X3传感器当前的模拟值
extern float EmulationData_1;       //X3传感器当前的模拟值
extern volatile unsigned char EmulationSensorChannel;   // X2要读取的传感器通道号

unsigned char GetCheckSumCode(void);
int getSeparator(char *rcvdata);
U16 uartcmd_process(U8 uartno,char *data,U16 len,char *rbuf);
int check_closecom(char *buf,char *rbuf);


int check_time  (int hour, int min,   int sec);                 // 校验时间
int check_data  (int year, int month, int day);                 // 校验日期
int check_format(char *p,  uint8 len, char *buf, uint8 num);    //快速校验 串格式
int check_digit (char *p,  uint8 len);                          //校验字符串是否全为数字
int Utils_CheckCmdStr(char *pStr);                    //检查收到字符串格式是否符合要求

// -----------------------------------------------------------------------
int cmd_setcom     (char *buf,char *rbuf);          //1　设置或读取设备的通讯参数
int cmd_autocheck  (char *buf,char *rbuf);          //2　设备自检
int cmd_help       (char *buf,char *rbuf);          //3　帮助命令
int cmd_qz         (char *buf,char *rbuf);          //4　设置或读取设备的区站号
int cmd_st         (char *buf,char *rbuf);          //5　设置或读取设备的服务类型
int cmd_di         (char *buf,char *rbuf);          //6　读取设备标识位
int cmd_id         (char *buf,char *rbuf);          //7　设置或读取设备
int cmd_lat        (char *buf,char *rbuf);          //8　设置或读取气象观测站的纬度
int cmd_long       (char *buf,char *rbuf);          //9　设置或读取气象观测站的经度
int cmd_asl        (char *buf,char *rbuf);          //28  设置或读取气象观测站的海拔
int cmd_date       (char *buf,char *rbuf);          //10　设置或读取设备日期
int cmd_time       (char *buf,char *rbuf);          //11　设置或读取设备时间
int cmd_datetime   (char *buf,char *rbuf);          //12　设置或读取设备日期与时间
int cmd_ftd        (char *buf,char *rbuf);          //13　设置或读取设备主动模式下的发送时间间隔
int cmd_stdev      (char *buf,char *rbuf);          //14　设置或读取设备数据标准偏差值计算的时间间隔
int cmd_fat        (char *buf,char *rbuf);          //15　设置或读取设备主动模式下数据发送时间
int cmd_setcomway  (char *buf,char *rbuf);          //16　设置握手机制方式
int cmd_ss         (char *buf,char *rbuf);          //17　读取设备各工作参数值
int cmd_stat       (char *buf,char *rbuf);          //18　读取设备工作状态数据
int cmd_at         (char *buf,char *rbuf);          //19　设置或读取设备正常工作温度范围
int cmd_vv         (char *buf,char *rbuf);          //20　设置或读取仪器正常工作电压范围
int cmd_sn         (char *buf,char *rbuf);          //21　设置或读取设备序列号
int cmd_qcps       (char *buf,char *rbuf);          //22　设置或读取采样值质量控制参数
int cmd_qcpm       (char *buf,char *rbuf);          //23　设置或读取瞬时气象值质量控制参数
int cmd_cr         (char *buf,char *rbuf);          //24　设置或读取设备的校正、检定信息
int cmd_readdata   (char *buf,char *rbuf);          //25　实时读取数据
int cmd_down       (char *buf,char *rbuf);          //26　历史数据下载
int cmd_reset      (char *buf,char *rbuf);          //27　重启设备
int cmd_superadmin (char *buf,char *rbuf);          //29  操作权限命令
int cmd_unit       (char *buf,char *rbuf);          //30  设置气压传感器的单位
// ------------------------------------------------------------------------------
// --------------------------- 私有命令 --------------------------------------------
int cmd_debugon     (char *buf,char *rbuf);     //A.1 　打开调试模式
int cmd_debugoff    (char *buf,char *rbuf);     //A.2 　关闭调试模式
int cmd_version     (char *buf,char *rbuf);     //A.3　版本号查询
int cmd_restore     (char *buf,char *rbuf);     //A.4　恢复出厂设置
int cmd_starttime   (char *buf,char *rbuf);     //A.5　设备开始运行时间
int cmd_secdout_on  (char *buf,char *rbuf);     //A.6　输出秒级数据
int cmd_secdout_off (char *buf,char *rbuf);
int cmd_erase_check (char *buf,char *rbuf);
int cmd_cfc         (char *buf,char *rbuf);     //A.7　标定系数
int cmd_windh       (char *buf,char *rbuf);     //A.8　风传感器拨码开关设置
int cmd_stsensor    (char *buf,char *rbuf);     //A.9　地温传感器配置
int cmd_shsensor    (char *buf,char *rbuf);     //A.10　土壤水分传感器配置
int cmd_version_cfg (char *buf,char *rbuf);      //12    硬件版本号写入

//--------------------------- 私有命令结束  ------------------------------------------

#endif
