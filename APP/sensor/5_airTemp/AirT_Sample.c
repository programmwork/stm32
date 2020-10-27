/*************************************************************************************************************
;*--------------------------------------------- �ļ���Ϣ ----------------------------------------------------
;* �ļ����� : 
;* �ļ����� : 
;* ����˵�� :
;*-------------------------------------------- ���°汾��Ϣ -------------------------------------------------
;* �޸����� :
;* �޸����� :
;* �汾���� :
;*-------------------------------------------- ��ʷ�汾��Ϣ -------------------------------------------------
;* �ļ����� :
;* �������� :
;* �汾���� :
;*************************************************************************************************************/
#include "sensor_basic.h"

float data_sample_buf[SAMPLE_DATA_NUM];//�������ݱ��滺����
float data_sample_buf_history[SAMPLE_DATA_NUM];//�������ݱ��滺����
unsigned char sample_qc[MAX_SENSOR_NUM][4];		//�ڶ���ֵ �Ͳ���ֵ�й�
unsigned char sample_qc_history[MAX_SENSOR_NUM][4];

float data_sample_buf_5[5 * SAMPLE_DATA_NUM];//5min�������ݱ��滺����
float data_sample_buf_history_5[5 * SAMPLE_DATA_NUM];//5min�������ݱ��滺����
unsigned char sample_qc_5[MAX_SENSOR_NUM][19];      //5min�ڶ���ֵ �Ͳ���ֵ�й�
unsigned char sample_qc_history_5[MAX_SENSOR_NUM][19];

unsigned char Num_sample = 0;
unsigned char Num_sample_5 = 0;

unsigned char sec_flag = 0;

unsigned char temp_sample_event = 0;
float temp_value[MAX_SENSOR_NUM] = {0};

float         LastValue_s   [MAX_ELEMENT_NUM];
unsigned char beenRun_s     [MAX_ELEMENT_NUM];
float         LastValue     [MAX_ELEMENT_NUM];
unsigned char beenRun       [MAX_ELEMENT_NUM];
float         LastValue_H   [MAX_ELEMENT_NUM];
unsigned char beenRun_H     [MAX_ELEMENT_NUM];

