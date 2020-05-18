/*************************************************************************************************************
;*--------------------------------------------- 文件信息 ----------------------------------------------------
;* 文件名称 : 
;* 文件功能 : 
;* 补充说明 :
;*-------------------------------------------- 最新版本信息 -------------------------------------------------
;* 修改作者 :王建佳
;* 修改日期 :2015-5-12
;* 版本声明 :
;*-------------------------------------------- 历史版本信息 -------------------------------------------------
;* 文件作者 : 王建佳
;* 创建日期 : 2015-5-8
;* 版本声明 :
;*************************************************************************************************************/
#include "sensor_basic.h"

#define FENGSU_pamA         0.296                   //风速传感器系数
#define FENGSU_pamB         0.04909

#define PI 3.1415928
#define FX_START_ADDR_INDEX 1280    //第5page

float wdx=0;
float wdy=0;
float arch = 0;
static unsigned char Flag_max_feng = 0;//风向查询此标志位


float data_sample_buf[SAMPLE_DATA_NUM];//采样数据保存缓冲区
float data_sample_buf_history[SAMPLE_DATA_NUM];//采样数据保存缓冲区
unsigned char sample_qc[SAMPLE_DATA_NUM/8 +1];
unsigned char sample_qc_history[SAMPLE_DATA_NUM/8 +1];

unsigned char data_valid_num = 0;//上一分钟有效采样数据长度
static unsigned int sample_num_ws = 0;

float         LastValue_ws250[MAX_SENSOR_NUM];
unsigned char beenRun_ws250  [MAX_SENSOR_NUM];
float         LastValue_ws3s [MAX_SENSOR_NUM];
unsigned char beenRun_ws3s   [MAX_SENSOR_NUM];
float         LastValue_wd   [MAX_SENSOR_NUM];
unsigned char beenRun_wd     [MAX_SENSOR_NUM];

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
  unsigned char temp_fengsu_event = 0;
  float value_FengSu = 0;
  
  temp_fengsu_event = Fengsu_engine(&value_FengSu);//读频率值
  
  if(Flag_DataEmulationMode == 1)//当X1为1时传感器允许产生模拟数据
  {
    if(EmulationSensorChannel == 2)//风速
    {
      value_FengSu  = EmulationData_1/10;
      //  EmulationData_1 = 0;
    }
    //    else return 0;
  }
  //  else return 0;
  
  if(1!=temp_fengsu_event)
  {
    return 0;
  }
  else
  {    
    if(Flag_DataEmulationMode == 0)//当X1为1时传感器允许产生模拟数据
    {
      value_FengSu = (float)(FENGSU_pamA + FENGSU_pamB * value_FengSu);
      if(value_FengSu < 0.35) value_FengSu = 0;   /* 【jeff.liu】 */
    }
    else
    {
      if(EmulationDataType == 1)//是否允许在模拟值
        value_FengSu = (float)(FENGSU_pamA + FENGSU_pamB * value_FengSu);
      else if(EmulationDataType == 0)
        value_FengSu = value_FengSu;
    }
    //aX^2 + bX + c
    sensors_tempdata->sensor[0].fengsu_MomentaryValue.data =  bcm_info.sensor.cr[0].a * value_FengSu * value_FengSu+\
                                                              bcm_info.sensor.cr[0].b * value_FengSu + bcm_info.sensor.cr[0].c;

    //保存采样数据
    data_sample_buf[sample_num_ws] = (int)(sensors_tempdata->sensor[0].fengsu_MomentaryValue.data);
    
    Quality_Sec_Ctrl(sensors_tempdata,0);//质量控制
    //保存非正确标志位   风速前30个字节
    if(sensors_data.sensor[0].fengsu_MomentaryValue.qc == QC_OK)
      sample_qc[sample_num_ws/8] |=(1<<(sample_num_ws%8));
    
    sample_num_ws++;
    if(sample_num_ws == 240) sample_num_ws = 0; 
    
  }
  return 1;
}
/*
*函数功能：秒级定时采集 
*入口参数：采样数据结构体
*出口参数：采样数据结构体
*调用者  ：sapi.c中的定时采样调度者   每秒进行一次调用
*函数引用：Element_SecSample(&sensors_data)
*/
unsigned char Element_SecSample(sensors_data_t *sensors_tempdata)
{
  unsigned char temp_fengxiang_event = 0;
  float value_FengXiang = 0;
  
  temp_fengxiang_event = FengXiang_engine(&value_FengXiang);//读电压值

  if(Flag_DataEmulationMode == 1)//当X1为1时传感器允许产生模拟数据
  {
    if(EmulationSensorChannel == 1)//风向
    {
      temp_fengxiang_event = 1;
      value_FengXiang  = EmulationData;
    }
    //    else return 0;
  }
  
  if(1!=temp_fengxiang_event)
  {
    return 0;
  }
  else
  {
    if(Flag_DataEmulationMode == 0)//当X1为0时传感器恢复正常实测状态
    {
      sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data = value_FengXiang;
      
//     sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data = \
//       sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data *sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data * bcm_info.sensor.ab1+
//        sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data * bcm_info.sensor.ab2 + bcm_info.sensor.ab3;
//           sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data = \
//        sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data * bcm_info.sensor.ab1 + bcm_info.sensor.ab2;
      
      
        //  *pFengXiang = bcm_info.sensor.ab1 * v_Desired + bcm_info.sensor.ab2;    // 1.5米风进行一次线性校准

    }
    else//模拟状态
    {
      if(EmulationDataType == 1)//是否允许在模拟值
        sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data = value_FengXiang;
      else if(EmulationDataType == 0)
        sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data = value_FengXiang;
    }
    
    data_sample_buf[240+m_tempdata.m_RtcTateTime.sec] = (int)(sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data);

      Quality_Sec_Ctrl(sensors_tempdata,1);//质量控制    
      //保存非正确标志位   风速前15个字节
      if(sensors_tempdata->sensor[0].fengxiang_MomentaryValue.qc == QC_OK)
        sample_qc[m_tempdata.m_RtcTateTime.sec/8+30] |=(1<<(m_tempdata.m_RtcTateTime.sec%8));
      
      if(m_tempdata.SecDataOut == true)
      {
        char temp[32];
        sprintf(temp,"<SEC_WD:%.0f,%d  ", sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data,\
                                           sensors_tempdata->sensor[0].fengxiang_MomentaryValue.qc);
        uartSendStr(0,(unsigned char*)temp,strlen(temp));

        sprintf(temp,"SEC_WS:%.0f,%d>\r\n", sensors_tempdata->sensor[0].fengsu_3s.data,\
                                           sensors_tempdata->sensor[0].fengsu_3s.qc);
        uartSendStr(0,(unsigned char*)temp,strlen(temp));
      }
  }
  return 1;
}


