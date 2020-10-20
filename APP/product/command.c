/************************************************************************************************************
;*--------------------------------------------- 文件信息 ----------------------------------------------------
;* 文件名称 : 智能型传感器设备通信命令
;* 文件功能 :
;* 补充说明 :
;*-------------------------------------------- 最新版本信息 -------------------------------------------------
;* 修改作者 :
;* 修改日期 :
;* 版本声明 :
;*-------------------------------------------- 历史版本信息 -------------------------------------------------
;* 文件作者 : LY
;* 创建日期 : 2017-12-7
;* 版本声明 : 完成通信命令及所有命令的读取设置，包含私有命令
***********************************************************************************************************/

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "main.h" 

#include <math.h>
#include <time.h>

#define SN_NUM 36

int check_datetime(s_RtcTateTime_t *datetime);
time_t get_softimer_sec(s_RtcTateTime_t *temp_t);
time_t get_softimer(s_RtcTateTime_t *temp_t);

volatile unsigned char Flag_DataEmulationMode = 0;   // X1允许产生模拟数据标志位
volatile unsigned char EmulationSensorChannel = 0;   // X2要读取的传感器通道号
volatile float         EmulationData = 0;            // X3传感器当前的模拟值
volatile unsigned char EmulationDataType = 0;        // X4 传感器当前的模拟值数据类型
float                  EmulationData_1 = 0.0;

#define DEV_TYPE_MAX  14
char *dev_type[DEV_TYPE_MAX]={
    "YAWP",     //0 称重降水仪
    //---------自动气候站--------------
    "YACS",     //1 自动气候站
    "YHMS",     //2 湿度数字传感器
    "YTMP",     //3 温度数字传感器
    "YIST",     //4 红外地表温度
    "YTPS",     //5 气压数字传感器
    "YWPD",     //6 风速风向数字传感器
    "YWYG",     //7 感雨数字传感器
    "YTBR",     //8 翻斗雨量数字传感器(0.1mm)
    "YSGT",     //9 浅层地面温度数字传感器
    "YSMS",     //10 土壤水分数字传感器
    "YTRS",     //11 总辐射数字传感器

    "YSDR",     //12 日照
    "YROS"      //13 辐射观测仪

};

const bcm_control_t bcm_control={
    {
        {"SETCOM"    ,&cmd_setcom},            //1　设置或读取设备的通讯参数
        {"AUTOCHECK" ,&cmd_autocheck},         //2　设备自检
        {"HELP"      ,&cmd_help},              //3　帮助命令
        {"QZ"        ,&cmd_qz},                //4　设置或读取设备的区站号
        {"ST"        ,&cmd_st},                //5　设置或读取设备的服务类型
        {"DI"        ,&cmd_di},                //6　读取设备标识位
        {"ID"        ,&cmd_id},                //7　设置或读取设备
        {"LAT"       ,&cmd_lat},               //8　设置或读取气象观测站的纬度
        {"LONG"      ,&cmd_long},              //9　设置或读取气象观测站的经度
        {"ASL"       ,&cmd_asl},               //28　设置或读取气象观测站的海拔
        {"DATE"      ,&cmd_date},              //10　设置或读取设备日期
        {"TIME"      ,&cmd_time},              //11　设置或读取设备时间
        {"DATETIME"  ,&cmd_datetime},          //12　设置或读取设备日期与时间
        {"FTD"       ,&cmd_ftd},               //13　设置或读取设备主动模式下的发送时间间隔
        {"STDEV"     ,&cmd_stdev},             //14　设置或读取设备数据标准偏差值计算的时间间隔
        {"FAT"       ,&cmd_fat},               //15　设置或读取设备主动模式下数据发送时间
        {"SETCOMWAY" ,&cmd_setcomway},         //16　设置握手机制方式
        {"SS"        ,&cmd_ss},                //17　读取设备各工作参数值
        {"STAT"      ,&cmd_stat},              //18　读取设备工作状态数据
        {"AT"        ,&cmd_at},                //19　设置或读取设备正常工作温度范围
        {"VV"        ,&cmd_vv },               //20　设置或读取仪器正常工作电压范围
        {"SN"        ,&cmd_sn},                //21　设置或读取设备序列号
        {"QCPS"      ,&cmd_qcps},              //22　设置或读取采样值质量控制参数
        {"QCPM"      ,&cmd_qcpm},              //23　设置或读取瞬时气象值质量控制参数
        {"CR"        ,&cmd_cr},                //24　设置或读取设备的校正、检定信息
        {"READDATA"  ,&cmd_readdata},          //25　实时读取数据
        {"DOWN"      ,&cmd_down},              //26　历史数据下载
        {"RESET"     ,&cmd_reset},             //27　重启设备

        {"SUPERADMIN",&cmd_superadmin},        //29　操作权限指令
        {"UNIT"      ,&cmd_unit},              //30  气压单位修改命令

//------------------------------ 私有命令 -------------------------------------
        {"DEBUGON"   ,&cmd_debugon},           //1 调试模式开
        {"DEBUGOFF"  ,&cmd_debugoff},          //2 调试模式关
        {"VERSION"   ,&cmd_version},           //3 版本号读取
        {"RESTORE"   ,&cmd_restore},           //4 恢复出厂设置
        {"STARTTIME" ,&cmd_starttime},         //5 设备开始运行时间
        {"SECDOUTON" ,&cmd_secdout_on},        //6 输出秒级数据
        {"SECDOUTOFF",&cmd_secdout_off},       //7 关闭秒级数据输出
        {"ERASECR"   ,&cmd_erase_check},       //
        {"CFC"       ,&cmd_cfc},               //8 标定传感器系数
        {"WINDH"     ,&cmd_windh},             //9 风拨码开关设置
        {"STSENSOR"  ,&cmd_stsensor},          //10 地温传感器数量及深度配置
        {"SHSENSOR"  ,&cmd_shsensor},          //11 土壤水分传感器数量及深度配置
        {"VERSIONCFG",&cmd_version_cfg},       //12 版本号写入
    }
};


int save_element(short temp_num, float temp_min, float temp_max, char *temp_doubt, char *temp_err, char *temp_change, qm_t *temp_qm);   //存储QS
int check_element(short temp_num, qm_t *temp_qm);                //校验QS
int save_sensor(short temp_num, float temp_min, float temp_max, char *temp_change, qs_t *temp_qs);
int check_sensor(short temp_num, qs_t *temp_qs);

int getSeparator(char *rcvdata)
{
    U8 i=0;
    for( i=0;i<strlen(rcvdata);i++)
    {
        if((rcvdata[i]==',') || (rcvdata[i]==' ') || (rcvdata[i]==0x0D)|| (rcvdata[i]==0x0A))
        {
            break;
        }
    }
    return i;
}

