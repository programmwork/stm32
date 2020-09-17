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
    //10cm
    bcm_info.sensor.qm[0].min        = -90;          //����
    bcm_info.sensor.qm[0].max        = 90;           //����
    bcm_info.sensor.qm[0].doubtful   = 5;            //��������
    bcm_info.sensor.qm[0].err        = 10;           //��������
    bcm_info.sensor.qm[0].changerate = 0;            //��СӦ�ñ仯������
    //20cm
    bcm_info.sensor.qm[1].min        = -90;          //����
    bcm_info.sensor.qm[1].max        = 90;           //����
    bcm_info.sensor.qm[1].doubtful   = 5;            //��������
    bcm_info.sensor.qm[1].err        = 10;           //��������
    bcm_info.sensor.qm[1].changerate = 0;            //��СӦ�ñ仯������
    //30cm
    bcm_info.sensor.qm[2].min        = -80;          //����
    bcm_info.sensor.qm[2].max        = 80;           //����
    bcm_info.sensor.qm[2].doubtful   = 2;            //��������
    bcm_info.sensor.qm[2].err        = 5;            //��������
    bcm_info.sensor.qm[2].changerate = 0;            //��СӦ�ñ仯������
    //40cm
    bcm_info.sensor.qm[3].min        = -70;          //����
    bcm_info.sensor.qm[3].max        = 70;           //����
    bcm_info.sensor.qm[3].doubtful   = 1;            //��������
    bcm_info.sensor.qm[3].err        = 5;            //��������
    bcm_info.sensor.qm[3].changerate = 0;            //��СӦ�ñ仯������
    //50cm
    bcm_info.sensor.qm[4].min        = -60;          //����
    bcm_info.sensor.qm[4].max        = 60;           //����
    bcm_info.sensor.qm[4].doubtful   = 1;            //��������
    bcm_info.sensor.qm[4].err        = 3;           //��������
    bcm_info.sensor.qm[4].changerate = 0;            //��СӦ�ñ仯������
    //50cm
    bcm_info.sensor.qm[5].min        = -60;          //����
    bcm_info.sensor.qm[5].max        = 60;           //����
    bcm_info.sensor.qm[5].doubtful   = 1;            //��������
    bcm_info.sensor.qm[5].err        = 3;           //��������
    bcm_info.sensor.qm[5].changerate = 0;            //��СӦ�ñ仯������

    //50cm
    bcm_info.sensor.qm[6].min        = -60;          //����
    bcm_info.sensor.qm[6].max        = 60;           //����
    bcm_info.sensor.qm[6].doubtful   = 1;            //��������
    bcm_info.sensor.qm[6].err        = 3;           //��������
    bcm_info.sensor.qm[6].changerate = 0;            //��СӦ�ñ仯������

    //50cm
    bcm_info.sensor.qm[7].min        = -60;          //����
    bcm_info.sensor.qm[7].max        = 60;           //����
    bcm_info.sensor.qm[7].doubtful   = 1;            //��������
    bcm_info.sensor.qm[7].err        = 3;           //��������
    bcm_info.sensor.qm[7].changerate = 0;            //��СӦ�ñ仯������

    //���û��ȡ��Ҫ�ز���ֵ�������Ʋ���(QS)
    //10 20 30 40 50cm
    bcm_info.sensor.qs[0].min    = -100;       //����
    bcm_info.sensor.qs[0].max    = 100;        //����
    bcm_info.sensor.qs[0].change = 2;          //�������仯ֵ
    bcm_info.sensor.qs[1].min    = -100;       //����
    bcm_info.sensor.qs[1].max    = 100;        //����
    bcm_info.sensor.qs[1].change = 2;          //�������仯ֵ
    bcm_info.sensor.qs[2].min    = -100;       //����
    bcm_info.sensor.qs[2].max    = 100;        //����
    bcm_info.sensor.qs[2].change = 2;          //�������仯ֵ
    bcm_info.sensor.qs[3].min    = -100;       //����
    bcm_info.sensor.qs[3].max    = 100;        //����
    bcm_info.sensor.qs[3].change = 2;          //�������仯ֵ
    bcm_info.sensor.qs[4].min    = -100;       //����
    bcm_info.sensor.qs[4].max    = 100;        //����
    bcm_info.sensor.qs[4].change = 2;          //�������仯ֵ
    bcm_info.sensor.qs[5].min    = -100;       //����
    bcm_info.sensor.qs[5].max    = 100;        //����
    bcm_info.sensor.qs[5].change = 2;          //�������仯ֵ
    bcm_info.sensor.qs[6].min    = -100;       //����
    bcm_info.sensor.qs[6].max    = 100;        //����
    bcm_info.sensor.qs[6].change = 2;          //�������仯ֵ
    bcm_info.sensor.qs[7].min    = -100;       //����
    bcm_info.sensor.qs[7].max    = 100;        //����
    bcm_info.sensor.qs[7].change = 2;          //�������仯ֵ

	//����ϵ��
    for(i = 0; i < 8; i++)
    {
       bcm_info.sensor.cr[i].a = 0;
       bcm_info.sensor.cr[i].b = 1;
       bcm_info.sensor.cr[i].c = 0;
    }
	
    bcm_info.flag   = 0;                  //�����־ FLASH �����

    strncpy((char *)bcm_info.sensor.res, "000",3);       //Ԥ��

    strncpy((char *)bcm_info.sensor.str_name[0], "AB10", 4);
    strncpy((char *)bcm_info.sensor.str_name[1], "AB20", 4);
    strncpy((char *)bcm_info.sensor.str_name[2], "AB30", 4);
    strncpy((char *)bcm_info.sensor.str_name[3], "AB40", 4);
    strncpy((char *)bcm_info.sensor.str_name[4], "AB50", 4);
    strncpy((char *)bcm_info.sensor.str_name[5], "AB60", 4);
    strncpy((char *)bcm_info.sensor.str_name[6], "AB70", 4);
    strncpy((char *)bcm_info.sensor.str_name[7], "AB80", 4);

    strncpy((char *)bcm_info.sensor.str_name[8], "ABl10", 5);
    strncpy((char *)bcm_info.sensor.str_name[9], "ABl20", 5);
    strncpy((char *)bcm_info.sensor.str_name[10], "ABl30", 5);
    strncpy((char *)bcm_info.sensor.str_name[11], "ABl40", 5);
    strncpy((char *)bcm_info.sensor.str_name[12], "ABl50", 5);
    strncpy((char *)bcm_info.sensor.str_name[13], "ABl60", 5);
    strncpy((char *)bcm_info.sensor.str_name[14], "ABl70", 5);
    strncpy((char *)bcm_info.sensor.str_name[15], "ABl80", 5);


    //�������ڶ����ṹ��sensor�ű�Ҫ��ר�ò���
    bcm_info.sensor.data_num  = 10;                //�۲�Ҫ������
    bcm_info.sensor.status_num = 0;                //״̬��������
	bcm_info.sensor.ce = 0;               // ��ѹ����������  0-����   3-̩��
    bcm_info.sensor.apunit = 1;           // ��ѹ������λ���� ��λĬ��Ϊ����
    bcm_info.sensor.SensorNum = 5;        // ����������

    memset(&sensor_state,0,sizeof(sensor_state_t));  //�忨״̬��ʼ��

}


