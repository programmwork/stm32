/*************************************************************************************************************
;*--------------------------------------------- �ļ���Ϣ ----------------------------------------------------
;* �ļ����� : 
;* �ļ����� : 
;* ����˵�� :
;*-------------------------------------------- ���°汾��Ϣ -------------------------------------------------
;* �޸����� :������
;* �޸����� :2015-5-12
;* �汾���� :
;*-------------------------------------------- ��ʷ�汾��Ϣ -------------------------------------------------
;* �ļ����� : ������
;* �������� : 2015-5-8
;* �汾���� :
;*************************************************************************************************************/
#include "sensor_basic.h"

#define FENGSU_pamA         0.296                   //���ٴ�����ϵ��
#define FENGSU_pamB         0.04909

#define PI 3.1415928
#define FX_START_ADDR_INDEX 1280    //��5page

float wdx=0;
float wdy=0;
float arch = 0;
static unsigned char Flag_max_feng = 0;//�����ѯ�˱�־λ


float data_sample_buf[SAMPLE_DATA_NUM];//�������ݱ��滺����
float data_sample_buf_history[SAMPLE_DATA_NUM];//�������ݱ��滺����
unsigned char sample_qc[SAMPLE_DATA_NUM/8 +1];
unsigned char sample_qc_history[SAMPLE_DATA_NUM/8 +1];