/*=============================================================================================
*�������ܣ����뼶��ʱ�ɼ� 
*��ڲ������������ݽṹ��
*���ڲ������������ݽṹ��
*������  ��sapi.c�еĶ�ʱ����������   ÿ�����һ�ε���
*�������ã�Element_SecSample(&sensors_data)
==============================================================================================*/
unsigned char Element_MsSample(sensors_data_t *sensors_tempdata)
{
  return 1;
}
/*============================================================================================
*�������ܣ���ʱ�ɼ�
*��ڲ������������ݽṹ��
*���ڲ������������ݽṹ��
*������  ��sapi.c�еĶ�ʱ����������   ÿ�����һ�ε���
*�������ã�Element_SecSample(&sensors_data)
============================================================================================*/
unsigned char Element_SecSample(sensors_data_t *sensors_tempdata)
{
    unsigned char i = 0;

  if(Num_sample<SAMPLE_COUNT)
  {
    if(temp_sample_event == 0)
    {
        temp_sample_event= AirTemp_engine(temp_value);//�������������
    }
    
    if((temp_sample_event == 1) && (sec_flag == 1))//û�вɼ����
    {
        temp_sample_event = 0;
        sec_flag = 0;

        //�ɼ����
      if(Flag_DataEmulationMode == 1)//��X1Ϊ1ʱ�������������ģ������
      {
        if((EmulationSensorChannel >=1)&&(EmulationSensorChannel<=MAX_SENSOR_NUM))
        {
          if(EmulationDataType == 0)
            temp_value[EmulationSensorChannel-1] = (float)((float)EmulationData_1/10);
          else
            temp_value[EmulationSensorChannel-1] = (float)((float)EmulationData_1/1000);
        }
        else return 0;
      }

      for(i = 0;i<MAX_SENSOR_NUM;i++)
      {
        if(i == (EmulationSensorChannel-1))
        {
          if(EmulationDataType == 1)//������ģ��ֵ��ʹ��ϵ��
          {
           // sensors_tempdata->sensor[i].secdata.data = \
           //   (float)(((-PARA_A+sqrt(PARA_A*PARA_A-4*PARA_B*(1-(temp_value[i])/100)))/(2*PARA_B)));
          }
          else if(EmulationDataType == 0)//��������ģ��ֵ��ʹ��ϵ��)
          {
            sensors_tempdata->sensor[i].secdata.data = temp_value[i];
          }
        }
        else
        {
          sensors_tempdata->sensor[i].secdata.data = temp_value[i];
        }    
      }

      for(i = 0;i<MAX_SENSOR_NUM;i++)
      {
        //У������  CR
        sensors_tempdata->sensor[i].secdata.data =  bcm_info.sensor.cr[i].a * temp_value[i] * temp_value[i] +\
                                             bcm_info.sensor.cr[i].b * temp_value[i] + bcm_info.sensor.cr[i].c;
      }
      
      for(i = 0;i<MAX_SENSOR_NUM;i++)
      {
        data_sample_buf[SAMPLE_COUNT*i+Num_sample] = sensors_tempdata->sensor[i].secdata.data;

        if(Num_sample_5 < 150)
        {
            data_sample_buf_5[SAMPLE_COUNT*i+Num_sample_5] = sensors_tempdata->sensor[i].secdata.data;
        }
      }
      Quality_Sec_Ctrl(sensors_tempdata);//��������
      
      for(i = 0;i<MAX_SENSOR_NUM;i++)
      {
        if(sensors_tempdata->sensor[i].secdata.qc == QC_OK) //��ȷ��QCֵ��д1 ��bit����
        {
          sample_qc[i][(Num_sample)/8] |= (1<<((Num_sample)%8));

          sample_qc_5[i][Num_sample_5/8] |=(1<<((Num_sample_5)%8));
        }
      }
      Num_sample++;
      Num_sample_5++;

      if(m_tempdata.SecDataOut == true)//�뼶���� ���ݼ��ʿ��� ��ӡ
      {
        char temp[32];
        float temp_data;
        uartSendStr(UARTDEV_1,"<",1);
        for(i = 0;i<MAX_SENSOR_NUM;i++)
        {
            temp_data = sensors_tempdata->sensor[i].secdata.data;
            if((sensors_tempdata->sensor[i].secdata.qc > bcm_info.sensor.qs[i].max)\
                    || (sensors_tempdata->sensor[i].secdata.data < bcm_info.sensor.qs[i].min))
            {
                temp_data = INVALID_DATA;
            }
            sprintf(temp,"%.2f,%d  ", temp_data, sensors_tempdata->sensor[i].secdata.qc);
            uartSendStr(UARTDEV_1,(unsigned char *)temp,strlen(temp));
        }
        uartSendStr(UARTDEV_1,">\r\n",3);
        return 1;
      }
    }
  }
  return 0;
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
    uint16 i = 0;
  //�忨�Լ�
  Self_test();

	memcpy(data_sample_buf_history,data_sample_buf,sizeof(data_sample_buf));
	memset(data_sample_buf,0,sizeof(data_sample_buf));	//�������ݻ�������0
  
	memcpy(sample_qc_history,sample_qc,sizeof(sample_qc));
	memset(sample_qc,0,sizeof(sample_qc));
	
	memcpy(data_sample_buf_history_5,data_sample_buf_5,sizeof(data_sample_buf_5));
	memcpy(sample_qc_history_5,sample_qc_5,sizeof(sample_qc_5));

	for(i = 0;i < 150;i++)
	{
	    if(CHECK_QC_IS_OK(sample_qc_5[0], i))
	    {
            sensors_tempdata->sensor[1].secdata_add.data += data_sample_buf_5[i];
            sensors_tempdata->sensor[1].secdata_counter++;
	    }

	}

	if(Num_sample_5 <= 120)
	{
	    memset(&data_sample_buf_5[Num_sample_5],0,sizeof(float) * 30);	//�������ݻ�������0

	    for(i = Num_sample_5;i < (Num_sample_5 + 30);i++)
        {
            sample_qc_5[0][i/8] &= ~(1<<(i%8));
        }
	}
	else
	{
	    memset(data_sample_buf_5, 0, sizeof(float) * 30);
        for(i = 0;i < 30;i++)
        {
            sample_qc_5[0][i/8] &= ~(1<<(i%8));
        }
    }

	for(i = 0;i<MAX_SENSOR_NUM;i++)
	{
        if(sensors_tempdata->sensor[i].secdata_counter< (2*SAMPLE_COUNT/3))//2*60/3
        {//ȱ��
          sensors_tempdata->sensor[i].mindata.data=INVALID_DATA;
          sensors_tempdata->sensor[i].mindata.qc=QC_MISSED;
        }
        else
        {//��ȷ
          sensors_tempdata->sensor[i].mindata.data= sensors_tempdata->sensor[i].secdata_add.data \
                                                  / sensors_tempdata->sensor[i].secdata_counter;
          Quality_Min_Ctrl(sensors_tempdata,i);
        }

        //5����ƽ��ֵ
        if(sensors_tempdata->sensor[i + MAX_SENSOR_NUM].secdata_counter < (2*SAMPLE_COUNT*5 / 3))
        {
            sensors_tempdata->sensor[i + MAX_SENSOR_NUM].mindata.data = INVALID_DATA;
            sensors_tempdata->sensor[i + MAX_SENSOR_NUM].mindata.qc = QC_MISSED;
        }
        else
        {
            sensors_tempdata->sensor[i + MAX_SENSOR_NUM].mindata.data = \
                sensors_tempdata->sensor[i + MAX_SENSOR_NUM].secdata_add.data / sensors_tempdata->sensor[i + MAX_SENSOR_NUM].secdata_counter;
            sensors_tempdata->sensor[i + MAX_SENSOR_NUM].mindata.qc = QC_OK;
        }
	}

	Num_sample = 0;

	if(Num_sample_5 > 120)
	{
	    Num_sample_5 = 0;
	}

    for(i = 0;i<MAX_ELEMENT_NUM;i++)
    {//����������0
        sensors_tempdata->sensor[i].secdata_add.data = 0;
        sensors_tempdata->sensor[i].secdata_counter = 0;
    }

    return 1;
}