static void WindDirectCal(float *winddir_x,float *winddir_y,float *winddir_d)
{
  static float temp_winddirect = 0;
  if(*winddir_y==0)
    *winddir_d=90;
  else
    *winddir_d=(atan(*winddir_x/ (*winddir_y)))*180/PI;//得到分钟风向平均值,弧度换成角度
  
  if((*winddir_x>0)&&(*winddir_y<0))
    *winddir_d=*winddir_d+180;
  else if((*winddir_x<0)&&(*winddir_y<0))
    *winddir_d=*winddir_d+180;
  else if((*winddir_x<0)&&(*winddir_y>0))
    *winddir_d=*winddir_d+360;
  
  if(*winddir_d>360)
    *winddir_d=*winddir_d-360;
  if((*winddir_x==0)&&(*winddir_y==-1))
    *winddir_d=*winddir_d+180;
  temp_winddirect = *winddir_d;
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
    unsigned char i = 0;
    float add = 0;
    unsigned char counter = 0;
  
    //若整分钟的时间不够，可以将其定位在某一秒内
    //板卡自检
    Self_test();
  
    sample_data_save((char *)data_sample_buf,sizeof(data_sample_buf));//采样数据保存

    memcpy(data_sample_buf_history,data_sample_buf,sizeof(data_sample_buf));
    memset(data_sample_buf,0,sizeof(data_sample_buf));  //采样数据缓冲区清0

    memcpy(sample_qc_history,sample_qc,sizeof(sample_qc));
    memset(sample_qc,0,sizeof(sample_qc));

  //data_valid_num =sensors_tempdata->sensor[0].secdata_counter; 
  
  //---------------------------------1min、2min   avg  wind speed--------------      
  //需要先减去1min、2min最前面的值
  //  if(sensors_tempdata->sensor[0].secdata_counter>80)//2min   120*2/3=80
  //  uint8 temp_wjj[48];        //////////////20151206   可以减去
  //  static uint8 min1_count = 0,min2_count = 0,min10_count = 0;
  
  /*【jeff.liu】*/
  //  if(m_tempdata.debugon == 1)
  //  {
  //    //    char temp_wjj[32];
  //    
  //    sprintf(temp_wjj,"MIN1_COUNT_WS:%d\r\n",
  //            sensors_tempdata->sensor[0].fengsu_min1_counter          ); 	// 【待修改】暂时显示频率和采码值
  //    uartSendStr(0,(const char*)temp_wjj,strlen(temp_wjj));
  //  }
  
  //----------------------------1min 风速
  if(sensors_tempdata->sensor[0].fengsu_min1_counter>45)//1min
  {
    sensors_tempdata->sensor[0].fengsu_1min_avg.data = \
      (float)(sensors_tempdata->sensor[0].avg1minsum.data/(float)sensors_tempdata->sensor[0].fengsu_min1_counter);
    Quality_Min_Ctrl(sensors_tempdata,0);
  }
  else
  {
    sensors_tempdata->sensor[0].fengsu_1min_avg.data = INVALID_DATA;
    sensors_tempdata->sensor[0].fengsu_1min_avg.qc = QC_MISSED;
  }
  //----------------------------2min  风速
  sensors_tempdata->sensor[0].fengsu_min2_counter+=sensors_tempdata->sensor[0].fengsu_min1_counter;
  sensors_tempdata->sensor[0].avg2minsum.data+=sensors_tempdata->sensor[0].avg1minsum.data;
  if(sensors_tempdata->sensor[0].fengsu_min2_counter>90)//2min   120*3/4=90
  {
    sensors_tempdata->sensor[0].fengsu_2min_avg.data = \
      (float)(sensors_tempdata->sensor[0].avg2minsum.data/(float)sensors_tempdata->sensor[0].fengsu_min2_counter);
    Quality_Min_Ctrl(sensors_tempdata,1);//2min  质控
  }
  else
  {
    sensors_tempdata->sensor[0].fengsu_2min_avg.data = INVALID_DATA;
    sensors_tempdata->sensor[0].fengsu_2min_avg.qc = QC_MISSED;
  }
  /*【jeff.liu】*/
  //  if(m_tempdata.debugon == 1)
  //  {
  //    sprintf(temp_wjj,"MIN2_COUNT_WS:%d\r\n",
  //            sensors_tempdata->sensor[0].fengsu_min2_counter); 	// 【待修改】暂时显示频率和采码值
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //  }
  
  //1min正确累计值赋给2min
  sensors_tempdata->sensor[0].fengsu_min2_counter = sensors_tempdata->sensor[0].fengsu_min1_counter;
  //1min正确累计值赋给2min
  sensors_tempdata->sensor[0].avg2minsum.data = sensors_tempdata->sensor[0].avg1minsum.data;
  
  //---------------------------------10min   风速--------------     
  add = 0;
  counter = 0;
  //累加前9分钟数据
  for(i=1;i<10;i++)
  {
    if(sensors_tempdata->sensor[0].fengsumindatalast9min[i].qc == QC_OK)
    {
      add += sensors_tempdata->sensor[0].fengsumindatalast9min[i].data;		
      counter++;
    }
  }
  //累加当前分钟数据
  if(sensors_tempdata->sensor[0].fengsu_1min_avg.qc== QC_OK) 
  {
    add += sensors_tempdata->sensor[0].fengsu_1min_avg.data;
    counter++;
  }
  
  /* 滑动 保存前9个采样值 */
  for(i=0;i<9;i++)
  {
    sensors_tempdata->sensor[0].fengsumindatalast9min[i].data =\
      sensors_tempdata->sensor[0].fengsumindatalast9min[i+1].data;
    sensors_tempdata->sensor[0].fengsumindatalast9min[i].qc = \
      sensors_tempdata->sensor[0].fengsumindatalast9min[i+1].qc;
  }
  sensors_tempdata->sensor[0].fengsumindatalast9min[9].data = \
    sensors_tempdata->sensor[0].fengsu_1min_avg.data;
  sensors_tempdata->sensor[0].fengsumindatalast9min[9].qc = \
    sensors_tempdata->sensor[0].fengsu_1min_avg.qc;
  
  //计算当前的平均值及质控
  if(counter < 8)	//缺测 	  10*3/4
  {
    sensors_tempdata->sensor[0].fengsu_10min_avg.data = INVALID_DATA ;
    sensors_tempdata->sensor[0].fengsu_10min_avg.qc = QC_MISSED;			
  }
  else				          // 正确									
  {
    sensors_tempdata->sensor[0].fengsu_10min_avg.data = (float)(add/(float)counter);
    Quality_Min_Ctrl(sensors_tempdata,2);//10 min ws
  }
  //mindata_counter:1min的平均值计数
  sensors_tempdata->sensor[0].fengsu_min1_counter=0;
  //mindata_add:1min的 平均值累加
  sensors_tempdata->sensor[0].avg1minsum.data=0;
  


  //----------------------------瞬时  风向
  Quality_Min_Ctrl(sensors_tempdata,6);
  
  //----------------------------1min 风向
  if(sensors_tempdata->sensor[0].fengxiang_min1_counter>45)//1min
  {    //x平均值
    sensors_tempdata->sensor[0].avg1m_x.data = sensors_tempdata->sensor[0].avg1m_x_sum.data / sensors_tempdata->sensor[0].fengxiang_min1_counter;
    sensors_tempdata->sensor[0].avg1m_x.qc = QC_OK;
    //y平均值
    sensors_tempdata->sensor[0].avg1m_y.data = sensors_tempdata->sensor[0].avg1m_y_sum.data / sensors_tempdata->sensor[0].fengxiang_min1_counter;
    sensors_tempdata->sensor[0].avg1m_y.qc = QC_OK;
    //-----------------计算1min的平均分向
    WindDirectCal(&sensors_tempdata->sensor[0].avg1m_x.data,\
      &sensors_tempdata->sensor[0].avg1m_y.data,\
        &sensors_tempdata->sensor[0].fengxiang_1min_avg.data );  
    Quality_Min_Ctrl(sensors_tempdata,3);
  }
  else
  {
    sensors_tempdata->sensor[0].fengxiang_1min_avg.data = INVALID_DATA;
    sensors_tempdata->sensor[0].fengxiang_1min_avg.qc = QC_MISSED;
    
    sensors_tempdata->sensor[0].avg1m_x.data = INVALID_DATA;
    sensors_tempdata->sensor[0].avg1m_y.data = INVALID_DATA;
    
    sensors_tempdata->sensor[0].avg1m_x.qc = QC_MISSED;
    sensors_tempdata->sensor[0].avg1m_y.qc = QC_MISSED;
  }
  
  //  if(m_tempdata.debugon == 3)
  //  {
  //    sprintf(temp_wjj,"avg:1_x:%f,1_y:%f,wd_1min:%f\r\n",
  //            sensors_tempdata->sensor[0].avg1m_x.data,
  //            sensors_tempdata->sensor[0].avg1m_y.data,
  //            sensors_tempdata->sensor[0].fengxiang_1min_avg.data
  //              ); 	// 【待修改】暂时显示频率和采码值
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //  }
  
  //-------------//保存此分钟的风向  作为极大风速对应的极大风向----------- 
  if(Flag_max_feng == 1)
  {
    Flag_max_feng = 0;
    sensors_tempdata->sensor[0].fengxiang_max.data = sensors_tempdata->sensor[0].fengxiang_1min_avg.data;
    sensors_tempdata->sensor[0].fengxiang_max.qc = sensors_tempdata->sensor[0].fengxiang_1min_avg.qc;
  } 
  
  //----------------------------2min  风向
  
  sensors_tempdata->sensor[0].fengxiang_min2_counter+=sensors_tempdata->sensor[0].fengxiang_min1_counter;
  sensors_tempdata->sensor[0].avg2m_x_sum.data+=sensors_tempdata->sensor[0].avg1m_x_sum.data;
  sensors_tempdata->sensor[0].avg2m_y_sum.data+=sensors_tempdata->sensor[0].avg1m_y_sum.data;
  
  //  if(m_tempdata.debugon ==3)
  //  {
  //    sprintf(temp_wjj,"sum:1_x,%f,1_y,%f,2_x,%f,2_y,%f\r\n",
  //            sensors_tempdata->sensor[0].avg1m_x_sum.data,
  //            sensors_tempdata->sensor[0].avg1m_y_sum.data,
  //            sensors_tempdata->sensor[0].avg2m_x_sum.data,
  //            sensors_tempdata->sensor[0].avg2m_y_sum.data
  //              ); 	// 【待修改】暂时显示频率和采码值
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //  } 
  if(sensors_tempdata->sensor[0].fengxiang_min2_counter>90)//2min   120*3/4=90
  { //x平均值
    sensors_tempdata->sensor[0].avg2m_x.data = sensors_tempdata->sensor[0].avg2m_x_sum.data / sensors_tempdata->sensor[0].fengxiang_min2_counter;
    //y平均值
    sensors_tempdata->sensor[0].avg2m_y.data = sensors_tempdata->sensor[0].avg2m_y_sum.data / sensors_tempdata->sensor[0].fengxiang_min2_counter;
    
    //-----------------计算2min的平均分向
    WindDirectCal(&sensors_tempdata->sensor[0].avg2m_x.data,\
      &sensors_tempdata->sensor[0].avg2m_y.data,\
        &sensors_tempdata->sensor[0].fengxiang_2min_avg.data );  
    
    Quality_Min_Ctrl(sensors_tempdata,4);
  }
  else
  {
    sensors_tempdata->sensor[0].fengxiang_2min_avg.data = INVALID_DATA;
    sensors_tempdata->sensor[0].fengxiang_2min_avg.qc = QC_MISSED;
    
    sensors_tempdata->sensor[0].avg2m_x.data = INVALID_DATA;
    sensors_tempdata->sensor[0].avg2m_y.data = INVALID_DATA;
    
    sensors_tempdata->sensor[0].avg2m_x.qc = QC_MISSED;
    sensors_tempdata->sensor[0].avg2m_y.qc = QC_MISSED;
    
  } 
  //  if(m_tempdata.debugon == 3)
  //  {
  //    sprintf(temp_wjj,"MIN2_COUNT_WD:%d\r\n",
  //            sensors_tempdata->sensor[0].fengxiang_min2_counter); 	// 【待修改】暂时显示频率和采码值
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //    
  //    sprintf(temp_wjj,"avg:2_x:%f,2_y:%f,wd_2min:%f\r\n",
  //            sensors_tempdata->sensor[0].avg2m_x.data,
  //            sensors_tempdata->sensor[0].avg2m_y.data,
  //            sensors_tempdata->sensor[0].fengxiang_2min_avg.data
  //              ); 	// 【待修改】暂时显示频率和采码值
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //    
  //  } 
  //1min正确累计值赋给2min
  sensors_tempdata->sensor[0].fengxiang_min2_counter = sensors_tempdata->sensor[0].fengxiang_min1_counter;
  //1min正确累计值赋给2min
  sensors_tempdata->sensor[0].avg2m_x_sum.data = sensors_tempdata->sensor[0].avg1m_x_sum.data;
  sensors_tempdata->sensor[0].avg2m_y_sum.data = sensors_tempdata->sensor[0].avg1m_y_sum.data;
  
  //---------------------------------10min   风向--------------     
  float add_x = 0,add_y = 0;
  unsigned char counter_x = 0,counter_y = 0;
  float avg10_x = 0,avg10_y = 0;
  
  //累加前9分钟数据
  //  qcdata_t fengxiangmindatalast9min_x[10];	      //过去10分钟的x分量平均值数据 
  //  qcdata_t fengxiangmindatalast9min_y[10];	      //过去10分钟的y分量平均值数据 
  
  for(i=1;i<10;i++)//????????????????????????????????????
  {
    if(sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[i].qc == QC_OK)
    {
      add_x += sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[i].data;		
      counter_x++;
    }
    if(sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[i].qc == QC_OK)
    {
      add_y += sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[i].data;		
      counter_y++;
    }
  }
  //累加当前分钟数据
  if(sensors_tempdata->sensor[0].avg1m_x.qc== QC_OK) 
  {
    add_x += sensors_tempdata->sensor[0].avg1m_x.data;
    counter_x++;
  }
  if(sensors_tempdata->sensor[0].avg1m_y.qc== QC_OK) 
  {
    add_y += sensors_tempdata->sensor[0].avg1m_y.data;
    counter_y++;
  }

  /* 滑动 保存前9个采样值 */
  for(i=0;i<9;i++)
  {
    sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[i].data =\
      sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[i+1].data;
    sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[i].qc = \
      sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[i+1].qc;
    
    sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[i].data =\
      sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[i+1].data;
    sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[i].qc = \
      sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[i+1].qc;
    
  }
  sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[9].data = \
    sensors_tempdata->sensor[0].avg1m_x.data;
  sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[9].qc = \
    sensors_tempdata->sensor[0].avg1m_x.qc;
  
  sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[9].data = \
    sensors_tempdata->sensor[0].avg1m_y.data;
  sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[9].qc = \
    sensors_tempdata->sensor[0].avg1m_y.qc;
  
  //  if(m_tempdata.debugon == 3)
  //  {
  //    uartSendStr(0,"wd_10m:",7);
  //    
  //    for(unsigned char i=0;i<10;i++)
  //    {
  //      sprintf(temp_wjj,"x,%f,y,%f,",
  //              sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[i].data, 	// 【待修改】暂时显示频率和采码值
  //              sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[i].data); 	// 【待修改】暂时显示频率和采码值
  //      uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //    }
  //    uartSendStr(0,"\r\n",2);
  //  }
  
  //计算当前的平均值及质控
  if((counter_x>=8)&&(counter_y>=8))				          // 正确									
  {
    avg10_x = add_x/counter_x;
    avg10_y = add_y/counter_y;
    
    
    //-----------------计算10min的平均分向
    WindDirectCal(&avg10_x,&avg10_y, &sensors_tempdata->sensor[0].fengxiang_10min_avg.data );  
    Quality_Min_Ctrl(sensors_tempdata,5);
  }
  else	// 存疑	  10*3/4
  {
    sensors_tempdata->sensor[0].fengxiang_10min_avg.data = INVALID_DATA ;
    sensors_tempdata->sensor[0].fengxiang_10min_avg.qc = QC_MISSED;			
  } 
  //  if(m_tempdata.debugon == 3)
  //  {
  //    sprintf(temp_wjj,"add10:--x:%f,y:%f\r\n",
  //            add_x,add_y); 	// 【待修改】暂时显示频率和采码值
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //    sprintf(temp_wjj,"counter10:x--%d,y--%d\r\n",
  //            counter_x,counter_y); 	// 【待修改】暂时显示频率和采码值
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //    sprintf(temp_wjj,"avg10:x--%f,y--%f\r\n",
  //            avg10_x, avg10_y); 	// 【待修改】暂时显示频率和采码值
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //    
  //  } 
  
  //mindata_counter:1min的平均值计数
  sensors_tempdata->sensor[0].fengxiang_min1_counter=0;
  //mindata_add:1min的 平均值累加
  sensors_tempdata->sensor[0].avg1m_x_sum.data=0;
  sensors_tempdata->sensor[0].avg1m_y_sum.data=0;
  
  
  //  data_valid_num  = 0;  
  //  memset(data_sample_buf,0,sizeof(data_sample_buf));//采样数据缓冲区清0
  //  if(m_tempdata.debugon == 3)
  //  {
  //    sprintf(temp_wjj,"MIN_WD:%f,%d,%f,%d,%f,%d\r\n",
  //            sensors_tempdata->sensor[0].fengxiang_1min_avg.data,
  //            sensors_tempdata->sensor[0].fengxiang_1min_avg.qc,
  //            sensors_tempdata->sensor[0].fengxiang_2min_avg.data,
  //            sensors_tempdata->sensor[0].fengxiang_2min_avg.qc,
  //            sensors_tempdata->sensor[0].fengxiang_10min_avg.data,
  //            sensors_tempdata->sensor[0].fengxiang_10min_avg.qc);
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //  } 
  return 1;
}

