//**************************************************************************************************/
//  Filename:       ClimiteSensor.c
//  Revised:        $Date: 2009-12-30 12:32:18 -0800 (Wed, 30 Dec 2009) $
//  Revision:       $Revision: 21417 $
//
//  Description:    Sample application for a Climite sensor utilizing the Climite API.

//**************************************************************************************************/

#include "sensor_basic.h"

extern unsigned char DS3231_ReadTime(s_RtcTateTime_t *prtc);
extern unsigned char DS3231_SetTime(s_RtcTateTime_t *prtc);

uint32 ChannelList[16] = {0x00000800,0x00001000,0x00002000,0x00004000,0x00008000,0x00010000,0x00020000,0x00040000,
0x00080000,0x00100000,0x00200000,0x00400000,0x00800000,0x01000000,0x02000000,0x04000000};

void Init_sys_cfg()
{
    unsigned char i;

    memset(&bcm_info,0,sizeof(bcm_info_t));
    //ͨѶ����
    bcm_info.common.se[0].baudrate = 19200;
    bcm_info.common.se[0].datasbit = 8;
    bcm_info.common.se[0].parity   ='N';
    bcm_info.common.se[0].stopbits = 1;

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
    //1min ����
    bcm_info.sensor.qm[WIND_VELOCITY_1_M].min        = 0;          //����
    bcm_info.sensor.qm[WIND_VELOCITY_1_M].max        = 75;           //����
    bcm_info.sensor.qm[WIND_VELOCITY_1_M].doubtful   = 10;            //��������
    bcm_info.sensor.qm[WIND_VELOCITY_1_M].err        = 20;           //��������
    bcm_info.sensor.qm[WIND_VELOCITY_1_M].changerate = INVALID_DATA;            //��СӦ�ñ仯������
    //2 min ����
    bcm_info.sensor.qm[WIND_VELOCITY_2_M].min        = 0;          //����
    bcm_info.sensor.qm[WIND_VELOCITY_2_M].max        = 75;           //����
    bcm_info.sensor.qm[WIND_VELOCITY_2_M].doubtful   = 10;            //��������
    bcm_info.sensor.qm[WIND_VELOCITY_2_M].err        = 20;           //��������
    bcm_info.sensor.qm[WIND_VELOCITY_2_M].changerate = INVALID_DATA;            //��СӦ�ñ仯������
    //10min ����
    bcm_info.sensor.qm[WIND_VELOCITY_10_M].min        = 0;          //����
    bcm_info.sensor.qm[WIND_VELOCITY_10_M].max        = 75;           //����
    bcm_info.sensor.qm[WIND_VELOCITY_10_M].doubtful   = 10;            //��������
    bcm_info.sensor.qm[WIND_VELOCITY_10_M].err        = 20;            //��������
    bcm_info.sensor.qm[WIND_VELOCITY_10_M].changerate = INVALID_DATA;            //��СӦ�ñ仯������
    //1min ����
    bcm_info.sensor.qm[WIND_DIRECTION_1_M].min        = 0;          //����
    bcm_info.sensor.qm[WIND_DIRECTION_1_M].max        = 360;           //����
    bcm_info.sensor.qm[WIND_DIRECTION_1_M].doubtful   = INVALID_DATA;            //��������
    bcm_info.sensor.qm[WIND_DIRECTION_1_M].err        = INVALID_DATA;            //��������
    bcm_info.sensor.qm[WIND_DIRECTION_1_M].changerate = 10;            //��СӦ�ñ仯������
    //2min ����
    bcm_info.sensor.qm[WIND_DIRECTION_2_M].min        = 0;          //����
    bcm_info.sensor.qm[WIND_DIRECTION_2_M].max        = 360;           //����
    bcm_info.sensor.qm[WIND_DIRECTION_2_M].doubtful   = INVALID_DATA;            //��������
    bcm_info.sensor.qm[WIND_DIRECTION_2_M].err        = INVALID_DATA;           //��������
    bcm_info.sensor.qm[WIND_DIRECTION_2_M].changerate = 10;            //��СӦ�ñ仯������
    //10min ����
    bcm_info.sensor.qm[WIND_DIRECTION_10_M].min        = 0;          //����
    bcm_info.sensor.qm[WIND_DIRECTION_10_M].max        = 360;           //����
    bcm_info.sensor.qm[WIND_DIRECTION_10_M].doubtful   = INVALID_DATA;          //��������
    bcm_info.sensor.qm[WIND_DIRECTION_10_M].err        = INVALID_DATA;            //��������
    bcm_info.sensor.qm[WIND_DIRECTION_10_M].changerate = 10;            //��СӦ�ñ仯������
    //˲ʱ ����
    bcm_info.sensor.qm[WIND_DIRECTION_0_M].min        = 0;          //����
    bcm_info.sensor.qm[WIND_DIRECTION_0_M].max        = 360;           //����
    bcm_info.sensor.qm[WIND_DIRECTION_0_M].doubtful   = INVALID_DATA;          //��������
    bcm_info.sensor.qm[WIND_DIRECTION_0_M].err        = INVALID_DATA;            //��������
    bcm_info.sensor.qm[WIND_DIRECTION_0_M].changerate = 10;            //��СӦ�ñ仯������
    //3s ����
    bcm_info.sensor.qm[WIND_VELOCITY_3_S].min        = 0;          //����
    bcm_info.sensor.qm[WIND_VELOCITY_3_S].max        = 150;           //����
    bcm_info.sensor.qm[WIND_VELOCITY_3_S].doubtful   = 10;          //��������
    bcm_info.sensor.qm[WIND_VELOCITY_3_S].err        = 20;            //��������
    bcm_info.sensor.qm[WIND_VELOCITY_3_S].changerate = INVALID_DATA;            //��СӦ�ñ仯������

    //���û��ȡ��Ҫ�ز���ֵ�������Ʋ���(QS)
    //˲ʱ����
    bcm_info.sensor.qs[WIND_VELOCITY_250_S].min    = 0;       //����
    bcm_info.sensor.qs[WIND_VELOCITY_250_S].max    = 75;        //����
    bcm_info.sensor.qs[WIND_VELOCITY_250_S].change = 20;          //�������仯ֵ
    //˲ʱ����
    bcm_info.sensor.qs[WIND_DIRECTION_1_S].min    = 0;       //����
    bcm_info.sensor.qs[WIND_DIRECTION_1_S].max    = 360;        //����
    bcm_info.sensor.qs[WIND_DIRECTION_1_S].change = INVALID_DATA;          //�������仯ֵ\

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
    bcm_info.sensor.data_num  = 12;                  //�۲�Ҫ������
    bcm_info.sensor.status_num = 0;                //״̬��������
	bcm_info.sensor.ce = 0;               // ��ѹ����������  0-����   3-̩��
    bcm_info.sensor.apunit = 1;           // ��ѹ������λ���� ��λĬ��Ϊ����
    bcm_info.sensor.SensorNum = 2;        // ����������

    memset(&bcm_info.sensor.str_name,0,10*8);
    memset(&sensor_state,0,sizeof(sensor_state_t));  //�忨״̬��ʼ��

}

