/************************************************************************************************************
;*--------------------------------------------- �ļ���Ϣ ----------------------------------------------------
;* �ļ����� : �����ʹ������豸ͨ������
;* �ļ����� :
;* ����˵�� :
;*-------------------------------------------- ���°汾��Ϣ -------------------------------------------------
;* �޸����� : LY
;* �޸����� : 2017-12-28
;* �汾���� : ���˽������
;*-------------------------------------------- ��ʷ�汾��Ϣ -------------------------------------------------
;* �ļ����� : LY
;* �������� : 2017-12-7
;* �汾���� :
***********************************************************************************************************/

#ifndef __COMMAND_H__
#define __COMMAND_H__
 
#include "typedef.h"
#include "defpam.h"


typedef enum
{
    /* command index*/     
    SETCOM,            //1�����û��ȡ�豸��ͨѶ����
    AUTOCHECK,         //2���豸�Լ�
    HELP,              //3����������
    QZ,                //4�����û��ȡ�豸����վ��
    ST,                //5�����û��ȡ�豸�ķ�������
    DI,                //6����ȡ�豸��ʶλ
    ID,                //7�����û��ȡ�豸
    LAT,               //8�����û��ȡ����۲�վ��γ��
    LONG,              //9�����û��ȡ����۲�վ�ľ���
    DATE,              //10�����û��ȡ�豸����
    TIME,              //11�����û��ȡ�豸ʱ��
    DATETIME,          //12�����û��ȡ�豸������ʱ��
    FTD,               //13�����û��ȡ�豸����ģʽ�µķ���ʱ����
    STDEV,             //14�����û��ȡ�豸���ݱ�׼ƫ��ֵ�����ʱ����
    FAT,               //15�����û��ȡ�豸����ģʽ�����ݷ���ʱ��
    SETCOMWAY,         //16���������ֻ��Ʒ�ʽ
    SS,                //17����ȡ�豸����������ֵ
    STAT,              //18����ȡ�豸����״̬����
    AT,                //19�����û��ȡ�豸���������¶ȷ�Χ
    VV,                //20�����û��ȡ��������������ѹ��Χ
    SN,                //21�����û��ȡ�豸���к�
    QCPS,              //22�����û��ȡ����ֵ�������Ʋ���
    QCPM,              //23�����û��ȡ˲ʱ����ֵ�������Ʋ���
    CR,                //24�����û��ȡ�豸��У�����춨��Ϣ
    READDATA,          //25��ʵʱ��ȡ����
    DOWN,              //26����ʷ��������
    RESET27,             //27�������豸
    ASL,               //28�����û��ȡ����۲�վ�ĺ���
    SUPERADMIN,        //29������Ȩ��ָ��
    UNIT,              //30  ��ѹ��λ�޸�����

//------------------- ˽������ ---------------------------
    DEBUGON,           //1 ����ģʽ��
    DEBUGOFF,          //2 ����ģʽ��
    VERSION,           //3 �汾�Ŷ�ȡ
    RESTORE,           //4 �ָ���������
    STARTTIME,         //5 �豸��ʼ����ʱ��
    SECDOUTON,         //6 ����뼶����
    SECDOUTOFF,
    ERASECR,
    CFC,               //7 �궨������ϵ��
    WINDH,             //8 �粦�뿪������
    STSENSOR,          //9 ���´������������������
    SHSENSOR,          //10 ����ˮ�ִ������������������
    HARDVERCFG,         //Ӳ���汾��д��

	MAX_COMMAND_NUM
} cmd_index_t;


typedef struct { 
	struct{
         char  *name;
	     int   (*action)( char *buf, char *rbuf);
	      }dispatch[MAX_COMMAND_NUM];
} bcm_control_t;



extern volatile unsigned char Flag_DataEmulationMode;  // X1�������ģ�����ݱ�־λ
extern volatile unsigned char EmulationDataType;// X4 ��������ǰ��ģ��ֵ��������
extern volatile float EmulationData;//X3��������ǰ��ģ��ֵ
extern float EmulationData_1;       //X3��������ǰ��ģ��ֵ
extern volatile unsigned char EmulationSensorChannel;   // X2Ҫ��ȡ�Ĵ�����ͨ����

unsigned char GetCheckSumCode(void);
int getSeparator(char *rcvdata);
U16 uartcmd_process(U8 uartno,char *data,U16 len,char *rbuf);
int check_closecom(char *buf,char *rbuf);


int check_time  (int hour, int min,   int sec);                 // У��ʱ��
int check_data  (int year, int month, int day);                 // У������
int check_format(char *p,  uint8 len, char *buf, uint8 num);    //����У�� ����ʽ
int check_digit (char *p,  uint8 len);                          //У���ַ����Ƿ�ȫΪ����
int Utils_CheckCmdStr(char *pStr);                    //����յ��ַ�����ʽ�Ƿ����Ҫ��

