/*
 * defpam.h
 *
 *  Created on: 2016��3��7��
 *      Author: lenovo
 */

#ifndef  SOURCE_PRODUCT_DEFPAM_H_
#define  SOURCE_PRODUCT_DEFPAM_H_

#include "typedef.h"
#include "command.h"
//#include "ds3231.h"
//#include "w25x16.h"
//#include "adc12_a.h"
//#include "hal_AD7792.h"
//#include "uartprocess.h"

#pragma pack(1)//���ֽڶ���

#define TIMER_0_DELAY_MS 1 //��ʱ�� MS

#define MAX_PKGLEN 400				//���ݰ�����
#define MAX_UARTRCV_LEN 1024 //���ڽ��ճ���
#define UART_RCV_TIMEOUT_COUNTER 8 //���ճ�ʱ���ƣ���ʱʱ��=TIMER_0_DELAY_MS * UART_RCV_TIMEOUT_COUNTER
#define MAXN 16

/**************************************************/
/*���ڹ�����ʽ*/
enum{

        COMTYPE_RS232=0,
        COMTYPE_RS485,
        COMTYPE_RS422,
        MAX_UART_COMTYPE,
};
/*�������*/
enum {
        UARTDEV_1=0,
        UARTDEV_2,
        UARTDEV_3,
        //UARTDEV_4,
        MAX_UARTNUM,
};

/*�����豸����*/
enum{
        UART_DEVICE_DEBUG=0,    //����λ�����Կ�
        UART_DEVICE_SENSOR,     //SENSORģ��
        UART_DEVICE_LORA,       //LORA
	    UART_DEVICE_RS485,      //modbus �豸
	    MAX_UART_DEVICE_TYPE
};

typedef struct _uartinfo{
    U8 comworktype;//���ڹ������ͣ�232,485
    U8 uartdevtype;//�ҽ��豸����
}s_uartinfo_t;

typedef struct _rtcdatetime_t{
    U16 msec;
    U8	sec;
    U8	min;
    U8	hour;
    U8	day;
    U8	month;
    U16	year;
}s_RtcTateTime_t;


#define s_datetime_t s_RtcTateTime_t

typedef struct _baseinfo{
    unsigned char  sn[40];                 //���к� 36�ַ�
    unsigned short id;                     //�豸ID
    unsigned char  hard_version[20];       //Ӳ���汾��
}s_baseinfo_t;

#define MAX_SERVER_LINK_NUM 1
#define MAX_SERVER_SENDQUEUE_NUM 4 //server ���Ի��������
#define SAVEPAM_STARTFLAG_CHAR 0x55
#define SAVEPAM_ENDFLAG_CHAR 0xAA

//----------------------------------------------------------
//�ڲ��� flash 4�飬ÿ���СΪ 128byte
typedef struct {        //������Ϣ
    U8 save_start_flag;//�����־

    s_baseinfo_t m_baseinfo;
    s_uartinfo_t m_uartinfo[MAX_UARTNUM];

    U8 save_end_flag;//�����־
}s_defdata_t;
/*
typedef struct {        //У����Ϣ
   U8 save_start_flag;//�����־

   s_check_info_t m_check_info;

   U8 save_end_flag;//�����־
}s_check_t;

typedef struct {        //�춨��Ϣ
   U8 save_start_flag;//�����־

   s_check_info_t m_calib_info;

   U8 save_end_flag;//�����־
}s_calib_t;*/

//typedef struct {        //Ԥ��
//
//  }s_reserve_t;
//---------------------------------------------------------
typedef struct _uartrcv{
    U8 buff[MAX_UARTRCV_LEN+1];

    U16 WD;
    U16 RD;

    U8 (*process)(U8 uartno,U8 devtype);//�ص�����
}s_uartrcv_t;


 //����flash�ϣ��ж���Ƭsector ������
typedef struct _rcdFlash{
    U16 recordNum;//�ܹ��ж��������ݼ�¼
    struct{
        U16 sectorNo;
        U16 pageNo;
        U16 pieceNo;
        U16 recordnumOfSector;//��ǰsector�ж��ٸ�record
    }currentRcd;
}s_rcdFlash_t;

typedef struct _rcdHeadOne{
    //ʱ��������,��¼����ʱ����Ϣ
    U8 year;
    U8 mon;
    U8 day;
    U8 hour;
    U8 min;
    U8 sec;

    U16 msgflag;
    U8 msgno;
    U8 sendok;//�Ƿ��ͳɹ�
}s_rcdHeadOne_t;