/*====================================================================================
*函数功能：秒级质量控制
*入口参数：采样数据结构体
*出口参数：质量控制码
*调用者  ：秒级采样函数
*函数引用：Quality_Sec_Ctrl(&sensors_data)
====================================================================================*/
unsigned char Quality_Sec_Ctrl(sensors_data_t *sensors_tempdata,unsigned char ch)
{ 
  float add = 0;
  unsigned char counter = 0;
  unsigned char i = 0;
  float result_sensor;

  switch(ch)
  {
  case 0://250ms风速检测
    result_sensor = sensors_tempdata->sensor[0].fengsu_MomentaryValue.data;
    
    //上下限判断
    if((result_sensor < bcm_info.sensor.qs[WIND_VELOCITY_250_S].min) || (result_sensor > bcm_info.sensor.qs[WIND_VELOCITY_250_S].max))
    {
    //  sensors_tempdata->sensor[0].fengsu_MomentaryValue.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengsu_MomentaryValue.qc=QC_ERROR;
    }
    else
    {    
      sensors_tempdata->sensor[0].fengsu_MomentaryValue.qc=\
        iQCPS_Code_ws250(0, sensors_tempdata->sensor[0].fengsu_MomentaryValue.data, &bcm_info.sensor.qs[WIND_VELOCITY_250_S]);
    }
    //---------------------------------3s   avg--------------
    add = 0;counter = 0;
    for(i=1;i<12;i++)
    {
      if(sensors_tempdata->sensor[0].fengsusecdatalast3s[i].qc == QC_OK)
      {
        add += sensors_tempdata->sensor[0].fengsusecdatalast3s[i].data;		
        counter++;
      }
    }
    //累加当前值
    if(sensors_tempdata->sensor[0].fengsu_MomentaryValue.qc== QC_OK) 
    {
      add += sensors_tempdata->sensor[0].fengsu_MomentaryValue.data;
      counter++;
    }
    //计算当前3s的平均值及质控
    float temp_fengsu_3s_data = 0;
    if(counter < 10)	// 存疑	  12*3/4
    {
      sensors_tempdata->sensor[0].fengsu_3s.data = INVALID_DATA ;
      sensors_tempdata->sensor[0].fengsu_3s.qc = QC_MISSED;			
    }
    else				          // 正确									
    {
      temp_fengsu_3s_data = (float)(add/(float)counter); 
      sensors_tempdata->sensor[0].fengsu_3s.data = temp_fengsu_3s_data;
      sensors_tempdata->sensor[0].fengsu_3s.qc = \
          iQCPM_Code(0,sensors_tempdata->sensor[0].fengsu_3s.data,&bcm_info.sensor.qm[WIND_VELOCITY_3_S]);
    }
    
    //        //保存采样数据
    //    if(sensors_data.sensor[0].fengsu_3s.data == INVALID_DATA)
    //      data_sample_buf[sample_num_ws] = (int)(temp_fengsu_3s_data);
    //    else
    //      data_sample_buf[sample_num_ws] = (int)(temp_fengsu_3s_data*10);
    //  
    
    /* 滑动 保存12个采样值 */
    for(i=0;i<11;i++)
    {
      sensors_tempdata->sensor[0].fengsusecdatalast3s[i].data =\
        sensors_tempdata->sensor[0].fengsusecdatalast3s[i+1].data;
      sensors_tempdata->sensor[0].fengsusecdatalast3s[i].qc = \
        sensors_tempdata->sensor[0].fengsusecdatalast3s[i+1].qc;
    }
    sensors_tempdata->sensor[0].fengsusecdatalast3s[11].data = \
      sensors_tempdata->sensor[0].fengsu_MomentaryValue.data;
    sensors_tempdata->sensor[0].fengsusecdatalast3s[11].qc = \
      sensors_tempdata->sensor[0].fengsu_MomentaryValue.qc;
    
    
#if 0
    /*【jeff.liu】*/
    //    if(m_tempdata.debugon == 1)
    //    {
    //      
    //      uartSendStr(0,"ws_sample:",10);
    //      
    //      for(unsigned char i=0;i<12;i++)
    //      {
    //        len_1 = sprintf(temp_wjj,"%.2f  ",
    //                        sensors_tempdata->sensor[0].fengsusecdatalast3s[i].data); 	// 【待修改】暂时显示频率和采码值
    //        uartSendStr(0,(unsigned char *)temp_wjj,len_1);
    //      }
    //      uartSendStr(0,"\r\n",2);
    //    }
#endif    
    //---------------------------------分钟极大分速--------------
    if(sensors_tempdata->sensor[0].fengsu_3s.qc == QC_OK)
    {
      if(sensors_tempdata->sensor[0].fengsu_max.data<=sensors_tempdata->sensor[0].fengsu_3s.data)
      {
        sensors_tempdata->sensor[0].fengsu_max.data = sensors_tempdata->sensor[0].fengsu_3s.data;
        sensors_tempdata->sensor[0].fengsu_max.qc = sensors_tempdata->sensor[0].fengsu_3s.qc;
        Flag_max_feng = 1;//风向查询此标志位
      }
    }
    
    if( m_tempdata.event.Flag_1s==true)//确认是整秒的值      需要从定时器传上来
    {//在整秒的时候累加1min  或2min的值  
      m_tempdata.event.Flag_1s = false;
      if(sensors_tempdata->sensor[0].fengsu_3s.qc == QC_OK)
      {//保存往前推1min的整秒3s平均风速值累加
        sensors_tempdata->sensor[0].fengsu_min1_counter++;
        //count_1min++;
        sensors_tempdata->sensor[0].avg1minsum.data += sensors_tempdata->sensor[0].fengsu_3s.data;
      }        
#if 0
      /*【jeff.liu】*/
      //      if(m_tempdata.debugon == 1)
      //      {
      //        uartSendStr(0,"\r\n00ms_fengsu_3s",17);
      //      }
#endif     
    }
    
#if 0
    if(m_tempdata.SecDataOut == 1)
    {
      

      counter_1++;
      //      if((counter_1 & 0x07) == 0)
      //      {
      //        len_1 = sprintf(temp_wjj,"ws_3s:%.1fm/s,%d\r\n",
      //                        sensors_data.sensor[0].fengsu_3s.data,\
        //                          sensors_data.sensor[0].fengsu_3s.qc);        
        //        //      HalUARTWrite(0,(unsigned char *)temp_wjj,len_1);
        //        uartSendStr(0,(unsigned char *)temp_wjj,len_1);
        //      }
    }
#endif    
    return 1;

  case 1://1s风向检测
    //    uint8 temp_wjj[32];    
    //    int len_1;
    result_sensor = sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data;
    //上下限判断
    if((result_sensor<bcm_info.sensor.qs[WIND_DIRECTION_1_S].min)\
      ||(result_sensor>bcm_info.sensor.qs[WIND_DIRECTION_1_S].max))
    {
      //sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengxiang_MomentaryValue.qc=QC_ERROR;
    }
    else
    {    //wjj  20150623
      sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data=result_sensor;
      sensors_tempdata->sensor[0].fengxiang_MomentaryValue.qc=\
        iQCPS_Code_wd(0,sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data,&bcm_info.sensor.qs[WIND_DIRECTION_1_S]);
    }
#if 0
    //    if(m_tempdata.debugon == 1)
    //    {
    //      len_1 = sprintf(temp_wjj,"wd_1s:%.1f,%d,wd_1min:%.1f,%d\r\n",
    //                      sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data,\
      //                        sensors_tempdata->sensor[0].fengxiang_MomentaryValue.qc,\
        //                          sensors_tempdata->sensor[0].fengxiang_1min_avg.data,\
          //                            sensors_tempdata->sensor[0].fengxiang_1min_avg.qc);        
          //      uartSendStr(0,(unsigned char *)temp_wjj,len_1);
          //    }
#endif
          //-------------------进行累加     
          
          if(sensors_tempdata->sensor[0].fengxiang_MomentaryValue.qc==QC_OK)
          {
            arch=2*PI*sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data/360;//角度换算成弧度
            wdx=wdx+sin(arch);
            wdy=wdy+cos(arch);
            
            //保存往前推2min的整秒1s平均风速值累加   mindatalast4min[0]
            sensors_tempdata->sensor[0].fengxiang_min1_counter++;  
            sensors_tempdata->sensor[0].avg1m_x_sum.data += wdx;      
            sensors_tempdata->sensor[0].avg1m_y_sum.data +=wdy;
            sensors_tempdata->sensor[0].avg1m_x_sum.qc = QC_OK;
            sensors_tempdata->sensor[0].avg1m_y_sum.qc = QC_OK;
            
            //      sprintf(temp_wjj,"sum:%d,%f,%f,%f,%f\r\n",
            //              sensors_tempdata->sensor[0].fengxiang_min1_counter,
            //              wdx,wdy,
            //              sensors_tempdata->sensor[0].avg1m_x_sum.data,
            //              sensors_tempdata->sensor[0].avg1m_y_sum.data
            //                ); 	// 【待修改】暂时显示频率和采码值
            //      uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
            
            wdx = 0;wdy = 0;
          }

    return 1;
    
  default:
    return 0;
  }
  
}
/*
*函数功能：分钟级质量控制
*入口参数：采样数据结构体
*出口参数：质量控制码
*调用者  ：分钟采样函数
*函数引用：Quality_Sec_Ctrl(&sensors_data,0)
*/
unsigned char Quality_Min_Ctrl(sensors_data_t *sensors_tempdata,unsigned char mins)
{
  float result_sensor, max = 0, min = 0;
  short changerate = 0;
  unsigned char i = 0, j = 0, count = 0;

  switch(mins)
  {
  case 0://一分钟风速的质量控制
    result_sensor = sensors_tempdata->sensor[0].fengsu_1min_avg.data;
    //上下限判断
    if((result_sensor<bcm_info.sensor.qm[WIND_VELOCITY_1_M].min)||\
      (result_sensor>bcm_info.sensor.qm[WIND_VELOCITY_1_M].max))
    {
      //sensors_tempdata->sensor[0].fengsu_1min_avg.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengsu_1min_avg.qc=QC_ERROR;
    }
    else
    {
        sensors_tempdata->sensor[0].fengsu_1min_avg.qc=\
            iQCPM_Code(WIND_VELOCITY_1_M,result_sensor,&bcm_info.sensor.qm[WIND_VELOCITY_1_M]);
    }
    break;
    
  case 1://二分钟风速的质量控制
    result_sensor = sensors_tempdata->sensor[0].fengsu_2min_avg.data;
    //上下限判断
    if((result_sensor<bcm_info.sensor.qm[WIND_VELOCITY_2_M].min)||\
      (result_sensor>bcm_info.sensor.qm[WIND_VELOCITY_2_M].max))
    {
     // sensors_tempdata->sensor[0].fengsu_2min_avg.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengsu_2min_avg.qc=QC_ERROR;
    }
    else
    {
        //      sensors_tempdata->sensor[0].fengsu_2min_avg.data=result_sensor;//此句可以不要
        sensors_tempdata->sensor[0].fengsu_2min_avg.qc=\
            iQCPM_Code(WIND_VELOCITY_2_M,result_sensor,&bcm_info.sensor.qm[WIND_VELOCITY_2_M]);
    }
    break;
    
  case 2://十分钟风速的质量控制
    result_sensor = sensors_tempdata->sensor[0].fengsu_10min_avg.data;
    //上下限判断
    if((result_sensor<bcm_info.sensor.qm[WIND_VELOCITY_10_M].min)||\
      (result_sensor>bcm_info.sensor.qm[WIND_VELOCITY_10_M].max))
    {
      //sensors_tempdata->sensor[0].fengsu_10min_avg.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengsu_10min_avg.qc=QC_ERROR;
    }
    else
    {
        //      sensors_tempdata->sensor[0].fengsu_10min_avg.data=result_sensor;//此句可以不要
        sensors_tempdata->sensor[0].fengsu_10min_avg.qc=\
            iQCPM_Code(WIND_VELOCITY_10_M,result_sensor,&bcm_info.sensor.qm[WIND_VELOCITY_10_M]);
    }
    break;
    
  case 3://一分钟风向的质量控制
    result_sensor = sensors_tempdata->sensor[0].fengxiang_1min_avg.data;
    int temp_result_sensor = 0;
    temp_result_sensor = (int)result_sensor;
    //上下限判断
    if((temp_result_sensor<bcm_info.sensor.qm[WIND_DIRECTION_1_M].min)||\
      (temp_result_sensor>bcm_info.sensor.qm[WIND_DIRECTION_1_M].max))
    {
      //sensors_tempdata->sensor[0].fengxiang_1min_avg.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengxiang_1min_avg.qc=QC_ERROR;
    }
    else
    {
        //      sensors_tempdata->sensor[0].fengsu_1min_avg.data=result_sensor;//此句可以不要
        sensors_tempdata->sensor[0].fengxiang_1min_avg.qc=\
            iQCPM_Code(WIND_DIRECTION_1_M,result_sensor,&bcm_info.sensor.qm[WIND_DIRECTION_1_M]);
    }

    //最小应变化检测
    if(sensors_tempdata->sensor[0].fengxiang_1min_avg.qc != QC_MISSED)
    {
      sensors_tempdata->sensor[0].changerate[0].hour_data[sensors_tempdata->sensor[0].changerate[0].hourdata_counter] = result_sensor;
    }
    else
    {
        sensors_tempdata->sensor[0].changerate[0].hour_data[sensors_tempdata->sensor[0].changerate[0].hourdata_counter] = INVALID_DATA;
    }

    sensors_tempdata->sensor[0].changerate[0].hourdata_counter++;

    if((sensors_tempdata->sensor[0].changerate[0].hourdata_counter >= 60) || (sensors_tempdata->sensor[0].changerate[0].hour_data_flag == 1))
    {
        sensors_tempdata->sensor[0].changerate[0].hour_data_flag = 1;

        if((bcm_info.sensor.qm[WIND_DIRECTION_1_M].changerate != INVALID_DATA) && \
              (sensors_tempdata->sensor[0].fengxiang_1min_avg.qc == QC_OK) && (sensors_tempdata->sensor[0].fengsu_10min_avg.data >= 0.1))
        {
            for(i = 0; i < 60; i++)
            {
                if(sensors_tempdata->sensor[0].changerate[0].hour_data[i] != INVALID_DATA)
                {
                    if(count == 0)
                    {
                        min = sensors_tempdata->sensor[0].changerate[0].hour_data[i];
                    }

                    if(sensors_tempdata->sensor[0].changerate[0].hour_data[i] < min)
                    {
                        min = sensors_tempdata->sensor[0].changerate[0].hour_data[i];
                    }

                    count++;
                }
            }

            if(count >= 2)
            {
                for(i = 0;i < 60;i++)
                {
                    if(sensors_tempdata->sensor[0].changerate[0].hour_data[i] != INVALID_DATA)
                    {
                        changerate = (short)fabs(sensors_tempdata->sensor[0].changerate[0].hour_data[i] - min);
                        if(changerate > 180)
                            changerate = 360 - changerate;

                        if(changerate >= bcm_info.sensor.qm[WIND_DIRECTION_1_M].changerate)
                        {
                            break;
                        }
                    }
                }

                if(i >= 60)
                {
                    sensors_tempdata->sensor[0].fengxiang_1min_avg.qc = QC_DOUBT;
                }
            }
      }
	  
	  if(sensors_tempdata->sensor[0].changerate[0].hourdata_counter >= 60)
      	sensors_tempdata->sensor[0].changerate[0].hourdata_counter = 0;
    }
    break;
    
  case 4://二分钟风向的质量控制
    result_sensor = sensors_tempdata->sensor[0].fengxiang_2min_avg.data;
    //上下限判断
    if((result_sensor<bcm_info.sensor.qm[WIND_DIRECTION_2_M].min)||\
      (result_sensor>bcm_info.sensor.qm[WIND_DIRECTION_2_M].max))
    {
      //sensors_tempdata->sensor[0].fengxiang_2min_avg.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengxiang_2min_avg.qc=QC_ERROR;
    }
    else
    {
        //      sensors_tempdata->sensor[0].fengsu_2min_avg.data=result_sensor;//此句可以不要
        sensors_tempdata->sensor[0].fengxiang_2min_avg.qc=\
            iQCPM_Code(WIND_DIRECTION_2_M,result_sensor,&bcm_info.sensor.qm[WIND_DIRECTION_2_M]);
    }

    //最小应变化检测
    if(sensors_tempdata->sensor[0].fengxiang_2min_avg.qc != QC_MISSED)
    {
      sensors_tempdata->sensor[0].changerate[1].hour_data[sensors_tempdata->sensor[0].changerate[1].hourdata_counter] = result_sensor;
    }
    else
    {
        sensors_tempdata->sensor[0].changerate[1].hour_data[sensors_tempdata->sensor[0].changerate[1].hourdata_counter] = INVALID_DATA;
    }

    sensors_tempdata->sensor[0].changerate[1].hourdata_counter++;

    if((sensors_tempdata->sensor[0].changerate[1].hourdata_counter >= 60) || (sensors_tempdata->sensor[0].changerate[1].hour_data_flag == 1))
    {
        sensors_tempdata->sensor[0].changerate[1].hour_data_flag = 1;

        if((bcm_info.sensor.qm[WIND_DIRECTION_2_M].changerate != INVALID_DATA) && \
                (sensors_tempdata->sensor[0].fengxiang_2min_avg.qc == QC_OK) && (sensors_tempdata->sensor[0].fengsu_10min_avg.data >= 0.1))
        {
            for(i = 0; i < 60; i++)
            {
                if(sensors_tempdata->sensor[0].changerate[1].hour_data[i] != INVALID_DATA)
                {
                    if(count == 0)
                    {
                        min = sensors_tempdata->sensor[0].changerate[1].hour_data[i];
                    }

                    if(sensors_tempdata->sensor[0].changerate[1].hour_data[i] < min)
                    {
                        min = sensors_tempdata->sensor[0].changerate[1].hour_data[i];
                    }

                    count++;
                }
            }

            if(count >= 2)
            {
                for(i = 0;i < 60;i++)
                {
                    if(sensors_tempdata->sensor[0].changerate[1].hour_data[i] != INVALID_DATA)
                    {
                        changerate = (short)fabs(sensors_tempdata->sensor[0].changerate[1].hour_data[i] - min);
                        if(changerate > 180)
                            changerate = 360 - changerate;

                        if(changerate >= bcm_info.sensor.qm[WIND_DIRECTION_2_M].changerate)
                        {
                            break;
                        }
                    }
                }

                if(i >= 60)
                {
                    sensors_tempdata->sensor[0].fengxiang_2min_avg.qc = QC_DOUBT;
                }
            }
        }
	  
        if(sensors_tempdata->sensor[0].changerate[1].hourdata_counter >= 60)
            sensors_tempdata->sensor[0].changerate[1].hourdata_counter = 0;
    }
    break;
    
  case 5://十分钟风向的质量控制
    result_sensor = sensors_tempdata->sensor[0].fengxiang_10min_avg.data;
    //上下限判断
    if((result_sensor<bcm_info.sensor.qm[WIND_DIRECTION_10_M].min)||\
      (result_sensor>bcm_info.sensor.qm[WIND_DIRECTION_10_M].max))
    {
    //  sensors_tempdata->sensor[0].fengxiang_10min_avg.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengxiang_10min_avg.qc=QC_ERROR;
    }
    else
    {
        //      sensors_tempdata->sensor[0].fengsu_10min_avg.data=result_sensor;//此句可以不要
        sensors_tempdata->sensor[0].fengxiang_10min_avg.qc=\
            iQCPM_Code(WIND_DIRECTION_10_M,result_sensor,&bcm_info.sensor.qm[WIND_DIRECTION_10_M]);
    }

    //最小应变化检测
    if(sensors_tempdata->sensor[0].fengxiang_10min_avg.qc != QC_MISSED)
    {
      sensors_tempdata->sensor[0].changerate[2].hour_data[sensors_tempdata->sensor[0].changerate[2].hourdata_counter] = result_sensor;
    }
    else
    {
        sensors_tempdata->sensor[0].changerate[2].hour_data[sensors_tempdata->sensor[0].changerate[2].hourdata_counter] = INVALID_DATA;
    }

    sensors_tempdata->sensor[0].changerate[2].hourdata_counter++;

    if((sensors_tempdata->sensor[0].changerate[2].hourdata_counter >= 60) || (sensors_tempdata->sensor[0].changerate[2].hour_data_flag == 1))
    {
        sensors_tempdata->sensor[0].changerate[2].hour_data_flag = 1;

        if((bcm_info.sensor.qm[WIND_DIRECTION_10_M].changerate != INVALID_DATA) &&\
                (sensors_tempdata->sensor[0].fengxiang_10min_avg.qc == QC_OK) && (sensors_tempdata->sensor[0].fengsu_10min_avg.data >= 0.1))
        {
            for(i = 0; i < 60; i++)
            {
                if(sensors_tempdata->sensor[0].changerate[2].hour_data[i] != INVALID_DATA)
                {
                    if(count == 0)
                    {
                        min = sensors_tempdata->sensor[0].changerate[2].hour_data[i];
                    }
                        if(sensors_tempdata->sensor[0].changerate[2].hour_data[i] < min)
                    {
                        min = sensors_tempdata->sensor[0].changerate[2].hour_data[i];
                    }

                    count++;
                }
            }

            if(count >= 2)
            {
                for(i = 0;i < 60;i++)
                {
                    if(sensors_tempdata->sensor[0].changerate[2].hour_data[i] != INVALID_DATA)
                    {
                        changerate = (short)fabs(sensors_tempdata->sensor[0].changerate[2].hour_data[i] - min);
                        if(changerate > 180)
                            changerate = 360 - changerate;

                        if(changerate >= bcm_info.sensor.qm[WIND_DIRECTION_10_M].changerate)
                        {
                            break;
                        }
                    }
                }

                if(i >= 60)
                {
                    sensors_tempdata->sensor[0].fengxiang_10min_avg.qc = QC_DOUBT;
                }
            }
        }
	  
        if(sensors_tempdata->sensor[0].changerate[2].hourdata_counter >= 60)
            sensors_tempdata->sensor[0].changerate[2].hourdata_counter = 0;
    }
    break;
    
  case 6://瞬时风向的质量控制
    result_sensor = sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data;
    //上下限判断
    if((result_sensor<bcm_info.sensor.qm[WIND_DIRECTION_0_M].min)||\
      (result_sensor>bcm_info.sensor.qm[WIND_DIRECTION_0_M].max))
    {
    //  sensors_tempdata->sensor[0].fengxiang_10min_avg.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengxiang_1min.qc=QC_ERROR;
	 // sensors_tempdata->sensor[0].fengxiang_1min.data = INVALID_DATA;
    }
    else
    {
        //      sensors_tempdata->sensor[0].fengsu_10min_avg.data=result_sensor;//此句可以不要
        sensors_tempdata->sensor[0].fengxiang_1min.qc=\
            iQCPM_Code(WIND_DIRECTION_0_M,result_sensor,&bcm_info.sensor.qm[WIND_DIRECTION_0_M]);
		sensors_tempdata->sensor[0].fengxiang_1min.data = result_sensor;
    }

    //最小应变化检测
    if(sensors_tempdata->sensor[0].fengxiang_1min.qc != QC_MISSED)
    {
      sensors_tempdata->sensor[0].changerate[3].hour_data[sensors_tempdata->sensor[0].changerate[3].hourdata_counter] = result_sensor;
    }
    else
    {
        sensors_tempdata->sensor[0].changerate[3].hour_data[sensors_tempdata->sensor[0].changerate[3].hourdata_counter] = INVALID_DATA;
    }

    sensors_tempdata->sensor[0].changerate[3].hourdata_counter++;

    if((sensors_tempdata->sensor[0].changerate[3].hourdata_counter >= 60) || (sensors_tempdata->sensor[0].changerate[3].hour_data_flag == 1))
    {
        sensors_tempdata->sensor[0].changerate[3].hour_data_flag = 1;

        if((bcm_info.sensor.qm[WIND_DIRECTION_0_M].changerate != INVALID_DATA) && \
                (sensors_tempdata->sensor[0].fengxiang_1min.qc == QC_OK) && (sensors_tempdata->sensor[0].fengsu_10min_avg.data >= 0.1))
        {
            for(i = 0; i < 60; i++)
            {
                if(sensors_tempdata->sensor[0].changerate[3].hour_data[i] != INVALID_DATA)
                {
                    if(count == 0)
                    {
                        min = sensors_tempdata->sensor[0].changerate[3].hour_data[i];
                    }

                    if(sensors_tempdata->sensor[0].changerate[3].hour_data[i] < min)
                    {
                        min = sensors_tempdata->sensor[0].changerate[3].hour_data[i];
                    }

                    count++;
                }
            }

            if(count >= 2)
            {
                for(i = 0;i < 60;i++)
                {
                    if(sensors_tempdata->sensor[0].changerate[3].hour_data[i] != INVALID_DATA)
                    {
                        changerate = (short)fabs(sensors_tempdata->sensor[0].changerate[3].hour_data[i] - min);
                        if(changerate > 180)
                            changerate = 360 - changerate;

                        if(changerate >= bcm_info.sensor.qm[WIND_DIRECTION_0_M].changerate)
                        {
                            break;
                        }
                    }
                }

                if(i >= 60)
                {
                    sensors_tempdata->sensor[0].fengxiang_1min.qc = QC_DOUBT;
                }
            }
      }
	  
        if(sensors_tempdata->sensor[0].changerate[3].hourdata_counter >= 60)
      	sensors_tempdata->sensor[0].changerate[3].hourdata_counter = 0;
    }
    break;

  default:
   return 0;
  }

  return 1;
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
    unsigned char j = 0;
  //--------------------------风速------------------------------------
  //250ms风速采样值
  sensors_tempdata->sensor[0].fengsu_MomentaryValue.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengsu_MomentaryValue.qc=QC_MISSED;
  for(j = 0;j<12;j++) //过去3s内12个采样数据
  {      
    sensors_tempdata->sensor[0].fengsusecdatalast3s[j].data=INVALID_DATA;
    sensors_tempdata->sensor[0].fengsusecdatalast3s[j].qc=QC_MISSED;
    
  }
  //瞬时风速(3秒平均)
  sensors_tempdata->sensor[0].fengsu_3s.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengsu_3s.qc=QC_MISSED;
  
  //放2min中最前面一个3s平均值
  sensors_tempdata->sensor[0].fensu_2min_3s.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fensu_2min_3s.qc=QC_MISSED;
  //分钟数据 存放1min中最前面一个3s平均值
  sensors_tempdata->sensor[0].fensu_1min_3s.data=INVALID_DATA;	
  sensors_tempdata->sensor[0].fensu_1min_3s.qc=QC_MISSED;
  
  
  //秒数据累加或1min的平均值累加
  sensors_tempdata->sensor[0].avg1minsum.data=0;
  sensors_tempdata->sensor[0].avg1minsum.qc=QC_MISSED;
  //秒数据个数 //1分钟数据个数
  sensors_tempdata->sensor[0].fengsu_min1_counter=0;
  //分钟数据	累加或2min的平均值累加
  sensors_tempdata->sensor[0].avg2minsum.data=0;	
  sensors_tempdata->sensor[0].avg2minsum.qc=QC_MISSED;
  //分钟数据	个数//2分钟数据个数 
  sensors_tempdata->sensor[0].fengsu_min2_counter=0;	
  
  //一分钟平均风速
  sensors_tempdata->sensor[0].fengsu_1min_avg.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengsu_1min_avg.qc=QC_MISSED;
  
  //二分钟平均风速
  sensors_tempdata->sensor[0].fengsu_2min_avg.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengsu_2min_avg.qc=QC_MISSED;
  
  //十分钟平均风速
  sensors_tempdata->sensor[0].fengsu_10min_avg.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengsu_10min_avg.qc=QC_MISSED;
  
  for(j = 0;j<10;j++) //过去9分钟的分钟数据
  {      
    sensors_tempdata->sensor[0].fengsumindatalast9min[j].data=INVALID_DATA;
    sensors_tempdata->sensor[0].fengsumindatalast9min[j].qc=QC_MISSED;
  }
  
  // //分钟内极大风速
  sensors_tempdata->sensor[0].fengsu_max.data=0;
  sensors_tempdata->sensor[0].fengsu_max.qc=QC_MISSED;
  
  //--------------------------风向------------------------------------
  //极大风向
  sensors_tempdata->sensor[0].fengxiang_max.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengxiang_max.qc=QC_MISSED;
  
  //1s风向采样值
  sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengxiang_MomentaryValue.qc=QC_MISSED;
  
  
  sensors_tempdata->sensor[0].fengxiang_1min_avg.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengxiang_1min_avg.qc=QC_MISSED;
  
  sensors_tempdata->sensor[0].fengxiang_2min_avg.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengxiang_2min_avg.qc=QC_MISSED;
  
  sensors_tempdata->sensor[0].fengxiang_10min_avg.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengxiang_10min_avg.qc=QC_MISSED;
  
  //过去9分钟的分钟数据
  for(j = 0;j<10;j++)
  {      
    sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[j].data=INVALID_DATA;
    sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[j].qc=QC_MISSED;
    
    
    sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[j].data=INVALID_DATA;
    sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[j].qc=QC_MISSED;
  }
  
  //1min的x平均值累加
  sensors_tempdata->sensor[0].avg1m_x_sum.data=0;
  sensors_tempdata->sensor[0].avg1m_x_sum.qc=QC_MISSED;
  //1min的y平均值累加
  sensors_tempdata->sensor[0].avg1m_y_sum.data=0;
  sensors_tempdata->sensor[0].avg1m_y_sum.qc=QC_MISSED;
  
  //2min的x平均值累加
  sensors_tempdata->sensor[0].avg2m_x_sum.data=0;
  sensors_tempdata->sensor[0].avg2m_x_sum.qc=QC_MISSED;
  //2min的y平均值累加
  sensors_tempdata->sensor[0].avg2m_y_sum.data=0;
  sensors_tempdata->sensor[0].avg2m_y_sum.qc=QC_MISSED;
  
  //1min的x平均值
  sensors_tempdata->sensor[0].avg1m_x.data=0;
  sensors_tempdata->sensor[0].avg1m_x.qc=QC_MISSED;
  //1min的y平均值
  sensors_tempdata->sensor[0].avg1m_y.data=0;
  sensors_tempdata->sensor[0].avg1m_y.qc=QC_MISSED;
  
  //2min的x平均值
  sensors_tempdata->sensor[0].avg2m_x.data=0;
  sensors_tempdata->sensor[0].avg2m_x.qc=QC_MISSED;
  //2min的y平均值
  sensors_tempdata->sensor[0].avg2m_y.data=0;
  sensors_tempdata->sensor[0].avg2m_y.qc=QC_MISSED;
  
  //秒数据个数 //1分钟数据个数
  sensors_tempdata->sensor[0].fengxiang_min1_counter=0;
  //分钟数据	个数//2分钟数据个数 
  sensors_tempdata->sensor[0].fengxiang_min2_counter=0;	
  
  
  //分钟内极大风速（瞬时风速）对应风向
  //  sensors_tempdata->sensor[0].fengxiang_max.data=0;
  //  sensors_tempdata->sensor[0].fengxiang_max.qc=QC_MISSED;
  
  for(j = 0; j < 4; j++)
   {
   	sensors_tempdata->sensor[0].changerate[j].hourdata_counter = 0;   //小时数据个数累加
    sensors_tempdata->sensor[0].changerate[j].num = 0;
    sensors_tempdata->sensor[0].changerate[j].hour_data_flag = 0;
	}	

    memset(data_sample_buf,0,sizeof(data_sample_buf));//采样数据缓冲区清0
    memset(data_sample_buf_history,0,sizeof(data_sample_buf_history));  //采样数据缓冲区清0
    memset(sample_qc,0,sizeof(sample_qc));//采样标志位缓冲区清0
    memset(sample_qc_history,0,sizeof(sample_qc_history));

	//初始化 QC数据缓存
	memset(LastValue_s, 0, sizeof(LastValue_s));
	memset(beenRun_s,   0, MAX_SENSOR_NUM);
	memset(LastValue,   0, sizeof(LastValue));
	memset(beenRun,     0, MAX_SENSOR_NUM);
	memset(LastValue_H, 0, sizeof(LastValue_H));
	memset(beenRun_H,   0, MAX_SENSOR_NUM);

	memset(LastValue_ws250, 0, sizeof(LastValue_ws250));
	memset(beenRun_ws250,   0, MAX_SENSOR_NUM);
	memset(LastValue_ws3s,  0, sizeof(LastValue_ws3s));
	memset(beenRun_ws3s,    0, MAX_SENSOR_NUM);
	memset(LastValue_wd,    0, sizeof(LastValue_wd));
	memset(beenRun_wd,      0, MAX_SENSOR_NUM);

}
/*
*函数功能：质量控制初始化
*入口参数：采样数据结构体
*出口参数：无
*调用者  ：sapi.c中的系统初始化函数
*函数引用：Quality_Init(&sensors_data)
*/
void Quality_Init(sensors_data_t *sensors_tempdata)
{//需要从flash中读出来这个值
  //风速
  //sensors_tempdata->qualityCtrl[0].secdata_ctrl_t.Vmin=bcm_info.sensor.qs.sensorxiaxian1;
  //sensors_tempdata->qualityCtrl[0].secdata_ctrl_t.Vmax=bcm_info.sensor.qs.sensorshangxian1;
  //sensors_tempdata->qualityCtrl[0].secdata_ctrl_t.Rmax=bcm_info.sensor.qs.sensormaxsulu1;
  //sensors_tempdata->qualityCtrl[0].mindata_ctrl_t.Vmin=bcm_info.sensor.qs.sensorxiaxian1;
  //sensors_tempdata->qualityCtrl[0].mindata_ctrl_t.Vmax=bcm_info.sensor.qs.sensorshangxian1;
  
  //风向
  //sensors_tempdata->qualityCtrl[1].secdata_ctrl_t.Vmin=bcm_info.sensor.qs.sensorxiaxian2;
  //sensors_tempdata->qualityCtrl[1].secdata_ctrl_t.Vmax=bcm_info.sensor.qs.sensorshangxian2;
  //sensors_tempdata->qualityCtrl[1].secdata_ctrl_t.Rmax=bcm_info.sensor.qs.sensorshangxian2;
  //sensors_tempdata->qualityCtrl[1].mindata_ctrl_t.Vmin=bcm_info.sensor.qs.sensorxiaxian2;
  //sensors_tempdata->qualityCtrl[1].mindata_ctrl_t.Vmax=bcm_info.sensor.qs.sensorshangxian2;
} 

