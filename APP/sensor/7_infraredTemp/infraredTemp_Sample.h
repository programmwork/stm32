/*
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
;************************************************************************************************************
*/
#ifndef __EARTH_TEMP_SAMPLING_H_
#define	__EARTH_TEMP_SAMPLING_H_

#include "EarthTemp_Sensor.h"

typedef struct _qcflashdata{ 
  float data;
  unsigned char qc;
}qcfalshdata_t;

struct flashdata{
  qcfalshdata_t mindata;//��������  
};

typedef struct 
{
  unsigned char flag;
  U32 time;
  struct flashdata sensor[MAX_SENSOR_NUM];//�洢λ���������¶ȣ���֧������5���ӵ�ƽ��ֵ������ֻ��5��һ���ӵ�ƽ��ֵ
  short boardstemp;
  short boardsvolt;
  long Standard_Deviation[STANDARD_DEV_NUM];
  sensor_state_t state;
}FLASH_Store_MinData_t;

//--------------------�������� ---------------------------
extern unsigned char Num_sample;
extern unsigned char sec_flag;
extern float data_sample_buf_history[SAMPLE_DATA_NUM]; //�������ݱ��滺����

//------------------��������----------------------------
extern int GetMinData(char *Header_packet,sensors_data_t *sensors_tempdata,unsigned char num_mins);

extern unsigned char Element_MsSample(sensors_data_t *sensors_tempdata);
extern unsigned char Element_SecSample(sensors_data_t *sensors_tempdata);
extern unsigned char Element_MinSample(sensors_data_t *sensors_tempdata);

static unsigned char Quality_Sec_Ctrl(sensors_data_t *sensors_tempdata);
static unsigned char Quality_Min_Ctrl(sensors_data_t *sensors_tempdata,unsigned char ch);

extern void SampleData_Init(sensors_data_t *sensors_tempdata);
extern void Quality_Init(sensors_data_t *sensors_tempdata);

void GetSampleData(sensors_data_t *sensors_tempdata,float *dat,uint8 ch);

unsigned char iQCPS_Code(int sensorNo,float value,qs_t *qcctrl);    //�������������ƺ���
unsigned char iQCPM_Code(int sensorNo,float value,qm_t *qcctrl);    //���������������ƺ���
#endif