char data_II[64], data_buf[MAX_PKGLEN];
U16 uartcmd_process(U8 uartno,char *data,U16 len,char *rbuf)
{
    
    int index,ret;
    U8 i;
    char *temp_di = NULL, *p = NULL, *temp_id = NULL;
    unsigned short id_num;
    index=getSeparator(data);

    if((index>20)||(index<2))
    {
        //  return sprintf(rbuf, "<BADCOMMAND>\r\n");
        return 0;
    }
    else
    {
        memset(data_II,0,64);
        memcpy(data_II,data,index);//指令copy到data_II
    }

    m_tempdata.currentUartIndex=uartno;


    //解析 data 提取ID DI.........................................
    i = 0;
    memset(data_buf,0,MAX_PKGLEN);
    memcpy(data_buf,data,len);
    if(0 == Utils_CheckCmdStr(data_buf))
        {return 0;}
    p = strtok(data_buf, ",");
    while(p)
    {
        switch(i)
        {
        case 1: temp_di = p;  break;//校验DI
        case 2: temp_id = p;  break;//校验 ID
        default:    {break;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    //判DI （DI != 设备DI 并且 ！= YALL 判错 跳出）
    if((0 != strcmp(temp_di,(char *)&sensor_di)) && (0 != strcmp(temp_di,"YALL")))
        {return 0; }

    //判ID
    if(0 == strcmp(temp_di,"YALL")) //DI=YALL
    {
        if(0 != strcmp(temp_id,"FFF"))   { return 0; }
    }
    else //DI=设备DI
    {
        if( 3 != strlen(temp_id))   //ID长度为3
            { return 0; }
        if(-1 == check_digit(temp_id, strlen(temp_id)))
            { return 0; }
        id_num = atoi(temp_id);
        if(id_num != m_defdata.m_baseinfo.id)
            { return 0; }

    }
     //判命令
    for(  i=0; i<MAX_COMMAND_NUM; i++)
    {
        //  if(0== memcmp(data_II, bcm_control.dispatch[i].name,index))
        if(0== strcmp(data_II, bcm_control.dispatch[i].name))
        {
            ret = (bcm_control.dispatch[i].action)(data, rbuf);     //回调函数
            m_tempdata.currentCmdIndex=i;
            return ret;
        }
    }
    return  sprintf(rbuf, "<%s,%03d,BADCOMMAND>\r\n",sensor_di,m_defdata.m_baseinfo.id);
}



int check_closecom(char *buf,char *rbuf)
{
    int rlen=0;
    if(!memcmp(buf,"CLOSECOM",8))
    {
        m_tempdata.m_opencomset_t.flag=false;
        m_tempdata.m_opencomset_t.counter=0;
        rlen=sprintf(rbuf,"<T>\r\n");
        return rlen;
    }
    else
        return 0;
}

time_t get_softimer(s_RtcTateTime_t *temp_t)
{
    struct tm temp;
    time_t data = 0;

    temp.tm_year = temp_t->year - 1900;
    temp.tm_mon = temp_t->month - 1;
    temp.tm_mday = temp_t->day;
    temp.tm_hour = temp_t->hour;
    temp.tm_min = temp_t->min;
    temp.tm_sec = temp_t->sec;
    data = mktime(&temp);
    data = data / 60;

    return data;
}

time_t get_softimer_sec(s_RtcTateTime_t *temp_t)
{
    struct tm temp;
    time_t data = 0;

    temp.tm_year = temp_t->year - 1900;
    temp.tm_mon = temp_t->month - 1;
    temp.tm_mday = temp_t->day;
    temp.tm_hour = temp_t->hour;
    temp.tm_min = temp_t->min;
    temp.tm_sec = temp_t->sec;
    data = mktime(&temp);

    return data;
}

/*==================================================================
*函数：             cmd_setcom
*作者：
*日期：           2017-12-07
*功能：           B.1　设置或读取设备的通讯参数
           参数： 设备标识符,设备ID,波特率,数据位,奇偶校验,停止位
           波特率范围：（1200,2400,4800,9600, 19200,38400,57600,115200）
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_setcom(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    unsigned long int temp_rate = 0;
    unsigned char temp_data, temp_stop;
             char *temp_par = NULL;
    char num = 0;
    unsigned long  baudrate_tab[] = {1200,2400,4800,9600, 19200,38400,57600,115200};

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2: break;
         /* {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
            if(-1 == check_digit(p, strlen(p)))//校验 串口波特率 是否为数字
            {goto err;}
            temp_rate = atol(p);
            break;
          }
        case 4:
          {
            if(-1 == check_digit(p, 1))//校验 数据位 是否为数字
            {goto err;}
            temp_data = atoi(p);
            break;
          }
        case 5:{ temp_par = p; break;}
        case 6:
          {
            if(-1 == check_digit(p, 1))//校验  停止位 是否为数字
            {goto err;}
            temp_stop = atoi(p);
            break;
          }
        case 7: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 7) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
      { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
      { return 0; }*/
    if(i == 3)  // 打印 读取
    {
    rlen = sprintf((char *)rbuf,"<%s,%03d,%ld,%d,%c,%d>\r\n",
                              sensor_di,
                              m_defdata.m_baseinfo.id,
                              bcm_info.common.se[0].baudrate,
                              bcm_info.common.se[0].datasbit,
                              bcm_info.common.se[0].parity,
                              bcm_info.common.se[0].stopbits);
                return rlen;
    }

    //2 校验 串口波特率  并写入
    for(num = 0; num < 8; num ++)
    {
        if(temp_rate == baudrate_tab[num])
        {   bcm_info.common.se[0].baudrate = temp_rate;
            break;
        }
    }
    if(num == 8)
    {goto err;}
    //3 校验 数据位  并写入
    if((temp_data < 5) || (temp_data > 8))
    {goto err;}
    bcm_info.common.se[0].datasbit = temp_data;

    //4 校验 校验位  并写入
    if((0 != strncmp((char *)temp_par,"N",1)) && (0 != strncmp((char *)temp_par,"O",1)) && (0 != strncmp((char *)temp_par,"E",1)))
    {goto err;}
    strncpy((char *)&bcm_info.common.se[0].parity,temp_par,1);

    //5 校验 校验位  并写入
    if((1 != temp_stop) && (2 != temp_stop))
    {goto err;}
    bcm_info.common.se[0].stopbits = temp_stop;

    //6 写入FLASH  并返回成功
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n%ld,%d,%c,%d\r\n",
                              sensor_di,m_defdata.m_baseinfo.id,

                              bcm_info.common.se[0].baudrate,
                              bcm_info.common.se[0].datasbit,
                              bcm_info.common.se[0].parity,
                              bcm_info.common.se[0].stopbits);
    m_tempdata.event.uart_config = true;
    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*函数：             cmd_autocheck
*作者：
*日期：           2017-12-07
*功能：          B.2　设备自检
         设备日期、时间，通讯端口的通讯参数，设备状态信息(厂家自定义)等
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_autocheck(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
     /*   if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
        }*/
        p = strtok(NULL, ",");
        i++;
    }
  if(i != 3) {goto err;}
    //1、验证设备标识符,ID是否正确
    //验证DI
 /*   if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/

     // 打印 读取
    rlen = sprintf((char *)rbuf,"<%s,%03d,T,%04d-%02d-%02d,%02d:%02d:%02d,%ld,%d,%c,%d>\r\n",
                  sensor_di,
                  m_defdata.m_baseinfo.id,

                  m_tempdata.m_RtcTateTime.year,
                  m_tempdata.m_RtcTateTime.month,
                  m_tempdata.m_RtcTateTime.day,
                  m_tempdata.m_RtcTateTime.hour,
                  m_tempdata.m_RtcTateTime.min,
                  m_tempdata.m_RtcTateTime.sec,

                  bcm_info.common.se[0].baudrate,
                  bcm_info.common.se[0].datasbit,
                  bcm_info.common.se[0].parity,
                  bcm_info.common.se[0].stopbits
                  //设备状态信息（厂家自行定义格式不定）
                  );
    return rlen;


err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}

/*==================================================================
*函数：             cmd_help
*作者：
*日期：           2017-12-07
*功能：          B.3　帮助命令
         设备标识符，设备ID，终端命令清单
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_help(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;

  if(0 == Utils_CheckCmdStr(buf))
  {goto err;}
  p = strtok(buf, ",");
  while(p)
  {
   /*   if(1 == i) {temp_di = p;}
      if(2 == i)
      {
          if(-1 == check_digit(p, 3))//校验 ID是否为数字
          {goto err;}
          temp_id = atoi(p);
      }*/
      p = strtok(NULL, ",");
      i++;
  }
  if(i != 3) {goto err;}
  //1、验证设备标识符,ID是否正确
  //验证DI
 /* if(0 != strcmp(temp_di,(char *)&sensor_di))
      {goto err;}
  //验证ID
  if(temp_id != m_defdata.m_baseinfo.id)
      {goto err;}*/

    rlen = sprintf((char *)rbuf,"<%s,%03d,SETCOM,AUTOCHECK,HELP,QZ,ST,DI,ID,LAT,LONG,UNIT,DATE,TIME,DATETIME,FTD,STDEV,FAT,SETCOMWAY,"
                   "SS,STAT,AT,VV,SN,QCPS,QCPM,CR,READDATA,DOWN,RESET>\r\n",
                         sensor_di,
                         m_defdata.m_baseinfo.id
                                         );
    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*函数：             cmd_qz
*作者：
*日期：           2017-12-07
*功能：        B.4　设置或读取设备的区站号
         设备标识符,设备ID ,设备区站号（6位字符）
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_qz(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    unsigned long temp_qz = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;
         /* {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
            if(-1 == check_digit(p, 5))//校验 区站号 是否为数字 5位
            {goto err;}
            temp_qz = atol(p);
            break;
          }
        case 7: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
    rlen = sprintf((char *)rbuf,"<%s,%03d,%ld>\r\n",
                              sensor_di,
                              m_defdata.m_baseinfo.id,
                              bcm_info.common.qz);
                return rlen;
    }

    //2 校验QZ 是否为5位数
    if((temp_qz >99999) || (temp_qz < 10000))
      { goto err; }

    //3 写入FLASH  并返回成功
    bcm_info.common.qz = temp_qz;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
      rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}

/*==================================================================
*函数：             cmd_st
*作者：
*日期：           2017-12-07
*功能：       B. 5　设置或读取设备的服务类型
         设备标识符,设备ID 服务类型（2位数字）
示例：00基准站，01基本站，02一般站，03区域气象站，04交通气象站，05电力气象站，06农业气象站，07旅游气象站，
08海洋气象站，09风能气象站，10太阳能气象站，11生态气象站，12气象辐射站，13便携站，14自动气候站，15 大气本底站，
16 大气成分站，17 沙尘暴站，18环境气象站。
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_st(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    short temp_st = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
         /* {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
            if(-1 == check_digit(p, 2))//校验 服务类型  是否为数字 2位
            {goto err;}
            temp_st = atoi(p);
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
    rlen = sprintf((char *)rbuf,"<%s,%03d,%02d>\r\n",
                              sensor_di,
                              m_defdata.m_baseinfo.id,
                              bcm_info.common.st);
                return rlen;
    }

    //2 校验ST 是否在规定范围内
    if((temp_st > 99) || (temp_st < 0))
      { goto err; }

    //3 写入FLASH  并返回成功
    bcm_info.common.st = temp_st;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}

/*==================================================================
*函数：              cmd_di
*作者：
*日期：           2017-12-08
*功能： B.6　读取设备标识位
      设备标识符,FFF
"YAWS","YROS","YCCL","YCCR","YCLR","YFSV","YLTV","YWTQ","YWTR","YWDP","YWDY"
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_di(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    char *dev_di = NULL;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i) {dev_di  = p;}
        p = strtok(NULL, ",");
        i++;
    }
      if(i != 3) {goto err;}
    //1、验证
    //验证YALL
   if(0 != strcmp(temp_di,"YALL"))
      { goto err; }
    
    //验证FFF 
    if(3 != strlen(dev_di))   { goto err; }
	if(0 != strncmp(dev_di,"FFF",3))//校验 是否为FFF
      { goto err; }
   
    //打印 读取
    rlen = sprintf((char *)rbuf,"<%s,%03d>\r\n",
                           sensor_di,
                           m_defdata.m_baseinfo.id
                                           );
     return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}

/*==================================================================
*函数：              cmd_id
*作者：
*日期：           2017-12-08
*功能： B.7　设置或读取设备
      设备标识符,3位数字
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_id(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    char *dev_id = NULL;
    short temp_id = 0;
    short old_id = 0;
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i) {dev_id = p;}
        if(3 == i){
            if(-1 == check_digit(p, strlen(p))) //校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }
      if((i != 3) && (i != 4)) {goto err;}
    //1、验证
    //验证YALL
    if((0 == strcmp(temp_di,"YALL")) && (i == 3)) //读取
   {
     if(3 != strlen(dev_id) )   {goto err;}
     if(0 == strncmp(dev_id,"FFF",3))
     {   //打印 读取
        rlen = sprintf((char *)rbuf,"<%s,%03d>\r\n",
                           sensor_di,
                           m_defdata.m_baseinfo.id
                                           );
        return rlen;
     }
     else
     {return 0;}
   }
    else if((0 == strcmp(temp_di,(char *)sensor_di)) && (i == 4))	//设置
    {
      if(3 != strlen(dev_id) )   {return 0;}
      if(-1 == check_digit(dev_id, 3)) //校验 ID是否为数字
          {return 0;}
      if(atoi(dev_id) != m_defdata.m_baseinfo.id)
          { return 0; }
      if((temp_id <0) || (temp_id > 999))
          {goto err;}

      old_id = m_defdata.m_baseinfo.id;
      m_defdata.m_baseinfo.id = temp_id;

    }
    else
      { goto err; }

    //7、 存入 flash 并返回
    pamsave(&m_defdata);

    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,old_id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}

/*==================================================================
*函数：              cmd_lat
*作者：
*日期：           2017-12-08
*功能：B. 8　设置或读取气象观测站的纬度
    设备标识符,3位数字
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_lat(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int degree, min, sec;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
          /*{
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
            if(-1 == check_format(p, strlen(p), ".", 2))
              {goto err;}
            if(sscanf(p,"%d.%d.%d",&degree,&min,&sec)!=3)
              {goto err;}
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
			rlen = sprintf((char *)rbuf,"<%s,%03d,%02d.%02d.%02d>\r\n",
                                             sensor_di,
                                             m_defdata.m_baseinfo.id,
                                             bcm_info.common.Lat.degree,
                                             bcm_info.common.Lat.min,
                                             bcm_info.common.Lat.sec
                                             );
       return rlen;;
    }
    
    
    //2 验证lat数值准确性
    if((degree < 0) || (degree > 90))
    	{ goto err; }
    if((min < 0) || (min > 59))
    	{ goto err; }
    if((sec < 0) || (sec > 59))
    	{ goto err; }	
    if((degree == 90) && ((min != 0) || (sec != 0)))
        { goto err; }
    //3 写入 存入flash 并返回
    bcm_info.common.Lat.degree = degree;
    bcm_info.common.Lat.min    = min;
    bcm_info.common.Lat.sec    = sec;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}
/*==================================================================
*函数：              cmd_long
*作者：
*日期：           2017-12-08
*功能：B. 9　设置或读取气象观测站的经度
    设备标识符,3位数字
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_long(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int degree, min, sec;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
       /*   {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
          	if(-1 == check_format(p, strlen(p), ".", 2))
          	 {goto err;}
            if(sscanf(p,"%d.%d.%d",&degree,&min,&sec)!=3)
             {goto err;}
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%03d.%02d.%02d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.Long.degree,
                                         bcm_info.common.Long.min,
                                         bcm_info.common.Long.sec
                                         );
       return rlen;;
    }
    
    //2 验证long数值准确性
    if((degree < 0) || (degree > 180))
    	{ goto err; }
    if((min < 0) || (min > 59))
    	{ goto err; }
    if((sec < 0) || (sec > 59))
    	{ goto err; }	
    if((degree == 180) && ((min != 0) || (sec != 0)) )
        { goto err; }
    //3 写入 存入flash 并返回
    bcm_info.common.Long.degree = degree;
    bcm_info.common.Long.min    = min;
    bcm_info.common.Long.sec    = sec;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}
/*==================================================================
*函数：              cmd_asl
*作者：
*日期：           2017-12-08
*功能： B.28　设置或读取气象观测站的海拔，  增加命令
    设备标识符,3位数字，5位数字，扩大十倍存储
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_asl(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    long temp_asl = 0;

    if(0 == Utils_CheckCmdStr(buf))
     {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
         /* {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
            if(5 < strlen(p))
            {goto err;}
            if((0 != strncmp(p,"-",1)) && (-1 == check_digit(p, 1)))
             {goto err;}
          	if(-1 == check_digit(p+1, strlen(p+1)))//校验 海拔
          	 {goto err;}
            temp_asl = atol(p);
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%05ld>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.High
                                         );
       return rlen;;
    }
    
    //2 验证long数值准确性
    if((temp_asl < -1540) || (temp_asl > 88440))//临时选取 8844，珠峰高度， -154 吐鲁番盆地
    	{ goto err; }

    //3 写入 存入flash 并返回
    bcm_info.common.High = temp_asl;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}
/*==================================================================
*函数：              cmd_date
*作者：
*日期：           2017-12-10
*功能：  B.10　设置或读取设备日期
    设备标识符,设备ID,YYYY-MM-DD（YYYY为年，MM为月，DD为日）
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_date(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int year, mon, day;
    s_RtcTateTime_t temp_datetime;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
         /* {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
          	if(10 !=  strlen(p)) //校验长度
          	 {goto err;}
          	 if(-1 == check_format(p, strlen(p), "-", 2))
          	 {goto err;}
            if(sscanf(p,"%d-%d-%d",&year,&mon,&day)!=3)
             {goto err;}
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%04d-%02d-%02d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         m_tempdata.m_RtcTateTime.year,
                                         m_tempdata.m_RtcTateTime.month,
                                         m_tempdata.m_RtcTateTime.day
                                         );
       return rlen;;
    }
    
    //2 校验日期数值准确性
        if( -1 == check_data(year, mon, day))
            {goto err; }


        temp_datetime.year  = year;
        temp_datetime.month = mon;
        temp_datetime.day   = day;
        temp_datetime.hour  = m_tempdata.m_RtcTateTime.hour;
        temp_datetime.min   = m_tempdata.m_RtcTateTime.min;
        temp_datetime.sec   = m_tempdata.m_RtcTateTime.sec;
        check_datetime(&temp_datetime);

        //3 写入结构体 写入时钟 返回正确
        m_tempdata.m_RtcTateTime.year  =year;
        m_tempdata.m_RtcTateTime.month =mon;
        m_tempdata.m_RtcTateTime.day   =day;
        //DS3231_SetTime(&m_tempdata.m_RtcTateTime);
        rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
          return rlen;

    err:
      rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
      return rlen;
}
/*==================================================================
*函数：              cmd_time
*作者：
*日期：           2017-12-10
*功能：  B.11　设置或读取设备时间
    设备标识符,设备ID,HH:MM:SS（HH为时，MM为分，SS为秒）
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_time(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int hour ,min, sec;
    s_RtcTateTime_t temp_datetime;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
         /* {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
            break;
          }   */
        case 3:
          {
          	if(8 !=  strlen(p)) //校验长度
          	 {goto err;}
          	 if(-1 == check_format(p, strlen(p), ":", 2))
          	 {goto err;}
            if(sscanf(p,"%d:%d:%d",&hour ,&min, &sec)!=3)
             {goto err;}
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%02d:%02d:%02d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         m_tempdata.m_RtcTateTime.hour,
                                         m_tempdata.m_RtcTateTime.min,
                                         m_tempdata.m_RtcTateTime.sec
                                         );
       return rlen;;
    }
    
    //2 校验日期数值准确性
    if( -1 == check_time(hour, min, sec))
        {goto err; }

    temp_datetime.year  = m_tempdata.m_RtcTateTime.year;
    temp_datetime.month = m_tempdata.m_RtcTateTime.month;
    temp_datetime.day   = m_tempdata.m_RtcTateTime.day;
    temp_datetime.hour  = hour;
    temp_datetime.min   = min;
    temp_datetime.sec   = sec;
    check_datetime(&temp_datetime);

    //3 写入结构体 写入时钟 返回正确
    m_tempdata.m_RtcTateTime.hour  =hour;
    m_tempdata.m_RtcTateTime.min   =min;
    m_tempdata.m_RtcTateTime.sec   =sec;
    //DS3231_SetTime(&m_tempdata.m_RtcTateTime);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}

/*==================================================================
*函数：              cmd_datetime
*作者：
*日期：           2017-12-10
*功能：  B.12　设置或读取设备日期与时间
    设备标识符,设备ID,YYYY-MM-DD,HH:MM:SS
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_datetime(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int year, mon, day, hour ,min, sec;
    s_RtcTateTime_t temp_datetime;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
         /* {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
        {
        	if(10 !=  strlen(p)) //校验长度 不带 0x0d+0x0a
        	 {goto err;}
        	 if(-1 == check_format(p, strlen(p), "-", 2))
        	 {goto err;}
          if(sscanf(p,"%d-%d-%d",&year,&mon,&day)!=3)
           {goto err;}
          break;
        }  
        case 4:
          {
          	if(8 !=  strlen(p)) //校验长度
          	 {goto err;}
          	 if(-1 == check_format(p, strlen(p), ":", 2))
          	 {goto err;}
            if(sscanf(p,"%d:%d:%d",&hour ,&min, &sec)!=3)
             {goto err;}
            break;
          }
        case 5: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 5) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%04d-%02d-%02d,%02d:%02d:%02d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,

                                         m_tempdata.m_RtcTateTime.year,
                                         m_tempdata.m_RtcTateTime.month,
                                         m_tempdata.m_RtcTateTime.day,
                                         
                                         m_tempdata.m_RtcTateTime.hour,
                                         m_tempdata.m_RtcTateTime.min,
                                         m_tempdata.m_RtcTateTime.sec
                                         );
       return rlen;
    }
 		
 		//2 校验日期数值准确性
    if( -1 == check_data(year, mon, day))
        {goto err; }               
    //3 校验时间数值准确性
    if( -1 == check_time(hour, min, sec))
        {goto err; }

    temp_datetime.year  = year;
    temp_datetime.month = mon;
    temp_datetime.day   = day;
    temp_datetime.hour  = hour;
    temp_datetime.min   = min;
    temp_datetime.sec   = sec;
    check_datetime(&temp_datetime);

    //4 写入结构体 写入时钟 返回正确
    m_tempdata.m_RtcTateTime.year  =year;
    m_tempdata.m_RtcTateTime.month =mon;
    m_tempdata.m_RtcTateTime.day   =day;

    m_tempdata.m_RtcTateTime.hour  =hour;
    m_tempdata.m_RtcTateTime.min   =min;
    m_tempdata.m_RtcTateTime.sec   =sec;
    
    //DS3231_SetTime(&m_tempdata.m_RtcTateTime);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*函数：              cmd_ftd
*作者：
*日期：           2017-12-10
*功能：  B.13　设置或读取设备日期与时间
            设备标识符,设备ID ,FI ,mmC
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_ftd(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    short temp_fi,temp_min;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
        /*{
          if(-1 == check_digit(p, 3))//校验 ID是否为数字
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(-1 == check_digit(p, 3))//校验 FI是否为数字
           {goto err;}
        	temp_fi=atoi(p);
          break;
        }  
        case 4:
        {
        	if(-1 == check_digit(p, 2))//校验 时间间隔
           {goto err;}
          if(0!= strncmp(p+2,"M",1))
           {goto err;} 
      		temp_min=atoi(p);
        	break;
        }
        case 5: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 5) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,001,%02dM>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.ftd
                                         );
       return rlen;
    }
    
    
    //2 校验FI 只能是001
 	  if(temp_fi != 1) 
  	{ goto err; }  
    //3 校验发送秒
    if((temp_min < 1 ) || (temp_min > 59 ))
    { goto err; } 
    	
    //4 写入结构体 写入时钟 返回正确
    bcm_info.common.ftd = temp_min;
    save_sys_cfg(&bcm_info);	
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	

}
/*==================================================================
*函数：              cmd_stdev
*作者：
*日期：           2017-12-10
*功能：  B.14　设置或读取设备数据标准偏差值计算的时间间隔
            设备标识符,设备ID ,FI ,mmC
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_stdev(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    short temp_fi,temp_stdev;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
     /*   {
          if(-1 == check_digit(p, 3))//校验 ID是否为数字
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(-1 == check_digit(p, 3))//校验 FI是否为数字
           {goto err;}
        	temp_fi=atoi(p);
          break;
        }  
        case 4:
        {
        	if(-1 == check_digit(p, 2))//校验 时间间隔
           {goto err;}
          if(0!= strncmp(p+2,"M",1))
           {goto err;} 
          temp_stdev=atoi(p);
        	break;
        }
        case 5: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 5) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,001,%02dM>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.stdev
                                         );
       return rlen;
    }
    
    
    //2 校验FI 只能是001
 	  if(temp_fi != 1) 
  	{ goto err; }  
    //3 校验发送秒
    if((temp_stdev < 1 ) || (temp_stdev > 59 ))
    { goto err; } 
    	
    //4 写入结构体 写入时钟 返回正确
    bcm_info.common.stdev = temp_stdev;
    save_sys_cfg(&bcm_info);	
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	
}
/*==================================================================
*函数：              cmd_fat
*作者：
*日期：           2017-12-10
*功能：  B.15　设置或读取设备主动模式下数据发送时间
            设备标识符,设备ID,FI,mmm
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_fat(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    short temp_fi,temp_sec;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
      /*  {
          if(-1 == check_digit(p, 3))//校验 ID是否为数字
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(-1 == check_digit(p, 3))//校验 FI是否为数字
           {goto err;}
        	temp_fi=atoi(p);
          break;
        }  
        case 4:
        {
        	if(-1 == check_digit(p, 3))//校验 时间间隔  是否为数字
           {goto err;}
      		temp_sec=atoi(p);
        	break;
        }
        case 5: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 5) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%03d,%03d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.fi,
                                         bcm_info.common.ft
                                         );
       return rlen;
    }
    
    
    //2 校验FI 只能是001
 	  if(temp_fi != 1) 
  	{ goto err; }  
    //3 校验发送秒
    if((temp_sec < 0 ) || (temp_sec > 600 ))
    { goto err; } 
    	
    //4 写入结构体 写入时钟 返回正确
    bcm_info.common.ft = temp_sec;
    save_sys_cfg(&bcm_info);	
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	
  
}
/*==================================================================
*函数：              cmd_setcomway
*作者：
*日期：           2017-12-10
*功能：  B.16　设置握手机制方式
     设备标识符,设备ID,握手方式（1为主动发送方式，0为被动读取方式） 默认被动
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_setcomway(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    char com_way = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
     /*   if(1 == i)  {temp_di = p;}
        if(2 == i)
          {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
          }*/
        if(3 == i) 
        	{
        	if(-1 == check_digit(p, 1))//校验  握手方式是否为数字
            {goto err;}
            com_way = atoi(p);
        	}
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}
    //1、验证
    //验证DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
       {
           rlen = sprintf((char *)rbuf,"<%s,%03d,%d>\r\n",
                                            sensor_di,
                                            m_defdata.m_baseinfo.id,
                                            bcm_info.common.sw
                                            );
          return rlen;
       }

 
 	  //2 校验设置值
 	  if((0 != com_way) && (1 != com_way))
 	  	{ goto err; }
 	  //3 设置通信方式 返回成功
    bcm_info.common.sw = com_way;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",
                           sensor_di,
                           m_defdata.m_baseinfo.id
                                           );
     return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}
