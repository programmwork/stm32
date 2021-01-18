/*************************************************************************************************************
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
;*************************************************************************************************************/
#include "sensor_basic.h"

float data_sample_buf[SAMPLE_DATA_NUM];//采样数据保存缓冲区
float data_sample_buf_history[SAMPLE_DATA_NUM];//采样数据保存缓冲区
unsigned char sample_qc[MAX_SENSOR_NUM][4];		//第二个值 和采样值有关
unsigned char sample_qc_history[MAX_SENSOR_NUM][4];

unsigned char Num_sample = 0;

float         LastValue_s   [MAX_SENSOR_NUM];
unsigned char beenRun_s     [MAX_SENSOR_NUM];
float         LastValue     [MAX_SENSOR_NUM];
unsigned char beenRun       [MAX_SENSOR_NUM];
float         LastValue_H   [MAX_SENSOR_NUM];
unsigned char beenRun_H     [MAX_SENSOR_NUM];

/*=============================================================================================
*函数功能：毫秒级定时采集 
*入口参数：采样数据结构体
*出口参数：采样数据结构体
*调用者  ：sapi.c中的定时采样调度者   每秒进行一次调用
*函数引用：Element_SecSample(&sensors_data)
==============================================================================================*/
unsigned char Element_MsSample(sensors_data_t *sensors_tempdata)
{
  return 1;
}
/*============================================================================================
*函数功能：定时采集
*入口参数：采样数据结构体
*出口参数：采样数据结构体
*调用者  ：sapi.c中的定时采样调度者   每秒进行一次调用
*函数引用：Element_SecSample(&sensors_data)
============================================================================================*/
unsigned char Element_SecSample(sensors_data_t *sensors_tempdata)
{
    unsigned char i = 0;
    float temp_value[MAX_SENSOR_NUM + 1] = {0};
    unsigned char temp_sample_event = 0;

  if(Num_sample<SAMPLE_COUNT)
  {
    temp_sample_event= AirP_engine(temp_value);//读计数清零计数
    if(temp_sample_event!=1)//没有采集完成
      return 0;
    else
    {//采集完成
      if(Flag_DataEmulationMode == 1)//当X1为1时传感器允许产生模拟数据
      {
        if(EmulationSensorChannel == 1)
        {
          sensors_tempdata->sensor[0].secdata.data  = EmulationData/10;//比例因子为1
          //          EmulationData = 0;
          if(EmulationDataType == 1)//是否允许在模拟值
            sensors_tempdata->sensor[0].secdata.data =  sensors_tempdata->sensor[0].secdata.data;
          else if(EmulationDataType == 0)
            sensors_tempdata->sensor[0].secdata.data =  sensors_tempdata->sensor[0].secdata.data;
        }
        else return 0;
      }

      for(i = 0;i<MAX_SENSOR_NUM;i++)
      {
        //校正参数  CR
        sensors_tempdata->sensor[i].secdata.data =  bcm_info.sensor.cr[i].a * temp_value[i] * temp_value[i] +\
                                             bcm_info.sensor.cr[i].b * temp_value[i] + bcm_info.sensor.cr[i].c;
      }
      
      for(i = 0;i<MAX_SENSOR_NUM;i++)
      {
        data_sample_buf[SAMPLE_COUNT*i+Num_sample] = sensors_tempdata->sensor[i].secdata.data;
      }
      Quality_Sec_Ctrl(sensors_tempdata);//质量控制
      
      for(i = 0;i<MAX_SENSOR_NUM;i++)
      {
        if(sensors_tempdata->sensor[i].secdata.qc == QC_OK) //正确的QC值，写1 按bit处理
          sample_qc[i][(Num_sample)/8] |= (1<<((Num_sample)%8));
      }
      Num_sample++;

      if(m_tempdata.SecDataOut == true)//秒级采样 数据及质控码 打印
      {
        char temp[32];
        uartSendStr(UARTDEV_1,"<",1);
        for(i = 0;i<MAX_SENSOR_NUM;i++)
        {
          sprintf(temp,"%.2f,%d  ",
                  sensors_tempdata->sensor[i].secdata.data,
                  sensors_tempdata->sensor[i].secdata.qc);
          uartSendStr(UARTDEV_1,(unsigned char *)temp,strlen(temp));
        }
        sprintf(temp,"P_T:%.2f", temp_value[1]);
        uartSendStr(UARTDEV_1,(unsigned char *)temp,strlen(temp));

        uartSendStr(UARTDEV_1,">\r\n",3);
        return 1;
      }
    }
  }
  return 0;
}