unsigned char data_valid_num = 0;//��һ������Ч�������ݳ���
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
*�������ܣ����뼶��ʱ�ɼ� 
*��ڲ������������ݽṹ��
*���ڲ������������ݽṹ��
*������  ��sapi.c�еĶ�ʱ����������   ÿ�����һ�ε���
*�������ã�Element_SecSample(&sensors_data)
==============================================================================================*/
unsigned char Element_MsSample(sensors_data_t *sensors_tempdata)
{
  unsigned char temp_fengsu_event = 0;
  float value_FengSu = 0;
  
  temp_fengsu_event = Fengsu_engine(&value_FengSu);//��Ƶ��ֵ
  
  if(Flag_DataEmulationMode == 1)//��X1Ϊ1ʱ�������������ģ������
  {
    if(EmulationSensorChannel == 2)//����
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
    if(Flag_DataEmulationMode == 0)//��X1Ϊ1ʱ�������������ģ������
    {
      value_FengSu = (float)(FENGSU_pamA + FENGSU_pamB * value_FengSu);
      if(value_FengSu < 0.35) value_FengSu = 0;   /* ��jeff.liu�� */
    }
    else
    {
      if(EmulationDataType == 1)//�Ƿ�������ģ��ֵ
        value_FengSu = (float)(FENGSU_pamA + FENGSU_pamB * value_FengSu);
      else if(EmulationDataType == 0)
        value_FengSu = value_FengSu;
    }
    //aX^2 + bX + c
    sensors_tempdata->sensor[0].fengsu_MomentaryValue.data =  bcm_info.sensor.cr[0].a * value_FengSu * value_FengSu+\
                                                              bcm_info.sensor.cr[0].b * value_FengSu + bcm_info.sensor.cr[0].c;

    //�����������
    data_sample_buf[sample_num_ws] = (int)(sensors_tempdata->sensor[0].fengsu_MomentaryValue.data);
    
    Quality_Sec_Ctrl(sensors_tempdata,0);//��������
    //�������ȷ��־λ   ����ǰ30���ֽ�
    if(sensors_data.sensor[0].fengsu_MomentaryValue.qc == QC_OK)
      sample_qc[sample_num_ws/8] |=(1<<(sample_num_ws%8));
    
    sample_num_ws++;
    if(sample_num_ws == 240) sample_num_ws = 0; 
    
  }
  return 1;
}
/*
*�������ܣ��뼶��ʱ�ɼ� 
*��ڲ������������ݽṹ��
*���ڲ������������ݽṹ��
*������  ��sapi.c�еĶ�ʱ����������   ÿ�����һ�ε���
*�������ã�Element_SecSample(&sensors_data)
*/
unsigned char Element_SecSample(sensors_data_t *sensors_tempdata)
{
  unsigned char temp_fengxiang_event = 0;
  float value_FengXiang = 0;
  
  temp_fengxiang_event = FengXiang_engine(&value_FengXiang);//����ѹֵ

  if(Flag_DataEmulationMode == 1)//��X1Ϊ1ʱ�������������ģ������
  {
    if(EmulationSensorChannel == 1)//����
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
    if(Flag_DataEmulationMode == 0)//��X1Ϊ0ʱ�������ָ�����ʵ��״̬
    {
      sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data = value_FengXiang;
      
//     sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data = \
//       sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data *sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data * bcm_info.sensor.ab1+
//        sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data * bcm_info.sensor.ab2 + bcm_info.sensor.ab3;
//           sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data = \
//        sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data * bcm_info.sensor.ab1 + bcm_info.sensor.ab2;
      
      
        //  *pFengXiang = bcm_info.sensor.ab1 * v_Desired + bcm_info.sensor.ab2;    // 1.5�׷����һ������У׼

    }
    else//ģ��״̬
    {
      if(EmulationDataType == 1)//�Ƿ�������ģ��ֵ
        sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data = value_FengXiang;
      else if(EmulationDataType == 0)
        sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data = value_FengXiang;
    }
    
    data_sample_buf[240+m_tempdata.m_RtcTateTime.sec] = (int)(sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data);

      Quality_Sec_Ctrl(sensors_tempdata,1);//��������    
      //�������ȷ��־λ   ����ǰ15���ֽ�
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
    *winddir_d=(atan(*winddir_x/ (*winddir_y)))*180/PI;//�õ����ӷ���ƽ��ֵ,���Ȼ��ɽǶ�
  
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
*�������ܣ���ʱ�ɼ�
*��ڲ������������ݽṹ��
*���ڲ������������ݽṹ��
*������  ��sapi.c�еĶ�ʱ����������   ÿ���ӽ���һ�ε���
*�������ã�Element_MinSample(&sensors_data)
*/
unsigned char Element_MinSample(sensors_data_t *sensors_tempdata)
{
    unsigned char i = 0;
    float add = 0;
    unsigned char counter = 0;
  
    //�������ӵ�ʱ�䲻�������Խ��䶨λ��ĳһ����
    //�忨�Լ�
    Self_test();
  
    sample_data_save((char *)data_sample_buf,sizeof(data_sample_buf));//�������ݱ���

    memcpy(data_sample_buf_history,data_sample_buf,sizeof(data_sample_buf));
    memset(data_sample_buf,0,sizeof(data_sample_buf));  //�������ݻ�������0

    memcpy(sample_qc_history,sample_qc,sizeof(sample_qc));
    memset(sample_qc,0,sizeof(sample_qc));

  //data_valid_num =sensors_tempdata->sensor[0].secdata_counter; 
  
  //---------------------------------1min��2min   avg  wind speed--------------      
  //��Ҫ�ȼ�ȥ1min��2min��ǰ���ֵ
  //  if(sensors_tempdata->sensor[0].secdata_counter>80)//2min   120*2/3=80
  //  uint8 temp_wjj[48];        //////////////20151206   ���Լ�ȥ
  //  static uint8 min1_count = 0,min2_count = 0,min10_count = 0;
  
  /*��jeff.liu��*/
  //  if(m_tempdata.debugon == 1)
  //  {
  //    //    char temp_wjj[32];
  //    
  //    sprintf(temp_wjj,"MIN1_COUNT_WS:%d\r\n",
  //            sensors_tempdata->sensor[0].fengsu_min1_counter          ); 	// �����޸ġ���ʱ��ʾƵ�ʺͲ���ֵ
  //    uartSendStr(0,(const char*)temp_wjj,strlen(temp_wjj));
  //  }
  
  //----------------------------1min ����
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
  //----------------------------2min  ����
  sensors_tempdata->sensor[0].fengsu_min2_counter+=sensors_tempdata->sensor[0].fengsu_min1_counter;
  sensors_tempdata->sensor[0].avg2minsum.data+=sensors_tempdata->sensor[0].avg1minsum.data;
  if(sensors_tempdata->sensor[0].fengsu_min2_counter>90)//2min   120*3/4=90
  {
    sensors_tempdata->sensor[0].fengsu_2min_avg.data = \
      (float)(sensors_tempdata->sensor[0].avg2minsum.data/(float)sensors_tempdata->sensor[0].fengsu_min2_counter);
    Quality_Min_Ctrl(sensors_tempdata,1);//2min  �ʿ�
  }
  else
  {
    sensors_tempdata->sensor[0].fengsu_2min_avg.data = INVALID_DATA;
    sensors_tempdata->sensor[0].fengsu_2min_avg.qc = QC_MISSED;
  }
  /*��jeff.liu��*/
  //  if(m_tempdata.debugon == 1)
  //  {
  //    sprintf(temp_wjj,"MIN2_COUNT_WS:%d\r\n",
  //            sensors_tempdata->sensor[0].fengsu_min2_counter); 	// �����޸ġ���ʱ��ʾƵ�ʺͲ���ֵ
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //  }
  
  //1min��ȷ�ۼ�ֵ����2min
  sensors_tempdata->sensor[0].fengsu_min2_counter = sensors_tempdata->sensor[0].fengsu_min1_counter;
  //1min��ȷ�ۼ�ֵ����2min
  sensors_tempdata->sensor[0].avg2minsum.data = sensors_tempdata->sensor[0].avg1minsum.data;
  
  //---------------------------------10min   ����--------------     
  add = 0;
  counter = 0;
  //�ۼ�ǰ9��������
  for(i=1;i<10;i++)
  {
    if(sensors_tempdata->sensor[0].fengsumindatalast9min[i].qc == QC_OK)
    {
      add += sensors_tempdata->sensor[0].fengsumindatalast9min[i].data;		
      counter++;
    }
  }
  //�ۼӵ�ǰ��������
  if(sensors_tempdata->sensor[0].fengsu_1min_avg.qc== QC_OK) 
  {
    add += sensors_tempdata->sensor[0].fengsu_1min_avg.data;
    counter++;
  }
  
  /* ���� ����ǰ9������ֵ */
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
  
  //���㵱ǰ��ƽ��ֵ���ʿ�
  if(counter < 8)	//ȱ�� 	  10*3/4
  {
    sensors_tempdata->sensor[0].fengsu_10min_avg.data = INVALID_DATA ;
    sensors_tempdata->sensor[0].fengsu_10min_avg.qc = QC_MISSED;			
  }
  else				          // ��ȷ									
  {
    sensors_tempdata->sensor[0].fengsu_10min_avg.data = (float)(add/(float)counter);
    Quality_Min_Ctrl(sensors_tempdata,2);//10 min ws
  }
  //mindata_counter:1min��ƽ��ֵ����
  sensors_tempdata->sensor[0].fengsu_min1_counter=0;
  //mindata_add:1min�� ƽ��ֵ�ۼ�
  sensors_tempdata->sensor[0].avg1minsum.data=0;
  


  //----------------------------˲ʱ  ����
  Quality_Min_Ctrl(sensors_tempdata,6);
  
  //----------------------------1min ����
  if(sensors_tempdata->sensor[0].fengxiang_min1_counter>45)//1min
  {    //xƽ��ֵ
    sensors_tempdata->sensor[0].avg1m_x.data = sensors_tempdata->sensor[0].avg1m_x_sum.data / sensors_tempdata->sensor[0].fengxiang_min1_counter;
    sensors_tempdata->sensor[0].avg1m_x.qc = QC_OK;
    //yƽ��ֵ
    sensors_tempdata->sensor[0].avg1m_y.data = sensors_tempdata->sensor[0].avg1m_y_sum.data / sensors_tempdata->sensor[0].fengxiang_min1_counter;
    sensors_tempdata->sensor[0].avg1m_y.qc = QC_OK;
    //-----------------����1min��ƽ������
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
  //              ); 	// �����޸ġ���ʱ��ʾƵ�ʺͲ���ֵ
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //  }
  
  //-------------//����˷��ӵķ���  ��Ϊ������ٶ�Ӧ�ļ������----------- 
  if(Flag_max_feng == 1)
  {
    Flag_max_feng = 0;
    sensors_tempdata->sensor[0].fengxiang_max.data = sensors_tempdata->sensor[0].fengxiang_1min_avg.data;
    sensors_tempdata->sensor[0].fengxiang_max.qc = sensors_tempdata->sensor[0].fengxiang_1min_avg.qc;
  } 
  
  //----------------------------2min  ����
  
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
  //              ); 	// �����޸ġ���ʱ��ʾƵ�ʺͲ���ֵ
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //  } 
  if(sensors_tempdata->sensor[0].fengxiang_min2_counter>90)//2min   120*3/4=90
  { //xƽ��ֵ
    sensors_tempdata->sensor[0].avg2m_x.data = sensors_tempdata->sensor[0].avg2m_x_sum.data / sensors_tempdata->sensor[0].fengxiang_min2_counter;
    //yƽ��ֵ
    sensors_tempdata->sensor[0].avg2m_y.data = sensors_tempdata->sensor[0].avg2m_y_sum.data / sensors_tempdata->sensor[0].fengxiang_min2_counter;
    
    //-----------------����2min��ƽ������
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
  //            sensors_tempdata->sensor[0].fengxiang_min2_counter); 	// �����޸ġ���ʱ��ʾƵ�ʺͲ���ֵ
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //    
  //    sprintf(temp_wjj,"avg:2_x:%f,2_y:%f,wd_2min:%f\r\n",
  //            sensors_tempdata->sensor[0].avg2m_x.data,
  //            sensors_tempdata->sensor[0].avg2m_y.data,
  //            sensors_tempdata->sensor[0].fengxiang_2min_avg.data
  //              ); 	// �����޸ġ���ʱ��ʾƵ�ʺͲ���ֵ
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //    
  //  } 
  //1min��ȷ�ۼ�ֵ����2min
  sensors_tempdata->sensor[0].fengxiang_min2_counter = sensors_tempdata->sensor[0].fengxiang_min1_counter;
  //1min��ȷ�ۼ�ֵ����2min
  sensors_tempdata->sensor[0].avg2m_x_sum.data = sensors_tempdata->sensor[0].avg1m_x_sum.data;
  sensors_tempdata->sensor[0].avg2m_y_sum.data = sensors_tempdata->sensor[0].avg1m_y_sum.data;
  
  //---------------------------------10min   ����--------------     
  float add_x = 0,add_y = 0;
  unsigned char counter_x = 0,counter_y = 0;
  float avg10_x = 0,avg10_y = 0;
  
  //�ۼ�ǰ9��������
  //  qcdata_t fengxiangmindatalast9min_x[10];	      //��ȥ10���ӵ�x����ƽ��ֵ���� 
  //  qcdata_t fengxiangmindatalast9min_y[10];	      //��ȥ10���ӵ�y����ƽ��ֵ���� 
  
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
  //�ۼӵ�ǰ��������
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

  /* ���� ����ǰ9������ֵ */
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
  //              sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[i].data, 	// �����޸ġ���ʱ��ʾƵ�ʺͲ���ֵ
  //              sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[i].data); 	// �����޸ġ���ʱ��ʾƵ�ʺͲ���ֵ
  //      uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //    }
  //    uartSendStr(0,"\r\n",2);
  //  }
  
  //���㵱ǰ��ƽ��ֵ���ʿ�
  if((counter_x>=8)&&(counter_y>=8))				          // ��ȷ									
  {
    avg10_x = add_x/counter_x;
    avg10_y = add_y/counter_y;
    
    
    //-----------------����10min��ƽ������
    WindDirectCal(&avg10_x,&avg10_y, &sensors_tempdata->sensor[0].fengxiang_10min_avg.data );  
    Quality_Min_Ctrl(sensors_tempdata,5);
  }
  else	// ����	  10*3/4
  {
    sensors_tempdata->sensor[0].fengxiang_10min_avg.data = INVALID_DATA ;
    sensors_tempdata->sensor[0].fengxiang_10min_avg.qc = QC_MISSED;			
  } 
  //  if(m_tempdata.debugon == 3)
  //  {
  //    sprintf(temp_wjj,"add10:--x:%f,y:%f\r\n",
  //            add_x,add_y); 	// �����޸ġ���ʱ��ʾƵ�ʺͲ���ֵ
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //    sprintf(temp_wjj,"counter10:x--%d,y--%d\r\n",
  //            counter_x,counter_y); 	// �����޸ġ���ʱ��ʾƵ�ʺͲ���ֵ
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //    sprintf(temp_wjj,"avg10:x--%f,y--%f\r\n",
  //            avg10_x, avg10_y); 	// �����޸ġ���ʱ��ʾƵ�ʺͲ���ֵ
  //    uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
  //    
  //  } 
  
  //mindata_counter:1min��ƽ��ֵ����
  sensors_tempdata->sensor[0].fengxiang_min1_counter=0;
  //mindata_add:1min�� ƽ��ֵ�ۼ�
  sensors_tempdata->sensor[0].avg1m_x_sum.data=0;
  sensors_tempdata->sensor[0].avg1m_y_sum.data=0;
  
  
  //  data_valid_num  = 0;  
  //  memset(data_sample_buf,0,sizeof(data_sample_buf));//�������ݻ�������0
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
*�������ܣ��뼶��������
*��ڲ������������ݽṹ��
*���ڲ���������������
*������  ���뼶��������
*�������ã�Quality_Sec_Ctrl(&sensors_data)
====================================================================================*/
unsigned char Quality_Sec_Ctrl(sensors_data_t *sensors_tempdata,unsigned char ch)
{ 
  float add = 0;
  unsigned char counter = 0;
  unsigned char i = 0;
  float result_sensor;

  switch(ch)
  {
  case 0://250ms���ټ��
    result_sensor = sensors_tempdata->sensor[0].fengsu_MomentaryValue.data;
    
    //�������ж�
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
    //�ۼӵ�ǰֵ
    if(sensors_tempdata->sensor[0].fengsu_MomentaryValue.qc== QC_OK) 
    {
      add += sensors_tempdata->sensor[0].fengsu_MomentaryValue.data;
      counter++;
    }
    //���㵱ǰ3s��ƽ��ֵ���ʿ�
    float temp_fengsu_3s_data = 0;
    if(counter < 10)	// ����	  12*3/4
    {
      sensors_tempdata->sensor[0].fengsu_3s.data = INVALID_DATA ;
      sensors_tempdata->sensor[0].fengsu_3s.qc = QC_MISSED;			
    }
    else				          // ��ȷ									
    {
      temp_fengsu_3s_data = (float)(add/(float)counter); 
      sensors_tempdata->sensor[0].fengsu_3s.data = temp_fengsu_3s_data;
      sensors_tempdata->sensor[0].fengsu_3s.qc = \
          iQCPM_Code(0,sensors_tempdata->sensor[0].fengsu_3s.data,&bcm_info.sensor.qm[WIND_VELOCITY_3_S]);
    }
    
    //        //�����������
    //    if(sensors_data.sensor[0].fengsu_3s.data == INVALID_DATA)
    //      data_sample_buf[sample_num_ws] = (int)(temp_fengsu_3s_data);
    //    else
    //      data_sample_buf[sample_num_ws] = (int)(temp_fengsu_3s_data*10);
    //  
    
    /* ���� ����12������ֵ */
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
    /*��jeff.liu��*/
    //    if(m_tempdata.debugon == 1)
    //    {
    //      
    //      uartSendStr(0,"ws_sample:",10);
    //      
    //      for(unsigned char i=0;i<12;i++)
    //      {
    //        len_1 = sprintf(temp_wjj,"%.2f  ",
    //                        sensors_tempdata->sensor[0].fengsusecdatalast3s[i].data); 	// �����޸ġ���ʱ��ʾƵ�ʺͲ���ֵ
    //        uartSendStr(0,(unsigned char *)temp_wjj,len_1);
    //      }
    //      uartSendStr(0,"\r\n",2);
    //    }
#endif    
    //---------------------------------���Ӽ������--------------
    if(sensors_tempdata->sensor[0].fengsu_3s.qc == QC_OK)
    {
      if(sensors_tempdata->sensor[0].fengsu_max.data<=sensors_tempdata->sensor[0].fengsu_3s.data)
      {
        sensors_tempdata->sensor[0].fengsu_max.data = sensors_tempdata->sensor[0].fengsu_3s.data;
        sensors_tempdata->sensor[0].fengsu_max.qc = sensors_tempdata->sensor[0].fengsu_3s.qc;
        Flag_max_feng = 1;//�����ѯ�˱�־λ
      }
    }
    
    if( m_tempdata.event.Flag_1s==true)//ȷ���������ֵ      ��Ҫ�Ӷ�ʱ��������
    {//�������ʱ���ۼ�1min  ��2min��ֵ  
      m_tempdata.event.Flag_1s = false;
      if(sensors_tempdata->sensor[0].fengsu_3s.qc == QC_OK)
      {//������ǰ��1min������3sƽ������ֵ�ۼ�
        sensors_tempdata->sensor[0].fengsu_min1_counter++;
        //count_1min++;
        sensors_tempdata->sensor[0].avg1minsum.data += sensors_tempdata->sensor[0].fengsu_3s.data;
      }        
#if 0
      /*��jeff.liu��*/
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

  case 1://1s������
    //    uint8 temp_wjj[32];    
    //    int len_1;
    result_sensor = sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data;
    //�������ж�
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
          //-------------------�����ۼ�     
          
          if(sensors_tempdata->sensor[0].fengxiang_MomentaryValue.qc==QC_OK)
          {
            arch=2*PI*sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data/360;//�ǶȻ���ɻ���
            wdx=wdx+sin(arch);
            wdy=wdy+cos(arch);
            
            //������ǰ��2min������1sƽ������ֵ�ۼ�   mindatalast4min[0]
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
            //                ); 	// �����޸ġ���ʱ��ʾƵ�ʺͲ���ֵ
            //      uartSendStr(0,(unsigned char *)temp_wjj,strlen((char *)temp_wjj));
            
            wdx = 0;wdy = 0;
          }

    return 1;
    
  default:
    return 0;
  }
  
}
/*
*�������ܣ����Ӽ���������
*��ڲ������������ݽṹ��
*���ڲ���������������
*������  �����Ӳ�������
*�������ã�Quality_Sec_Ctrl(&sensors_data,0)
*/
unsigned char Quality_Min_Ctrl(sensors_data_t *sensors_tempdata,unsigned char mins)
{
  float result_sensor, max = 0, min = 0;
  short changerate = 0;
  unsigned char i = 0, j = 0, count = 0;

  switch(mins)
  {
  case 0://һ���ӷ��ٵ���������
    result_sensor = sensors_tempdata->sensor[0].fengsu_1min_avg.data;
    //�������ж�
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
    
  case 1://�����ӷ��ٵ���������
    result_sensor = sensors_tempdata->sensor[0].fengsu_2min_avg.data;
    //�������ж�
    if((result_sensor<bcm_info.sensor.qm[WIND_VELOCITY_2_M].min)||\
      (result_sensor>bcm_info.sensor.qm[WIND_VELOCITY_2_M].max))
    {
     // sensors_tempdata->sensor[0].fengsu_2min_avg.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengsu_2min_avg.qc=QC_ERROR;
    }
    else
    {
        //      sensors_tempdata->sensor[0].fengsu_2min_avg.data=result_sensor;//�˾���Բ�Ҫ
        sensors_tempdata->sensor[0].fengsu_2min_avg.qc=\
            iQCPM_Code(WIND_VELOCITY_2_M,result_sensor,&bcm_info.sensor.qm[WIND_VELOCITY_2_M]);
    }
    break;
    
  case 2://ʮ���ӷ��ٵ���������
    result_sensor = sensors_tempdata->sensor[0].fengsu_10min_avg.data;
    //�������ж�
    if((result_sensor<bcm_info.sensor.qm[WIND_VELOCITY_10_M].min)||\
      (result_sensor>bcm_info.sensor.qm[WIND_VELOCITY_10_M].max))
    {
      //sensors_tempdata->sensor[0].fengsu_10min_avg.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengsu_10min_avg.qc=QC_ERROR;
    }
    else
    {
        //      sensors_tempdata->sensor[0].fengsu_10min_avg.data=result_sensor;//�˾���Բ�Ҫ
        sensors_tempdata->sensor[0].fengsu_10min_avg.qc=\
            iQCPM_Code(WIND_VELOCITY_10_M,result_sensor,&bcm_info.sensor.qm[WIND_VELOCITY_10_M]);
    }
    break;
    
  case 3://һ���ӷ������������
    result_sensor = sensors_tempdata->sensor[0].fengxiang_1min_avg.data;
    int temp_result_sensor = 0;
    temp_result_sensor = (int)result_sensor;
    //�������ж�
    if((temp_result_sensor<bcm_info.sensor.qm[WIND_DIRECTION_1_M].min)||\
      (temp_result_sensor>bcm_info.sensor.qm[WIND_DIRECTION_1_M].max))
    {
      //sensors_tempdata->sensor[0].fengxiang_1min_avg.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengxiang_1min_avg.qc=QC_ERROR;
    }
    else
    {
        //      sensors_tempdata->sensor[0].fengsu_1min_avg.data=result_sensor;//�˾���Բ�Ҫ
        sensors_tempdata->sensor[0].fengxiang_1min_avg.qc=\
            iQCPM_Code(WIND_DIRECTION_1_M,result_sensor,&bcm_info.sensor.qm[WIND_DIRECTION_1_M]);
    }

    //��СӦ�仯���
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
    
  case 4://�����ӷ������������
    result_sensor = sensors_tempdata->sensor[0].fengxiang_2min_avg.data;
    //�������ж�
    if((result_sensor<bcm_info.sensor.qm[WIND_DIRECTION_2_M].min)||\
      (result_sensor>bcm_info.sensor.qm[WIND_DIRECTION_2_M].max))
    {
      //sensors_tempdata->sensor[0].fengxiang_2min_avg.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengxiang_2min_avg.qc=QC_ERROR;
    }
    else
    {
        //      sensors_tempdata->sensor[0].fengsu_2min_avg.data=result_sensor;//�˾���Բ�Ҫ
        sensors_tempdata->sensor[0].fengxiang_2min_avg.qc=\
            iQCPM_Code(WIND_DIRECTION_2_M,result_sensor,&bcm_info.sensor.qm[WIND_DIRECTION_2_M]);
    }

    //��СӦ�仯���
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
    
  case 5://ʮ���ӷ������������
    result_sensor = sensors_tempdata->sensor[0].fengxiang_10min_avg.data;
    //�������ж�
    if((result_sensor<bcm_info.sensor.qm[WIND_DIRECTION_10_M].min)||\
      (result_sensor>bcm_info.sensor.qm[WIND_DIRECTION_10_M].max))
    {
    //  sensors_tempdata->sensor[0].fengxiang_10min_avg.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengxiang_10min_avg.qc=QC_ERROR;
    }
    else
    {
        //      sensors_tempdata->sensor[0].fengsu_10min_avg.data=result_sensor;//�˾���Բ�Ҫ
        sensors_tempdata->sensor[0].fengxiang_10min_avg.qc=\
            iQCPM_Code(WIND_DIRECTION_10_M,result_sensor,&bcm_info.sensor.qm[WIND_DIRECTION_10_M]);
    }

    //��СӦ�仯���
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
    
  case 6://˲ʱ�������������
    result_sensor = sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data;
    //�������ж�
    if((result_sensor<bcm_info.sensor.qm[WIND_DIRECTION_0_M].min)||\
      (result_sensor>bcm_info.sensor.qm[WIND_DIRECTION_0_M].max))
    {
    //  sensors_tempdata->sensor[0].fengxiang_10min_avg.data=INVALID_DATA;
      sensors_tempdata->sensor[0].fengxiang_1min.qc=QC_ERROR;
	 // sensors_tempdata->sensor[0].fengxiang_1min.data = INVALID_DATA;
    }
    else
    {
        //      sensors_tempdata->sensor[0].fengsu_10min_avg.data=result_sensor;//�˾���Բ�Ҫ
        sensors_tempdata->sensor[0].fengxiang_1min.qc=\
            iQCPM_Code(WIND_DIRECTION_0_M,result_sensor,&bcm_info.sensor.qm[WIND_DIRECTION_0_M]);
		sensors_tempdata->sensor[0].fengxiang_1min.data = result_sensor;
    }

    //��СӦ�仯���
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
*�������ܣ��������ݳ�ʼ��
*��ڲ������������ݽṹ��
*���ڲ�������
*������  ��sapi.c�е�ϵͳ��ʼ������
*�������ã�SampleData_Init(&sensors_data)
*/
void SampleData_Init(sensors_data_t *sensors_tempdata)
{ 
    unsigned char j = 0;
  //--------------------------����------------------------------------
  //250ms���ٲ���ֵ
  sensors_tempdata->sensor[0].fengsu_MomentaryValue.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengsu_MomentaryValue.qc=QC_MISSED;
  for(j = 0;j<12;j++) //��ȥ3s��12����������
  {      
    sensors_tempdata->sensor[0].fengsusecdatalast3s[j].data=INVALID_DATA;
    sensors_tempdata->sensor[0].fengsusecdatalast3s[j].qc=QC_MISSED;
    
  }
  //˲ʱ����(3��ƽ��)
  sensors_tempdata->sensor[0].fengsu_3s.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengsu_3s.qc=QC_MISSED;
  
  //��2min����ǰ��һ��3sƽ��ֵ
  sensors_tempdata->sensor[0].fensu_2min_3s.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fensu_2min_3s.qc=QC_MISSED;
  //�������� ���1min����ǰ��һ��3sƽ��ֵ
  sensors_tempdata->sensor[0].fensu_1min_3s.data=INVALID_DATA;	
  sensors_tempdata->sensor[0].fensu_1min_3s.qc=QC_MISSED;
  
  
  //�������ۼӻ�1min��ƽ��ֵ�ۼ�
  sensors_tempdata->sensor[0].avg1minsum.data=0;
  sensors_tempdata->sensor[0].avg1minsum.qc=QC_MISSED;
  //�����ݸ��� //1�������ݸ���
  sensors_tempdata->sensor[0].fengsu_min1_counter=0;
  //��������	�ۼӻ�2min��ƽ��ֵ�ۼ�
  sensors_tempdata->sensor[0].avg2minsum.data=0;	
  sensors_tempdata->sensor[0].avg2minsum.qc=QC_MISSED;
  //��������	����//2�������ݸ��� 
  sensors_tempdata->sensor[0].fengsu_min2_counter=0;	
  
  //һ����ƽ������
  sensors_tempdata->sensor[0].fengsu_1min_avg.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengsu_1min_avg.qc=QC_MISSED;
  
  //������ƽ������
  sensors_tempdata->sensor[0].fengsu_2min_avg.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengsu_2min_avg.qc=QC_MISSED;
  
  //ʮ����ƽ������
  sensors_tempdata->sensor[0].fengsu_10min_avg.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengsu_10min_avg.qc=QC_MISSED;
  
  for(j = 0;j<10;j++) //��ȥ9���ӵķ�������
  {      
    sensors_tempdata->sensor[0].fengsumindatalast9min[j].data=INVALID_DATA;
    sensors_tempdata->sensor[0].fengsumindatalast9min[j].qc=QC_MISSED;
  }
  
  // //�����ڼ������
  sensors_tempdata->sensor[0].fengsu_max.data=0;
  sensors_tempdata->sensor[0].fengsu_max.qc=QC_MISSED;
  
  //--------------------------����------------------------------------
  //�������
  sensors_tempdata->sensor[0].fengxiang_max.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengxiang_max.qc=QC_MISSED;
  
  //1s�������ֵ
  sensors_tempdata->sensor[0].fengxiang_MomentaryValue.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengxiang_MomentaryValue.qc=QC_MISSED;
  
  
  sensors_tempdata->sensor[0].fengxiang_1min_avg.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengxiang_1min_avg.qc=QC_MISSED;
  
  sensors_tempdata->sensor[0].fengxiang_2min_avg.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengxiang_2min_avg.qc=QC_MISSED;
  
  sensors_tempdata->sensor[0].fengxiang_10min_avg.data=INVALID_DATA;
  sensors_tempdata->sensor[0].fengxiang_10min_avg.qc=QC_MISSED;
  
  //��ȥ9���ӵķ�������
  for(j = 0;j<10;j++)
  {      
    sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[j].data=INVALID_DATA;
    sensors_tempdata->sensor[0].fengxiangmindatalast9min_x[j].qc=QC_MISSED;
    
    
    sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[j].data=INVALID_DATA;
    sensors_tempdata->sensor[0].fengxiangmindatalast9min_y[j].qc=QC_MISSED;
  }
  
  //1min��xƽ��ֵ�ۼ�
  sensors_tempdata->sensor[0].avg1m_x_sum.data=0;
  sensors_tempdata->sensor[0].avg1m_x_sum.qc=QC_MISSED;
  //1min��yƽ��ֵ�ۼ�
  sensors_tempdata->sensor[0].avg1m_y_sum.data=0;
  sensors_tempdata->sensor[0].avg1m_y_sum.qc=QC_MISSED;
  
  //2min��xƽ��ֵ�ۼ�
  sensors_tempdata->sensor[0].avg2m_x_sum.data=0;
  sensors_tempdata->sensor[0].avg2m_x_sum.qc=QC_MISSED;
  //2min��yƽ��ֵ�ۼ�
  sensors_tempdata->sensor[0].avg2m_y_sum.data=0;
  sensors_tempdata->sensor[0].avg2m_y_sum.qc=QC_MISSED;
  
  //1min��xƽ��ֵ
  sensors_tempdata->sensor[0].avg1m_x.data=0;
  sensors_tempdata->sensor[0].avg1m_x.qc=QC_MISSED;
  //1min��yƽ��ֵ
  sensors_tempdata->sensor[0].avg1m_y.data=0;
  sensors_tempdata->sensor[0].avg1m_y.qc=QC_MISSED;
  
  //2min��xƽ��ֵ
  sensors_tempdata->sensor[0].avg2m_x.data=0;
  sensors_tempdata->sensor[0].avg2m_x.qc=QC_MISSED;
  //2min��yƽ��ֵ
  sensors_tempdata->sensor[0].avg2m_y.data=0;
  sensors_tempdata->sensor[0].avg2m_y.qc=QC_MISSED;
  
  //�����ݸ��� //1�������ݸ���
  sensors_tempdata->sensor[0].fengxiang_min1_counter=0;
  //��������	����//2�������ݸ��� 
  sensors_tempdata->sensor[0].fengxiang_min2_counter=0;	
  
  
  //�����ڼ�����٣�˲ʱ���٣���Ӧ����
  //  sensors_tempdata->sensor[0].fengxiang_max.data=0;
  //  sensors_tempdata->sensor[0].fengxiang_max.qc=QC_MISSED;
  
  for(j = 0; j < 4; j++)
   {
   	sensors_tempdata->sensor[0].changerate[j].hourdata_counter = 0;   //Сʱ���ݸ����ۼ�
    sensors_tempdata->sensor[0].changerate[j].num = 0;
    sensors_tempdata->sensor[0].changerate[j].hour_data_flag = 0;
	}	

    memset(data_sample_buf,0,sizeof(data_sample_buf));//�������ݻ�������0
    memset(data_sample_buf_history,0,sizeof(data_sample_buf_history));  //�������ݻ�������0
    memset(sample_qc,0,sizeof(sample_qc));//������־λ��������0
    memset(sample_qc_history,0,sizeof(sample_qc_history));

	//��ʼ�� QC���ݻ���
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
*�������ܣ��������Ƴ�ʼ��
*��ڲ������������ݽṹ��
*���ڲ�������
*������  ��sapi.c�е�ϵͳ��ʼ������
*�������ã�Quality_Init(&sensors_data)
*/
void Quality_Init(sensors_data_t *sensors_tempdata)
{//��Ҫ��flash�ж��������ֵ
  //����
  //sensors_tempdata->qualityCtrl[0].secdata_ctrl_t.Vmin=bcm_info.sensor.qs.sensorxiaxian1;
  //sensors_tempdata->qualityCtrl[0].secdata_ctrl_t.Vmax=bcm_info.sensor.qs.sensorshangxian1;
  //sensors_tempdata->qualityCtrl[0].secdata_ctrl_t.Rmax=bcm_info.sensor.qs.sensormaxsulu1;
  //sensors_tempdata->qualityCtrl[0].mindata_ctrl_t.Vmin=bcm_info.sensor.qs.sensorxiaxian1;
  //sensors_tempdata->qualityCtrl[0].mindata_ctrl_t.Vmax=bcm_info.sensor.qs.sensorshangxian1;
  
  //����
  //sensors_tempdata->qualityCtrl[1].secdata_ctrl_t.Vmin=bcm_info.sensor.qs.sensorxiaxian2;
  //sensors_tempdata->qualityCtrl[1].secdata_ctrl_t.Vmax=bcm_info.sensor.qs.sensorshangxian2;
  //sensors_tempdata->qualityCtrl[1].secdata_ctrl_t.Rmax=bcm_info.sensor.qs.sensorshangxian2;
  //sensors_tempdata->qualityCtrl[1].mindata_ctrl_t.Vmin=bcm_info.sensor.qs.sensorxiaxian2;
  //sensors_tempdata->qualityCtrl[1].mindata_ctrl_t.Vmax=bcm_info.sensor.qs.sensorshangxian2;
} 

/*
*�������ܣ��������������ƺ���
*��ڲ�����sensorNo--��֧��������value--��������ֵ��*qcctrl--qc��ʼ��������������
*���ڲ���������������������
*������  ��Quality_Sec_Ctrl����
*�������ã�iQCPS_Code(0,sensors_tempdata->sensor[0].secdata.data,&sensors_tempdata->qualityCtrl[0]);
*/

unsigned char iQCPS_Code_ws250(int sensorNo,float value,qs_t *qcctrl)
{
  //  static unsigned char flag[MAX_SENSOR_NUM];
  unsigned char iRet=QC_ERROR;
  
  if((value >=qcctrl->min )&&( value <= qcctrl->max))
  {
    iRet = QC_OK;//��ȷ
  }
  else
  {
    iRet = QC_ERROR;	//����
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
    iRet = QC_OK;//��ȷ
  }
  else
  {
    iRet =QC_ERROR;	//����
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
    iRet = QC_OK;//��ȷ
  }
  else
  {
    iRet =QC_ERROR;	//����
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
    iRet = QC_OK;//��ȷ
  }
  else
  {
    iRet =QC_ERROR; //����
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
 * �������ƣ� GetMinData
 * �������ܣ����ɷ�������
 *
 * ��ڲ�����    char *Header_packet   �������һƬ�ڴ��׵�ַ
            sensors_data_t *sensors_tempdata ��Ҫ�洢�Ľṹ��ָ��
            unsigned char num_mins  ����ѡ�� 0--��ʷ�������� (DOWN)
                                            1--�������ݴ洢 �����Է������ݴ洢�������ã�
                                            2--��ǰ��ʷ���ݶ�ȡ��READDATA��
                                            3--Ԥ��  �Զ�����
 * ���ڲ�����    ��
 * ��дʱ�䣺
 * �޸�ʱ�䣺
==================================================================*/
int GetMinData(char *Header_packet,sensors_data_t *sensors_tempdata,unsigned char num_mins)
{
  unsigned int lenlen = 0;
  UINT32 temp_time = 0;
  unsigned short i;
  s_RtcTateTime_t temp_time_struct;//ʱ��ṹ��
  uint16 Checksum = 0;
  bcm_info_t info;
  FLASH_Store_MinData_t Mindata_temp;//����һ����ʱ�����ݽṹ��  sensors_data_t
  float standardDeviation_1 = 0;
  
  memcpy(&info, &bcm_info,sizeof(bcm_info_t));
  
  strcpy(Header_packet,"BG,");//��ͷ
  
  S32ToStrAlignRight(info.common.version, 3, Header_packet+3);//�汾��
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

  S32ToStrAlignRight(info.common.qz, 5, Header_packet+lenlen);//�豸��վ��
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

  S32ToStrAlignRight(info.common.Lat.degree, 2, Header_packet+lenlen);//�豸γ��
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(info.common.Lat.min, 2, Header_packet+lenlen);//�豸γ��
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(info.common.Lat.sec, 2, Header_packet+lenlen);//�豸γ��
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

  S32ToStrAlignRight(info.common.Long.degree, 3, Header_packet+lenlen);//�豸����
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(info.common.Long.min, 2, Header_packet+lenlen);//�豸����
  lenlen=strlen((char *)Header_packet);
  S32ToStrAlignRight(info.common.Long.sec, 2, Header_packet+lenlen);//�豸����
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;
  
  S32ToStrAlignRight(info.common.High, 5, Header_packet+lenlen);//�豸����
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

  S32ToStrAlignRight(info.common.st, 2, Header_packet+lenlen);//�豸��������
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;
  
  strncpy(Header_packet+lenlen,(char const *)sensor_di,4);//�豸��ʶ
  lenlen= lenlen +4; 
  Header_packet[lenlen]=',';
  lenlen++;
  
  S32ToStrAlignRight(m_defdata.m_baseinfo.id, 3, Header_packet+lenlen);//�豸ID
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;    
  
  if(num_mins == 1)//��Ҫ�洢�������ݣ����Է���������������
  {
      Convert_Time_to_min(&temp_time);
      temp_time = temp_time;    //ǰһ�������ݣ���ǰ����ʱ��ǩ
      Convert_Sec_to_Time(&temp_time_struct, temp_time*60);
  }
  else if(num_mins == 0)//����ʷ���� DOWN
  {
      Convert_Sec_to_Time(&temp_time_struct,m_tempdata.time_start*60);//����ʷ����
  }
  else if(num_mins == 2)//����ʷ���� READDATA
  {
      Convert_Sec_to_Time(&temp_time_struct,m_tempdata.time_start_read*60);//����ʷ����
  }
  else if(num_mins == 3)//�Զ�������ʷ����
  {

  }
  
  S32ToStrAlignRight(temp_time_struct.year,4, Header_packet+lenlen);//ʱ��
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
  S32ToStrAlignRight(info.common.fi, 3, Header_packet+lenlen);//֡��ʶ
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

  if(Flag_nodata ==0)  //flash��������  ִ�д˶�
  { 
  S32ToStrAlignRight(info.sensor.data_num, 3, Header_packet+lenlen);//�۲�Ҫ������
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

  //---------------------------state information---------------------------
  if(num_mins == 1)//��Ҫ�洢�������ݣ�������������
  { //���ݱ�����flash�ṹ��   �洢ʱ���ݾ��Ŵ�100��ȡ��
      FLASH_Store_MinData.boardsvolt = sensors_tempdata->boardsvolt_data;
      FLASH_Store_MinData.boardstemp = sensors_tempdata->boardstemp_data;
      Mindata_temp.boardsvolt = sensors_tempdata->boardsvolt_data;
      Mindata_temp.boardstemp = sensors_tempdata->boardstemp_data;

      memcpy(&FLASH_Store_MinData.state, &sensor_state, sizeof(sensor_state_t));
      memcpy(&Mindata_temp.state, &sensor_state, sizeof(sensor_state_t));

      FLASH_Store_MinData.flag = 1;   //flash�洢��־
  }
  else if((num_mins == 0)||(num_mins == 2))//����Ҫ�洢�ķ������ݣ�������ʷ���ݶ�ȡ����
  {
      Mindata_temp.boardsvolt = FLASH_Read_MinData.boardsvolt;
      Mindata_temp.boardstemp = FLASH_Read_MinData.boardstemp;

      memcpy(&Mindata_temp.state, &FLASH_Read_MinData.state, sizeof(sensor_state_t));
  }

  S32ToStrAlignRight(Mindata_temp.state.state_num, 2, Header_packet+lenlen);//�豸״̬������
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;

//---------------------------head  finish--------------------------

    //---------------------------data  start---------------------------

    //-AFA--1.5�׸�˲ʱ����(3��ƽ��)    fengsu_3s-------------3sƽ����������
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengsu_3s, &FLASH_Read_MinData.sensor[0].fengsu_3s, \
                    &sensors_tempdata->sensor[0].fengsu_3s, &Mindata_temp.sensor[0].fengsu_3s, num_mins, 1);
    
    //-AFB150	1.5�׸�һ����ƽ������     fengsu_1min_avg-------1minƽ����������
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengsu_1min_avg, &FLASH_Read_MinData.sensor[0].fengsu_1min_avg, \
                    &sensors_tempdata->sensor[0].fengsu_1min_avg, &Mindata_temp.sensor[0].fengsu_1min_avg, num_mins, 1);
    
    //-AFC150	1.5�׸߶�����ƽ������   fengsu_2min_avg---fengsu_2min_avgƽ����������
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengsu_2min_avg, &FLASH_Read_MinData.sensor[0].fengsu_2min_avg, \
                    &sensors_tempdata->sensor[0].fengsu_2min_avg, &Mindata_temp.sensor[0].fengsu_2min_avg, num_mins, 1);
      
    //----AFD150	1.5�׸�ʮ����ƽ������   fengsu_10min_avg--------fengsu_10min_avgƽ����������
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengsu_10min_avg, &FLASH_Read_MinData.sensor[0].fengsu_10min_avg, \
                    &sensors_tempdata->sensor[0].fengsu_10min_avg, &Mindata_temp.sensor[0].fengsu_10min_avg, num_mins, 1);
 
    //-----AFA150a	1.5�׸߷����ڼ������-----------fengsu_maxƽ����������
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengsu_max, &FLASH_Read_MinData.sensor[0].fengsu_max, \
                    &sensors_tempdata->sensor[0].fengsu_max, &Mindata_temp.sensor[0].fengsu_max, num_mins, 1);
    
    //---AEA150	1.5�׸�˲ʱ����1s������------fengxiang_MomentaryValueƽ����������
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengxiang_1min, &FLASH_Read_MinData.sensor[0].fengxiang_1min, \
                    &sensors_tempdata->sensor[0].fengxiang_1min, &Mindata_temp.sensor[0].fengxiang_1min, num_mins, 0);

    //----AEB150	1.5�׸�һ����ƽ������ -----------fengxiang_1min_avgƽ����������
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengxiang_1min_avg, &FLASH_Read_MinData.sensor[0].fengxiang_1min_avg, \
                    &sensors_tempdata->sensor[0].fengxiang_1min_avg, &Mindata_temp.sensor[0].fengxiang_1min_avg, num_mins, 0);
      
    //-AEC150	1.5�׸߶�����ƽ������---------------fengxiang_2min_avgƽ����������
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengxiang_2min_avg, &FLASH_Read_MinData.sensor[0].fengxiang_2min_avg, \
                    &sensors_tempdata->sensor[0].fengxiang_2min_avg, &Mindata_temp.sensor[0].fengxiang_2min_avg, num_mins, 0);
      
    //-AED150	1.5�׸�ʮ����ƽ������------------fengxiang_10min_avgƽ����������
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengxiang_10min_avg, &FLASH_Read_MinData.sensor[0].fengxiang_10min_avg, \
                    &sensors_tempdata->sensor[0].fengxiang_10min_avg, &Mindata_temp.sensor[0].fengxiang_10min_avg, num_mins, 0);
      
    //-AEF150	1.5�׸߷����ڼ�����٣�˲ʱ���٣���Ӧ����   fengxiang_max
      Data_Copy(&FLASH_Store_MinData.sensor[0].fengxiang_max, &FLASH_Read_MinData.sensor[0].fengxiang_max, \
                    &sensors_tempdata->sensor[0].fengxiang_max, &Mindata_temp.sensor[0].fengxiang_max, num_mins, 0);


    //-------------------����---------------------------------
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

    //---------------------------AEBl150   1���ӷ�����ӱ�׼�� -------------------------
    strcpy(Header_packet+lenlen,"AEBl");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;

    if(num_mins == 1)//��Ҫ�洢�������ݣ�������������
    { //���ݱ�����flash�ṹ��     
      standardDeviation_1 = StandardDeviation(data_sample_buf_history + 240, (sample_qc_history + 30), 60);
      if(INVALID_DATA != Mindata_temp.sensor[0].fengxiang_1min_avg.data)
          Mindata_temp.Standard_Deviation[0] = standardDeviation_1 * 100;
      else
          Mindata_temp.Standard_Deviation[0] = INVALID_DATA;
      
      FLASH_Store_MinData.Standard_Deviation[0] = Mindata_temp.Standard_Deviation[0];
    }
    else if((num_mins == 0)||(num_mins == 2))//����Ҫ�洢�ķ������ݣ�������ʷ���ݶ�ȡ����
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

    //-------------------����---------------------------------
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
    //---------------------------AFAl150  1 ���ӷ��ٷ��ӱ�׼�� -------------------------
    strcpy(Header_packet+lenlen,"AFBl");
    lenlen=strlen((char *)Header_packet);
    strcpy(Header_packet+lenlen,str_name);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    
    if(num_mins == 1)//��Ҫ�洢�������ݣ�������������
    { //���ݱ�����flash�ṹ��
      
      standardDeviation_1 = StandardDeviation(data_sample_buf_history, sample_qc_history, 240);
      if(INVALID_DATA != Mindata_temp.sensor[0].fengsu_1min_avg.data)
          Mindata_temp.Standard_Deviation[1] = standardDeviation_1 * 100;
      else
          Mindata_temp.Standard_Deviation[1] = INVALID_DATA;
      
      FLASH_Store_MinData.Standard_Deviation[1] = Mindata_temp.Standard_Deviation[1];
    }
    else if((num_mins == 0)||(num_mins == 2))//����Ҫ�洢�ķ������ݣ�������ʷ���ݶ�ȡ����
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

    //״̬��Ϣ
    strcpy(Header_packet+lenlen,"z,");  //�Լ�״̬
    lenlen+=2;
    S32ToStrAlignRight(Mindata_temp.state.self_test, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;

    if(0 != Mindata_temp.state.board_temp)
    {
        strcpy(Header_packet+lenlen,"wA");  //����
        lenlen=strlen((char *)Header_packet);
        Header_packet[lenlen]=',';
        lenlen++;
        S32ToStrAlignRight(Mindata_temp.state.board_temp, 1, Header_packet+lenlen);
        lenlen=strlen((char *)Header_packet);
        Header_packet[lenlen]=',';
        lenlen++;
    }

    strcpy(Header_packet+lenlen,"xA");  //��ӵ�Դ
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    S32ToStrAlignRight(Mindata_temp.state.exter_power, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;

    if(0 != Mindata_temp.state.board_volt)
    {
        strcpy(Header_packet+lenlen,"xB");  //��ѹ
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
        strcpy(Header_packet+lenlen,"xH");  //��������״̬
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
        strcpy(Header_packet+lenlen,"xJ");  //��ӵ�Դ״̬
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
        strcpy(Header_packet+lenlen,"xD");  //���ص�ѹ״̬
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
  
  S32ToStrAlignRight(Checksum, 4, Header_packet+lenlen);//�۲�Ҫ������
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]=',';
  lenlen++;
  //---------------------------packet end---------------------------
  strcpy(Header_packet+lenlen,"ED\r\n");
  
  lenlen=strlen((char *)Header_packet);
  Header_packet[lenlen]= '\0';

  if(num_mins == 1)//��Ҫ�洢�������ݣ�������������
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
  s_RtcTateTime_t temp_time_struct;//ʱ��ṹ��
  
  bcm_info_t info;

  memcpy(&info, &bcm_info,sizeof(bcm_info_t));
  
  memset(Header_packet,0,40);
  
  //---------------------��ͷ-------------------
  strcpy(Header_packet,"BG,");//��ͷ
  uartSendStr(0,(unsigned char *)Header_packet,0);
  
  uartSendStr(0,(unsigned char *)sensor_di,4);
  
  uartSendStr(0,",",0);
  
  S32ToStrAlignRight(m_defdata.m_baseinfo.id, 3, Header_packet);//�豸ID
  Header_packet[3]=',';
  lenlen = 4;
  
  temp_time_struct = m_tempdata.m_RtcTateTime;
  
  temp_time_struct.sec = 0;//����Ϊ0
  
  S32ToStrAlignRight(temp_time_struct.year,4, Header_packet+lenlen);//ʱ��
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
  //---------------------��������-------------------
  //  FLASH_Store_MinData_t Mindata_temp;//����һ����ʱ�����ݽṹ��  sensors_data_t
  if(info.sensor.ce == 1)//����Ϊ1.5m�ķ�
  {
    if(ch == 1)//����
    {
      strcpy(Header_packet,"AEA150");
    }
    else if(ch == 2)//����
    {
      strcpy(Header_packet,"AFA150");
    }
    Header_packet[6]=',';
    lenlen = 7; 
  }
  else if(info.sensor.ce == WIND_10)//����Ϊ10m�ķ�
  {
    if(ch == 1)//����
    {
      strcpy(Header_packet,"AEA");
    }
    else if(ch == 2)//����
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
  
  //ȡ���ݳ���
  int *temp_data_buf = (int *)Header_packet;
  char int8_1[8];
  if(ch == 1)//����
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
  else if(ch == 2)//����
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
  
  //---------------------����-------------------
  strcpy(Header_packet,"ED\r\n");  
  //  lenlen=strlen((char *)Header_packet);
  uartSendStr(0,(unsigned char *)Header_packet,0);
}