char buf_QS[90]={0}, buf_QM[150]={0};
/*==================================================================
*函数：             cmd_ss
*作者：
*日期：           2017-12-10
*功能：          B.17　读取设备各工作参数值
设备各工作参数值（应包括所有进行设置的参数，参数顺序按照相应命令出现的顺序。
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_ss(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, j = 0;
    int  rlen = 0, len_QS = 0, len_QM = 0;
    

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
    /*    if(1 == i)  {temp_di = p;}
        if(2 == i)
          {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
          }*/
        p = strtok(NULL, ",");
        i++;
    }
      if(i != 3) {goto err;}
    //1、验证
    //验证DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }
 */
    //QCPS,QCPM
    for(i = 0; i < DEV_TYPE_MAX; i ++)
    {
        if(0 == strncmp((char *)&sensor_di,(char *)dev_type[i],4))
          break;
    }
    if(i == 6)   //风
    {

        len_QS = sprintf((char *)buf_QS,",1,%.1f,%.1f,%.1f,2,%.1f,%.1f,/",
                                    bcm_info.sensor.qs[0].min,      //下限
                                    bcm_info.sensor.qs[0].max,      //上限
                                    bcm_info.sensor.qs[0].change,

                                    bcm_info.sensor.qs[1].min,      //下限
                                    bcm_info.sensor.qs[1].max       //上限
                                    );
       for(j = 11 ; j < 15 ; j++)//风速
           len_QM +=  sprintf((char *)buf_QM+len_QM,",%d,%.1f,%.1f,%.1f,%.1f,/",
                          j,
                          bcm_info.sensor.qm[j-11].min,        //下限
                          bcm_info.sensor.qm[j-11].max,        //上限
                          bcm_info.sensor.qm[j-11].doubtful,   //存疑门限
                          bcm_info.sensor.qm[j-11].err         //错误门限
                          );
       for(j = 21 ; j < 25 ; j++)//风向
          len_QM +=  sprintf((char *)buf_QM+len_QM,",%d,%.1f,%.1f,/,/,%.1f",
                         j,
                         bcm_info.sensor.qm[j-17].min,        //下限
                         bcm_info.sensor.qm[j-17].max,        //上限
                         bcm_info.sensor.qm[j-17].changerate  //最小应该变化的速率
                         );
    }
    else if(i == 9)//地温
    {
        for(j  = 0 ; j < MAX_SENSOR_NUM ; j++)
            len_QS += sprintf((char *)buf_QS+len_QS,",%d,%.1f,%.1f,%.1f",
                                     j+1,
                                     bcm_info.sensor.qs[j].min,      //下限
                                     bcm_info.sensor.qs[j].max,      //上限
                                     bcm_info.sensor.qs[j].change
                                     );
        for(j  = 0 ; j < MAX_SENSOR_NUM ; j++)
            len_QM +=  sprintf((char *)buf_QM+len_QM,",%d,%.1f,%.1f,%.1f,%.1f,/",
                                    j+11,
                                    bcm_info.sensor.qm[j].min,        //下限
                                    bcm_info.sensor.qm[j].max,        //上限
                                    bcm_info.sensor.qm[j].doubtful,   //存疑门限
                                    bcm_info.sensor.qm[j].err         //错误门限
                                    );
    }
    else
    {
        for(j  = 0 ; j < MAX_SENSOR_NUM ; j++)
            len_QS += sprintf((char *)buf_QS+len_QS,",%d,%.1f,%.1f,%.1f",
                                     j+1,
                                     bcm_info.sensor.qs[j].min,      //下限
                                     bcm_info.sensor.qs[j].max,      //上限
                                     bcm_info.sensor.qs[j].change
                                     );
        for(j  = 0 ; j < MAX_SENSOR_NUM ; j++)
            len_QM +=  sprintf((char *)buf_QM+len_QM,",%d,%.1f,%.1f,%.1f,%.1f,%.1f",
                                    j+11,
                                    bcm_info.sensor.qm[j].min,        //下限
                                    bcm_info.sensor.qm[j].max,        //上限
                                    bcm_info.sensor.qm[j].doubtful,   //存疑门限
                                    bcm_info.sensor.qm[j].err,         //错误门限
                                    bcm_info.sensor.qm[j].changerate  //最小应该变化的速率
                                    );
    }

 	  //2 输出状态 返回成功
    rlen = sprintf((char *)rbuf,"<%s,%03d,SETCOM,%ld,%d,%c,%d,"
                           "QZ,%ld,ST,%02d,DI,%s,ID,%03d,"
                           "LAT,%02d.%02d.%02d,LONG,%03d.%02d.%02d,ASL,%ld,"
                           "DATE,%04d-%02d-%02d,TIME,%02d:%02d:%02d,"
                           "FTD,%02dM,STDEV,%02dM,FAT,%03d,SETCOMWAY,%01d,"
                           "STAT,%05d,%05d,%05d,AT,%05d,%05d,VV,%05d,%05d,SN,%s,",
                           sensor_di,
                           m_defdata.m_baseinfo.id,
                           
                           bcm_info.common.se[0].baudrate,
                           bcm_info.common.se[0].datasbit,
                           bcm_info.common.se[0].parity,
                           bcm_info.common.se[0].stopbits,
                           
                           bcm_info.common.qz,
                           bcm_info.common.st,
                           sensor_di,
                           m_defdata.m_baseinfo.id,
                           
                           bcm_info.common.Lat.degree,
                           bcm_info.common.Lat.min,
                           bcm_info.common.Lat.sec,
                           bcm_info.common.Long.degree,
                           bcm_info.common.Long.min,
                           bcm_info.common.Long.sec,
                           bcm_info.common.High,
                           
                           m_tempdata.m_RtcTateTime.year,
                           m_tempdata.m_RtcTateTime.month,
                           m_tempdata.m_RtcTateTime.day,
                           m_tempdata.m_RtcTateTime.hour,
                           m_tempdata.m_RtcTateTime.min,
                           m_tempdata.m_RtcTateTime.sec,

                           bcm_info.common.stdev,
                           bcm_info.common.ftd,
                           bcm_info.common.ft,
                           bcm_info.common.sw,

                           sensors_data.boardstemp_data,
                           sensors_data.boardsvolt_data,
                           sensors_data.boardsvolt_data,
                           bcm_info.common.work_temp_min,
                           bcm_info.common.work_temp_max,
                           bcm_info.common.boardsvolt_min,
                           bcm_info.common.boardsvolt_max,
                           m_defdata.m_baseinfo.sn
                           //待添加其他的工作参数-----------------------
                                           );

    rlen +=sprintf((char *)rbuf+rlen,"QCPS%s,QCPM%s>\r\n",buf_QS,buf_QM);

    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

}
/*==================================================================
*函数：             cmd_stat
*作者：
*日期：           2017-12-10
*功能：          B.18　读取设备工作状态数据
         设备标识符，设备ID
         包括内部电路温度、内部电路电压、外接电源电压，其他状态自定义
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_stat(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}

    p = strtok(buf, ",");
    while(p)
    {
       /* if(1 == i)  {temp_di = p;}
        if(2 == i)
          {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
          }*/
        p = strtok(NULL, ",");
        i++;
    }
      if(i != 3) {goto err;}
    //1、验证
    //验证DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
 
 	  //2 输出状态 返回成功
    rlen = sprintf((char *)rbuf,"<%s,%03d,%05d,%05d,%05d>\r\n",
                           sensor_di,
                           m_defdata.m_baseinfo.id,
                           sensors_data.boardstemp_data,
                           sensors_data.boardsvolt_data,
                           sensors_data.boardsvolt_data
                     //待添加其他的工作状态-----------------------
                                           );
     return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

}
/*==================================================================
*函数：              cmd_at
*作者：
*日期：           2017-12-11
*功能：  B.19　设置或读取设备正常工作温度范围
    设备标识符，设备ID，正常工作温度下限值，正常工作温度上限值,-100.0～100.0
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_at(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int temp_min,temp_max;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
       /* {
          if(-1 == check_digit(p, 3))//校验 ID是否为数字
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if((0 !=  strncmp(p,"-",1)) &&  (0 !=  strncmp(p,"0",1)))//验证最高为为 - 或 0
        	 {goto err;}
        	if(-1 == check_digit(p+1, 4))//校验 剩余是否为数字
           {goto err;}
        	temp_min=atoi(p);
          break;
        }  
        case 4:
        {
        	if((0 !=  strncmp(p,"-",1)) &&  (0 !=  strncmp(p,"0",1)))//验证最高为为 - 或 0
        	 {goto err;}
        	if(-1 == check_digit(p+1, 4))//校验 剩余是否为数字
           {goto err;}
      		temp_max=atoi(p);
        	break;
        }
        case 5: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 5) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
    /*if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%05d,%05d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.work_temp_min,
                                         bcm_info.common.work_temp_max
                                         );
       return rlen;
    }
    
    //2 校验 上下限范围
    if((temp_min < -1000 ) || (temp_min > 1000 ))
    	{ goto err; }
    if((temp_max < -1000 ) || (temp_max > 1000 ))
    	{ goto err; }	
    if(temp_max < temp_min )
    	{ goto err; }		
    	
    //3 写入结构体 写入时钟 返回正确
    bcm_info.common.work_temp_min = temp_min;
    bcm_info.common.work_temp_max = temp_max;
    save_sys_cfg(&bcm_info);

    //4 重新计算上下限
    board_temp_max = bcm_info.common.work_temp_max * 1.1;
    if(bcm_info.common.work_temp_min < 0)
    {
        board_temp_min = bcm_info.common.work_temp_min * 1.1;
    }
    else
    {
        board_temp_min = bcm_info.common.work_temp_min * 0.9;
    }

    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	
}
/*==================================================================
*函数：              cmd_vv
*作者：
*日期：           2017-12-11
*功能：  B.20　设置或读取仪器正常工作电压范围
    设备标识符，设备ID，正常工作电压下限值，正常工作电压上限值,
    值为5位，高位补零，取值范围为0～30V
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_vv(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int temp_min,temp_max;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2: break;//
       /* {
          if(-1 == check_digit(p, 3))//校验 ID是否为数字
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(-1 == check_digit(p, 5))//校验 剩余是否为数字
           {goto err;}
        	temp_min=atoi(p);
          break;
        }  
        case 4:
        {
        	if(-1 == check_digit(p, 5))//校验 剩余是否为数字
           {goto err;}
      		temp_max=atoi(p);
        	break;
        }
        case 5: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 5) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%05d,%05d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.boardsvolt_min,
                                         bcm_info.common.boardsvolt_max
                                         );
       return rlen;
    }
    
    //2 校验 上下限范围
    if((temp_min < 0 ) || (temp_min > 300 ))
    	{ goto err; }
    if((temp_max < 0 ) || (temp_max > 300 ))
    	{ goto err; }	
    if(temp_max < temp_min )
    	{ goto err; }		
    	
    //3 写入结构体 写入时钟 返回正确
    bcm_info.common.boardsvolt_min = temp_min;
    bcm_info.common.boardsvolt_max = temp_max;
    save_sys_cfg(&bcm_info);

    board_volt_max = bcm_info.common.boardsvolt_max * 1.1;
    board_volt_min = bcm_info.common.boardsvolt_min * 0.9;

    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	
}
/*==================================================================
*函数：              cmd_sn
*作者：
*日期：           2017-12-11
*功能：  B.21　设置或读取设备序列号
    设备标识符，设备ID
    操作需要权限
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_sn(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    char *temp_sn = NULL;
    unsigned char temp_chack_sn = 0, temp_chack_num = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}

    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2: break;//
        /*{
          if(-1 == check_digit(p, 3))//校验 ID是否为数字
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(SN_NUM != strlen(p))	//36位SN 装备类别码9+厂商识别码10+生产序列码14+校验3
           {goto err;}
        	temp_sn=p;
          break;
        }  
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // 打印 读取
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%s>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         m_defdata.m_baseinfo.sn
                                         );
       return rlen;
    }
    
    if(1 != m_tempdata.SuperadMin) //判定是否有操作权限
    {goto err;}

    //2 SN号 校验  只验证校验码
    temp_chack_num = atoi(temp_sn + SN_NUM - 3);
	for(i = 0; i< SN_NUM-3; i++)
	    temp_chack_sn = temp_chack_sn^(temp_sn[i]);
	if(temp_chack_sn != temp_chack_num)
	    {goto err;}
    	
    //3 清除结构体 写入结构体 写入时钟 返回正确
    memset(m_defdata.m_baseinfo.sn, 0x00, sizeof(m_defdata.m_baseinfo.sn));
    strncpy((char *)&m_defdata.m_baseinfo.sn, temp_sn,SN_NUM);
    pamsave(&m_defdata);

    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	
}
/*==================================================================
*函数：              cmd_qcps
*作者：
*日期：           2017-12-11
*功能：  B.22　设置或读取采样值质量控制参数
设备标识符，设备ID，感应元件标识编号，设备测量范围下限，设备测量范围上限，允许最大变化值。
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_qcps(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    short temp_num;
    float temp_min,temp_max;
    char *temp_change = NULL;
    
    qs_t temp_qs;
    
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
        /*{
          if(-1 == check_digit(p, 3))//校验 ID是否为数字
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(-1 == check_digit(p, 1))//校验 感应原件编号
           {goto err;}
        	temp_num=atoi(p);
          break;
        }  
        case 4:
        {
            if((0 !=  strncmp(p,"-",1)) &&(-1 == check_format(p, strlen(p), ".", 1)))//验证最高为为 - 或 0
            {goto err;}
            if((0 ==  strncmp(p,"-",1)) && (-1 == check_format(p+1, strlen(p+1), ".", 1)))//校验 测量范围下限
            {goto err;}
            temp_min=atof(p);
        	break;
        }
        case 5:
        {
        	if(-1 == check_format(p, strlen(p), ".", 1))//校验 测量范围上限
        	{goto err;} 
            temp_max=atof(p);
        	break;
        }
        case 6:
        {
            temp_change = p;
        	break;
        }
        case 7: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 7) && (i != 4))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
    /*if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }
     */
    if(-1 == check_sensor(temp_num, &temp_qs))
    	{ goto err;}
      
    if(i == 4)  // 打印 读取
    {
        for(i = 0; i < DEV_TYPE_MAX; i ++)
        {
            if(0 == strncmp((char *)&sensor_di,(char *)dev_type[i],4))
              break;
        }
        if((i == 6) && (temp_num == 2))   //风向
        {
            rlen = sprintf((char *)rbuf,"<%s,%03d,%d,%.1f,%.1f,/>\r\n",
                                        sensor_di,
                                        m_defdata.m_baseinfo.id,
                                        temp_num,
                                        temp_qs.min,
                                        temp_qs.max
                                        );
        }
        else
        {
            rlen = sprintf((char *)rbuf,"<%s,%03d,%d,%.1f,%.1f,%.1f>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         temp_num,
                                         temp_qs.min,
                                         temp_qs.max,
                                         temp_qs.change
                                         );

        }
        return rlen;
    }
    
    
    //2 校验感应原件标号
 	  if(-1 == save_sensor(temp_num, temp_min,temp_max, temp_change, &temp_qs))
    	{ goto err;} 

    //4 写入结构体 写入FLASH 返回正确
    memcpy(&bcm_info.sensor.qs[temp_num-1],&temp_qs,sizeof(qs_t));
      
    save_sys_cfg(&bcm_info);	
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	
}
/*==================================================================
*函数：              cmd_qcpm
*作者：
*日期：           2017-12-11
*功能： B.23　设置或读取瞬时气象值质量控制参数
设备标识符，设备ID，观测要素编号，要素极值下限，要素极值上限，存疑界限，错误界限，最小应该变化的速率。
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_qcpm(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    float temp_min,temp_max;
    char temp_num,*temp_change = NULL,*temp_doubt = NULL,*temp_err = NULL;
    qm_t temp_qm;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2: break;//
        /*{
          if(-1 == check_digit(p, 3))//校验 ID是否为数字
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
            if(-1 == check_digit(p, 2))//校验 观测要素编号
           {goto err;}
            temp_num=atoi(p);
          break;
        }  
        case 4:
        {
            if((0 !=  strncmp(p,"-",1)) &&(-1 == check_format(p, strlen(p), ".", 1)))//验证最高为为 - 或 0
              {goto err;}
            if((0 ==  strncmp(p,"-",1)) && (-1 == check_format(p+1, strlen(p+1), ".", 1)))//校验 测量范围下限
            {goto err;}
          temp_min=atof(p);
            break;
        }
        case 5:
        {
            if(-1 == check_format(p, strlen(p), ".", 1))//校验 测量范围上限
            {goto err;}
          temp_max=atof(p);
            break;
        }
        case 6:{temp_doubt = p; break;} // 存疑门限
        case 7:{temp_err   = p; break;} // 错误门限
        case 8:{temp_change= p; break;} // 最小应变量
        case 9: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 9) && (i != 4))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/

    if(-1 == check_element(temp_num, &temp_qm))
        { goto err;}

    if(i == 4)  // 打印 读取
    {
        for(i = 0; i < DEV_TYPE_MAX; i ++)
        {
            if(0 == strncmp((char *)&sensor_di,(char *)dev_type[i],4))
              break;
        }
        if(i == 6)    //风
        {//范围 11~14,21~24
            switch(temp_num)
           {
           case 11://风速
           case 12:
           case 13:
           case 14: rlen = sprintf((char *)rbuf,"<%s,%03d,%d,%.1f,%.1f,%.1f,%.1f,/>\r\n",
                                   sensor_di,
                                   m_defdata.m_baseinfo.id,
                                   temp_num,
                                   temp_qm.min,
                                   temp_qm.max,
                                   temp_qm.doubtful,
                                   temp_qm.err
                                   );
                       break;
           case 21: //风向
           case 22:
           case 23:
           case 24:rlen = sprintf((char *)rbuf,"<%s,%03d,%d,%.1f,%.1f,/,/,%.1f>\r\n",
                                  sensor_di,
                                  m_defdata.m_baseinfo.id,
                                  temp_num,
                                  temp_qm.min,
                                  temp_qm.max,
                                  temp_qm.changerate
                                  );
                      break;
           default:{goto err;}
           }
           return rlen;
        }
        else if(i == 9)    //地温
        {
            rlen = sprintf((char *)rbuf,"<%s,%03d,%d,%.1f,%.1f,%.1f,%.1f,/>\r\n",
                                               sensor_di,
                                               m_defdata.m_baseinfo.id,
                                               temp_num,
                                               temp_qm.min,
                                               temp_qm.max,
                                               temp_qm.doubtful,
                                               temp_qm.err
                                               );
            return rlen;
        }
        else
        {
          rlen = sprintf((char *)rbuf,"<%s,%03d,%d,%.1f,%.1f,%.1f,%.1f,%.1f>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         temp_num,
                                         temp_qm.min,
                                         temp_qm.max,
                                         temp_qm.doubtful,
                                         temp_qm.err,
                                         temp_qm.changerate
                                         );
          return rlen;
        }
    }
    
    
    //2 校验要素标号
    if(-1 == save_element(temp_num, temp_min,temp_max, temp_doubt, temp_err,temp_change, &temp_qm))
        { goto err;}


    //4 写入结构体 写入FLASH 返回正确
    for(i = 0; i < DEV_TYPE_MAX; i ++)
    {
        if(0 == strncmp((char *)&sensor_di,(char *)dev_type[i],4))
          break;
    }
    if(i == 6)    //风
    {//范围 11~14,21~24
        switch(temp_num)
       {
       case 11: memcpy(&bcm_info.sensor.qm[WIND_VELOCITY_3_S  ],&temp_qm,sizeof(qm_t)); break;
       case 12: memcpy(&bcm_info.sensor.qm[WIND_VELOCITY_1_M  ],&temp_qm,sizeof(qm_t)); break;
       case 13: memcpy(&bcm_info.sensor.qm[WIND_VELOCITY_2_M  ],&temp_qm,sizeof(qm_t)); break;
       case 14: memcpy(&bcm_info.sensor.qm[WIND_VELOCITY_10_M ],&temp_qm,sizeof(qm_t)); break;
       case 21: memcpy(&bcm_info.sensor.qm[WIND_DIRECTION_0_M ],&temp_qm,sizeof(qm_t)); break;
       case 22: memcpy(&bcm_info.sensor.qm[WIND_DIRECTION_1_M ],&temp_qm,sizeof(qm_t)); break;
       case 23: memcpy(&bcm_info.sensor.qm[WIND_DIRECTION_2_M ],&temp_qm,sizeof(qm_t)); break;
       case 24: memcpy(&bcm_info.sensor.qm[WIND_DIRECTION_10_M],&temp_qm,sizeof(qm_t)); break;
       default:{goto err;}
       }
    }
    else if((i == 9) || (i == 10))//地温
    {
        memcpy(&bcm_info.sensor.qm[(temp_num/10)-1],&temp_qm,sizeof(qm_t));
    }
    else
    {
        memcpy(&bcm_info.sensor.qm[temp_num-11],&temp_qm,sizeof(qm_t));
    }



    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*函数：              cmd_cr
*作者：
*日期：           2017-12-11
*功能：  B.24　设置或读取设备的校正、检定信息
    设备标识符，设备ID,校正/检定标识符，时间，期限，人员编号，参数。
    操作需要权限
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_cr(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    short temp_cak =0, j = 0, temp_term = 0;
    unsigned long temp_number = 0;
    int year, mon, day, num;
    char buffer[8][Flash_PAGE_SIZE/2-9]={0};    //128byte - 2byte(year)-1byte(mon)-1byte(day)-1byte(term)-4byte(number)
    s_check_cr_t    cfc[8];
    s_check_info_t  temp_check_info;
    float temp_a = 0, temp_b = 0, temp_c = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}

    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
        /*{
          if(-1 == check_digit(p, 3))//校验 ID是否为数字
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(-1 == check_digit(p, 1))//校正、检定标识符（1=校正 2=检定）
          {goto err;}
          temp_cak = atoi(p);
          break;
        }  
        case 4:
        {
        	if(8 != strlen(p))//日期 YYYYMMDD
          {goto err;}
          if(sscanf(p,"%04d%02d%02d",&year,&mon,&day)!=3)
           {goto err;}
          break;
        }  
        case 5:
        {
        	if(-1 == check_digit(p, 2))//有效期 2位数字
        	{goto err;}
        	temp_term=atoi(p);
        	break;
        }  
        case 6:
        {
        	if(-1 == check_digit(p, 5))//校验员 5位数字
        	{goto err;}
        	temp_number=atol(p);
        	break;
        }  
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        {
            if((Flash_PAGE_SIZE/2-7) <= strlen(p))//日期 YYYYMMDD
            {goto err;}
            strcpy(buffer[i-7],p);
            break;
        }
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(!( ((i>=8) && (i<=15)) || (i==4)))   {goto err;}

    //1、验证设备标识符,ID是否正确,校验信息类型
    //验证DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    //验证信息类型 
    if((temp_cak != 1) && (temp_cak != 2))
      { goto err; } 
    if(i == 4)  // 打印 读取
    {
        if(temp_cak == 1)
        {
            if(m_tempdata.CfcFlag == true)
                 return 0;
            else
            {
                m_tempdata.CfcFlag = true;
                cfc_offset = 0;
            }
        }
        if(temp_cak == 2)
        {
           if(m_tempdata.CheckFlag == true)
                return 0;
           else
           {
               m_tempdata.CheckFlag = true;
               check_offset = 0;
           }
        }
       return 0;
    }
    
    if(1 != m_tempdata.SuperadMin) //判定是否有操作权限
    {goto err;}
    //2 校验 写入时间
    if(-1 == check_data(year, mon, day))
    	{goto err;}   	
    temp_check_info.year = year;
    temp_check_info.mon  = mon;
    temp_check_info.day  = day;
    //3 校验 写入期限	
    if((temp_term < 0) || (temp_term > 99))
    	{goto err;}   	
    temp_check_info.term = temp_term;
    //3 校验 写入人员编号	
    if(temp_number > 99999)
    	{goto err;}
    temp_check_info.number = temp_number;

    //4 判断 校正 or 检定  信息
    memset(cfc,0,sizeof(cfc));
    if(temp_cak == 1)   //校正
    {
        for(j=0;j<8;j++)
        {
            if(sscanf(buffer[j],"%d#%f:%f:%f",&num,&temp_a,&temp_b,&temp_c)!=4)
                {goto err;}
            else
            {
                bcm_info.sensor.cr[num-1].a = temp_a;
                bcm_info.sensor.cr[num-1].b = temp_b;
                bcm_info.sensor.cr[num-1].c = temp_c;

                cfc[j].number = num;
                cfc[j].cr.a   = temp_a;
                cfc[j].cr.b   = temp_b;
                cfc[j].cr.c   = temp_c;

                temp_check_info.len = j+1;
            }
            if(j == (i-8))
                break;
        }
        if(0 == save_cr(temp_cak,&temp_check_info,(char *)cfc))
            {goto err;}
    }
    if(temp_cak == 2)   //检定
    {
        temp_check_info.len = strlen(buffer[0]);
        if(0 == save_cr(temp_cak,&temp_check_info,(char *)buffer))
            {goto err;}
    }

    //3 写入结构体 写入时钟 返回正确
    save_sys_cfg(&bcm_info);	
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	

}
/*==================================================================
*函数：              cmd_readdata
*作者：
*日期：           2017-12-11
*功能：  B.25　实时读取数据
设备标识符,设备ID,帧标识
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_readdata(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    unsigned long temp_time_now = 0;
    unsigned char temp_fi = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2: break;//
          /*{
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
            break;
          }      */
        case 3:
         {
            if(-1 == check_digit(p, 3))//校验是否含有帧标识
            {goto err;}
            temp_fi = atoi(p);
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
      
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    
    //2 解析当前时间秒
    temp_time_now = get_softimer(&m_tempdata.m_RtcTateTime);
    
    //若当前处于历史数据发送状态 则不执行此条命令 不响应
    if(0 != m_tempdata.Count_read_history)
        return 0;
    
    if(i == 3)  // 打印 读取当前分钟数
    {
        m_tempdata.time_start_read = temp_time_now;
        m_tempdata.Count_read_history =  1;
        return 0;
    }
    
    //3 校验FI 的取值范围 (0~83)(100~183)
 	if(temp_fi > 183)
  	{ goto err; }  
    if((temp_fi > 83) && (temp_fi < 100))
  	{ goto err; } 
    
    //4 取出后两位数值
    temp_fi = temp_fi % 100;
 
    
    //5 判断读取分钟数据还是小时数据
    if(temp_fi <=60 )//分钟
  	{
  		m_tempdata.time_interval_read = temp_fi;
  	}
    else
  	{		
  		m_tempdata.time_interval_read = (temp_fi-60+1)*60;
  		
  	}
  	//6 读取时间 制置读取数据数量标志
    m_tempdata.time_start_read = temp_time_now - (temp_time_now % m_tempdata.time_interval_read);
    m_tempdata.Count_read_history = 1;
    return 0;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*函数：              cmd_down
*作者：
*日期：           2017-12-11
*功能：  B.26　历史数据下载
设备标识符,设备ID,帧标识
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_down(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    unsigned char temp_fi = 0;
    unsigned long temp_time_now = 0, temp_time_start = 0, temp_time_end = 0;
    s_RtcTateTime_t start,end;
    int temp_a = 0, temp_b = 0, temp_c = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
        /*  {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
            break;
          }     */
        case 3:
         {
          if(10 !=  strlen(p)) //校验长度
            {goto err;}
          if(-1 == check_format(p, 10, "-", 2))
            {goto err;}
          if(3 != sscanf(p,"%d-%d-%d",&temp_a, &temp_b, &temp_c) )
            {goto err;}
          if( -1 == check_data(temp_a, temp_b, temp_c))
       		{goto err; }
          start.year  = (unsigned int )  temp_a ;
          start.month = (unsigned char) (temp_b & 0x00ff);
 		  start.day   = (unsigned char) (temp_c & 0x00ff);
          break;
          }  
        case 4:
          {
			 if(8 !=  strlen(p)) //校验长度
          	 {goto err;}
          	 if(-1 == check_format(p, 8, ":", 2))
          	 {goto err;}
             if(3 != sscanf(p,"%d:%d:%d",&temp_a, &temp_b, &temp_c) )
             {goto err;}
            if(-1 == check_time(temp_a, temp_b, temp_c))
        		{goto err; } 
            start.hour = (unsigned char) (temp_a & 0x00ff);
            start.min  = (unsigned char) (temp_b & 0x00ff);
            start.sec  = (unsigned char) (temp_c & 0x00ff);
            break;
          }
        case 5:
        {
        	if(10 !=  strlen(p)) //校验长度
            {goto err;}
          if(-1 == check_format(p, 10, "-", 2))
            {goto err;}
          if(3 != sscanf(p,"%d-%d-%d", &temp_a, &temp_b, &temp_c) )
           {goto err;}
          if( -1 == check_data(temp_a, temp_b, temp_c))
       		 {goto err; }
          end.year  = (unsigned int )  temp_a ;
          end.month = (unsigned char) (temp_b & 0x00ff);
          end.day   = (unsigned char) (temp_c & 0x00ff);
          break;
        }  
        case 6:
          {
          	if((10 !=  strlen(p)) && (8 !=  strlen(p))) //校验长度
          	 {goto err;}
          	if(-1 == check_format(p, 8, ":", 2))
          	 {goto err;}
            if(3 != sscanf(p,"%d:%d:%d", &temp_a, &temp_b, &temp_c))
             {goto err;}
            if( -1 == check_time(temp_a, temp_b, temp_c))
        	{goto err; }
            end.hour = (unsigned char) (temp_a & 0x00ff);
            end.min  = (unsigned char) (temp_b & 0x00ff);
            end.sec  = (unsigned char) (temp_c & 0x00ff);
            break;
          }       
        case 7:
         {
            if(-1 == check_digit(p, 3))//校验是否含有帧标识
            {goto err;}
            temp_fi = atoi(p);
            break;
          }
        case 8: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
      
    }
    if(( i != 8) && (i != 7))   {goto err;}

    //1、验证设备标识符,ID是否正确
    //验证DI
    /*if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    
    //若当前处于历史数据发送状态 则不执行此条命令 不响应
    if(0 != m_tempdata.Count_down_history)
        return 0;

    //2 解析当前时间秒
    temp_time_now   = get_softimer(&m_tempdata.m_RtcTateTime);
    temp_time_start = get_softimer(&start);
    temp_time_end   = get_softimer(&end);
    
    //3 校验时间
    if(temp_time_end < temp_time_start)
        { goto err; }
    if(temp_time_now < temp_time_start)
        { goto err; }
    if(temp_time_end > temp_time_now)
        { temp_time_end = temp_time_now; }

    if(i == 7)  // 打印 时间段内每一分钟的数据
    {

        m_tempdata.time_start = temp_time_start;
        m_tempdata.Count_down_history =  temp_time_end - temp_time_start + 1;
        m_tempdata.time_interval = 1;
        return 0;
    }
    
    //4 校验FI 的取值范围 (0~83)(100~183)
 	if(temp_fi > 183)
  	{ goto err; }  
    if((temp_fi > 83) && (temp_fi < 100))
  	{ goto err; } 
    
    //5 取出后两位数值
    temp_fi = temp_fi % 100;
    
    //6 判断读取分钟数据还是小时数据
    if(temp_fi <=60 )//分钟
  	{
  		m_tempdata.time_interval = temp_fi;
  	}
    else
  	{		
  		m_tempdata.time_interval = (temp_fi - 60 + 1) * 60;
  		
  	}
  	//7 读取时间 制置读取数据数量标志
    if(0 == (temp_time_start % 60))
    {
        m_tempdata.time_start = temp_time_start;
    }
    else
    {
        if(0 != ((temp_time_start % 60) % m_tempdata.time_interval))
            m_tempdata.time_start = temp_time_start + (m_tempdata.time_interval-((temp_time_start % 60) % m_tempdata.time_interval));
        else
            m_tempdata.time_start = temp_time_start;
    }

    m_tempdata.Count_down_history = (temp_time_end - m_tempdata.time_start) / m_tempdata.time_interval + 1;

    if(m_tempdata.Count_down_history > 720)
        m_tempdata.Count_down_history = 720;

    return 0;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;

}
/*==================================================================
*函数：              cmd_reset
*作者：
*日期：           2017-12-11
*功能：  B.27　重启设备
设备标识符，设备ID
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_reset(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
     /*   if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
        }*/
        p = strtok(NULL, ",");
        i++;
    }
    if(i != 3) {goto err;}
    //1、验证设备标识符,ID是否正确
    //验证DI