/*
*函数功能：定时采集
*入口参数：采样数据结构体
*出口参数：采样数据结构体
*调用者  ：sapi.c中的定时采样调度者   每分钟进行一次调用
*函数引用：Element_MinSample(&sensors_data)
*/
unsigned char Element_MinSample(sensors_data_t *sensors_tempdata)
{
    uint16 i = 0;
  
	//板卡自检
	Self_test();

	memcpy(data_sample_buf_history,data_sample_buf,sizeof(data_sample_buf));
	memset(data_sample_buf,0,sizeof(data_sample_buf));	//采样数据缓冲区清0
  
	memcpy(sample_qc_history,sample_qc,sizeof(sample_qc));
	memset(sample_qc,0,sizeof(sample_qc));

    for(i = 0;i<MAX_SENSOR_NUM;i++)
    {
        if(sensors_tempdata->sensor[i].secdata_counter< (2*SAMPLE_COUNT/3))//2*30/3
        {//缺测
            sensors_tempdata->sensor[i].mindata.data=INVALID_DATA;
            sensors_tempdata->sensor[i].mindata.qc=QC_MISSED;
        }
        else
        {//正确
            sensors_tempdata->sensor[i].mindata.data= sensors_tempdata->sensor[i].secdata_add.data \
                                                    / sensors_tempdata->sensor[i].secdata_counter;
           Quality_Min_Ctrl(sensors_tempdata,i);
        }
    }
    Num_sample = 0;

    for(i = 0;i<MAX_SENSOR_NUM;i++)
    {//分钟数据清0
        sensors_tempdata->sensor[i].secdata_add.data = 0;
        sensors_tempdata->sensor[i].secdata_counter = 0;
    }

    return 1;
}