typedef struct _rcdHeadOnePage{
	s_rcdHeadOne_t m_rcdHead_t[25];//һ��page ��25��head ��Ϣ
}s_rcdHeadOnePage_t;

typedef struct _rcdHeadOneSector{
	s_rcdHeadOnePage_t m_rcdHeadPage_t[16];//һ��sector ��16��page--400������
}s_rcdHeadOneSector_t;

#define MAX_RECORD_NUM 4096

typedef struct _tempdata
{
    s_RtcTateTime_t m_RtcTateTime;
    s_uartrcv_t m_uartrcv[MAX_UARTNUM];

    struct {
        U8 sample;
    }m_debuguint_t;

    struct{
        U8  flag;//�򿪴���͸����ʶ
        U8  mainuartno;//����͸���豸��
        U8  slaveuartno;//����͸���豸��
        U8  counter;//���main������ָ���ˣ����͸��60��
    }m_opencomset_t;

    struct{  /*�����־*/
        U8 msecevent;//1S
        U8 secevent;//1M
        U8 Flag_1s;
        U8 send_flag; //�Զ����ͳɹ���ʶ
        U8 minevent;//1M
        U8 hourevent;//1M
        U8 autosendevent;//�ɼ��¼�
        U8 minstorageevent;//�洢
        U8 hourstorageevent;//�洢
        U8 readdataevent;//��ʷ�����¼�  wjj 20150510
        U8 uart_config;
        U8 uart_config2;
        U8 uart_config_counter;
        U8 uart_config_counter2;
    }event;

    U8   currentCmdIndex;
    U8   currentUartIndex;  //0~3
    U8   reset;     //���ڼ��㿴�Ź���λʱ��
//----------------------------------------------------------
//down��ʷ����
    U16 Count_down_history;  //��ʷ��������  ���ܳ���60*12��
    U32 time_start;//��ʷ������ʼʱ��
    U16 time_interval;//��ȡ��ʷ���ݼ��
    //U8  Flag_Mindata_send;//�������ݷ��ͱ�־����af��ִ�����ж��Ƿ��Ƿ��ͷ�������ʧ��
//readdata ��ȡ��ǰ��ʷ����
    U8  Count_read_history;  //��ʷ��������  1��
    U32 time_start_read;//��ʷ������ʼʱ��
    U16 time_interval_read;//��ȡ��ʷ���ݼ��
//----------------------------------------------------------
    U8  SuperadMin;        // 20���ӵĲ���Ȩ������
    U8  SuperadMinCnt;     // ����20����
    U32 SysPowerON;        // ϵͳ�ϵ�ʱ��
    U8  DebugON;           // �Ƿ�򿪵��� 20min
    U8  DebugONCnt;        // ���Կ���ʱ��
    U8  SecDataOut;        // �뼶�������
//----------------------------------------------------------
    U8  CheckFlag;         //�춨��ȡ��־
    U8  CfcFlag;           //У����ȡ��־
}s_tempdata_t;

typedef struct{
    unsigned char state_num;                    //��ʾ״̬����
    unsigned char self_test;                    //�Լ�״̬
    unsigned char board_temp;                   //����
    unsigned char exter_power;                  //�ⲿ��Դ
    unsigned char board_volt;                   //��ѹ
    unsigned char board_current;                //�忨����
    unsigned char exter_volt;                   //�ⲿ��ѹ
    unsigned char bat_volt;                     //���ص�ѹ

#if 0
    unsigned char weigh_state;                  //ʢˮͰ����״̬
    unsigned char bucket_dev_state;             //����ʽ��������״̬���
    unsigned char bucket_block;                 //����ͲͲ�ڶ������
    unsigned char bucket_state;                 //����Ͳ�Ϸ���״̬���
    unsigned char count_bucket_state;           //��������״̬���
#endif

}sensor_state_t;

//���û��ȡ��Ҫ��˲ʱֵ�������Ʋ���(QM)
typedef struct {
    float               min;                 //����
    float               max;                 //����
    float               doubtful;            //��������
    float               err;                 //��������
    float               changerate;          //��СӦ�ñ仯������
}qm_t;

//���û��ȡ��Ҫ�ز���ֵ�������Ʋ���(QS)
typedef struct {
    float               min;             //����
    float               max;             //����
    float               change;          //�������仯ֵ
}qs_t;