/*    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }
*/
    //2 看门狗停止工作    设备重启
    //WDTCTL=WDT_ARST_1000;
    //Stop_Timer0();
    while(1)
    {
        m_tempdata.alive_counter=250;
    }
 
    
err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*函数：              cmd_superadmin
*作者：
*日期：           2017-12-14
*功能：  B.29　操作权限命令
设备标识符，设备ID
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_superadmin(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
       /* if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
        }*/
        p = strtok(NULL, ",");
        i++;
    }
    if(i != 3) {goto err;}
    //1、验证设备标识符,ID是否正确
    //验证DI
 /*   if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/

    //2 操作权限 准许
    m_tempdata.SuperadMin = 1;
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*函数：              cmd_unit
*作者：
*日期：           2017-12-25
*功能：  B.30 　设置或读取智能气压测量仪输出数据的单位   对应气压功能规格需求书的17条
设备标识符，设备ID
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_unit(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    char temp_unit = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        /*if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, 3))//校验 ID是否为数字
            {goto err;}
            temp_id = atoi(p);
        }*/
        if(3 == i)
        {
            if(-1 == check_digit(p, 1))//校验单位是否为数字
            {goto err;}
            temp_unit = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }
    if((i != 3) && (i != 4)) {goto err;}
    //1、验证设备标识符,ID是否正确
    //验证DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/

    if(i == 3)  //读取
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%d>\r\n",sensor_di,m_defdata.m_baseinfo.id,bcm_info.sensor.apunit);
        return rlen;
    }

    //2 写入气压单位 并保存 打印
    if((temp_unit < 1) || (temp_unit > 4))
      { goto err; }

    bcm_info.sensor.apunit = temp_unit;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
