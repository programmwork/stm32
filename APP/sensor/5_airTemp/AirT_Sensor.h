#ifndef _AIRT_SENSOR_H_
#define _AIRT_SENSOR_H_

#include <time.h>
#include "sysinit.h"

#define myApp_TIMER1_PERIOD_MS            500 //��2����ʱ������1����ʱ����ʱ���
#define myApp_TIMER2_PERIOD_MS            50  //��2����ʱ������2����ʱ����ʱ���

//�����¶ȣ����£�����ˮ��֮����뽫�˴��޸�
#define MAX_SENSOR_NUM                    1 //���崫��������������
#define SAMPLE_COUNT                      30//��ͨ������ѭ����
#define SAMPLE_DATA_NUM                   SAMPLE_COUNT * MAX_SENSOR_NUM   //5��ͨ����ÿ��ͨ��һ������30����
#define STANDARD_DEV_NUM                  2//��׼������
#define MAX_ELEMENT_NUM                   2

#define INVALID_DATA                      -999//������Ч����ֵ

//����������
enum QC_CODE_LIST{
         QC_OK=0,           //��ȷ
         QC_DOUBT=1,        //  ����
         QC_ERROR=2,        //   ����
         QC_NOEXAMINE=3,    // ����
         QC_MODIFY=4 ,      // �޸�
         QC_NOSAME=5,       // ��һ��
         QC_RESERVED,       // Ԥ��
         QC_NOMEASURE=7,   // ���۲�
         QC_MISSED=8,       //   ȱ��
         QC_NOCONTROL=9     //δ����������
};


#define QC_CODE_LIST unsigned char
typedef struct _qcdata{ 
  float data;
  unsigned char qc;
}qcdata_t;

typedef struct
{
  struct{
    float Vmin;
    float Vmax;
    float Rmax;
  }secdata_ctrl_t;
  
  struct{
    float Vmin;
    float Vmax;
  }mindata_ctrl_t; 
}qc_ctrl_t;

#define MAX_PKG_LEN       512//187�ֽ�
#define MAXSENDBUFFLEN    512

typedef struct{
  struct {
    qcdata_t secdata;			      //������   ���2min����ǰ��һ��3sƽ��ֵ
    qcdata_t mindata;			      //�������� ���1min����ǰ��һ��3sƽ��ֵ
    
    qcdata_t secdata_add;		      //�������ۼ�   ��1min��ƽ��ֵ�ۼ�
    unsigned char secdata_counter;    //�����ݸ���  2min����
    qcdata_t mindata_add;		      //���������ۼ�  ��2min��ƽ��ֵ�ۼ�
    unsigned char mindata_counter;	  //�������ݸ��� 1min����

    unsigned char hourdata_counter;   //Сʱ���ݸ����ۼ�
    unsigned char num;
    float hour_data[61];
    unsigned char hour_data_flag;  
    //unsigned char mindata5_counter;   //5min���ݸ����ۼ�
    //qcdata_t mindata5[10];
    //float mindata5_add;
  }sensor[MAX_ELEMENT_NUM];
  qc_ctrl_t qualityCtrl[MAX_ELEMENT_NUM];
   
  short boardstemp_data,boardsvolt_data;
    
  uint8  sendpacket[MAX_PKG_LEN];
  uint16 sendpacketlen;
  uint8  historyPacket[MAX_PKG_LEN];
  uint16 historyPacketLen;
}sensors_data_t;

void check_event_sample(void);
void check_event_calc(void);
uint8 startupPrint(void);
void Init_sys_cfg();

#endif
