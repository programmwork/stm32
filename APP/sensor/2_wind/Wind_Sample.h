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
;* �ļ����� : ������
;* �������� : 2015-5-8
;* �汾���� :
;************************************************************************************************************
*/
#ifndef __WIND_SAMPLING_H_
#define	__WIND_SAMPLING_H_

#include "Wind_Sensor.h"

#define WIND_1_5  0x01
#define WIND_10   0x03

typedef struct _qcflashdata{ 
  float data;
  unsigned char qc;
}qcfalshdata_t;

struct flashdata{
  
  qcfalshdata_t fengsu_3s;//AFA150 3sƽ����������  
  qcfalshdata_t fengsu_1min_avg;//AFB150 1����ƽ����������  
  qcfalshdata_t fengsu_2min_avg;//AFC150 2����ƽ����������  
  qcfalshdata_t fengsu_10min_avg;//AFD150 10����ƽ����������  
  qcfalshdata_t fengsu_max;//AFA150a  ������ٷ�������  
  
  
  qcfalshdata_t fengxiang_1min;			      //����˲ʱֵ����˲ʱ����1s������
  qcfalshdata_t fengxiang_1min_avg;			      //һ����ƽ������
  qcfalshdata_t fengxiang_2min_avg;			      //������ƽ������
  qcfalshdata_t fengxiang_10min_avg;			      //ʮ����ƽ������
  qcfalshdata_t fengxiang_max;			      //�����ڼ�����٣�˲ʱ���٣���Ӧ����
};

typedef struct 
{
  unsigned char flag;
  U32 time;
  struct flashdata sensor[1];//�洢λ���������¶ȣ���֧������5���ӵ�ƽ��ֵ������ֻ��5��һ���ӵ�ƽ��ֵ
  short boardstemp;
  short boardsvolt;
  long Standard_Deviation[STANDARD_DEV_NUM];
  sensor_state_t state;
}FLASH_Store_MinData_t;

//--------------------�������� ---------------------------
extern float data_sample_buf[SAMPLE_DATA_NUM];


//------------------��������----------------------------
extern int GetMinData(char *Header_packet,sensors_data_t *sensors_tempdata,unsigned char num_mins);

extern unsigned char Element_MsSample(sensors_data_t *sensors_tempdata);
extern unsigned char Element_SecSample(sensors_data_t *sensors_tempdata);
extern unsigned char Element_MinSample(sensors_data_t *sensors_tempdata);
extern unsigned char Quality_Sec_Ctrl(sensors_data_t *sensors_tempdata,unsigned char ch);
extern unsigned char Quality_Min_Ctrl(sensors_data_t *sensors_tempdata,unsigned char mins);

extern void SampleData_Init(sensors_data_t *sensors_tempdata);
extern void Quality_Init(sensors_data_t *sensors_tempdata);
//unsigned char iQCPS_Code(int sensorNo,float value,qm_t *qcctrl);
unsigned char iQCPM_Code(int sensorNo,float value,qm_t *qcctrl);
unsigned char iQCPS_Code_ws250(int sensorNo,float value,qs_t *qcctrl);
unsigned char iQCPS_Code_ws3s(int sensorNo,float value,qs_t *qcctrl);
unsigned char iQCPS_Code_wd(int sensorNo,float value,qs_t *qcctrl);

static void WindDirectCal(float *winddir_x,float *winddir_y,float *winddir_d);

void GetSampleData(sensors_data_t *sensors_tempdata,float *dat,uint8 ch);

#endif