//================================================== 解析命令调用函数 ===========================================
/*==================================================================
** 函数名称 ：unsigned char Utils_CheckCmdStr(char *pStr)
** 函数功能 ：检查收到字符串格式是否符合要求
** 入口参数 ：pStr：要检查的字符串指针
**
** 出口参数 ：0：格式错误
**     其它数值：这个字符串一共有几段（以逗号为分隔符）
==================================================================*/
int Utils_CheckCmdStr(char *pStr)
{
  char char_1;
  unsigned char sectionNum;
  char *pChar_1;

  if(pStr == 0) return 0;

  // 判断开头是否有逗号
  char_1 = *pStr;
  if(char_1 == ',') return 0;

  // 将回车替换为字符串结束符
  pChar_1 = strchr(pStr,'\r');
  if(pChar_1 == 0) return 0;
  if(pChar_1 == pStr)  return 0;
  *pChar_1 = '\0';

  // 判断是否有连续的分隔符和结尾是否有分隔符
  sectionNum = 1;
  while(1)
  {
    char_1 = *pStr++;
    if(char_1 == '\0')  return sectionNum;
    if(char_1 != ',')   continue;

    sectionNum++;

    char_1 = *pStr++;
    if(char_1 == '\0') return 0;  // 字符串结尾有分隔符
    if(char_1 == ',') return 0;   // 多个分隔符连续
  }
}
/*==================================================================
 *  函   数     名：check_digit
 *  作         者：xhf
 *  日         期：2014-05-23
 *  功            能：检查字符串是否全为数字
 *  输入参数：
 *         char *p (输入字符串）
 *         uint8 len (输入字符串长度）
 *  返  回    值： 类型（int  )
 *          返回0        表示全为数字
 *          返回-1      表示有数字以为的字符
 *  修改记录：
==================================================================*/
int check_digit(char *p, uint8 len)
{
    uint8 i;
    for(i=0; i<len; i++)
    {
        if(0 == isdigit(*(p+i)))
        {
            return -1;
        }
    }
    return 0;
}
/*==================================================================
 *  函   数     名：check_format
 *  作         者：
 *  日         期：2017-12-13
 *  功            能：检查字符串是否全为数字,分隔符是否满足要求
 *  输入参数：
 *         char *p (输入字符串）
 *         uint8 len (输入字符串长度）
 *         char *buf 分隔符
 *         uint8 num 分隔符个数
 *  返  回    值： 类型（int  )
 *          返回0        表示全为数字
 *          返回-1      表示有数字以为的字符
 *  修改记录：
==================================================================*/
int check_format(char *p, uint8 len, char *buf, uint8 num)
{
    uint8 i = 0, j = 0;
    for(i=0; i<len; i++)
    {
        if(0 == strncmp((p+i),buf,1)) //相等为 0
        {
            j++;
            if(j>num)
            {
                return -1;
            }
            continue;
        }
        if(0 == isdigit(*(p+i)))    //返回0 表示非数字
        {
            return -1;
        }
    }
    return 0;
}