/*
*函数功能：秒级质量控制
*入口参数：采样数据结构体
*出口参数：质量控制码
*调用者  ：秒级采样函数
*函数引用：Quality_Sec_Ctrl(&sensors_data)
*/
static unsigned char Quality_Sec_Ctrl(sensors_data_t *sensors_tempdata)
{
    unsigned char i = 0;

  for(i = 0;i<MAX_SENSOR_NUM;i++)
  {
    float result_sensor = sensors_tempdata->sensor[i].secdata.data;
    //上下限判断
    if((result_sensor<bcm_info.sensor.qs[i].min) || (result_sensor>bcm_info.sensor.qs[i].max))
    {
      //sensors_tempdata->sensor[i].secdata.data=INVALID_DATA;
      sensors_tempdata->sensor[i].secdata.qc=QC_ERROR; 
    }
    else
    {    
      sensors_tempdata->sensor[i].secdata.qc=iQCPS_Code(i,sensors_tempdata->sensor[i].secdata.data,&bcm_info.sensor.qs[i]);
      
      if(sensors_tempdata->sensor[i].secdata.qc == QC_OK)
      {
        sensors_tempdata->sensor[i].secdata_counter++;//分钟数据累加
        sensors_tempdata->sensor[i].secdata_add.data+=result_sensor;
        sensors_tempdata->sensor[i].secdata_add.qc=QC_OK; 
      }
    }
  }
  return 0;
}
/*
*函数功能：分钟级质量控制
*入口参数：采样数据结构体
*出口参数：质量控制码
*调用者  ：分钟采样函数
*函数引用：Quality_Sec_Ctrl(&sensors_data,0)
*/
static unsigned char Quality_Min_Ctrl(sensors_data_t *sensors_tempdata,unsigned char ch)
{
  float result_sensor, buffer = 0, max = 0, min = 0;
  unsigned char i = 0, j = 0, count = 0;

  //一分钟的质量控制
    result_sensor = sensors_tempdata->sensor[ch].mindata.data;
    //上下限判断
    if((result_sensor<bcm_info.sensor.qm[ch].min)||(result_sensor>bcm_info.sensor.qm[ch].max))
    {
      //sensors_tempdata->sensor[ch].mindata.data=INVALID_DATA;
      sensors_tempdata->sensor[ch].mindata.qc=QC_ERROR;
    }
    else
    {    
      sensors_tempdata->sensor[ch].mindata.qc=iQCPM_Code(ch,sensors_tempdata->sensor[ch].mindata.data,&bcm_info.sensor.qm[ch]);
    }

    //最小应变化检测
    if(sensors_tempdata->sensor[ch].mindata.qc != QC_MISSED)
    {
        sensors_tempdata->sensor[ch].hour_data[sensors_tempdata->sensor[ch].hourdata_counter] = result_sensor;
    }
    else
    {
        sensors_tempdata->sensor[ch].hour_data[sensors_tempdata->sensor[ch].hourdata_counter] = INVALID_DATA;
    }

    sensors_tempdata->sensor[ch].hourdata_counter++;

    if((sensors_tempdata->sensor[ch].hourdata_counter >= 60) || (sensors_tempdata->sensor[ch].hour_data_flag == 1))
    {
        sensors_tempdata->sensor[ch].hour_data_flag = 1;

        for(i = 0; i < 60; i++)
        {
            if(sensors_tempdata->sensor[ch].hour_data[i] != INVALID_DATA)
            {
                if(count == 0)
                {
                    max = sensors_tempdata->sensor[ch].hour_data[i];
                    min = sensors_tempdata->sensor[ch].hour_data[i];
                }

                if(sensors_tempdata->sensor[ch].hour_data[i] > max)
                {
                    max = sensors_tempdata->sensor[ch].hour_data[i];
                }

                if(sensors_tempdata->sensor[ch].hour_data[i] < min)
                {
                    min = sensors_tempdata->sensor[ch].hour_data[i];
                }

                count++;
            }
        }

        if((bcm_info.sensor.qm[ch].changerate != INVALID_DATA) && (sensors_tempdata->sensor[ch].mindata.qc == QC_OK))
        {
            if(count >= 2)
            {
                buffer = fabs(max - min);

                if(buffer < bcm_info.sensor.qm[ch].changerate)
                {
                    sensors_tempdata->sensor[ch].mindata.qc = QC_DOUBT;
                }
            }
        }

        if(sensors_tempdata->sensor[ch].hourdata_counter >= 60)
            sensors_tempdata->sensor[ch].hourdata_counter = 0;
    }
    return 0;
}
/*
*函数功能：采样数据初始化
*入口参数：采样数据结构体
*出口参数：无
*调用者  ：sapi.c中的系统初始化函数
*函数引用：SampleData_Init(&sensors_data)
*/
void SampleData_Init(sensors_data_t *sensors_tempdata)
{
    unsigned char i = 0;

	for(i = 0;i<MAX_SENSOR_NUM;i++)
	{
		//秒数据
		sensors_tempdata->sensor[i].secdata.data=INVALID_DATA;
		sensors_tempdata->sensor[i].secdata.qc=QC_MISSED;
		//秒数据累加
		sensors_tempdata->sensor[i].secdata_add.data=0;
		sensors_tempdata->sensor[i].secdata_add.qc=QC_MISSED;
		//秒数据个数
		sensors_tempdata->sensor[i].secdata_counter=0;
		//分钟数据	
		sensors_tempdata->sensor[i].mindata.data=INVALID_DATA;	
		sensors_tempdata->sensor[i].mindata.qc=QC_MISSED;
		//分钟数据	累加
		sensors_tempdata->sensor[i].mindata_add.data=0;	
		sensors_tempdata->sensor[i].mindata_add.qc=QC_MISSED;
		//分钟数据	个数
		sensors_tempdata->sensor[i].mindata_counter=0;	

        sensors_tempdata->sensor[i].hourdata_counter = 0;   //小时数据个数累加
        sensors_tempdata->sensor[i].num = 0;
        sensors_tempdata->sensor[i].hour_data_flag = 0;
	}

	memset(data_sample_buf,0,sizeof(data_sample_buf));//采样数据缓冲区清0
	memset(data_sample_buf_history,0,sizeof(data_sample_buf_history));//采样数据缓冲区清0
	memset(sample_qc,0,sizeof(sample_qc));//采样标志位缓冲区清0
	memset(sample_qc_history,0,sizeof(sample_qc_history));

	//初始化 QC数据缓存
	memset(LastValue_s, 0, sizeof(LastValue_s));
	memset(beenRun_s,   0, MAX_SENSOR_NUM);
	memset(LastValue,   0, sizeof(LastValue));
	memset(beenRun,     0, MAX_SENSOR_NUM);
	memset(LastValue_H, 0, sizeof(LastValue_H));
	memset(beenRun_H,   0, MAX_SENSOR_NUM);
}