/*
*�������ܣ��뼶��������
*��ڲ������������ݽṹ��
*���ڲ���������������
*������  ���뼶��������
*�������ã�Quality_Sec_Ctrl(&sensors_data)
*/
static unsigned char Quality_Sec_Ctrl(sensors_data_t *sensors_tempdata)
{
    unsigned char i = 0;

  for(i = 0;i<MAX_SENSOR_NUM;i++)
  {
    float result_sensor = sensors_tempdata->sensor[i].secdata.data;
    //�������ж�
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
        sensors_tempdata->sensor[i].secdata_counter++;//���������ۼ�
        sensors_tempdata->sensor[i].secdata_add.data+=result_sensor;
        sensors_tempdata->sensor[i].secdata_add.qc=QC_OK; 
      }
    }
  }
  return 0;
}
/*
*�������ܣ����Ӽ���������
*��ڲ������������ݽṹ��
*���ڲ���������������
*������  �����Ӳ�������
*�������ã�Quality_Sec_Ctrl(&sensors_data,0)
*/
static unsigned char Quality_Min_Ctrl(sensors_data_t *sensors_tempdata,unsigned char ch)
{
  float result_sensor, buffer = 0, max = 0, min = 0;
  unsigned char i = 0, count = 0;

  //һ���ӵ���������
    result_sensor = sensors_tempdata->sensor[ch].mindata.data;
    //�������ж�
    if((result_sensor<bcm_info.sensor.qm[ch].min)||(result_sensor>bcm_info.sensor.qm[ch].max))
    {
      //sensors_tempdata->sensor[ch].mindata.data=INVALID_DATA;
      sensors_tempdata->sensor[ch].mindata.qc=QC_ERROR;
    }
    else
    {    
      sensors_tempdata->sensor[ch].mindata.qc=iQCPM_Code(ch,sensors_tempdata->sensor[ch].mindata.data,&bcm_info.sensor.qm[ch]);
    }

    //��СӦ�仯���
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
*�������ܣ��������ݳ�ʼ��
*��ڲ������������ݽṹ��
*���ڲ�������
*������  ��sapi.c�е�ϵͳ��ʼ������
*�������ã�SampleData_Init(&sensors_data)
*/
void SampleData_Init(sensors_data_t *sensors_tempdata)
{
    unsigned char i = 0;

	for(i = 0;i<MAX_ELEMENT_NUM;i++)
	{
		//������
		sensors_tempdata->sensor[i].secdata.data=INVALID_DATA;
		sensors_tempdata->sensor[i].secdata.qc=QC_MISSED;
		//�������ۼ�
		sensors_tempdata->sensor[i].secdata_add.data=0;
		sensors_tempdata->sensor[i].secdata_add.qc=QC_MISSED;
		//�����ݸ���
		sensors_tempdata->sensor[i].secdata_counter=0;
		//��������	
		sensors_tempdata->sensor[i].mindata.data=INVALID_DATA;	
		sensors_tempdata->sensor[i].mindata.qc=QC_MISSED;
		//��������	�ۼ�
		sensors_tempdata->sensor[i].mindata_add.data=0;	
		sensors_tempdata->sensor[i].mindata_add.qc=QC_MISSED;
		//��������	����
		sensors_tempdata->sensor[i].mindata_counter=0;	

        sensors_tempdata->sensor[i].hourdata_counter = 0;   //Сʱ���ݸ����ۼ�
        sensors_tempdata->sensor[i].num = 0;
        sensors_tempdata->sensor[i].hour_data_flag = 0;
	}

	memset(data_sample_buf,0,sizeof(data_sample_buf));//�������ݻ�������0
	memset(data_sample_buf_history,0,sizeof(data_sample_buf_history));//�������ݻ�������0
	memset(sample_qc,0,sizeof(sample_qc));//������־λ��������0
	memset(sample_qc_history,0,sizeof(sample_qc_history));//������־λ��������0
	
    memset(data_sample_buf_5,0,sizeof(data_sample_buf_5));//�������ݻ�������0
    memset(data_sample_buf_history_5,0,sizeof(data_sample_buf_history_5));//�������ݻ�������0
    memset(sample_qc_5,0,sizeof(sample_qc_5));//������־λ��������0
	memset(sample_qc_history_5,0,sizeof(sample_qc_history_5));//������־λ��������0
	
	//��ʼ�� QC���ݻ���
	memset(LastValue_s, 0, sizeof(LastValue_s));
	memset(beenRun_s,   0, MAX_ELEMENT_NUM);
	memset(LastValue,   0, sizeof(LastValue));
	memset(beenRun,     0, MAX_ELEMENT_NUM);
	memset(LastValue_H, 0, sizeof(LastValue_H));
	memset(beenRun_H,   0, MAX_ELEMENT_NUM);
}

/*================================================================================================
*�������ܣ��������������ƺ���
*��ڲ�����sensorNo--��֧��������value--��������ֵ��*qcctrl--qc��ʼ��������������
*���ڲ���������������������
*������  ��Quality_Sec_Ctrl����
*�������ã�iQCPS_Code(0,sensors_tempdata->sensor[0].secdata.data,&sensors_tempdata->qualityCtrl[0]);
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
*�������ܣ��������������ƺ���
*��ڲ�����sensorNo--��֧��������value--��������ֵ��*qcctrl--qc��ʼ��������������
*���ڲ���������������������
*������  ��Quality_Sec_Ctrl����
*�������ã�iQCPS_Code(0,sensors_tempdata->sensor[0].secdata.data,&sensors_tempdata->qualityCtrl[0]);
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
    //------------------------ 1MIN�������� -----------------------
     for(i = 0;i<MAX_ELEMENT_NUM;i++)
    {
      Data_Copy(&FLASH_Store_MinData.sensor[i].mindata, &FLASH_Read_MinData.sensor[i].mindata, \
                            &sensors_tempdata->sensor[i].mindata, &Mindata_temp.sensor[i].mindata, num_mins, 1);
    }

    if(num_mins == 1)//��Ҫ�洢�������ݣ�������������
    { //���ݱ�����flash�ṹ��   �洢ʱ���ݾ��Ŵ�100��ȡ��
        FLASH_Store_MinData.boardsvolt = sensors_tempdata->boardsvolt_data;
        FLASH_Store_MinData.boardstemp = sensors_tempdata->boardstemp_data;
        Mindata_temp.boardsvolt = sensors_tempdata->boardsvolt_data;
        Mindata_temp.boardstemp = sensors_tempdata->boardstemp_data;
        FLASH_Store_MinData.flag = 1;   //flash�洢��־
    }
    else if((num_mins == 0)||(num_mins == 2))//����Ҫ�洢�ķ������ݣ�������ʷ���ݶ�ȡ����
    {
        Mindata_temp.boardsvolt = FLASH_Read_MinData.boardsvolt;
        Mindata_temp.boardstemp = FLASH_Read_MinData.boardstemp;
    }

    //---------------------------AAA---------------------------��������
    strcpy(Header_packet+lenlen,"AAA");
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    //mindata
    Pack_Data(&Mindata_temp.sensor[0].mindata, &lenlen, Header_packet, 4);

    //=============��׼��===============================================
    strcpy(Header_packet+lenlen,"AAAl");
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    
    if(num_mins == 1)//��Ҫ�洢�������ݣ�������������
    { //���ݱ�����flash�ṹ��
      standardDeviation_1 = StandardDeviation(data_sample_buf_history, sample_qc_history[0], SAMPLE_COUNT);
      if(INVALID_DATA != Mindata_temp.sensor[0].mindata.data)
          Mindata_temp.Standard_Deviation[0] = standardDeviation_1 * 10000;
      else
          Mindata_temp.Standard_Deviation[0] = INVALID_DATA;
      
      FLASH_Store_MinData.Standard_Deviation[0] = Mindata_temp.Standard_Deviation[0];
    }
    else if((num_mins == 0)||(num_mins == 2))//����Ҫ�洢�ķ������ݣ�������ʷ���ݶ�ȡ����
    {
        if(INVALID_DATA != Mindata_temp.sensor[0].mindata.data)
            Mindata_temp.Standard_Deviation[0] = FLASH_Read_MinData.Standard_Deviation[0];
        else
            Mindata_temp.Standard_Deviation[0] = INVALID_DATA;
    }
    else ;
    
    Pack_Datal(&Mindata_temp.Standard_Deviation[0], &lenlen, Header_packet, 4);

    //---------------------------AAA5i---------------------------5����ƽ������
    strcpy(Header_packet+lenlen,"AAA5i");
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    //mindata
    Pack_Data(&Mindata_temp.sensor[1].mindata, &lenlen, Header_packet, 4);
	
    //=============================================================================
    strcpy(Header_packet+lenlen,"AAAl5i");
    lenlen=strlen((char *)Header_packet);
    Header_packet[lenlen]=',';
    lenlen++;
    
    if(num_mins == 1)//��Ҫ�洢�������ݣ�������������
    { //���ݱ�����flash�ṹ��
      standardDeviation_1 = StandardDeviation(data_sample_buf_history_5, sample_qc_history_5[0], 150);
      if(INVALID_DATA != Mindata_temp.sensor[1].mindata.data)
          Mindata_temp.Standard_Deviation[1] = standardDeviation_1 * 10000;
      else
          Mindata_temp.Standard_Deviation[1] = INVALID_DATA;
      
      FLASH_Store_MinData.Standard_Deviation[1] = Mindata_temp.Standard_Deviation[1];
    }
    else if((num_mins == 0)||(num_mins == 2))//����Ҫ�洢�ķ������ݣ�������ʷ���ݶ�ȡ����
    {
        if(INVALID_DATA != Mindata_temp.sensor[1].mindata.data)
            Mindata_temp.Standard_Deviation[1] = FLASH_Read_MinData.Standard_Deviation[1];
        else
            Mindata_temp.Standard_Deviation[1] = INVALID_DATA;
    }
    else ;

    Pack_Datal(&Mindata_temp.Standard_Deviation[1], &lenlen, Header_packet, 4);

    //AAA qc
    S32ToStrAlignRight(Mindata_temp.sensor[0].mindata.qc, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);
    //AAAl qc
    Header_packet[lenlen]=QC_NOCONTROL+0x30;
    lenlen++;
	//AAA5i qc
    S32ToStrAlignRight(Mindata_temp.sensor[1].mindata.qc, 1, Header_packet+lenlen);
    lenlen=strlen((char *)Header_packet);  
    //AAAl5i qc
    Header_packet[lenlen]=QC_NOCONTROL+0x30;
    lenlen++;     

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

  return lenlen++;
}

void GetSampleData(sensors_data_t *sensors_tempdata,float *dat,uint8 ch)
{
  char Header_packet[40];
  
  unsigned int lenlen = 0;
  unsigned char i = 0;
  s_RtcTateTime_t temp_time_struct;//ʱ��ṹ��
  
  bcm_info_t info;

  memcpy(&info, &bcm_info,sizeof(bcm_info_t));
  
  memset(Header_packet,0,40);
  
  //---------------------��ͷ-------------------
  strcpy(Header_packet,"BG,");//��ͷ
  uartSendStr(UARTDEV_1,(unsigned char *)Header_packet,0);
  
  uartSendStr(UARTDEV_1,(unsigned char *)sensor_di,4);
  
  uartSendStr(UARTDEV_1,",",0);
  
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
  uartSendStr(UARTDEV_1,(unsigned char *)Header_packet,0);
  
  memset(Header_packet,0,40);
  //---------------------��������-------------------
  
  uartSendStr(UARTDEV_1,(unsigned char *)Header_packet,0);
  
  
  memset(Header_packet,0,40);
  lenlen = 0;
  
  //ȡ���ݳ���
  char int8_1[8];
  for(i=0; i<SAMPLE_COUNT; i++)
  {
    S32ToStrAlignRight(data_sample_buf_history[(ch-1)*SAMPLE_COUNT+i], 4, int8_1);
    uartSendStr(UARTDEV_1,(unsigned char *)int8_1,0);
    uartSendStr(UARTDEV_1,",",0);
  }
  
  //---------------------����-------------------
  strcpy(Header_packet,"ED\r\n");  
  //  lenlen=strlen((char *)Header_packet);
  uartSendStr(UARTDEV_1,(unsigned char *)Header_packet,0);
}