/*==================================================================
*函数：             check_data
*作者：
*日期：		2017-12-11
*功能：   校验日期
*输入：   year, month, day		int型的年月日
*
 *  返  回    值： 类型（int  )
 *          返回0        写入日期成功
 *          返回-1       写入日期失败
*修改记录：
==================================================================*/
int check_data(int year, int month, int day)
{
	unsigned char month_lab[]={0,31,28,31,30,31,30,31,31,30,31,30,31};


	  //校验日期是否符合要求
	  if((((year%4)==0)&&((year%100)!=0))||((year%400)==0))//闰年，更新2月日期
	  {
	      month_lab[2]=29;
	  }
	  if((year < 2000) || (year > 2100))
	      {goto err; }
	  if((month>12) || (month < 1))
	      {goto err; }
	  if((day > month_lab[month]) || (day < 1))
	      {goto err; }
	  return 0;
err:
    return -1;
}

/*==================================================================
*函数：             check_time
*作者：
*日期：2017-12-11
*功能：校验时间
*输入：  hour, min, sec  int型的时分秒

 *  返  回    值： 类型（int  )
 *          返回0        写入日期成功
 *          返回-1       写入日期失败
*修改记录：2017-12-10，LY，修改返回值，修改校验
==================================================================*/
int check_time(int hour, int min, int sec)
{

    //校验时间是否符合要求
    if((hour>23) || (hour < 0))
        {goto err;}
    if((min>59) || (min < 0))
        {goto err;}
    if((sec>59) || (sec < 0))
        {goto err;}
        
    return 0;
err:
    return -1;
}
/*==================================================================
*函数：             check_datetime
*作者：
*日期：2017-12-11
*功能：校验时间
*输入：  hour, min, sec  int型的时分秒

 *  返  回    值： 类型（int  )
 *          返回0        写入日期成功
 *          返回-1       写入日期失败
*修改记录：2017-12-10，LY，修改返回值，修改校验
==================================================================*/
int check_datetime(s_RtcTateTime_t *datetime)
{
    unsigned long temp_time_now = 0, temp_time_chack = 0;
    unsigned long temp,timep;
    unsigned long pageIndex;
    unsigned long addr;
    unsigned char i;
    //校验时间是否符合要求
 //   GPIO_setOutputLowOnPin(GPIO_PORT_P2,  GPIO_PIN3);

    temp_time_now   = get_softimer_sec(&m_tempdata.m_RtcTateTime);
    temp_time_chack = get_softimer_sec(datetime);

    temp = abs(temp_time_chack - temp_time_now);    //秒差异
    timep = temp_time_now/60;

    //1 当前分钟不等
    if((temp > 60) || (datetime->min != m_tempdata.m_RtcTateTime.min))
    {
        for(i = 0; i< 1; i++)                         //擦除sector数
       {
           pageIndex = ((timep-16*i) % Flash_PAGE_NUMBER) + FLASH_DATA_START;   // 数据位于哪个扇区
           addr = pageIndex << 8;
           W25Q128_Erase_Chip(W25Q128_ERS_SEC, addr);
       }
    }
    else    //当前分钟内
    {   //从前往后对时  并且当前时钟小于3s
       if((m_tempdata.m_RtcTateTime.sec <= 3) && (temp_time_chack > temp_time_now))
        {
            for(i = 0; i< 1; i++)                         //擦除sector数
            {
                pageIndex = ((timep-16*i) % Flash_PAGE_NUMBER) + FLASH_DATA_START;   // 数据位于哪个扇区
                addr = pageIndex << 8;
                if((pageIndex & 0x0F) == 0) // 判断是否为一个sector的开始
                    W25Q128_Erase_Chip(W25Q128_ERS_SEC,addr);
            }
        }
    }

   // GPIO_setOutputHighOnPin(GPIO_PORT_P2,  GPIO_PIN3);
    return 0;
}

/*==================================================================
*函数：             check_sensor
*作者：
*日期：2017-12-16
*功能：检查传感器编号和传感器类型是否符合
返回传感器的 上下限 及 最大变化值
*输入：   *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
 *  返  回    值： 类型（int  )
 *          返回0        写入日期成功
 *          返回-1       写入日期失败
*修改记录：2017-12-10，LY，修改返回值，修改校验
==================================================================*/
int check_sensor(short temp_num, qs_t *temp_qs)
{
    unsigned char value;

    for(value = 0; value < DEV_TYPE_MAX; value ++)
    {
      if(0 == strncmp((char *)&sensor_di,(char *)dev_type[value],4))
          break;
    }

	switch(value)
	{
		case 6 :	//风
		{
			if((temp_num < 1) || (temp_num >2))	{goto err;}
			memcpy(temp_qs,	(char *)&bcm_info.sensor.qs[temp_num - 1],sizeof(qs_t));
		}break;
		case 9:	//地温
		{
			if((temp_num < 1) || (temp_num >MAX_SENSOR_NUM))	{goto err;}
			memcpy(temp_qs,	(char *)&bcm_info.sensor.qs[temp_num - 1],sizeof(qs_t));
		}break;
		case 10:	//土壤水分
		{
		    if((temp_num < 1) || (temp_num >MAX_SENSOR_NUM)) {goto err;}
			memcpy(temp_qs,	(char *)&bcm_info.sensor.qs[temp_num - 1],sizeof(qs_t));
		}break;
		case 5:	//气压
		{
		    if(temp_num != 1)  {goto err;}

		    if(bcm_info.sensor.apunit == 3)
		    {
		        temp_qs->change = bcm_info.sensor.qs[temp_num - 1].change * 0.750062;
		        temp_qs->max    = bcm_info.sensor.qs[temp_num - 1].max    * 0.750062;
		        temp_qs->min    = bcm_info.sensor.qs[temp_num - 1].min    * 0.750062;
		    }
		    else if(bcm_info.sensor.apunit == 4)
            {
                temp_qs->change = bcm_info.sensor.qs[temp_num - 1].change * 0.029530;
                temp_qs->max    = bcm_info.sensor.qs[temp_num - 1].max    * 0.029530;
                temp_qs->min    = bcm_info.sensor.qs[temp_num - 1].min    * 0.029530;
            }
		    else
		    {
		        memcpy(temp_qs, (char *)&bcm_info.sensor.qs[temp_num - 1],sizeof(qs_t));
		    }
		}break;
		case 12://日照
		case 11://总辐射
		case 2:	//湿度
		case 3:	//温度
		case 8:	//翻斗
		case 14:	//称重
		{
			if(temp_num != 1)	{goto err;}
			memcpy(temp_qs,	(char *)&bcm_info.sensor.qs[temp_num - 1],sizeof(qs_t));
		}break;
		default:	{goto err;}
	}
	return 0;
	
err:	return -1;		
}

/*==================================================================
*函数：             save_sensor
*作者：
*日期：2017-12-16
*功能：检查传感器编号和传感器类型是否符合
保存 传感器的 上下限 及 最大变化值
*输入：   *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
 *  返  回    值： 类型（int  )
 *          返回0        写入日期成功
 *          返回-1       写入日期失败
*修改记录：2017-12-10，LY，修改返回值，修改校验
==================================================================*/
int save_sensor(short temp_num, float temp_min, float temp_max, char *temp_change, qs_t *temp_qs)
{
    unsigned char value;

       for(value = 0; value < DEV_TYPE_MAX; value ++)
       {
           if(0 == strncmp((char *)&sensor_di,(char *)dev_type[value],4))
             break;
       }

	switch(value)
	{
		case 6:	//风
		{
			if(1 == temp_num)//风速
			{
				if(temp_min < 0)			{goto err;}
				if(temp_max > 200)			{goto err;}
				if(-1 == check_format(temp_change, strlen(temp_change), ".", 1))  {goto err;}
				if((atof(temp_change) < 0) || (atof(temp_change) > 200))		    {goto err;}
				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = atof(temp_change);
			}	
			else if(2 == temp_num)	//风向
			{
				if(temp_min < 0)			{goto err;}
				if(temp_max > 360)		    {goto err;}
                if(0 != strcmp(temp_change,"/"))                {goto err;}
				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = INVALID_DATA;
			}	
			else
				{goto err;}	
		}break;
		case 9:	//地温
		{
			if((0 < temp_num) && (MAX_SENSOR_NUM >= temp_num))
			{
				if(temp_min < -100) 		{goto err;}
				if(temp_max > 100)  		{goto err;}
				if(temp_max < temp_min)		{goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change), ".", 1))  {goto err;}
				if((atof(temp_change) > 10) || (atof(temp_change) < 0))		    {goto err;}
				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = atof(temp_change);
			}	
			else
				{goto err;}	
		}break;
		case 10:	//土壤水分
		{
			if((0 < temp_num) && (MAX_SENSOR_NUM >= temp_num))
			{
				if(temp_min < 0) 			{goto err;}
				if(temp_max > 100)			{goto err;}
				if(temp_max < temp_min) 	{goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change), ".", 1))  {goto err;}
                if((atof(temp_change) > 20) || (atof(temp_change) < 0))          {goto err;}

				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = atof(temp_change);
			}	
			else
				{goto err;}	
		}break;
//		case 12,	//日照
//		case 11,	//总辐射
		
		case 2:	//湿度
		{
			if(1 == temp_num)
			{
				if(temp_min < 0) 			{goto err;}
				if(temp_max > 100)			{goto err;}
				if(temp_max < temp_min) 	{goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change), ".", 1))  {goto err;}
                if((atof(temp_change) > 20) || (atof(temp_change) < 0))          {goto err;}

				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = atof(temp_change);
			}	
			else
				{goto err;}	
		}break;
		case 3:	//温度
		{
			if(1 == temp_num)
			{
				if(temp_min < -100) 		{goto err;}
				if(temp_max > 100)			{goto err;}
				if(temp_max < temp_min)	    {goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change), ".", 1))   {goto err;}
                if((atof(temp_change) > 10) || (atof(temp_change) < 0))           {goto err;}
				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = atof(temp_change);
			}	
			else
				{goto err;}	
		}break;
		case 5:	//气压
		{
			if(1 == temp_num)
			{
			    if(3 == bcm_info.sensor.apunit) // * 0.750062
			    {
                    if(temp_min < 75.0076)          {goto err;}
                    if(temp_max > 1125.093)         {goto err;}
                    if(temp_max < temp_min)         {goto err;}
                    if(-1 == check_format(temp_change, strlen(temp_change), ".", 1)) {goto err;}
                    if((atof(temp_change) > 0.750062) || (atof(temp_change) < 0))       {goto err;}

                    temp_qs->min    = temp_min / 0.750062;
                    temp_qs->max    = temp_max / 0.750062;
                    temp_qs->change = atof(temp_change) / 0.750062;
			    }
			    else if(4 == bcm_info.sensor.apunit)// * 0.029530
			    {
                    if(temp_min < 2.953)          {goto err;}
                    if(temp_max > 44.295)         {goto err;}
                    if(temp_max < temp_min)       {goto err;}
                    if(-1 == check_format(temp_change, strlen(temp_change), ".", 1)) {goto err;}
                    if((atof(temp_change) > 0.029530) || (atof(temp_change) < 0))       {goto err;}

                    temp_qs->min    = temp_min / 0.029530;
                    temp_qs->max    = temp_max / 0.029530;
                    temp_qs->change = atof(temp_change) / 0.029530;
			    }
			    else
			    {
	                if(temp_min < 100)          {goto err;}
	                if(temp_max > 1500)         {goto err;}
	                if(temp_max < temp_min)     {goto err;}
	                if(-1 == check_format(temp_change, strlen(temp_change), ".", 1)) {goto err;}
	                if((atof(temp_change) > 1) || (atof(temp_change) < 0))       {goto err;}

	                temp_qs->min    = temp_min;
	                temp_qs->max    = temp_max;
	                temp_qs->change = atof(temp_change);
			    }
			}	
			else
				{goto err;}
		}break;
		case 8:	//翻斗
		{
			if(1 == temp_num)
			{
				if(temp_min < 0) 			{goto err;}
				if(temp_max > 20)			{goto err;}
				if(temp_max < temp_min) 	{goto err;}
                if(0 != strcmp(temp_change,"/"))         {goto err;}

				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = INVALID_DATA;
			}	
			else
				{goto err;}
		}break;
		case 14:	//称重
		{
			if(1 == temp_num)
			{
				if(temp_min < 0) 			{goto err;}
				if(temp_max > 20)			{goto err;}
				if(temp_max < temp_min)	    {goto err;}
                if(0 != strcmp(temp_change,"/"))         {goto err;}
				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = INVALID_DATA;
			}	
			else
				{goto err;}	
		}break;
		default:	{goto err;}
	}
	return 0;
	