/*================================================================================================
*函数功能：秒数据质量控制函数
*入口参数：sensorNo--几支传感器，value--采样数据值，*qcctrl--qc初始化的质量控制码
*出口参数：运算后的质量控制码
*调用者  ：Quality_Sec_Ctrl函数
*函数引用：iQCPS_Code(0,sensors_tempdata->sensor[0].secdata.data,&sensors_tempdata->qualityCtrl[0]);
================================================================================================*/
unsigned char iQCPS_Code(int sensorNo, float value, qs_t *qcctrl)
{
  unsigned char iRet=QC_OK;

  if(beenRun_s[sensorNo]==0)
  {
    beenRun_s[sensorNo]=1;
  }
  else
  {
      if(qcctrl->change != INVALID_DATA)
      {
          if(fabs(value-LastValue_s[sensorNo]) > qcctrl->change)
              iRet = QC_DOUBT;
      }
  }
  LastValue_s[sensorNo]=value;
  return iRet; 
  
}

/*
*函数功能：秒数据质量控制函数
*入口参数：sensorNo--几支传感器，value--采样数据值，*qcctrl--qc初始化的质量控制码
*出口参数：运算后的质量控制码
*调用者  ：Quality_Sec_Ctrl函数
*函数引用：iQCPS_Code(0,sensors_tempdata->sensor[0].secdata.data,&sensors_tempdata->qualityCtrl[0]);
*/
unsigned char iQCPM_Code(int sensorNo,float value,qm_t *qcctrl)
{
  unsigned char iRet=QC_OK;

  if(beenRun[sensorNo]==0)
  {
    beenRun[sensorNo]=1;
  }
  else
  {
      if(qcctrl->doubtful != INVALID_DATA)
      {
        if(fabs(value-LastValue[sensorNo])>qcctrl->doubtful)
        {
          iRet =QC_DOUBT;
        }
      }

      if(qcctrl->err != INVALID_DATA)
      {
        if(fabs(value-LastValue[sensorNo])>qcctrl->err)
        {
          iRet =QC_ERROR;
        }
      }
  }

  LastValue[sensorNo]=value;
  return iRet;
}