/*
*函数功能：秒数据质量控制函数
*入口参数：sensorNo--几支传感器，value--采样数据值，*qcctrl--qc初始化的质量控制码
*出口参数：运算后的质量控制码
*调用者  ：Quality_Sec_Ctrl函数
*函数引用：iQCPS_Code(0,sensors_tempdata->sensor[0].secdata.data,&sensors_tempdata->qualityCtrl[0]);
*/

unsigned char iQCPS_Code_ws250(int sensorNo,float value,qs_t *qcctrl)
{
  //  static unsigned char flag[MAX_SENSOR_NUM];
  unsigned char iRet=QC_ERROR;
  
  if((value >=qcctrl->min )&&( value <= qcctrl->max))
  {
    iRet = QC_OK;//正确
  }
  else
  {
    iRet = QC_ERROR;	//错误
    return iRet;
  }
  
  if(beenRun_ws250[sensorNo]==0)
  {
    beenRun_ws250[sensorNo]=1;
  }
  else
  {
      if(qcctrl->change != INVALID_DATA)
      {
        if(fabs(value-LastValue_ws250[sensorNo])>qcctrl->change)
          iRet = QC_DOUBT;
      }
  }
  LastValue_ws250[sensorNo]=value;
  return iRet; 
  
}


unsigned char iQCPS_Code_ws3s(int sensorNo,float value,qs_t *qcctrl)
{
  unsigned char iRet=QC_ERROR;
  
  
  if((value >=qcctrl->min )&&( value <= qcctrl->max))
  {
    iRet = QC_OK;//正确
  }
  else
  {
    iRet =QC_ERROR;	//错误
    return iRet;
  }
  
  if(beenRun_ws3s[sensorNo]==0)
  {
    beenRun_ws3s[sensorNo]=1;
  }
  else
  {
    if(qcctrl->change != INVALID_DATA)
    {
      if(fabs(value-LastValue_ws3s[sensorNo])>qcctrl->change)
        iRet =QC_DOUBT;
    }
  }
  LastValue_ws3s[sensorNo]=value;
  return iRet; 
  
}