err:	return -1;		
}
/*==================================================================
*函数：             check_element
*作者：
*日期：2017-12-16
*功能：检查传感器编号和传感器类型是否符合
返回传感器的 上下限 及 最大变化值
*输入：   *buf  (输入数据）输入数据地址
*      *rbuf (输出数据）控制结构地址
*
 *  返  回    值： 类型（int  )
 *          返回0        写入日期成功
 *          返回-1       写入日期失败
*修改记录：2017-12-20，LY，修改返回值，修改校验
==================================================================*/
int check_element(short temp_num, qm_t *temp_qm)
{
    unsigned char value;

    memset(temp_qm,'/',sizeof(qm_t));
    for(value = 0; value < DEV_TYPE_MAX; value ++)
    {
      if(0 == strncmp((char *)&sensor_di,(char *)dev_type[value],4))
          break;
    }

    switch(value)
    {
        case 6 :    //风
        {//范围 11~14,21~24
            switch(temp_num)
            {
            case 11: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_VELOCITY_3_S],sizeof(qm_t));   break;
            case 12: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_VELOCITY_1_M],sizeof(qm_t));   break;
            case 13: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_VELOCITY_2_M],sizeof(qm_t));   break;
            case 14: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_VELOCITY_10_M],sizeof(qm_t));  break;
            case 21: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_DIRECTION_0_M],sizeof(qm_t));  break;
            case 22: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_DIRECTION_1_M],sizeof(qm_t));  break;
            case 23: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_DIRECTION_2_M],sizeof(qm_t));  break;
            case 24: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_DIRECTION_10_M],sizeof(qm_t)); break;
            default: goto err;
            }
        }break;
        case 9: //地温
        {
            switch(temp_num)
            {
            case 11:
            case 21:
            case 31:
            case 41:
            case 51:
            case 61:
            case 71:
            case 81:
                memcpy(temp_qm, (char *)&bcm_info.sensor.qm[(temp_num/10) - 1],sizeof(qm_t));  break;
            default: goto err;
            }
        }break;
        case 10:    //土壤水分
        {
            switch(temp_num)
            {
            case 11:
            case 21:
            case 31:
            case 41:
            case 51:
                memcpy(temp_qm, (char *)&bcm_info.sensor.qm[(temp_num/10) - 1],sizeof(qm_t));
                break;
            default:   {goto err;}
            }
        }break;
        case 3: //温度
        {
            if((temp_num != 11) && (temp_num != 12))   {goto err;}
            memcpy(temp_qm, (char *)&bcm_info.sensor.qm[temp_num - 11],sizeof(qm_t));
        }break;
        case 12://日照
        case 11://总辐射
        case 2: //湿度
        case 5: //气压
        case 8: //翻斗
        case 14://称重
        {
            if(temp_num != 11)   {goto err;}
            memcpy(temp_qm, (char *)&bcm_info.sensor.qm[temp_num - 11],sizeof(qm_t));
        }break;
        default:    {goto err;}
    }
    return 0;
    