/*==================================================================
 * 函数名称： GetMinData
 * 函数功能：生成分钟数据
 *
 * 入口参数：    char *Header_packet   申请的这一片内存首地址
            sensors_data_t *sensors_tempdata 需要存储的结构体指针
            unsigned char num_mins  功能选择： 0--历史数据下载 (DOWN)
                                            1--分钟数据存储 （来自分钟数据存储函数调用）
                                            2--当前历史数据读取（READDATA）
                                            3--预留  自动补传
 * 出口参数：    无
 * 编写时间：
 * 修改时间：
==================================================================*/
int GetMinData(char *Header_packet,sensors_data_t *sensors_tempdata,unsigned char num_mins)
{
  unsigned int lenlen = 0;
  UINT32 temp_time = 0;
  unsigned short i;
  s_RtcTateTime_t temp_time_struct;//时间结构体
  uint16 Checksum = 0;
  bcm_info_t info;
  FLASH_Store_MinData_t Mindata_temp;//定义一个临时的数据结构体  sensors_data_t
  float standardDeviation_1 = 0;
  
  memcpy(&info, &bcm_info,sizeof(bcm_info_t));
  
  strcpy(Header_packet,"BG,");//包头
  
  S32ToStrAlignRight(info.common.version, 3, Header_packet+3);//版本号
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

  S32ToStrAlignRight(info.common.qz, 5, Header_packet+lenlen);//设备区站号
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

  S32ToStrAlignRight(info.common.Lat.degree, 2, Header_packet+lenlen);//设备纬度
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(info.common.Lat.min, 2, Header_packet+lenlen);//设备纬度
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(info.common.Lat.sec, 2, Header_packet+lenlen);//设备纬度
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

  S32ToStrAlignRight(info.common.Long.degree, 3, Header_packet+lenlen);//设备经度
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(info.common.Long.min, 2, Header_packet+lenlen);//设备经度
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(info.common.Long.sec, 2, Header_packet+lenlen);//设备经度
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;
  
  S32ToStrAlignRight(info.common.High, 5, Header_packet+lenlen);//设备海拔
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

  S32ToStrAlignRight(info.common.st, 2, Header_packet+lenlen);//设备服务类型
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;
  
  strncpy(Header_packet+lenlen,(char const *)sensor_di,4);//设备标识
  lenlen= lenlen +4; 
  Header_packet[lenlen]=',';
  lenlen++;
  
  S32ToStrAlignRight(m_defdata.m_baseinfo.id, 3, Header_packet+lenlen);//设备ID
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;    
  
  if(num_mins == 1)//需要存储分钟数据，来自分钟数据主动调用
  {
      Convert_Time_to_min(&temp_time);
      temp_time = temp_time;    //前一分钟数据，打当前分钟时间签
      Convert_Sec_to_Time(&temp_time_struct, temp_time*60);
  }
  else if(num_mins == 0)//读历史数据 DOWN
  {
      Convert_Sec_to_Time(&temp_time_struct,m_tempdata.time_start*60);//读历史数据
  }
  else if(num_mins == 2)//读历史数据 READDATA
  {
      Convert_Sec_to_Time(&temp_time_struct,m_tempdata.time_start_read*60);//读历史数据
  }
  else if(num_mins == 3)//自动补传历史数据
  {

  }
  
  S32ToStrAlignRight(temp_time_struct.year,4, Header_packet+lenlen);//时间
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(temp_time_struct.month,2, Header_packet+lenlen);
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(temp_time_struct.day,2, Header_packet+lenlen);
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(temp_time_struct.hour,2, Header_packet+lenlen);
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(temp_time_struct.min,2, Header_packet+lenlen);
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(temp_time_struct.sec,2, Header_packet+lenlen);
  lenlen=strlen((char *)Header_packet);
  
  Header_packet[lenlen]=',';
  lenlen++;
  
  info.common.fi = 1;
  S32ToStrAlignRight(info.common.fi, 3, Header_packet+lenlen);//帧标识
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

  if(Flag_nodata ==0)  //flash中无数据  执行此段
  {
  S32ToStrAlignRight(info.sensor.data_num, 3, Header_packet+lenlen);//观测要素数量
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

  //---------------------------state information---------------------------
  if(num_mins == 1)//需要存储分钟数据，来自主动调用
  { //数据保存至flash结构体   存储时数据均放大100倍取整
      FLASH_Store_MinData.boardsvolt = sensors_tempdata->boardsvolt_data;
      FLASH_Store_MinData.boardstemp = sensors_tempdata->boardstemp_data;
      Mindata_temp.boardsvolt = sensors_tempdata->boardsvolt_data;
      Mindata_temp.boardstemp = sensors_tempdata->boardstemp_data;

      memcpy(&FLASH_Store_MinData.state, &sensor_state, sizeof(sensor_state_t));
      memcpy(&Mindata_temp.state, &sensor_state, sizeof(sensor_state_t));

      FLASH_Store_MinData.flag = 1;   //flash存储标志
  }
  else if((num_mins == 0)||(num_mins == 2))//不需要存储的分钟数据，来自历史数据读取调用
  {
      Mindata_temp.boardsvolt = FLASH_Read_MinData.boardsvolt;
      Mindata_temp.boardstemp = FLASH_Read_MinData.boardstemp;

      memcpy(&Mindata_temp.state, &FLASH_Read_MinData.state, sizeof(sensor_state_t));
  }

  S32ToStrAlignRight(Mindata_temp.state.state_num, 2, Header_packet+lenlen);//设备状态变量数
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

//---------------------------head  finish--------------------------
    //---------------------------data  start---------------------------
    //------------------------ 1MIN分钟数据 -----------------------
    if(num_mins == 1)//需要存储分钟数据，来自主动调用
    { //数据保存至flash结构体   存储时数据均放大100倍取整
        if(sensors_tempdata->sensor[0].mindata.data == INVALID_DATA)
        {
			FLASH_Store_MinData.sensor[0].mindata.data = INVALID_DATA;
			Mindata_temp.sensor[0].mindata.data = INVALID_DATA;
		}
		else
        {
		  if((bcm_info.sensor.apunit == 1) || (bcm_info.sensor.apunit == 2))
		  {
		      FLASH_Store_MinData.sensor[0].mindata.data = (long)((sensors_tempdata->sensor[0].mindata.data * 100));  //放大十倍
		      Mindata_temp.sensor[0].mindata.data = sensors_tempdata->sensor[0].mindata.data * 10;
		  }
		  else if(bcm_info.sensor.apunit == 3)  //放大百倍，并转换成 单位 毫米汞柱
		  {
		      FLASH_Store_MinData.sensor[0].mindata.data = (long)((sensors_tempdata->sensor[0].mindata.data) * 100 * 0.750062);
		      Mindata_temp.sensor[0].mindata.data = (sensors_tempdata->sensor[0].mindata.data) * 100 * 0.750062;
		  }
		  else if(bcm_info.sensor.apunit == 4)  //放大百倍，并转换成 单位 英寸汞柱
		  {
		      FLASH_Store_MinData.sensor[0].mindata.data = (long)((sensors_tempdata->sensor[0].mindata.data) * 100* 0.029530);
		      Mindata_temp.sensor[0].mindata.data = (sensors_tempdata->sensor[0].mindata.data) * 100 * 0.029530;
		  }
        }
        FLASH_Store_MinData.sensor[0].mindata.qc = sensors_tempdata->sensor[0].mindata.qc;
        FLASH_Store_MinData.boardsvolt = sensors_tempdata->boardsvolt_data;
        FLASH_Store_MinData.boardstemp = sensors_tempdata->boardstemp_data;
      
        Mindata_temp.sensor[0].mindata.qc   = sensors_tempdata->sensor[0].mindata.qc;
        Mindata_temp.boardsvolt = sensors_tempdata->boardsvolt_data;
        Mindata_temp.boardstemp = sensors_tempdata->boardstemp_data;
        FLASH_Store_MinData.flag = 1;   //flash存储标志
    }
    else if((num_mins == 0)||(num_mins == 2))//不需要存储的分钟数据，来自历史数据读取调用
    {
        if(FLASH_Read_MinData.sensor[0].mindata.data == INVALID_DATA)
        	Mindata_temp.sensor[0].mindata.data = INVALID_DATA;
        else
        {
        	Mindata_temp.sensor[0].mindata.data = (float)((float)FLASH_Read_MinData.sensor[0].mindata.data/10);
        }  
        Mindata_temp.sensor[0].mindata.qc   = FLASH_Read_MinData.sensor[0].mindata.qc;
        Mindata_temp.boardsvolt = FLASH_Read_MinData.boardsvolt;
        Mindata_temp.boardstemp = FLASH_Read_MinData.boardstemp;
    }

    strcpy(Header_packet+lenlen,"AGA");
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;

    //mindata
    Pack_Data(&Mindata_temp.sensor[0].mindata, &lenlen, Header_packet, 5);
    
    //=========================== 标准差 AGAl ======================================
    strcpy(Header_packet+lenlen,"AGAl");
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    
    if(num_mins == 1)//需要存储分钟数据，来自主动调用
    { //数据保存至flash结构体
      standardDeviation_1 = StandardDeviation(data_sample_buf_history,sample_qc_history[0],SAMPLE_COUNT);
      if(INVALID_DATA != Mindata_temp.sensor[0].mindata.data)
          Mindata_temp.Standard_Deviation[0] = standardDeviation_1 * 10000;
      else
          Mindata_temp.Standard_Deviation[0] = INVALID_DATA;
      
      FLASH_Store_MinData.Standard_Deviation[0] = Mindata_temp.Standard_Deviation[0];
    }
    else if((num_mins == 0)||(num_mins == 2))//不需要存储的分钟数据，来自历史数据读取调用
    {
        if(INVALID_DATA != Mindata_temp.sensor[0].mindata.data)
            Mindata_temp.Standard_Deviation[0] = FLASH_Read_MinData.Standard_Deviation[0];
        else
            Mindata_temp.Standard_Deviation[0] = INVALID_DATA;
    }
    else ;
    
    Pack_Datal(&Mindata_temp.Standard_Deviation[0], &lenlen, Header_packet, 6);

    //AGA qc
    S32ToStrAlignRight(Mindata_temp.sensor[0].mindata.qc, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    //AGAI qc
    Header_packet[lenlen]=QC_NOCONTROL+0x30;
    lenlen++;
    Header_packet[lenlen]=',';
    lenlen++;

    //状态信息
    strcpy(Header_packet+lenlen,"z,");  //自检状态
    lenlen+=2;
    S32ToStrAlignRight(Mindata_temp.state.self_test, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;

    if(0 != Mindata_temp.state.board_temp)
    {
        strcpy(Header_packet+lenlen,"wA");  //板温
        lenlen=strlen((char *)Header_packet);
        Header_packet[lenlen]=',';
        lenlen++;
        S32ToStrAlignRight(Mindata_temp.state.board_temp, 1, Header_packet+lenlen);
        lenlen=strlen((char *)Header_packet);
        Header_packet[lenlen]=',';
        lenlen++;
    }

    strcpy(Header_packet+lenlen,"xA");  //外接电源
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    S32ToStrAlignRight(Mindata_temp.state.exter_power, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;

    if(0 != Mindata_temp.state.board_volt)
    {
        strcpy(Header_packet+lenlen,"xB");  //板压
        lenlen=strlen((char *)Header_packet);
        Header_packet[lenlen]=',';
        lenlen++;
        S32ToStrAlignRight(Mindata_temp.state.board_volt, 1, Header_packet+lenlen);
        lenlen=strlen((char *)Header_packet);
        Header_packet[lenlen]=',';
        lenlen++;
    }

    if(0 != Mindata_temp.state.board_current)
    {
        strcpy(Header_packet+lenlen,"xH");  //工作电流状态
        lenlen=strlen((char *)Header_packet);
        Header_packet[lenlen]=',';
        lenlen++;
        S32ToStrAlignRight(Mindata_temp.state.board_current, 1, Header_packet+lenlen);
        lenlen=strlen((char *)Header_packet);
        Header_packet[lenlen]=',';
        lenlen++;
    }

    if(0 != Mindata_temp.state.exter_volt)
    {
        strcpy(Header_packet+lenlen,"xJ");  //外接电源状态
        lenlen=strlen((char *)Header_packet);
        Header_packet[lenlen]=',';
        lenlen++;
        S32ToStrAlignRight(Mindata_temp.state.exter_volt, 1, Header_packet+lenlen);
        lenlen=strlen((char *)Header_packet);
        Header_packet[lenlen]=',';
        lenlen++;
    }

    /*if(0 != Mindata_temp.state.bat_volt)
    {
        strcpy(Header_packet+lenlen,"xD");  //蓄电池电压状态
        lenlen=strlen((char *)Header_packet);
        Header_packet[lenlen]=',';
        lenlen++;
        S32ToStrAlignRight(Mindata_temp.state.bat_volt, 1, Header_packet+lenlen);
        lenlen=strlen((char *)Header_packet);
        Header_packet[lenlen]=',';
        lenlen++;
    }*/
  }
  else
  {
      strcpy(Header_packet+lenlen,"/////");
      lenlen=strlen((char *)Header_packet);
      Header_packet[lenlen]=',';
      lenlen++;
  }
  //---------------------------data  end---------------------------
  //---------------------------Checksum---------------------------
  for(i=0; i<lenlen; i++)
  {
    Checksum += Header_packet[i]; 
  }
  Checksum = Checksum%10000;
  //Checksum ^= (((unsigned int)GetCheckSumCode() << 8) | (unsigned int)GetCheckSumCode());
  
  S32ToStrAlignRight(Checksum, 4, Header_packet+lenlen);//观测要素数量
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;
  //---------------------------packet end---------------------------
  strcpy(Header_packet+lenlen,"ED\r\n");
  
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]= '\0';

  return lenlen++;
}

void GetSampleData(sensors_data_t *sensors_tempdata,float *dat,uint8 ch)
{
  char Header_packet[40];
  
  unsigned int lenlen = 0;
  unsigned char i = 0;
  s_RtcTateTime_t temp_time_struct;//时间结构体
  
  bcm_info_t info;

  memcpy(&info, &bcm_info,sizeof(bcm_info_t));
  
  memset(Header_packet,0,40);
  
  //---------------------包头-------------------
  strcpy(Header_packet,"BG,");//包头
  uartSendStr(UARTDEV_1,(unsigned char *)Header_packet,0);
  
  uartSendStr(UARTDEV_1,(unsigned char *)sensor_di,4);
  
  uartSendStr(UARTDEV_1,",",0);
  
  S32ToStrAlignRight(m_defdata.m_baseinfo.id, 3, Header_packet);//设备ID
  Header_packet[3]=',';
  lenlen = 4;
  
  temp_time_struct = m_tempdata.m_RtcTateTime;
  
  temp_time_struct.sec = 0;//让秒为0
  
  S32ToStrAlignRight(temp_time_struct.year,4, Header_packet+lenlen);//时间
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(temp_time_struct.month,2, Header_packet+lenlen);
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(temp_time_struct.day,2, Header_packet+lenlen);
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(temp_time_struct.hour,2, Header_packet+lenlen);
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(temp_time_struct.min,2, Header_packet+lenlen);
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(temp_time_struct.sec,2, Header_packet+lenlen);
  lenlen=strlen((char *)Header_packet);
  
  Header_packet[lenlen]=',';
  lenlen++;
  Header_packet[lenlen]= '\0';
  uartSendStr(UARTDEV_1,(unsigned char *)Header_packet,0);
  
  memset(Header_packet,0,40);
  //---------------------数据主体-------------------
  
  strcpy(Header_packet,"ADA");
  Header_packet[3]=',';
  lenlen = 4;
  
  
  //取数据出来
  char int8_1[8];
  for(i=0; i<SAMPLE_COUNT; i++)
  {
    S32ToStrAlignRight(data_sample_buf_history[(ch-1)*SAMPLE_COUNT+i], 5, int8_1);
    uartSendStr(UARTDEV_1,(unsigned char *)int8_1,0);
    uartSendStr(UARTDEV_1,",",0);
  }
  
  //---------------------结束-------------------
  strcpy(Header_packet,"ED\r\n");  
  //  lenlen=strlen((char *)Header_packet);
  uartSendStr(UARTDEV_1,(unsigned char *)Header_packet,0);
}