void check_event_sample(void)
{
    //ms�뼶�¼�����
    if(m_tempdata.event.msecevent == true)//��Ƶ��ֵ
    {
        if(1 == Element_MsSample(&sensors_data))
        {
            //�ɼ��ɹ�
            m_tempdata.event.msecevent = false;//�����������ڽ�ֹ����
        }
    }
    
    //�뼶�¼�����
    //��ѯ�Ƿ񵽲�������
    if(m_tempdata.event.secevent==true)
    {
        m_tempdata.event.secevent=false;
    
        //ÿ30���� �ĵ�50���Ӳʱ�Ӹ���ʱ��Уʱ
        if((m_tempdata.m_RtcTateTime.min%30 == 0)&&(m_tempdata.m_RtcTateTime.sec == 50))
        {
            s_RtcTateTime_t time_struct_hardrtc_temp;
            if(DS3231_ReadTime(&time_struct_hardrtc_temp)==1)  //ʱ��У����ȷ�Ÿ���ʱ��Уʱ
            {
                m_tempdata.m_RtcTateTime = time_struct_hardrtc_temp;
            }
        }

        if(1 == Element_SecSample(&sensors_data))	//�ɼ��ɹ�
        {
         //   flag_check_event = 0;//�����������ڽ�ֹ����
        }
    }
}
/*

check_event_calc()
check_event_storage()
��ʵʱ��Ҫ�󲻸ߣ��ɷŵ� MY_TEMP_EVT �¼������

*/
void check_event_calc(void)
{
    uint8 i = 0;

    Element_MinSample(&sensors_data);

    memset(&sensors_data.sendpacket,0,MAX_PKG_LEN);
    sensors_data.sendpacketlen=GetMinData((char *)sensors_data.sendpacket,&sensors_data,1);
    if(Flag_DataEmulationMode == 1)//��Ҫ����������ݰ�
    {
      for(i = 0;i<2;i++)
        GetSampleData( &sensors_data,data_sample_buf,i+1);
    }
}