err:    return -1;
}
/*==================================================================
*函数：             save_sensor
*作者：
*日期：2017-12-16
*功能：检查传感器编号和传感器类型是否符合
保存 传感器的 上下限 及 最大变化值
*输入：   *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
 *  返  回    值： 类型（int  )
 *          返回0        写入日期成功
 *          返回-1       写入日期失败
*修改记录：2017-12-20，LY，修改返回值，修改校验
==================================================================*/
int save_element(short temp_num, float temp_min, float temp_max, char *temp_doubt, char *temp_err, char *temp_change, qm_t *temp_qm)
{
    unsigned char value;

       for(value = 0; value < DEV_TYPE_MAX; value ++)
       {
           if(0 == strncmp((char *)&sensor_di,(char *)dev_type[value],4))
             break;
       }

    switch(value)
    {
        case 6: //风
        {

            if((temp_num > 24) || (temp_num < 11))    {goto err;}
            if((temp_num > 14) && (temp_num < 21))    {goto err;}
            if(temp_min > temp_max)  {goto err;}
            //风向  大小边界
            if((temp_num > 21) && (temp_num < 24) && (temp_max > 360))  {goto err;}
            if((temp_num > 21) && (temp_num < 24) && (temp_min < 0))    {goto err;}
            //风速 小边界
            if((temp_num > 11) && (temp_num < 14) && (temp_min < 0))    {goto err;}
            if((temp_num > 11) && (temp_num < 14) && (temp_max > 200))  {goto err;}

            //判定风向 存疑门限,错误,最小应变量
            if(2 == (temp_num /10))
            {
                if(0 != strcmp(temp_doubt,"/"))         {goto err;}
                if(0 != strcmp(temp_err  ,"/"))         {goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change), ".", 1))   {goto err;}

                if((0 > atof(temp_change) ) || (360 < atof(temp_change)))          {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = INVALID_DATA;
                temp_qm->err         = INVALID_DATA;
                temp_qm->changerate  = atof(temp_change);
            }
            //判定风速度 存疑门限,错误,最小应变量
            if(1 == (temp_num /10))
            {

                if(-1 == check_format(temp_doubt, strlen(temp_doubt), ".", 1))  {goto err;}
                if(-1 == check_format(temp_err, strlen(temp_err), ".", 1))      {goto err;}
                if(0 != strcmp(temp_change,"/"))     {goto err;}

                if((0 > atof(temp_doubt)) || (200 < atof(temp_doubt)))        {goto err;}
                if((0 > atof(temp_err))   || (200 < atof(temp_err)))              {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = atof(temp_doubt);
                temp_qm->err         = atof(temp_err);
                temp_qm->changerate  = INVALID_DATA;
            }
         }break;
        case 9: //地温
        {
            if((temp_num != 11) && (temp_num != 21) && (temp_num != 31) && (temp_num != 41) &&
                    (temp_num != 51)&& (temp_num != 61) && (temp_num != 71) && (temp_num != 81)) {goto err;}
            if(temp_min < -100)                 {goto err;}
            if(temp_max > 100)                  {goto err;}
            if(temp_max < temp_min)             {goto err;}
            if(-1 == check_format(temp_doubt, strlen(temp_doubt), ".", 1)) {goto err;}
            if(-1 == check_format(temp_err,   strlen(temp_err),   ".", 1)) {goto err;}
            if(0 != strncmp(temp_change  ,"/",1))    {goto err;}

            if((0 > atof(temp_doubt)) || (100 < atof(temp_doubt)))        {goto err;}
            if((0 > atof(temp_err))   || (100 < atof(temp_err)))              {goto err;}

            temp_qm->min         = temp_min;
            temp_qm->max         = temp_max;
            temp_qm->doubtful    = atof(temp_doubt);
            temp_qm->err         = atof(temp_err);
            temp_qm->changerate  = INVALID_DATA;
        }break;
        case 10:    //土壤水分
        {
            if((temp_num != 11) && (temp_num != 21) && (temp_num != 31) && (temp_num != 41) && (temp_num != 51))  {goto err;}
            if(temp_max < temp_min)                {goto err;}
            if((temp_min < 0) || (temp_max > 100)) {goto err;}
            if(-1 == check_format(temp_doubt, strlen(temp_doubt), ".", 1)) {goto err;}
            if(-1 == check_format(temp_err,   strlen(temp_err),   ".", 1)) {goto err;}
            if(-1 == check_format(temp_change,   strlen(temp_change),   ".", 1)) {goto err;}

            if((atof(temp_doubt)  <0) || (atof(temp_doubt)  >100))  {goto err;}
            if((atof(temp_err)    <0) || (atof(temp_err)    >100))  {goto err;}
            if((atof(temp_change) <0) || (atof(temp_change) >100))  {goto err;}

            temp_qm->min         = temp_min;
            temp_qm->max         = temp_max;
            temp_qm->doubtful    = atof(temp_doubt);
            temp_qm->err         = atof(temp_err);
            temp_qm->changerate  = atof(temp_change);
        }break;
//      case 12,    //日照
//      case 11,    //总辐射

        case 2: //湿度
        {
            if(11 == temp_num)
            {
                if(temp_min < 0)            {goto err;}
                if(temp_max > 100)          {goto err;}
                if(temp_max < temp_min)     {goto err;}

                if(-1 == check_format(temp_doubt, strlen(temp_doubt),   ".", 1))     {goto err;}
                if(-1 == check_format(temp_err,   strlen(temp_err),   ".", 1))     {goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change),   ".", 1))     {goto err;}

                if((atof(temp_doubt)  <0) || (atof(temp_doubt)  >100))  {goto err;}
                if((atof(temp_err)    <0) || (atof(temp_err)    >100))  {goto err;}
                if((atof(temp_change) <0) || (atof(temp_change) >100))  {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = atof(temp_doubt);
                temp_qm->err         = atof(temp_err);
                temp_qm->changerate  = atof(temp_change);
            }
            else
                {goto err;}
        }break;
        case 3: //温度
        {
            if((11 == temp_num) || (12 == temp_num))
            {
                if(temp_min < -100)         {goto err;}
                if(temp_max > 100)          {goto err;}
                if(temp_max < temp_min)     {goto err;}

                if(-1 == check_format(temp_doubt, strlen(temp_doubt),   ".", 1))     {goto err;}
                if(-1 == check_format(temp_err,   strlen(temp_err),   ".", 1))     {goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change),   ".", 1))     {goto err;}

                if((atof(temp_doubt)  <0) || (atof(temp_doubt)  >100))  {goto err;}
                if((atof(temp_err)    <0) || (atof(temp_err)    >100))  {goto err;}
                if((atof(temp_change) <0) || (atof(temp_change) >100))  {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = atof(temp_doubt);
                temp_qm->err         = atof(temp_err);
                temp_qm->changerate  = atof(temp_change);
            }
            else
                {goto err;}
        }break;
        case 5: //气压
        {
            if(11 == temp_num)
            {
                if(temp_min < 100)          {goto err;}
                if(temp_max > 1500)         {goto err;}
                if(temp_max < temp_min)     {goto err;}

                if(-1 == check_format(temp_doubt, strlen(temp_doubt),   ".", 1))     {goto err;}
                if(-1 == check_format(temp_err,   strlen(temp_err),   ".", 1))     {goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change),   ".", 1))     {goto err;}

                if((atof(temp_doubt)  <0) || (atof(temp_doubt)  >20))  {goto err;}
                if((atof(temp_err)    <0) || (atof(temp_err)    >20))  {goto err;}
                if((atof(temp_change) <0) || (atof(temp_change) >20))  {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = atof(temp_doubt);
                temp_qm->err         = atof(temp_err);
                temp_qm->changerate  = atof(temp_change);
            }
            else
                {goto err;}
        }break;
        case 8: //翻斗
        {
            if(11 == temp_num)
            {
                if(temp_min < 0)            {goto err;}
                if(temp_max > 20)           {goto err;}
                if(temp_max < temp_min)     {goto err;}

                if(0 != strcmp(temp_doubt   ,"/"))    {goto err;}
                if(0 != strcmp(temp_err     ,"/"))    {goto err;}
                if(0 != strcmp(temp_change  ,"/"))    {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = INVALID_DATA;
                temp_qm->err         = INVALID_DATA;
                temp_qm->changerate  = INVALID_DATA;
            }
            else
                {goto err;}
        }break;
        case 14:    //称重
        {
            if(11 == temp_num)
            {
                if(temp_min < 0)            {goto err;}
                if(temp_max > 20)           {goto err;}
                if(temp_max < temp_min)     {goto err;}

                if(-1 == check_digit(temp_doubt, 2)) {goto err;}
                if(-1 == check_digit(temp_err, 2))   {goto err;}
                if(0 != strcmp(temp_change  ,"/"))    {goto err;}

                if((atof(temp_doubt)  <0) || (atof(temp_doubt)  >20))  {goto err;}
                if((atof(temp_err)    <0) || (atof(temp_err)    >20))  {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = atof(temp_doubt);
                temp_qm->err         = atof(temp_err);
                temp_qm->changerate  = INVALID_DATA;
            }
            else
                {goto err;}
        }break;
        default:    {goto err;}
    }
    return 0;

err:    return -1;
}
//============================================= 私有命令 ====================================================
/*==================================================================
*函数：              cmd_debugon
*作者：
*日期：           2017-12-28
*功能：  A.1 　打开调试模式
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_debugon(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((18 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. DEBUGON,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//校验 ID是否为数字
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1、验证设备标识符,ID是否正确
    //验证DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //开启调试模式
    m_tempdata.DebugON = true;
    rlen = sprintf((char *)rbuf,"<Open debug mode.>\r\n");
    return rlen;
}
/*==================================================================
*函数：              cmd_debugoff
*作者：
*日期：           2017-12-28
*功能：  A.2 　关闭调试模式
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_debugoff(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((19 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. DEBUGOFF,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//校验 ID是否为数字
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1、验证设备标识符,ID是否正确
    //验证DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    m_tempdata.DebugON = false;
    m_tempdata.DebugONCnt = 0;
    //m_tempdata.SecDataOut = false;    //关闭私有命令权限时  关闭秒数据发送
    rlen = sprintf((char *)rbuf,"<Close debug mode.>\r\n");
    return rlen;
}
/*==================================================================
*函数：              cmd_version
*作者：
*日期：           2017-12-28
*功能：  A.3　版本号查询
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_version(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((18 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. VERSION,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//校验 ID是否为数字
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1、验证设备标识符,ID是否正确
    //验证DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //2、验证设备是否开启调试模式
    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }

    rlen = sprintf((char *)rbuf,"<VERSION:\r\nHardware: %s\r\nSoftware: %s>\r\n",m_defdata.m_baseinfo.hard_version,SOFT_VER);
    return rlen;
}
/*==================================================================
*函数：              cmd_restore
*作者：
*日期：           2017-12-28
*功能：  A.4　恢复出厂设置
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_restore(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((18 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. RESTORE,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//校验 ID是否为数字
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1、验证设备标识符,ID是否正确
    //验证DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //2、验证设备是否开启调试模式
    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }

    Init_sys_cfg();
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<Resume factory setting success.>\r\n");
    return rlen;
}
/*==================================================================
*函数：              cmd_starttime
*作者：
*日期：           2017-12-28
*功能：  A.5　设备开始运行时间
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_starttime(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;
    time_t  power_start = 0;
    s_RtcTateTime_t rtc_temp;

    if((20 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. STARTTIME,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//校验 ID是否为数字
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1、验证设备标识符,ID是否正确
    //验证DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //2、验证设备是否开启调试模式
    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }

    power_start = get_softimer(&m_tempdata.m_RtcTateTime) - m_tempdata.SysPowerON;
    Convert_Sec_to_Time(&rtc_temp, power_start*60);

    rlen = sprintf((char *)rbuf,"<Start time: %04d-%02d-%02d %02d:%02d:%02d.>\r\n",rtc_temp.year,rtc_temp.month,rtc_temp.day,
                                                                       rtc_temp.hour,rtc_temp.min,rtc_temp.sec);
    return 0;
}
/*==================================================================
*函数：              cmd_secdout_on
*作者：
*日期：           2017-12-28
*功能：  A.6　输出秒级数据
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_secdout_on(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((20 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. SECDOUTON,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//校验 ID是否为数字
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1、验证设备标识符,ID是否正确
    //验证DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //2、验证设备是否开启调试模式
    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }

    m_tempdata.SecDataOut = true;
    rlen = sprintf((char *)rbuf,"<Open the seconds data sent automatically.>\r\n");
    return rlen;
}
/*==================================================================
*函数：              cmd_secdout_off
*作者：
*日期：           2017-12-28
*功能：  A.7　关闭秒级数据输出
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_secdout_off(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((21 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. SECDOUTOFF,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//校验 ID是否为数字
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1、验证设备标识符,ID是否正确
    //验证DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //2、验证设备是否开启调试模式
    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }

    m_tempdata.SecDataOut = false;
    rlen = sprintf((char *)rbuf,"<Close the seconds data sent.>\r\n");
    return rlen;
}
/*==================================================================
*函数：              cmd_secdout_off
*作者：
*日期：           2017-12-28
*功能：  A.7　关闭秒级数据输出
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_erase_check(char *buf,char *rbuf)
{
    char *temp_di = NULL, *temp_erase = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((21 != strlen(buf)) || (4 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. ERASEOR,%s,%03d,XX>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//校验 ID是否为数字
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        if(3 == i)
        {
            temp_erase = p;
            if((0 != strcmp(temp_erase,"55"))  && (0 != strcmp(temp_erase,"AA")))
                { return 0; }
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1、验证设备标识符,ID是否正确
    //验证DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //验证ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //2、验证设备是否开启调试模式
    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }

    if(0 == strcmp(temp_erase,"55"))
    {
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, FLASH_CFC_START    <<8);
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, (FLASH_CFC_START+16)<<8);
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, (FLASH_CFC_START+32)<<8);
        rlen = sprintf((char *)rbuf,"<Erase CFC OK.>\r\n");
    }
    if(0 == strcmp(temp_erase,"AA"))
    {
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, FLASH_CHECK_START    <<8);
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, (FLASH_CHECK_START+16)<<8);
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, (FLASH_CHECK_START+32)<<8);
        rlen = sprintf((char *)rbuf,"<Erase CHECK OK.>\r\n");
    }


    return rlen;
}

/*==================================================================
*函数：              cmd_cfc
*作者：
*日期：           2017-12-28
*功能：  A.8　标定系数
*参数：传感器号，被标定的系数  aX^2 + bX + c
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_cfc(char *buf,char *rbuf)
{
    char i = 0, j = 0, rlen = 0, *temp_di = NULL, *p = NULL;
    int sensor_num = 0;
    float temp_a = 0, temp_b = 0, temp_c = 0;
    unsigned short temp_id = 0;

    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }
    j = Utils_CheckCmdStr(buf);
    if((4 != j) && (5 != j))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error.\r\nRead:  CFC,%s,%03d,XX\r\nWrite: CFC,%s,%03d,XX,a:b:c>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id,
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
            case 0: break;
            case 1:
            {
                temp_di = p;
                if(0 != strcmp(temp_di,(char *)&sensor_di))
                    return 0;
            }break;
            case 2:
            {
                if(-1 == check_digit(p, strlen(p)))//校验 ID是否为数字
                {
                    rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                    return rlen;
                }
                temp_id = atoi(p);
                if(temp_id != m_defdata.m_baseinfo.id)
                    return 0;
            }break;
            case 3:
            {
                if((2 != strlen(p)) || (-1 == check_digit(p, 2)))
                {
                    rlen = sprintf((char *)rbuf,"<The sensor number must be two digits. Less than two use zero.>\r\n");
                    return rlen;
                }
                sensor_num = atoi(p);
            }break;
            case 4:
            {
                if(3 != sscanf(p,"%f:%f:%f",&temp_a, &temp_b, &temp_c) )
                {
                    rlen = sprintf((char *)rbuf,"<Have to put in three coefficients, a*X^2 + b*X + c.>\r\n");
                    return rlen;
                }
            }break;
            case 5: break;
            default: return 0;
        }
        p = strtok(NULL, ",");
        i++;
    }
    //1 确认当前为何种传感器  需要是第几个传感器的参数
    for(j = 0; j < DEV_TYPE_MAX; j ++)
    {
       if(0 == strncmp((char *)&sensor_di,(char *)dev_type[j],4))
         break;
    }
    if(j == 6)    //风
    {
       if((sensor_num > 2) || (sensor_num < 1))
       {
           rlen = sprintf((char *)rbuf,"<Only 1 or 2. 1 is wind speed, 2 is wind direction.>\r\n");
           return rlen;
       }
    }
    else if((j == 9) || (j == 10))   //地温  土壤水分
    {
       if((sensor_num > bcm_info.sensor.SensorNum) || (sensor_num < 1))
       {
          rlen = sprintf((char *)rbuf,"<The sensor number cannot exceed the number of sensors.>\r\n");
          return rlen;
       }
    }
    else	//其他传感器 标定参数设置不能大于6
    {
       if((sensor_num > 6) || (sensor_num < 1))
       {
          rlen = sprintf((char *)rbuf,"<The sensor number cannot exceed 6.>\r\n");
          return rlen;
       }
    }

    //2 读出当前数据
    if( i == 4)
    {
        rlen = sprintf((char *)rbuf,"<The sensor %d. Coefficient %f:%f:%f.>\r\n",sensor_num,
                                               bcm_info.sensor.cr[sensor_num-1].a,
                                               bcm_info.sensor.cr[sensor_num-1].b,
                                               bcm_info.sensor.cr[sensor_num-1].c);
        return rlen;
    }

    //3 存储系数
    bcm_info.sensor.cr[sensor_num-1].a = temp_a;
    bcm_info.sensor.cr[sensor_num-1].b = temp_b;
    bcm_info.sensor.cr[sensor_num-1].c = temp_c;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<The coefficients are written successfully.>\r\n");
    return rlen;
}
/*==================================================================
*函数：              cmd_windh
*作者：
*日期：           2017-12-28
*功能：  A.9　风传感器拨码开关设置
*参数：拨码值，对应风杆高度
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_windh(char *buf,char *rbuf)
{
    char *p = NULL, i = 0, rlen = 0;
    unsigned char  sensor_num = 0;
    char *temp_higt = NULL;

    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       goto err;
    }
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1:
        {
            if((2 < strlen(p)) || (-1 == check_digit(p, strlen(p))))
            {
                rlen = sprintf((char *)rbuf,"<The sensor number must be digits and between 4 to 13.>\r\n");
                goto err;
            }
            sensor_num = atoi(p);
            break;
        }
        case 2:
          {
              if(-1 == check_digit(p, strlen(p)))//校验 ID是否为数字
              {
                 rlen = sprintf((char *)rbuf,"<The sensor number must be all digits.>\r\n");
                 goto err;
              }
              temp_higt = p;
              break;
          }
        case 3: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;

    }
    if(( i != 2) && (i != 3))   {goto err;}

    //校验传感器编号是否超范围
    if(sensor_num > 13)
    {
        rlen = sprintf((char *)rbuf,"<The sensor number is too big.>\r\n");
        goto err;
    }
    //校验输入参数范围
    if(atol(temp_higt) > 100000) //1000米
    {
        rlen = sprintf((char *)rbuf,"<The height of the wind rod is too high.>\r\n");
        goto err;
    }

    //写入数组
    if(sensor_num > 3)
    {
        strcpy(bcm_info.sensor.str_name[sensor_num-4],temp_higt);
        save_sys_cfg(&bcm_info);
        rlen = sprintf((char *)rbuf,"<Write to success.>\r\n");
    }
    else
        rlen = sprintf((char *)rbuf,"<Write failure. Default configuration.>\r\n");

    return rlen;

err:
  return rlen;
}
/*==================================================================
*函数：              cmd_stsensor
*作者：
*日期：           2017-12-28
*功能：  A.10　地温传感器配置
*参数：地温传感器使用个数，对应的观测层
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_stsensor(char *buf,char *rbuf)
{
    char rlen = 0, *temp_di = NULL;
    short temp_id = 0;
    char *p = NULL, i = 0, j = 0;

    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       goto err;
    }
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
            case 0: break;
            case 1:
            {
                temp_di = p;
                if(0 != strcmp(temp_di,(char *)&sensor_di))
                    return 0;
            }break;
            case 2:
            {
                if(-1 == check_digit(p, strlen(p)))//校验 ID是否为数字
                {
                    rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                    return rlen;
                }
                temp_id = atoi(p);
                if(temp_id != m_defdata.m_baseinfo.id)
                    return 0;
            }break;
            case 3:
            {
                if((1 != strlen(p)) || (-1 == check_digit(p, 1)))
                {
                    rlen = sprintf((char *)rbuf,"<The sensor number must be one digits.>\r\n");
                    goto err;
                }
                bcm_info.sensor.SensorNum = atoi(p);
                bcm_info.sensor.data_num = atoi(p) * 2;
                //校验传感器个数是否超范围
                if((bcm_info.sensor.SensorNum == 9) || (bcm_info.sensor.SensorNum == 0))
                {
                    rlen = sprintf((char *)rbuf,"<The sensor number is between 0 and 9.>\r\n");
                    goto err;
                }
            }break;
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            {
                if((0 == strncmp(p,"A",1)) || (0 == strncmp(p,"B",1)))
                {
                    memset(&bcm_info.sensor.str_name[i-4][0],0,sizeof(bcm_info.sensor.str_name[i-4]));
                    strncpy(&bcm_info.sensor.str_name[i-4][0],"AB",2);
                    strncpy(&bcm_info.sensor.str_name[i-4][2],p,1);

                    memset(&bcm_info.sensor.str_name[i+4][0],0,sizeof(bcm_info.sensor.str_name[i+4]));
                    strncpy(&bcm_info.sensor.str_name[i+4][0],"AB",2);
                    strncpy(&bcm_info.sensor.str_name[i+4][2],p,1);
                    strncpy(&bcm_info.sensor.str_name[i+4][3],"l",1);
                }
                else if(0 == check_digit(p, strlen(p)))
                {
                    if(atoi(p) < 1000)
                    {
                        memset(&bcm_info.sensor.str_name[i-4][0],0,sizeof(bcm_info.sensor.str_name[i-4]));
                        strncpy(&bcm_info.sensor.str_name[i-4][0],"AB",2);
                        strncpy(&bcm_info.sensor.str_name[i-4][2],p,strlen(p));

                        memset(&bcm_info.sensor.str_name[i+4][0],0,sizeof(bcm_info.sensor.str_name[i+4]));
                        strncpy(&bcm_info.sensor.str_name[i+4][0],"ABl",3);
                        strncpy(&bcm_info.sensor.str_name[i+4][3],p,strlen(p));
                    }
                    else
                    {
                        rlen = sprintf((char *)rbuf,"<The sensor deep must be less than 999.>\r\n");
                        goto err;
                    }
                }
                else
                {
                    rlen = sprintf((char *)rbuf,"<The sensor deep must be A or B or from 1 to 999.>\r\n");
                    goto err;
                }
            }break;
            case 12: break;
            default: return 0;
        }
        p = strtok(NULL, ",");
        i++;
    }

    if(( i != 3) && (i != (bcm_info.sensor.SensorNum +4)))   {goto err;}

    if(i == 3)
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d",sensor_di, m_defdata.m_baseinfo.id );

        for(j = 0; j < bcm_info.sensor.SensorNum; j++)
        {
            rlen += sprintf((char *)(rbuf+rlen),",%s",&bcm_info.sensor.str_name[j][0]);
        }

        rlen += sprintf((char *)(rbuf+rlen),">\r\n");

        return rlen;
    }
    //写入数组

    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<Write to success.>\r\n");
    return rlen;

err:
  return rlen;
}
/*==================================================================
*函数：              cmd_shsensor
*作者：
*日期：           2017-12-28
*功能：  A.11　土壤水分传感器配置
*参数：地温传感器使用个数，对应的观测层
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_shsensor(char *buf,char *rbuf)
{
    char *p = NULL, i = 0, rlen = 0;
    char sensor_num = 0, temp_deep = 0;

    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       goto err;
    }
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        if(i == 1)
        {
            if((2 < strlen(p)) || (-1 == check_digit(p, strlen(p))))
            {
                rlen = sprintf((char *)rbuf,"<The sensor number must be digits.>\r\n");
                goto err;
            }
            sensor_num = atoi(p);
            //校验传感器编号是否超范围
            if((sensor_num >10) || (sensor_num < 1))
            {
                rlen = sprintf((char *)rbuf,"<The sensor number is between 1 and 10.>\r\n");
                goto err;
            }
        }

        if ((1 < i)&& (i < sensor_num+2))
        {
            if(0 == check_digit(p, strlen(p)))
            {
                temp_deep = atoi(p);
                if(temp_deep < 1000)
                    strncpy(bcm_info.sensor.str_name[i-2],p,strlen(p));
                else
                {
                    rlen = sprintf((char *)rbuf,"<The sensor deep must be less than 999.>\r\n");
                    goto err;
                }
            }
            else
            {
                rlen = sprintf((char *)rbuf,"<The sensor deep must be digits.>\r\n");
                goto err;
            }
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 2) && (i != (sensor_num +2)))   {goto err;}

    //写入数组
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<Write to success.>\r\n");
    return rlen;

err:
  return rlen;
}

/*==================================================================
*函数：              cmd_shsensor
*作者：
*日期：           2017-12-28
*功能：  A.11　土壤水分传感器配置
*参数：地温传感器使用个数，对应的观测层
*输入：           *buf  (输入数据）输入数据地址
*         *rbuf (输出数据）控制结构地址
*
*返回：           打印字符串长度
*修改记录：
==================================================================*/
int cmd_version_cfg(char *buf,char *rbuf)
{
    char *p = NULL, i = 0, rlen = 0, buffer[20];
    memset(buffer, 0x00, sizeof(buffer));
    
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;
        case 2: break;
        case 3:
        {
            if(strlen(p) <= sizeof(buffer))
            {
                sprintf((char *)buffer, "%s", p);
            }
            else
            {
                goto err;
            }
            break;
        }
        default:    
            goto err;
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4))  
        goto err;
    
    //硬件版本写入结构体
    memset(m_defdata.m_baseinfo.hard_version, 0x00, sizeof(m_defdata.m_baseinfo.hard_version));
    sprintf((char *)m_defdata.m_baseinfo.hard_version, "%s", buffer);

    //结构体写入FLASH  
    pamsave(&m_defdata);
    
    //返回成功
    rlen = sprintf((char *)rbuf,"set hard version success");
    return rlen;
err:
    //返回失败
    rlen = sprintf((char *)rbuf,"set hard version fail");
    return rlen;
}