unsigned char iQCPS_Code_wd(int sensorNo,float value,qs_t *qcctrl)
{
  //  static unsigned char flag[MAX_SENSOR_NUM];
  unsigned char iRet=QC_ERROR;
  
  if((value >=qcctrl->min )&&( value <= qcctrl->max))
  {
    iRet = QC_OK;//正确
  }
  else
  {
    iRet =QC_ERROR;	//错误
    return iRet;
  }
  
  if(beenRun_wd[sensorNo]==0)
  {
    beenRun_wd[sensorNo]=1;
  }
  else
  {
      if(qcctrl->change != INVALID_DATA)
      {
          if(fabs(value-LastValue_wd[sensorNo])>qcctrl->change)
              iRet = QC_DOUBT;
      }
  }
  LastValue_wd[sensorNo]=value;
  return iRet; 
  
}
unsigned char iQCPM_Code(int sensorNo,float value,qm_t *qcctrl)
{
  //  static unsigned char flag[MAX_SENSOR_NUM];
  unsigned char iRet=QC_ERROR;

  if((value >=qcctrl->min )&&( value <= qcctrl->max))
  {
    iRet = QC_OK;//正确
  }
  else
  {
    iRet =QC_ERROR; //错误
    return iRet;
  }

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

    //-AFA--1.5米高瞬时风速(3秒平均)    fengsu_3s-------------3s平均风速数据
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengsu_3s, &FLASH_Read_MinData.sensor[0].fengsu_3s, \
                    &sensors_tempdata->sensor[0].fengsu_3s, &Mindata_temp.sensor[0].fengsu_3s, num_mins, 1);
    
    //-AFB150	1.5米高一分钟平均风速     fengsu_1min_avg-------1min平均风速数据
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengsu_1min_avg, &FLASH_Read_MinData.sensor[0].fengsu_1min_avg, \
                    &sensors_tempdata->sensor[0].fengsu_1min_avg, &Mindata_temp.sensor[0].fengsu_1min_avg, num_mins, 1);
    
    //-AFC150	1.5米高二分钟平均风速   fengsu_2min_avg---fengsu_2min_avg平均风速数据
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengsu_2min_avg, &FLASH_Read_MinData.sensor[0].fengsu_2min_avg, \
                    &sensors_tempdata->sensor[0].fengsu_2min_avg, &Mindata_temp.sensor[0].fengsu_2min_avg, num_mins, 1);
      
    //----AFD150	1.5米高十分钟平均风速   fengsu_10min_avg--------fengsu_10min_avg平均风速数据
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengsu_10min_avg, &FLASH_Read_MinData.sensor[0].fengsu_10min_avg, \
                    &sensors_tempdata->sensor[0].fengsu_10min_avg, &Mindata_temp.sensor[0].fengsu_10min_avg, num_mins, 1);
 
    //-----AFA150a	1.5米高分钟内极大风速-----------fengsu_max平均风速数据
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengsu_max, &FLASH_Read_MinData.sensor[0].fengsu_max, \
                    &sensors_tempdata->sensor[0].fengsu_max, &Mindata_temp.sensor[0].fengsu_max, num_mins, 1);
    
    //---AEA150	1.5米高瞬时风向（1s采样）------fengxiang_MomentaryValue平均风向数据
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengxiang_1min, &FLASH_Read_MinData.sensor[0].fengxiang_1min, \
                    &sensors_tempdata->sensor[0].fengxiang_1min, &Mindata_temp.sensor[0].fengxiang_1min, num_mins, 0);

    //----AEB150	1.5米高一分钟平均风向 -----------fengxiang_1min_avg平均风向数据
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengxiang_1min_avg, &FLASH_Read_MinData.sensor[0].fengxiang_1min_avg, \
                    &sensors_tempdata->sensor[0].fengxiang_1min_avg, &Mindata_temp.sensor[0].fengxiang_1min_avg, num_mins, 0);
      
    //-AEC150	1.5米高二分钟平均风向---------------fengxiang_2min_avg平均风向数据
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengxiang_2min_avg, &FLASH_Read_MinData.sensor[0].fengxiang_2min_avg, \
                    &sensors_tempdata->sensor[0].fengxiang_2min_avg, &Mindata_temp.sensor[0].fengxiang_2min_avg, num_mins, 0);
      
    //-AED150	1.5米高十分钟平均风向------------fengxiang_10min_avg平均风向数据
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengxiang_10min_avg, &FLASH_Read_MinData.sensor[0].fengxiang_10min_avg, \
                    &sensors_tempdata->sensor[0].fengxiang_10min_avg, &Mindata_temp.sensor[0].fengxiang_10min_avg, num_mins, 0);
      
    //-AEF150	1.5米高分钟内极大风速（瞬时风速）对应风向   fengxiang_max
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengxiang_max, &FLASH_Read_MinData.sensor[0].fengxiang_max, \
                    &sensors_tempdata->sensor[0].fengxiang_max, &Mindata_temp.sensor[0].fengxiang_max, num_mins, 0);


    //-------------------风向---------------------------------
    //---------------------------AEA150   fengxiang_MomentaryValue--------------------------
    strcpy(Header_packet+lenlen,"AEA");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    
    Pack_Data(&Mindata_temp.sensor[0].fengxiang_1min, &lenlen, Header_packet, 3);
    
    //---------------------------AEB150---------------------------
    strcpy(Header_packet+lenlen,"AEB");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;

    Pack_Data(&Mindata_temp.sensor[0].fengxiang_1min_avg, &lenlen, Header_packet, 3);

    //---------------------------AEBl150   1分钟风向分钟标准差 -------------------------
    strcpy(Header_packet+lenlen,"AEBl");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;

    if(num_mins == 1)//需要存储分钟数据，来自主动调用
    { //数据保存至flash结构体     
      standardDeviation_1 = StandardDeviation(data_sample_buf_history + 240, (sample_qc_history + 30), 60);
      if(INVALID_DATA != Mindata_temp.sensor[0].fengxiang_1min_avg.data)
          Mindata_temp.Standard_Deviation[0] = standardDeviation_1 * 100;
      else
          Mindata_temp.Standard_Deviation[0] = INVALID_DATA;
      
      FLASH_Store_MinData.Standard_Deviation[0] = Mindata_temp.Standard_Deviation[0];
    }
    else if((num_mins == 0)||(num_mins == 2))//不需要存储的分钟数据，来自历史数据读取调用
    {
        if(INVALID_DATA != Mindata_temp.sensor[0].fengxiang_1min_avg.data)
            Mindata_temp.Standard_Deviation[0] = FLASH_Read_MinData.Standard_Deviation[0];
        else
            Mindata_temp.Standard_Deviation[0] = INVALID_DATA;
    }
    else ;
    
    Pack_Datal(&Mindata_temp.Standard_Deviation[0], &lenlen, Header_packet, 5);

    //---------------------------AEC150--------------------------
    strcpy(Header_packet+lenlen,"AEC");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    //mindata
    Pack_Data(&Mindata_temp.sensor[0].fengxiang_2min_avg, &lenlen, Header_packet, 3);
    
    //---------------------------AED150--------------------------
    strcpy(Header_packet+lenlen,"AED");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    //mindata
    Pack_Data(&Mindata_temp.sensor[0].fengxiang_10min_avg, &lenlen, Header_packet, 3);
    
    //---------------------------AEF150--------------------------
    strcpy(Header_packet+lenlen,"AEF");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    //mindata
    Pack_Data(&Mindata_temp.sensor[0].fengxiang_max, &lenlen, Header_packet, 3);

    //-------------------风速---------------------------------
    //---------------------------AFA150---------------------------
    strcpy(Header_packet+lenlen,"AFA");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    //mindata
    Pack_Data(&Mindata_temp.sensor[0].fengsu_3s, &lenlen, Header_packet, 3);
    //---------------------------AFA150a   fengsu_max---------------------------
    strcpy(Header_packet+lenlen,"AFA");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,"a");
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    //mindata
    Pack_Data(&Mindata_temp.sensor[0].fengsu_max, &lenlen, Header_packet, 3);
    
    
    //---------------------------AFB150   fengsu_1min_avg---------------------------
    strcpy(Header_packet+lenlen,"AFB");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    //mindata
    Pack_Data(&Mindata_temp.sensor[0].fengsu_1min_avg, &lenlen, Header_packet, 3);
    //---------------------------AFAl150  1 分钟风速分钟标准差 -------------------------
    strcpy(Header_packet+lenlen,"AFBl");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    
    if(num_mins == 1)//需要存储分钟数据，来自主动调用
    { //数据保存至flash结构体
      
      standardDeviation_1 = StandardDeviation(data_sample_buf_history, sample_qc_history, 240);
      if(INVALID_DATA != Mindata_temp.sensor[0].fengsu_1min_avg.data)
          Mindata_temp.Standard_Deviation[1] = standardDeviation_1 * 100;
      else
          Mindata_temp.Standard_Deviation[1] = INVALID_DATA;
      
      FLASH_Store_MinData.Standard_Deviation[1] = Mindata_temp.Standard_Deviation[1];
    }
    else if((num_mins == 0)||(num_mins == 2))//不需要存储的分钟数据，来自历史数据读取调用
    {
        if(INVALID_DATA != Mindata_temp.sensor[0].fengsu_1min_avg.data)
            Mindata_temp.Standard_Deviation[1] = FLASH_Read_MinData.Standard_Deviation[1];
        else
            Mindata_temp.Standard_Deviation[1] = INVALID_DATA;
    }
    else ;
 
    Pack_Datal(&Mindata_temp.Standard_Deviation[1], &lenlen, Header_packet, 4);
    
    //---------------------------AFC150   fengsu_2min_avg--------------------------
    strcpy(Header_packet+lenlen,"AFC");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    //mindata
    Pack_Data(&Mindata_temp.sensor[0].fengsu_2min_avg, &lenlen, Header_packet, 3);
    
    //---------------------------AFD150  fengsu_10min_avg--------------------------
    strcpy(Header_packet+lenlen,"AFD");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    //mindata
    Pack_Data(&Mindata_temp.sensor[0].fengsu_10min_avg, &lenlen, Header_packet, 3);
    
    
    
    //AEA150 qc
    S32ToStrAlignRight(Mindata_temp.sensor[0].fengxiang_1min.qc, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    
    //AEB150 qc
    S32ToStrAlignRight(Mindata_temp.sensor[0].fengxiang_1min_avg.qc, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    //AEBl150 qc
    Header_packet[lenlen]=QC_NOCONTROL+0x30;
    lenlen++; 
    //AEC150 qc
    S32ToStrAlignRight(Mindata_temp.sensor[0].fengxiang_2min_avg.qc, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    
    //AED150 qc
    S32ToStrAlignRight(Mindata_temp.sensor[0].fengxiang_10min_avg.qc, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    
    //AEF150 qc
    S32ToStrAlignRight(Mindata_temp.sensor[0].fengxiang_max.qc, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    
    
    
    
    //AFA150 qc
    S32ToStrAlignRight(Mindata_temp.sensor[0].fengsu_3s.qc, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    //AFA150a qc
    S32ToStrAlignRight(Mindata_temp.sensor[0].fengsu_max.qc, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    
    //AFB150 qc
    S32ToStrAlignRight(Mindata_temp.sensor[0].fengsu_1min_avg.qc, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    //AFBl150 qc
    Header_packet[lenlen]=QC_NOCONTROL+0x30;
    lenlen++;  
    
    //AFC150 qc
    S32ToStrAlignRight(Mindata_temp.sensor[0].fengsu_2min_avg.qc, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    
    //AFD150 qc
    S32ToStrAlignRight(Mindata_temp.sensor[0].fengsu_10min_avg.qc, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    
    
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

  if(num_mins == 1)//需要存储分钟数据，来自主动调用
  {
      sensors_tempdata->sensor[0].fengsu_max.data = INVALID_DATA;
      sensors_tempdata->sensor[0].fengsu_max.qc = QC_MISSED;
  }

  return lenlen++;
}

void GetSampleData(sensors_data_t *sensors_tempdata,float *dat,uint8 ch)
{
  char Header_packet[40];
  
  unsigned int lenlen = 0;
  unsigned char i, j = 0;
  s_RtcTateTime_t temp_time_struct;//时间结构体
  
  bcm_info_t info;

  memcpy(&info, &bcm_info,sizeof(bcm_info_t));
  
  memset(Header_packet,0,40);
  
  //---------------------包头-------------------
  strcpy(Header_packet,"BG,");//包头
  uartSendStr(0,(unsigned char *)Header_packet,0);
  
  uartSendStr(0,(unsigned char *)sensor_di,4);
  
  uartSendStr(0,",",0);
  
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
  uartSendStr(0,(unsigned char*)Header_packet,0);
  
  memset(Header_packet,0,40);
  //---------------------数据主体-------------------
  //  FLASH_Store_MinData_t Mindata_temp;//定义一个临时的数据结构体  sensors_data_t
  if(info.sensor.ce == 1)//定义为1.5m的风
  {
    if(ch == 1)//风向
    {
      strcpy(Header_packet,"AEA150");
    }
    else if(ch == 2)//风速
    {
      strcpy(Header_packet,"AFA150");
    }
    Header_packet[6]=',';
    lenlen = 7; 
  }
  else if(info.sensor.ce == WIND_10)//定义为10m的风
  {
    if(ch == 1)//风向
    {
      strcpy(Header_packet,"AEA");
    }
    else if(ch == 2)//风速
    {
      strcpy(Header_packet,"AFA");
    }
    Header_packet[3]=',';
    lenlen = 4; 
  }
  
  Header_packet[lenlen]= '\0';
  uartSendStr(0,(unsigned char*)Header_packet,0);
  
  memset(Header_packet,0,40);
  lenlen = 0;
  
  //取数据出来
  int *temp_data_buf = (int *)Header_packet;
  char int8_1[8];
  if(ch == 1)//风向
  {
    for(i=0; i<4; i++)
    {
      sample_data_read(i * 30 + FX_START_ADDR_INDEX,(char *)temp_data_buf,30);
      for(j = 0;j<15;j++)
      {
        S32ToStrAlignRight(temp_data_buf[j]/10, 3, int8_1);
        uartSendStr(0,(UINT8 *)int8_1,0);
        uartSendStr(0,",",0);
      }      
    }
  }
  else if(ch == 2)//风速
  {
    for(i=0; i<24; i++)
    {
      sample_data_read(i * 20,(char *)temp_data_buf,20);
      for(j = 0; j<10; j++)
      {
        S32ToStrAlignRight(temp_data_buf[j], 3, int8_1);
    uartSendStr(0,(unsigned char *)int8_1,0);
    uartSendStr(0,",",0);
      }      
    }
  }
  
  //---------------------结束-------------------
  strcpy(Header_packet,"ED\r\n");  
  //  lenlen=strlen((char *)Header_packet);
  uartSendStr(0,(unsigned char *)Header_packet,0);
}

