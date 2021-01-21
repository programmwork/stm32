//**************************************************************************************************/
//  Filename:       ClimiteSensor.c
//  Revised:        $Date: 2009-12-30 12:32:18 -0800 (Wed, 30 Dec 2009) $
//  Revision:       $Revision: 21417 $
//
//  Description:    Sample application for a Climite sensor utilizing the Climite API.

//**************************************************************************************************/

#include "sensor_basic.h"


void Init_sys_cfg()
{
    unsigned char i;

    memset(&bcm_info,0,sizeof(bcm_info_t));
    //ͨѶ����
    bcm_info.common.se[0].baudrate = 19200;
    bcm_info.common.se[0].datasbit = 8;
    bcm_info.common.se[0].parity   ='N';
    bcm_info.common.se[0].stopbits = 1;

    bcm_info.common.se[1].baudrate = 19200;
    bcm_info.common.se[1].datasbit = 8;
    bcm_info.common.se[1].parity   ='N';
    bcm_info.common.se[1].stopbits = 1;


    bcm_info.common.version = 1;            //�汾��
    bcm_info.common.qz = 85794;             //��վ��
    bcm_info.common.st = 1;                 //��������

    //���� γ�� ����
    bcm_info.common.Lat .degree = 32;
    bcm_info.common.Lat .min    = 14;
    bcm_info.common.Lat .sec    = 20;
    bcm_info.common.Long.degree = 116;
    bcm_info.common.Long.min    = 34;
    bcm_info.common.Long.sec    = 18;
    bcm_info.common.High        = 1000;

    bcm_info.common.work_temp_max  = 800;        //�����¶�����
    bcm_info.common.work_temp_min  = -500;       //�����¶�����
    bcm_info.common.boardsvolt_max = 138;        //������ѹ����
    bcm_info.common.boardsvolt_min = 90;         //������ѹ����

    bcm_info.common.de = 0;                     //��׼ƫ��
    bcm_info.common.fi = 1;                     //֡��ʶ  3λ����
    bcm_info.common.at = 0;                     //���¶ȣ�������10���洢
    bcm_info.common.vv = 0;                     //��ص�ѹ��������10���洢

    bcm_info.common.stdev = 1;                  //���ݱ�׼��ֵ������
    bcm_info.common.ftd   = 1;                  //���û��ȡ���ִ���������ģʽ�µ����ݷ���ʱ����
    bcm_info.common.ft    = 30;                 //���û��ȡ���ִ���������ģʽ�µ����ݷ���ʱ��

    bcm_info.common.sw                   = 0;   //���û��ȡ���ֻ��Ʒ�ʽ
    bcm_info.common.mo                   = 1;   //���û��ȡ�������ķ������ݷ�ʽ
    bcm_info.common.sensortype           = 0;   //���������ͣ���ʪ�ȣ����¡�����
    bcm_info.common.time_to_autofilldata = 0;   //�Զ���������ʱ��
    strncpy((char *)bcm_info.common.res, "000",3);      //Ԥ��

    //���û��ȡ��Ҫ��˲ʱֵ�������Ʋ���(QM)
    //��ѹ
    bcm_info.sensor.qm[0].min        = 400;          //����
    bcm_info.sensor.qm[0].max        = 1100;           //����
    bcm_info.sensor.qm[0].doubtful   = 0.5;            //��������
    bcm_info.sensor.qm[0].err        = 2;           //��������
    bcm_info.sensor.qm[0].changerate = 0.1;            //��СӦ�ñ仯������
  
    //���û��ȡ��Ҫ�ز���ֵ�������Ʋ���(QS)
    //��ѹ
    bcm_info.sensor.qs[0].min    = 400;       //����
    bcm_info.sensor.qs[0].max    = 1100;      //����
    bcm_info.sensor.qs[0].change = 0.3;       //�������仯ֵ

	//����ϵ��
    for(i = 0; i < 6; i++)	
    {
       bcm_info.sensor.cr[i].a = 0;
       bcm_info.sensor.cr[i].b = 1;
       bcm_info.sensor.cr[i].c = 0;
    }

    bcm_info.flag   = 0;                  //�����־ FLASH �����

    strncpy((char *)bcm_info.sensor.res, "000",3);       //Ԥ��

    //�������ڶ����ṹ��sensor�ű�Ҫ��ר�ò���
    bcm_info.sensor.data_num  = 2;                  //�۲�Ҫ������
    bcm_info.sensor.status_num = 0;                //״̬��������
    bcm_info.sensor.ce = 2;               // ��ѹ����������  1-210 2-baro-1
    bcm_info.sensor.ce_H = 1;             // ʪ�ȴ���������  1-��׿�ݿ�          2-155
    bcm_info.sensor.apunit = 1;           // ��ѹ������λ���� ��λĬ��Ϊ����
    bcm_info.sensor.SensorNum = 1;        // ����������

    memset(&bcm_info.sensor.str_name,0,10*8);
    memset(&sensor_state,0,sizeof(sensor_state_t));  //�忨״̬��ʼ��

}