// -----------------------------------------------------------------------
int cmd_setcom     (char *buf,char *rbuf);          //1�����û��ȡ�豸��ͨѶ����
int cmd_autocheck  (char *buf,char *rbuf);          //2���豸�Լ�
int cmd_help       (char *buf,char *rbuf);          //3����������
int cmd_qz         (char *buf,char *rbuf);          //4�����û��ȡ�豸����վ��
int cmd_st         (char *buf,char *rbuf);          //5�����û��ȡ�豸�ķ�������
int cmd_di         (char *buf,char *rbuf);          //6����ȡ�豸��ʶλ
int cmd_id         (char *buf,char *rbuf);          //7�����û��ȡ�豸
int cmd_lat        (char *buf,char *rbuf);          //8�����û��ȡ����۲�վ��γ��
int cmd_long       (char *buf,char *rbuf);          //9�����û��ȡ����۲�վ�ľ���
int cmd_asl        (char *buf,char *rbuf);          //28  ���û��ȡ����۲�վ�ĺ���
int cmd_date       (char *buf,char *rbuf);          //10�����û��ȡ�豸����
int cmd_time       (char *buf,char *rbuf);          //11�����û��ȡ�豸ʱ��
int cmd_datetime   (char *buf,char *rbuf);          //12�����û��ȡ�豸������ʱ��
int cmd_ftd        (char *buf,char *rbuf);          //13�����û��ȡ�豸����ģʽ�µķ���ʱ����
int cmd_stdev      (char *buf,char *rbuf);          //14�����û��ȡ�豸���ݱ�׼ƫ��ֵ�����ʱ����
int cmd_fat        (char *buf,char *rbuf);          //15�����û��ȡ�豸����ģʽ�����ݷ���ʱ��
int cmd_setcomway  (char *buf,char *rbuf);          //16���������ֻ��Ʒ�ʽ
int cmd_ss         (char *buf,char *rbuf);          //17����ȡ�豸����������ֵ
int cmd_stat       (char *buf,char *rbuf);          //18����ȡ�豸����״̬����
int cmd_at         (char *buf,char *rbuf);          //19�����û��ȡ�豸���������¶ȷ�Χ
int cmd_vv         (char *buf,char *rbuf);          //20�����û��ȡ��������������ѹ��Χ
int cmd_sn         (char *buf,char *rbuf);          //21�����û��ȡ�豸���к�
int cmd_qcps       (char *buf,char *rbuf);          //22�����û��ȡ����ֵ�������Ʋ���
int cmd_qcpm       (char *buf,char *rbuf);          //23�����û��ȡ˲ʱ����ֵ�������Ʋ���
int cmd_cr         (char *buf,char *rbuf);          //24�����û��ȡ�豸��У�����춨��Ϣ
int cmd_readdata   (char *buf,char *rbuf);          //25��ʵʱ��ȡ����
int cmd_down       (char *buf,char *rbuf);          //26����ʷ��������
int cmd_reset      (char *buf,char *rbuf);          //27�������豸
int cmd_superadmin (char *buf,char *rbuf);          //29  ����Ȩ������
int cmd_unit       (char *buf,char *rbuf);          //30  ������ѹ�������ĵ�λ
// ------------------------------------------------------------------------------
// --------------------------- ˽������ --------------------------------------------
int cmd_debugon     (char *buf,char *rbuf);     //A.1 ���򿪵���ģʽ
int cmd_debugoff    (char *buf,char *rbuf);     //A.2 ���رյ���ģʽ
int cmd_version     (char *buf,char *rbuf);     //A.3���汾�Ų�ѯ
int cmd_restore     (char *buf,char *rbuf);     //A.4���ָ���������
int cmd_starttime   (char *buf,char *rbuf);     //A.5���豸��ʼ����ʱ��
int cmd_secdout_on  (char *buf,char *rbuf);     //A.6������뼶����
int cmd_secdout_off (char *buf,char *rbuf);
int cmd_erase_check (char *buf,char *rbuf);
int cmd_cfc         (char *buf,char *rbuf);     //A.7���궨ϵ��
int cmd_windh       (char *buf,char *rbuf);     //A.8���紫�������뿪������
int cmd_stsensor    (char *buf,char *rbuf);     //A.9�����´���������
int cmd_shsensor    (char *buf,char *rbuf);     //A.10������ˮ�ִ���������
int cmd_version_cfg (char *buf,char *rbuf);      //12    Ӳ���汾��д��

//--------------------------- ˽���������  ------------------------------------------

#endif
