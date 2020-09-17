
#ifndef _WIND_SENSOR_H
#define _WIND_SENSOR_H

#include <time.h>
#include "sysinit.h"

#define myApp_TIMER1_PERIOD_MS            500 //��2����ʱ������1����ʱ����ʱ���
#define myApp_TIMER2_PERIOD_MS            50  //��2����ʱ������2����ʱ����ʱ���

//�����¶ȣ����£�����ˮ��֮����뽫�˴��޸�
#define MAX_SENSOR_NUM                    8 //���崫��������������
#define SAMPLE_COUNT                      30//��ͨ������ѭ����
//#define SAMPLE_DATA_NUM                   SAMPLE_COUNT * MAX_SENSOR_NUM   //5��ͨ����ÿ��ͨ��һ������30����
#define SAMPLE_DATA_NUM 300   //����240��������60��
#define STANDARD_DEV_NUM                  2//��׼������
#define MAX_ELEMENT_NUM                   1

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
    //*****************************���·���

    qcdata_t fengsu_MomentaryValue;			      //����˲ʱֵ����
    qcdata_t fengsusecdatalast3s[12];	      //��ȥ3s��12����������
    qcdata_t fengsu_3s;			      //˲ʱ����(3��ƽ��)��������������
	
    qcdata_t fensu_2min_3s;			      //������   ���2min����ǰ��һ��3sƽ��ֵ
    qcdata_t fensu_1min_3s;			      //�������� ���1min����ǰ��һ��3sƽ��ֵ
    
    qcdata_t avg1minsum;		      //�������ۼ�   ��1min��ƽ��ֵ�ۼ�
    unsigned char fengsu_min1_counter;	      //�������ݸ��� 1min����
    qcdata_t fengsu_1min_avg;			      //һ����ƽ�����١���������	
    
	qcdata_t avg2minsum;		      //���������ۼ�  ��2min��ƽ��ֵ�ۼ�
    unsigned char fengsu_min2_counter;            //�����ݸ���  2min����
    qcdata_t fengsu_2min_avg;			      //������ƽ�����١���������

    qcdata_t fengsumindatalast9min[10];	      //��ȥ9���ӵķ�������
    qcdata_t fengsu_10min_avg;			      //ʮ����ƽ������~~~~~~
	
    qcdata_t fengsu_max;			      //�����ڼ������~~~~

//*****************************���·���
    qcdata_t fengxiang_MomentaryValue;			      //����˲ʱֵ����˲ʱ����1s������
    qcdata_t fengxiang_1min;                  //һ��������˲ʱ����
    qcdata_t fengxiang_1min_avg;			      //һ����ƽ������
    qcdata_t fengxiang_2min_avg;			      //������ƽ������
    qcdata_t fengxiang_10min_avg;			      //ʮ����ƽ������
    qcdata_t fengxiang_max;			      //�����ڼ�����٣�˲ʱ���٣���Ӧ����
    qcdata_t fengxiangmindatalast9min_x[10];	      //��ȥ10���ӵ�x����ƽ��ֵ���� 
    qcdata_t fengxiangmindatalast9min_y[10];	      //��ȥ10���ӵ�y����ƽ��ֵ���� 
	
    qcdata_t avg2m_x;			      	//2min��xƽ��ֵ
    qcdata_t avg2m_y;	      			//2min��yƽ��ֵ
    qcdata_t avg2m_x_sum;			      	//2min��xƽ��ֵ�ۼ�
    qcdata_t avg2m_y_sum;	      			//2min��yƽ��ֵ�ۼ�
    unsigned char fengxiang_min2_counter;            //�����ݸ���  2min����
	
    qcdata_t avg1m_x;			      	//1min��xƽ��ֵ
    qcdata_t avg1m_y;	     			//1min��yƽ��ֵ
    qcdata_t avg1m_x_sum;			      	//1min��xƽ��ֵ�ۼ�
    qcdata_t avg1m_y_sum;	     			//1min��yƽ��ֵ�ۼ�
    unsigned char fengxiang_min1_counter;	      //�������ݸ��� 1min����

    struct {
        unsigned char hourdata_counter;   //Сʱ���ݸ����ۼ�
        unsigned char num;
        float hour_data[61];
        unsigned char hour_data_flag;
    }changerate[4];

  }sensor[1];
    qc_ctrl_t qualityCtrl[MAX_SENSOR_NUM+1];//���ڷ��������Ҫ������ͬ����������
   
  short boardstemp_data,boardsvolt_data;
    
  uint8  sendpacket[MAX_PKG_LEN];
  uint16 sendpacketlen;
  uint8  historyPacket[MAX_PKG_LEN];
  uint16 historyPacketLen;
}sensors_data_t;


void Init_sys_cfg(void);

#endif