//����ʽ���̱�Уϵ�� a*X^2 + b*X + c
typedef struct {
    float               a;
    float               b;
    float               c;
}cr_t;
//ʪ��У׼
typedef struct {
      float               HUMI_DATA_12;       // 12%RHʱ��ֵ
      float               HUMI_DATA_35;       // 35%RHʱ��ֵ
      float               HUMI_DATA_55;       // 55%RHʱ��ֵ
      float               HUMI_DATA_75;       // 75%RHʱ��ֵ
      float               HUMI_DATA_90;       // 90%RHʱ��ֵ
}cr_hum_t;

typedef struct{//У�� �춨 �����Ϣ
    uint8      number;        //��������
    cr_t       cr;            //У��ϵ����a,b,c��
    //������Ϣ
}s_check_cr_t;

typedef struct{//У�� �춨 �����Ϣ
    uint8       flag;           //�洢��־
    uint16      year;           //У��ʱ��
    uint8       mon;
    uint8       day;
    uint8       term;           //У������
    uint32      number;         //У��Ա
    uint8       len;            //У��ʱ=ϵ��������a,b,c���� �춨ʱ=�ַ�������
    //������Ϣ
}s_check_info_t;                                     //������Ϣ


//���ڲ���
typedef struct {
    unsigned long int baudrate;         //������
    unsigned char     datasbit;         //����λ
    unsigned char     parity;           //У��λ
    unsigned char     stopbits;         //ֹͣλ
    unsigned char     res;              //Ԥ��
}se_t;







/*device info struct*/
typedef struct {
    unsigned char         flag;                      //�����־�������ж�FLASH�Ƿ񱣴�����

//��һ���ṹ��common�Ź��õĲ���
    struct {
        unsigned char         version;                //�汾��
        unsigned char         qz[6];                     //��վ��

        struct {                                      //����
            unsigned char     degree;        //��
            unsigned char     min;           //��
            unsigned char     sec;           //��
        }Lat;
        struct {                                      //γ��
            unsigned char     degree;        //��
            unsigned char     min;           //��
            unsigned char     sec;           //��
        }Long;
        long         High;                   //����

        unsigned short        st;                     //�豸��������
        short                 work_temp_max;          //�����¶�����
        short                 work_temp_min;          //�����¶�����
        short                 boardsvolt_max;         //������ѹ����
        short                 boardsvolt_min;         //������ѹ����

        unsigned char         de;                     //��׼ƫ��
        unsigned short        fi;                     //֡��ʶ  3λ����
                 short        at;                     //���¶ȣ�������10���洢
        unsigned short        vv;                     //��ص�ѹ��������10���洢
        //�������
        unsigned int          pid;                    //PANID  5
        unsigned char         cha;                    //����Ƶ�� 2
        unsigned char         typ;                    //�ڵ����� 1
        unsigned short        sa;                     //�̵�
        unsigned char         echo;                   //�����������

        se_t se[2];
        
        unsigned short        stdev;                  //���ݱ�׼��ֵ������
        unsigned short        ftd;                    //���û��ȡ���ִ���������ģʽ�µ����ݷ���ʱ����
        unsigned short        ft;                     //���û��ȡ���ִ���������ģʽ�µ����ݷ���ʱ��

        unsigned char         sw;                     //���û��ȡ���ֻ��Ʒ�ʽ
        unsigned char         mo;                     //���û��ȡ�������ķ������ݷ�ʽ
        unsigned char         sensortype;             //���������ͣ���ʪ�ȣ����¡�����
        unsigned char         time_to_autofilldata;   //�Զ���������ʱ��
		unsigned char         res[3];                 //Ԥ��
    }common;

    //�ڶ����ṹ��sensor�ű�Ҫ��ר�ò���
    struct{
        unsigned char         data_num;                  //�۲�Ҫ������
        unsigned char         status_num;                //״̬��������
        unsigned char         qc_en;
        qm_t                  qm[10];                     //���û��ȡ��Ҫ��˲ʱֵ�������Ʋ���(QM)
        qs_t                  qs[10];                     //���û��ȡ��Ҫ�ز���ֵ�������Ʋ���(QS)

        unsigned char         ce;            // ��ѹ����������
        unsigned char         ce_H;          // ʪ�ȴ���������
        unsigned char         res[3];        // Ԥ��
        unsigned char         apunit;        // ��ѹ��������
        cr_t                  cr[8];         // ��Уϵ��
        cr_hum_t              crHum[3];      // ʪ����������У׼  ����  ����  ����

        unsigned char         SensorNum;        // ����������
        char                  str_name[20][8];  // ���봫��������
    }sensor;

} bcm_info_t;

#pragma pack()

#endif
