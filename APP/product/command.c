/************************************************************************************************************
;*--------------------------------------------- �ļ���Ϣ ----------------------------------------------------
;* �ļ����� : �����ʹ������豸ͨ������
;* �ļ����� :
;* ����˵�� :
;*-------------------------------------------- ���°汾��Ϣ -------------------------------------------------
;* �޸����� :
;* �޸����� :
;* �汾���� :
;*-------------------------------------------- ��ʷ�汾��Ϣ -------------------------------------------------
;* �ļ����� : LY
;* �������� : 2017-12-7
;* �汾���� : ���ͨ�������������Ķ�ȡ���ã�����˽������
***********************************************************************************************************/

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "main.h" 

#include <math.h>
#include <time.h>

#define SN_NUM 36

int check_datetime(s_RtcTateTime_t *datetime);
time_t get_softimer_sec(s_RtcTateTime_t *temp_t);
time_t get_softimer(s_RtcTateTime_t *temp_t);

volatile unsigned char Flag_DataEmulationMode = 0;   // X1�������ģ�����ݱ�־λ
volatile unsigned char EmulationSensorChannel = 0;   // X2Ҫ��ȡ�Ĵ�����ͨ����
volatile float         EmulationData = 0;            // X3��������ǰ��ģ��ֵ
volatile unsigned char EmulationDataType = 0;        // X4 ��������ǰ��ģ��ֵ��������
float                  EmulationData_1 = 0.0;

#define DEV_TYPE_MAX  14
char *dev_type[DEV_TYPE_MAX]={
    "YAWP",     //0 ���ؽ�ˮ��
    //---------�Զ�����վ--------------
    "YACS",     //1 �Զ�����վ
    "YHMS",     //2 ʪ�����ִ�����
    "YTMP",     //3 �¶����ִ�����
    "YIST",     //4 ����ر��¶�
    "YTPS",     //5 ��ѹ���ִ�����
    "YWPD",     //6 ���ٷ������ִ�����
    "YWYG",     //7 �������ִ�����
    "YTBR",     //8 �����������ִ�����(0.1mm)
    "YSGT",     //9 ǳ������¶����ִ�����
    "YSMS",     //10 ����ˮ�����ִ�����
    "YTRS",     //11 �ܷ������ִ�����

    "YSDR",     //12 ����
    "YROS"      //13 ����۲���

};

const bcm_control_t bcm_control={
    {
        {"SETCOM"    ,&cmd_setcom},            //1�����û��ȡ�豸��ͨѶ����
        {"AUTOCHECK" ,&cmd_autocheck},         //2���豸�Լ�
        {"HELP"      ,&cmd_help},              //3����������
        {"QZ"        ,&cmd_qz},                //4�����û��ȡ�豸����վ��
        {"ST"        ,&cmd_st},                //5�����û��ȡ�豸�ķ�������
        {"DI"        ,&cmd_di},                //6����ȡ�豸��ʶλ
        {"ID"        ,&cmd_id},                //7�����û��ȡ�豸
        {"LAT"       ,&cmd_lat},               //8�����û��ȡ����۲�վ��γ��
        {"LONG"      ,&cmd_long},              //9�����û��ȡ����۲�վ�ľ���
        {"ASL"       ,&cmd_asl},               //28�����û��ȡ����۲�վ�ĺ���
        {"DATE"      ,&cmd_date},              //10�����û��ȡ�豸����
        {"TIME"      ,&cmd_time},              //11�����û��ȡ�豸ʱ��
        {"DATETIME"  ,&cmd_datetime},          //12�����û��ȡ�豸������ʱ��
        {"FTD"       ,&cmd_ftd},               //13�����û��ȡ�豸����ģʽ�µķ���ʱ����
        {"STDEV"     ,&cmd_stdev},             //14�����û��ȡ�豸���ݱ�׼ƫ��ֵ�����ʱ����
        {"FAT"       ,&cmd_fat},               //15�����û��ȡ�豸����ģʽ�����ݷ���ʱ��
        {"SETCOMWAY" ,&cmd_setcomway},         //16���������ֻ��Ʒ�ʽ
        {"SS"        ,&cmd_ss},                //17����ȡ�豸����������ֵ
        {"STAT"      ,&cmd_stat},              //18����ȡ�豸����״̬����
        {"AT"        ,&cmd_at},                //19�����û��ȡ�豸���������¶ȷ�Χ
        {"VV"        ,&cmd_vv },               //20�����û��ȡ��������������ѹ��Χ
        {"SN"        ,&cmd_sn},                //21�����û��ȡ�豸���к�
        {"QCPS"      ,&cmd_qcps},              //22�����û��ȡ����ֵ�������Ʋ���
        {"QCPM"      ,&cmd_qcpm},              //23�����û��ȡ˲ʱ����ֵ�������Ʋ���
        {"CR"        ,&cmd_cr},                //24�����û��ȡ�豸��У�����춨��Ϣ
        {"READDATA"  ,&cmd_readdata},          //25��ʵʱ��ȡ����
        {"DOWN"      ,&cmd_down},              //26����ʷ��������
        {"RESET"     ,&cmd_reset},             //27�������豸

        {"SUPERADMIN",&cmd_superadmin},        //29������Ȩ��ָ��
        {"UNIT"      ,&cmd_unit},              //30  ��ѹ��λ�޸�����

//------------------------------ ˽������ -------------------------------------
        {"DEBUGON"   ,&cmd_debugon},           //1 ����ģʽ��
        {"DEBUGOFF"  ,&cmd_debugoff},          //2 ����ģʽ��
        {"VERSION"   ,&cmd_version},           //3 �汾�Ŷ�ȡ
        {"RESTORE"   ,&cmd_restore},           //4 �ָ���������
        {"STARTTIME" ,&cmd_starttime},         //5 �豸��ʼ����ʱ��
        {"SECDOUTON" ,&cmd_secdout_on},        //6 ����뼶����
        {"SECDOUTOFF",&cmd_secdout_off},       //7 �ر��뼶�������
        {"ERASECR"   ,&cmd_erase_check},       //
        {"CFC"       ,&cmd_cfc},               //8 �궨������ϵ��
        {"WINDH"     ,&cmd_windh},             //9 �粦�뿪������
        {"STSENSOR"  ,&cmd_stsensor},          //10 ���´������������������
        {"SHSENSOR"  ,&cmd_shsensor},          //11 ����ˮ�ִ������������������
        {"VERSIONCFG",&cmd_version_cfg},       //12 �汾��д��
    }
};


int save_element(short temp_num, float temp_min, float temp_max, char *temp_doubt, char *temp_err, char *temp_change, qm_t *temp_qm);   //�洢QS
int check_element(short temp_num, qm_t *temp_qm);                //У��QS
int save_sensor(short temp_num, float temp_min, float temp_max, char *temp_change, qs_t *temp_qs);
int check_sensor(short temp_num, qs_t *temp_qs);

int getSeparator(char *rcvdata)
{
    U8 i=0;
    for( i=0;i<strlen(rcvdata);i++)
    {
        if((rcvdata[i]==',') || (rcvdata[i]==' ') || (rcvdata[i]==0x0D)|| (rcvdata[i]==0x0A))
        {
            break;
        }
    }
    return i;
}

char data_II[64], data_buf[MAX_PKGLEN];
U16 uartcmd_process(U8 uartno,char *data,U16 len,char *rbuf)
{
    
    int index,ret;
    U8 i;
    char *temp_di = NULL, *p = NULL, *temp_id = NULL;
    unsigned short id_num;
    index=getSeparator(data);

    if((index>20)||(index<2))
    {
        //  return sprintf(rbuf, "<BADCOMMAND>\r\n");
        return 0;
    }
    else
    {
        memset(data_II,0,64);
        memcpy(data_II,data,index);//ָ��copy��data_II
    }

    m_tempdata.currentUartIndex=uartno;


    //���� data ��ȡID DI.........................................
    i = 0;
    memset(data_buf,0,MAX_PKGLEN);
    memcpy(data_buf,data,len);
    if(0 == Utils_CheckCmdStr(data_buf))
        {return 0;}
    p = strtok(data_buf, ",");
    while(p)
    {
        switch(i)
        {
        case 1: temp_di = p;  break;//У��DI
        case 2: temp_id = p;  break;//У�� ID
        default:    {break;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    //��DI ��DI != �豸DI ���� ��= YALL �д� ������
    if((0 != strcmp(temp_di,(char *)&sensor_di)) && (0 != strcmp(temp_di,"YALL")))
        {return 0; }

    //��ID
    if(0 == strcmp(temp_di,"YALL")) //DI=YALL
    {
        if(0 != strcmp(temp_id,"FFF"))   { return 0; }
    }
    else //DI=�豸DI
    {
        if( 3 != strlen(temp_id))   //ID����Ϊ3
            { return 0; }
        if(-1 == check_digit(temp_id, strlen(temp_id)))
            { return 0; }
        id_num = atoi(temp_id);
        if(id_num != m_defdata.m_baseinfo.id)
            { return 0; }

    }
     //������
    for(  i=0; i<MAX_COMMAND_NUM; i++)
    {
        //  if(0== memcmp(data_II, bcm_control.dispatch[i].name,index))
        if(0== strcmp(data_II, bcm_control.dispatch[i].name))
        {
            ret = (bcm_control.dispatch[i].action)(data, rbuf);     //�ص�����
            m_tempdata.currentCmdIndex=i;
            return ret;
        }
    }
    return  sprintf(rbuf, "<%s,%03d,BADCOMMAND>\r\n",sensor_di,m_defdata.m_baseinfo.id);
}



int check_closecom(char *buf,char *rbuf)
{
    int rlen=0;
    if(!memcmp(buf,"CLOSECOM",8))
    {
        m_tempdata.m_opencomset_t.flag=false;
        m_tempdata.m_opencomset_t.counter=0;
        rlen=sprintf(rbuf,"<T>\r\n");
        return rlen;
    }
    else
        return 0;
}

time_t get_softimer(s_RtcTateTime_t *temp_t)
{
    struct tm temp;
    time_t data = 0;

    temp.tm_year = temp_t->year - 1900;
    temp.tm_mon = temp_t->month - 1;
    temp.tm_mday = temp_t->day;
    temp.tm_hour = temp_t->hour;
    temp.tm_min = temp_t->min;
    temp.tm_sec = temp_t->sec;
    data = mktime(&temp);
    data = data / 60;

    return data;
}

time_t get_softimer_sec(s_RtcTateTime_t *temp_t)
{
    struct tm temp;
    time_t data = 0;

    temp.tm_year = temp_t->year - 1900;
    temp.tm_mon = temp_t->month - 1;
    temp.tm_mday = temp_t->day;
    temp.tm_hour = temp_t->hour;
    temp.tm_min = temp_t->min;
    temp.tm_sec = temp_t->sec;
    data = mktime(&temp);

    return data;
}

/*==================================================================
*������             cmd_setcom
*���ߣ�
*���ڣ�           2017-12-07
*���ܣ�           B.1�����û��ȡ�豸��ͨѶ����
           ������ �豸��ʶ��,�豸ID,������,����λ,��żУ��,ֹͣλ
           �����ʷ�Χ����1200,2400,4800,9600, 19200,38400,57600,115200��
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_setcom(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    unsigned long int temp_rate = 0;
    unsigned char temp_data, temp_stop;
             char *temp_par = NULL;
    char num = 0;
    unsigned long  baudrate_tab[] = {1200,2400,4800,9600, 19200,38400,57600,115200};

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2: break;
         /* {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
            if(-1 == check_digit(p, strlen(p)))//У�� ���ڲ����� �Ƿ�Ϊ����
            {goto err;}
            temp_rate = atol(p);
            break;
          }
        case 4:
          {
            if(-1 == check_digit(p, 1))//У�� ����λ �Ƿ�Ϊ����
            {goto err;}
            temp_data = atoi(p);
            break;
          }
        case 5:{ temp_par = p; break;}
        case 6:
          {
            if(-1 == check_digit(p, 1))//У��  ֹͣλ �Ƿ�Ϊ����
            {goto err;}
            temp_stop = atoi(p);
            break;
          }
        case 7: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 7) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
      { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
      { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
    rlen = sprintf((char *)rbuf,"<%s,%03d,%ld,%d,%c,%d>\r\n",
                              sensor_di,
                              m_defdata.m_baseinfo.id,
                              bcm_info.common.se[0].baudrate,
                              bcm_info.common.se[0].datasbit,
                              bcm_info.common.se[0].parity,
                              bcm_info.common.se[0].stopbits);
                return rlen;
    }

    //2 У�� ���ڲ�����  ��д��
    for(num = 0; num < 8; num ++)
    {
        if(temp_rate == baudrate_tab[num])
        {   bcm_info.common.se[0].baudrate = temp_rate;
            break;
        }
    }
    if(num == 8)
    {goto err;}
    //3 У�� ����λ  ��д��
    if((temp_data < 5) || (temp_data > 8))
    {goto err;}
    bcm_info.common.se[0].datasbit = temp_data;

    //4 У�� У��λ  ��д��
    if((0 != strncmp((char *)temp_par,"N",1)) && (0 != strncmp((char *)temp_par,"O",1)) && (0 != strncmp((char *)temp_par,"E",1)))
    {goto err;}
    strncpy((char *)&bcm_info.common.se[0].parity,temp_par,1);

    //5 У�� У��λ  ��д��
    if((1 != temp_stop) && (2 != temp_stop))
    {goto err;}
    bcm_info.common.se[0].stopbits = temp_stop;

    //6 д��FLASH  �����سɹ�
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n%ld,%d,%c,%d\r\n",
                              sensor_di,m_defdata.m_baseinfo.id,

                              bcm_info.common.se[0].baudrate,
                              bcm_info.common.se[0].datasbit,
                              bcm_info.common.se[0].parity,
                              bcm_info.common.se[0].stopbits);
    m_tempdata.event.uart_config = true;
    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*������             cmd_autocheck
*���ߣ�
*���ڣ�           2017-12-07
*���ܣ�          B.2���豸�Լ�
         �豸���ڡ�ʱ�䣬ͨѶ�˿ڵ�ͨѶ�������豸״̬��Ϣ(�����Զ���)��
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_autocheck(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
     /*   if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
        }*/
        p = strtok(NULL, ",");
        i++;
    }
  if(i != 3) {goto err;}
    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
 /*   if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/

     // ��ӡ ��ȡ
    rlen = sprintf((char *)rbuf,"<%s,%03d,T,%04d-%02d-%02d,%02d:%02d:%02d,%ld,%d,%c,%d>\r\n",
                  sensor_di,
                  m_defdata.m_baseinfo.id,

                  m_tempdata.m_RtcTateTime.year,
                  m_tempdata.m_RtcTateTime.month,
                  m_tempdata.m_RtcTateTime.day,
                  m_tempdata.m_RtcTateTime.hour,
                  m_tempdata.m_RtcTateTime.min,
                  m_tempdata.m_RtcTateTime.sec,

                  bcm_info.common.se[0].baudrate,
                  bcm_info.common.se[0].datasbit,
                  bcm_info.common.se[0].parity,
                  bcm_info.common.se[0].stopbits
                  //�豸״̬��Ϣ���������ж����ʽ������
                  );
    return rlen;


err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}

/*==================================================================
*������             cmd_help
*���ߣ�
*���ڣ�           2017-12-07
*���ܣ�          B.3����������
         �豸��ʶ�����豸ID���ն������嵥
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_help(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;

  if(0 == Utils_CheckCmdStr(buf))
  {goto err;}
  p = strtok(buf, ",");
  while(p)
  {
   /*   if(1 == i) {temp_di = p;}
      if(2 == i)
      {
          if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
          {goto err;}
          temp_id = atoi(p);
      }*/
      p = strtok(NULL, ",");
      i++;
  }
  if(i != 3) {goto err;}
  //1����֤�豸��ʶ��,ID�Ƿ���ȷ
  //��֤DI
 /* if(0 != strcmp(temp_di,(char *)&sensor_di))
      {goto err;}
  //��֤ID
  if(temp_id != m_defdata.m_baseinfo.id)
      {goto err;}*/

    rlen = sprintf((char *)rbuf,"<%s,%03d,SETCOM,AUTOCHECK,HELP,QZ,ST,DI,ID,LAT,LONG,UNIT,DATE,TIME,DATETIME,FTD,STDEV,FAT,SETCOMWAY,"
                   "SS,STAT,AT,VV,SN,QCPS,QCPM,CR,READDATA,DOWN,RESET>\r\n",
                         sensor_di,
                         m_defdata.m_baseinfo.id
                                         );
    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*������             cmd_qz
*���ߣ�
*���ڣ�           2017-12-07
*���ܣ�        B.4�����û��ȡ�豸����վ��
         �豸��ʶ��,�豸ID ,�豸��վ�ţ�6λ�ַ���
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_qz(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    unsigned long temp_qz = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;
         /* {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
            if(-1 == check_digit(p, 5))//У�� ��վ�� �Ƿ�Ϊ���� 5λ
            {goto err;}
            temp_qz = atol(p);
            break;
          }
        case 7: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
    rlen = sprintf((char *)rbuf,"<%s,%03d,%ld>\r\n",
                              sensor_di,
                              m_defdata.m_baseinfo.id,
                              bcm_info.common.qz);
                return rlen;
    }

    //2 У��QZ �Ƿ�Ϊ5λ��
    if((temp_qz >99999) || (temp_qz < 10000))
      { goto err; }

    //3 д��FLASH  �����سɹ�
    bcm_info.common.qz = temp_qz;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
      rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}

/*==================================================================
*������             cmd_st
*���ߣ�
*���ڣ�           2017-12-07
*���ܣ�       B. 5�����û��ȡ�豸�ķ�������
         �豸��ʶ��,�豸ID �������ͣ�2λ���֣�
ʾ����00��׼վ��01����վ��02һ��վ��03��������վ��04��ͨ����վ��05��������վ��06ũҵ����վ��07��������վ��
08��������վ��09��������վ��10̫��������վ��11��̬����վ��12�������վ��13��Яվ��14�Զ�����վ��15 ��������վ��
16 �����ɷ�վ��17 ɳ����վ��18��������վ��
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_st(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    short temp_st = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
         /* {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
            if(-1 == check_digit(p, 2))//У�� ��������  �Ƿ�Ϊ���� 2λ
            {goto err;}
            temp_st = atoi(p);
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
    rlen = sprintf((char *)rbuf,"<%s,%03d,%02d>\r\n",
                              sensor_di,
                              m_defdata.m_baseinfo.id,
                              bcm_info.common.st);
                return rlen;
    }

    //2 У��ST �Ƿ��ڹ涨��Χ��
    if((temp_st > 99) || (temp_st < 0))
      { goto err; }

    //3 д��FLASH  �����سɹ�
    bcm_info.common.st = temp_st;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}

/*==================================================================
*������              cmd_di
*���ߣ�
*���ڣ�           2017-12-08
*���ܣ� B.6����ȡ�豸��ʶλ
      �豸��ʶ��,FFF
"YAWS","YROS","YCCL","YCCR","YCLR","YFSV","YLTV","YWTQ","YWTR","YWDP","YWDY"
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_di(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    char *dev_di = NULL;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i) {dev_di  = p;}
        p = strtok(NULL, ",");
        i++;
    }
      if(i != 3) {goto err;}
    //1����֤
    //��֤YALL
   if(0 != strcmp(temp_di,"YALL"))
      { goto err; }
    
    //��֤FFF 
    if(3 != strlen(dev_di))   { goto err; }
	if(0 != strncmp(dev_di,"FFF",3))//У�� �Ƿ�ΪFFF
      { goto err; }
   
    //��ӡ ��ȡ
    rlen = sprintf((char *)rbuf,"<%s,%03d>\r\n",
                           sensor_di,
                           m_defdata.m_baseinfo.id
                                           );
     return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}

/*==================================================================
*������              cmd_id
*���ߣ�
*���ڣ�           2017-12-08
*���ܣ� B.7�����û��ȡ�豸
      �豸��ʶ��,3λ����
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_id(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    char *dev_id = NULL;
    short temp_id = 0;
    short old_id = 0;
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i) {dev_id = p;}
        if(3 == i){
            if(-1 == check_digit(p, strlen(p))) //У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }
      if((i != 3) && (i != 4)) {goto err;}
    //1����֤
    //��֤YALL
    if((0 == strcmp(temp_di,"YALL")) && (i == 3)) //��ȡ
   {
     if(3 != strlen(dev_id) )   {goto err;}
     if(0 == strncmp(dev_id,"FFF",3))
     {   //��ӡ ��ȡ
        rlen = sprintf((char *)rbuf,"<%s,%03d>\r\n",
                           sensor_di,
                           m_defdata.m_baseinfo.id
                                           );
        return rlen;
     }
     else
     {return 0;}
   }
    else if((0 == strcmp(temp_di,(char *)sensor_di)) && (i == 4))	//����
    {
      if(3 != strlen(dev_id) )   {return 0;}
      if(-1 == check_digit(dev_id, 3)) //У�� ID�Ƿ�Ϊ����
          {return 0;}
      if(atoi(dev_id) != m_defdata.m_baseinfo.id)
          { return 0; }
      if((temp_id <0) || (temp_id > 999))
          {goto err;}

      old_id = m_defdata.m_baseinfo.id;
      m_defdata.m_baseinfo.id = temp_id;

    }
    else
      { goto err; }

    //7�� ���� flash ������
    pamsave(&m_defdata);

    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,old_id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}

/*==================================================================
*������              cmd_lat
*���ߣ�
*���ڣ�           2017-12-08
*���ܣ�B. 8�����û��ȡ����۲�վ��γ��
    �豸��ʶ��,3λ����
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_lat(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int degree, min, sec;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
          /*{
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
            if(-1 == check_format(p, strlen(p), ".", 2))
              {goto err;}
            if(sscanf(p,"%d.%d.%d",&degree,&min,&sec)!=3)
              {goto err;}
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
			rlen = sprintf((char *)rbuf,"<%s,%03d,%02d.%02d.%02d>\r\n",
                                             sensor_di,
                                             m_defdata.m_baseinfo.id,
                                             bcm_info.common.Lat.degree,
                                             bcm_info.common.Lat.min,
                                             bcm_info.common.Lat.sec
                                             );
       return rlen;;
    }
    
    
    //2 ��֤lat��ֵ׼ȷ��
    if((degree < 0) || (degree > 90))
    	{ goto err; }
    if((min < 0) || (min > 59))
    	{ goto err; }
    if((sec < 0) || (sec > 59))
    	{ goto err; }	
    if((degree == 90) && ((min != 0) || (sec != 0)))
        { goto err; }
    //3 д�� ����flash ������
    bcm_info.common.Lat.degree = degree;
    bcm_info.common.Lat.min    = min;
    bcm_info.common.Lat.sec    = sec;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}
/*==================================================================
*������              cmd_long
*���ߣ�
*���ڣ�           2017-12-08
*���ܣ�B. 9�����û��ȡ����۲�վ�ľ���
    �豸��ʶ��,3λ����
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_long(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int degree, min, sec;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
       /*   {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
          	if(-1 == check_format(p, strlen(p), ".", 2))
          	 {goto err;}
            if(sscanf(p,"%d.%d.%d",&degree,&min,&sec)!=3)
             {goto err;}
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%03d.%02d.%02d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.Long.degree,
                                         bcm_info.common.Long.min,
                                         bcm_info.common.Long.sec
                                         );
       return rlen;;
    }
    
    //2 ��֤long��ֵ׼ȷ��
    if((degree < 0) || (degree > 180))
    	{ goto err; }
    if((min < 0) || (min > 59))
    	{ goto err; }
    if((sec < 0) || (sec > 59))
    	{ goto err; }	
    if((degree == 180) && ((min != 0) || (sec != 0)) )
        { goto err; }
    //3 д�� ����flash ������
    bcm_info.common.Long.degree = degree;
    bcm_info.common.Long.min    = min;
    bcm_info.common.Long.sec    = sec;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}
/*==================================================================
*������              cmd_asl
*���ߣ�
*���ڣ�           2017-12-08
*���ܣ� B.28�����û��ȡ����۲�վ�ĺ��Σ�  ��������
    �豸��ʶ��,3λ���֣�5λ���֣�����ʮ���洢
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_asl(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    long temp_asl = 0;

    if(0 == Utils_CheckCmdStr(buf))
     {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
         /* {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
            if(5 < strlen(p))
            {goto err;}
            if((0 != strncmp(p,"-",1)) && (-1 == check_digit(p, 1)))
             {goto err;}
          	if(-1 == check_digit(p+1, strlen(p+1)))//У�� ����
          	 {goto err;}
            temp_asl = atol(p);
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%05ld>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.High
                                         );
       return rlen;;
    }
    
    //2 ��֤long��ֵ׼ȷ��
    if((temp_asl < -1540) || (temp_asl > 88440))//��ʱѡȡ 8844�����߶ȣ� -154 ��³�����
    	{ goto err; }

    //3 д�� ����flash ������
    bcm_info.common.High = temp_asl;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}
/*==================================================================
*������              cmd_date
*���ߣ�
*���ڣ�           2017-12-10
*���ܣ�  B.10�����û��ȡ�豸����
    �豸��ʶ��,�豸ID,YYYY-MM-DD��YYYYΪ�꣬MMΪ�£�DDΪ�գ�
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_date(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int year, mon, day;
    s_RtcTateTime_t temp_datetime;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
         /* {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
          {
          	if(10 !=  strlen(p)) //У�鳤��
          	 {goto err;}
          	 if(-1 == check_format(p, strlen(p), "-", 2))
          	 {goto err;}
            if(sscanf(p,"%d-%d-%d",&year,&mon,&day)!=3)
             {goto err;}
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%04d-%02d-%02d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         m_tempdata.m_RtcTateTime.year,
                                         m_tempdata.m_RtcTateTime.month,
                                         m_tempdata.m_RtcTateTime.day
                                         );
       return rlen;;
    }
    
    //2 У��������ֵ׼ȷ��
        if( -1 == check_data(year, mon, day))
            {goto err; }


        temp_datetime.year  = year;
        temp_datetime.month = mon;
        temp_datetime.day   = day;
        temp_datetime.hour  = m_tempdata.m_RtcTateTime.hour;
        temp_datetime.min   = m_tempdata.m_RtcTateTime.min;
        temp_datetime.sec   = m_tempdata.m_RtcTateTime.sec;
        check_datetime(&temp_datetime);

        //3 д��ṹ�� д��ʱ�� ������ȷ
        m_tempdata.m_RtcTateTime.year  =year;
        m_tempdata.m_RtcTateTime.month =mon;
        m_tempdata.m_RtcTateTime.day   =day;
        //DS3231_SetTime(&m_tempdata.m_RtcTateTime);
        rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
          return rlen;

    err:
      rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
      return rlen;
}
/*==================================================================
*������              cmd_time
*���ߣ�
*���ڣ�           2017-12-10
*���ܣ�  B.11�����û��ȡ�豸ʱ��
    �豸��ʶ��,�豸ID,HH:MM:SS��HHΪʱ��MMΪ�֣�SSΪ�룩
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_time(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int hour ,min, sec;
    s_RtcTateTime_t temp_datetime;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
         /* {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
            break;
          }   */
        case 3:
          {
          	if(8 !=  strlen(p)) //У�鳤��
          	 {goto err;}
          	 if(-1 == check_format(p, strlen(p), ":", 2))
          	 {goto err;}
            if(sscanf(p,"%d:%d:%d",&hour ,&min, &sec)!=3)
             {goto err;}
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%02d:%02d:%02d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         m_tempdata.m_RtcTateTime.hour,
                                         m_tempdata.m_RtcTateTime.min,
                                         m_tempdata.m_RtcTateTime.sec
                                         );
       return rlen;;
    }
    
    //2 У��������ֵ׼ȷ��
    if( -1 == check_time(hour, min, sec))
        {goto err; }

    temp_datetime.year  = m_tempdata.m_RtcTateTime.year;
    temp_datetime.month = m_tempdata.m_RtcTateTime.month;
    temp_datetime.day   = m_tempdata.m_RtcTateTime.day;
    temp_datetime.hour  = hour;
    temp_datetime.min   = min;
    temp_datetime.sec   = sec;
    check_datetime(&temp_datetime);

    //3 д��ṹ�� д��ʱ�� ������ȷ
    m_tempdata.m_RtcTateTime.hour  =hour;
    m_tempdata.m_RtcTateTime.min   =min;
    m_tempdata.m_RtcTateTime.sec   =sec;
    //DS3231_SetTime(&m_tempdata.m_RtcTateTime);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}

/*==================================================================
*������              cmd_datetime
*���ߣ�
*���ڣ�           2017-12-10
*���ܣ�  B.12�����û��ȡ�豸������ʱ��
    �豸��ʶ��,�豸ID,YYYY-MM-DD,HH:MM:SS
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_datetime(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int year, mon, day, hour ,min, sec;
    s_RtcTateTime_t temp_datetime;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
         /* {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
            break;
          }*/
        case 3:
        {
        	if(10 !=  strlen(p)) //У�鳤�� ���� 0x0d+0x0a
        	 {goto err;}
        	 if(-1 == check_format(p, strlen(p), "-", 2))
        	 {goto err;}
          if(sscanf(p,"%d-%d-%d",&year,&mon,&day)!=3)
           {goto err;}
          break;
        }  
        case 4:
          {
          	if(8 !=  strlen(p)) //У�鳤��
          	 {goto err;}
          	 if(-1 == check_format(p, strlen(p), ":", 2))
          	 {goto err;}
            if(sscanf(p,"%d:%d:%d",&hour ,&min, &sec)!=3)
             {goto err;}
            break;
          }
        case 5: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 5) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%04d-%02d-%02d,%02d:%02d:%02d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,

                                         m_tempdata.m_RtcTateTime.year,
                                         m_tempdata.m_RtcTateTime.month,
                                         m_tempdata.m_RtcTateTime.day,
                                         
                                         m_tempdata.m_RtcTateTime.hour,
                                         m_tempdata.m_RtcTateTime.min,
                                         m_tempdata.m_RtcTateTime.sec
                                         );
       return rlen;
    }
 		
 		//2 У��������ֵ׼ȷ��
    if( -1 == check_data(year, mon, day))
        {goto err; }               
    //3 У��ʱ����ֵ׼ȷ��
    if( -1 == check_time(hour, min, sec))
        {goto err; }

    temp_datetime.year  = year;
    temp_datetime.month = mon;
    temp_datetime.day   = day;
    temp_datetime.hour  = hour;
    temp_datetime.min   = min;
    temp_datetime.sec   = sec;
    check_datetime(&temp_datetime);

    //4 д��ṹ�� д��ʱ�� ������ȷ
    m_tempdata.m_RtcTateTime.year  =year;
    m_tempdata.m_RtcTateTime.month =mon;
    m_tempdata.m_RtcTateTime.day   =day;

    m_tempdata.m_RtcTateTime.hour  =hour;
    m_tempdata.m_RtcTateTime.min   =min;
    m_tempdata.m_RtcTateTime.sec   =sec;
    
    //DS3231_SetTime(&m_tempdata.m_RtcTateTime);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*������              cmd_ftd
*���ߣ�
*���ڣ�           2017-12-10
*���ܣ�  B.13�����û��ȡ�豸������ʱ��
            �豸��ʶ��,�豸ID ,FI ,mmC
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_ftd(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    short temp_fi,temp_min;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
        /*{
          if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(-1 == check_digit(p, 3))//У�� FI�Ƿ�Ϊ����
           {goto err;}
        	temp_fi=atoi(p);
          break;
        }  
        case 4:
        {
        	if(-1 == check_digit(p, 2))//У�� ʱ����
           {goto err;}
          if(0!= strncmp(p+2,"M",1))
           {goto err;} 
      		temp_min=atoi(p);
        	break;
        }
        case 5: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 5) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,001,%02dM>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.ftd
                                         );
       return rlen;
    }
    
    
    //2 У��FI ֻ����001
 	  if(temp_fi != 1) 
  	{ goto err; }  
    //3 У�鷢����
    if((temp_min < 1 ) || (temp_min > 59 ))
    { goto err; } 
    	
    //4 д��ṹ�� д��ʱ�� ������ȷ
    bcm_info.common.ftd = temp_min;
    save_sys_cfg(&bcm_info);	
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	

}
/*==================================================================
*������              cmd_stdev
*���ߣ�
*���ڣ�           2017-12-10
*���ܣ�  B.14�����û��ȡ�豸���ݱ�׼ƫ��ֵ�����ʱ����
            �豸��ʶ��,�豸ID ,FI ,mmC
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_stdev(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    short temp_fi,temp_stdev;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
     /*   {
          if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(-1 == check_digit(p, 3))//У�� FI�Ƿ�Ϊ����
           {goto err;}
        	temp_fi=atoi(p);
          break;
        }  
        case 4:
        {
        	if(-1 == check_digit(p, 2))//У�� ʱ����
           {goto err;}
          if(0!= strncmp(p+2,"M",1))
           {goto err;} 
          temp_stdev=atoi(p);
        	break;
        }
        case 5: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 5) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,001,%02dM>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.stdev
                                         );
       return rlen;
    }
    
    
    //2 У��FI ֻ����001
 	  if(temp_fi != 1) 
  	{ goto err; }  
    //3 У�鷢����
    if((temp_stdev < 1 ) || (temp_stdev > 59 ))
    { goto err; } 
    	
    //4 д��ṹ�� д��ʱ�� ������ȷ
    bcm_info.common.stdev = temp_stdev;
    save_sys_cfg(&bcm_info);	
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	
}
/*==================================================================
*������              cmd_fat
*���ߣ�
*���ڣ�           2017-12-10
*���ܣ�  B.15�����û��ȡ�豸����ģʽ�����ݷ���ʱ��
            �豸��ʶ��,�豸ID,FI,mmm
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_fat(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    short temp_fi,temp_sec;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
      /*  {
          if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(-1 == check_digit(p, 3))//У�� FI�Ƿ�Ϊ����
           {goto err;}
        	temp_fi=atoi(p);
          break;
        }  
        case 4:
        {
        	if(-1 == check_digit(p, 3))//У�� ʱ����  �Ƿ�Ϊ����
           {goto err;}
      		temp_sec=atoi(p);
        	break;
        }
        case 5: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 5) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%03d,%03d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.fi,
                                         bcm_info.common.ft
                                         );
       return rlen;
    }
    
    
    //2 У��FI ֻ����001
 	  if(temp_fi != 1) 
  	{ goto err; }  
    //3 У�鷢����
    if((temp_sec < 0 ) || (temp_sec > 600 ))
    { goto err; } 
    	
    //4 д��ṹ�� д��ʱ�� ������ȷ
    bcm_info.common.ft = temp_sec;
    save_sys_cfg(&bcm_info);	
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	
  
}
/*==================================================================
*������              cmd_setcomway
*���ߣ�
*���ڣ�           2017-12-10
*���ܣ�  B.16���������ֻ��Ʒ�ʽ
     �豸��ʶ��,�豸ID,���ַ�ʽ��1Ϊ�������ͷ�ʽ��0Ϊ������ȡ��ʽ�� Ĭ�ϱ���
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_setcomway(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    char com_way = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
     /*   if(1 == i)  {temp_di = p;}
        if(2 == i)
          {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
          }*/
        if(3 == i) 
        	{
        	if(-1 == check_digit(p, 1))//У��  ���ַ�ʽ�Ƿ�Ϊ����
            {goto err;}
            com_way = atoi(p);
        	}
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}
    //1����֤
    //��֤DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
       {
           rlen = sprintf((char *)rbuf,"<%s,%03d,%d>\r\n",
                                            sensor_di,
                                            m_defdata.m_baseinfo.id,
                                            bcm_info.common.sw
                                            );
          return rlen;
       }

 
 	  //2 У������ֵ
 	  if((0 != com_way) && (1 != com_way))
 	  	{ goto err; }
 	  //3 ����ͨ�ŷ�ʽ ���سɹ�
    bcm_info.common.sw = com_way;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",
                           sensor_di,
                           m_defdata.m_baseinfo.id
                                           );
     return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;
}
char buf_QS[90]={0}, buf_QM[150]={0};
/*==================================================================
*������             cmd_ss
*���ߣ�
*���ڣ�           2017-12-10
*���ܣ�          B.17����ȡ�豸����������ֵ
�豸����������ֵ��Ӧ�������н������õĲ���������˳������Ӧ������ֵ�˳��
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_ss(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, j = 0;
    int  rlen = 0, len_QS = 0, len_QM = 0;
    

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
    /*    if(1 == i)  {temp_di = p;}
        if(2 == i)
          {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
          }*/
        p = strtok(NULL, ",");
        i++;
    }
      if(i != 3) {goto err;}
    //1����֤
    //��֤DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }
 */
    //QCPS,QCPM
    for(i = 0; i < DEV_TYPE_MAX; i ++)
    {
        if(0 == strncmp((char *)&sensor_di,(char *)dev_type[i],4))
          break;
    }
    if(i == 6)   //��
    {

        len_QS = sprintf((char *)buf_QS,",1,%.1f,%.1f,%.1f,2,%.1f,%.1f,/",
                                    bcm_info.sensor.qs[0].min,      //����
                                    bcm_info.sensor.qs[0].max,      //����
                                    bcm_info.sensor.qs[0].change,

                                    bcm_info.sensor.qs[1].min,      //����
                                    bcm_info.sensor.qs[1].max       //����
                                    );
       for(j = 11 ; j < 15 ; j++)//����
           len_QM +=  sprintf((char *)buf_QM+len_QM,",%d,%.1f,%.1f,%.1f,%.1f,/",
                          j,
                          bcm_info.sensor.qm[j-11].min,        //����
                          bcm_info.sensor.qm[j-11].max,        //����
                          bcm_info.sensor.qm[j-11].doubtful,   //��������
                          bcm_info.sensor.qm[j-11].err         //��������
                          );
       for(j = 21 ; j < 25 ; j++)//����
          len_QM +=  sprintf((char *)buf_QM+len_QM,",%d,%.1f,%.1f,/,/,%.1f",
                         j,
                         bcm_info.sensor.qm[j-17].min,        //����
                         bcm_info.sensor.qm[j-17].max,        //����
                         bcm_info.sensor.qm[j-17].changerate  //��СӦ�ñ仯������
                         );
    }
    else if(i == 9)//����
    {
        for(j  = 0 ; j < MAX_SENSOR_NUM ; j++)
            len_QS += sprintf((char *)buf_QS+len_QS,",%d,%.1f,%.1f,%.1f",
                                     j+1,
                                     bcm_info.sensor.qs[j].min,      //����
                                     bcm_info.sensor.qs[j].max,      //����
                                     bcm_info.sensor.qs[j].change
                                     );
        for(j  = 0 ; j < MAX_SENSOR_NUM ; j++)
            len_QM +=  sprintf((char *)buf_QM+len_QM,",%d,%.1f,%.1f,%.1f,%.1f,/",
                                    j+11,
                                    bcm_info.sensor.qm[j].min,        //����
                                    bcm_info.sensor.qm[j].max,        //����
                                    bcm_info.sensor.qm[j].doubtful,   //��������
                                    bcm_info.sensor.qm[j].err         //��������
                                    );
    }
    else
    {
        for(j  = 0 ; j < MAX_SENSOR_NUM ; j++)
            len_QS += sprintf((char *)buf_QS+len_QS,",%d,%.1f,%.1f,%.1f",
                                     j+1,
                                     bcm_info.sensor.qs[j].min,      //����
                                     bcm_info.sensor.qs[j].max,      //����
                                     bcm_info.sensor.qs[j].change
                                     );
        for(j  = 0 ; j < MAX_SENSOR_NUM ; j++)
            len_QM +=  sprintf((char *)buf_QM+len_QM,",%d,%.1f,%.1f,%.1f,%.1f,%.1f",
                                    j+11,
                                    bcm_info.sensor.qm[j].min,        //����
                                    bcm_info.sensor.qm[j].max,        //����
                                    bcm_info.sensor.qm[j].doubtful,   //��������
                                    bcm_info.sensor.qm[j].err,         //��������
                                    bcm_info.sensor.qm[j].changerate  //��СӦ�ñ仯������
                                    );
    }

 	  //2 ���״̬ ���سɹ�
    rlen = sprintf((char *)rbuf,"<%s,%03d,SETCOM,%ld,%d,%c,%d,"
                           "QZ,%ld,ST,%02d,DI,%s,ID,%03d,"
                           "LAT,%02d.%02d.%02d,LONG,%03d.%02d.%02d,ASL,%ld,"
                           "DATE,%04d-%02d-%02d,TIME,%02d:%02d:%02d,"
                           "FTD,%02dM,STDEV,%02dM,FAT,%03d,SETCOMWAY,%01d,"
                           "STAT,%05d,%05d,%05d,AT,%05d,%05d,VV,%05d,%05d,SN,%s,",
                           sensor_di,
                           m_defdata.m_baseinfo.id,
                           
                           bcm_info.common.se[0].baudrate,
                           bcm_info.common.se[0].datasbit,
                           bcm_info.common.se[0].parity,
                           bcm_info.common.se[0].stopbits,
                           
                           bcm_info.common.qz,
                           bcm_info.common.st,
                           sensor_di,
                           m_defdata.m_baseinfo.id,
                           
                           bcm_info.common.Lat.degree,
                           bcm_info.common.Lat.min,
                           bcm_info.common.Lat.sec,
                           bcm_info.common.Long.degree,
                           bcm_info.common.Long.min,
                           bcm_info.common.Long.sec,
                           bcm_info.common.High,
                           
                           m_tempdata.m_RtcTateTime.year,
                           m_tempdata.m_RtcTateTime.month,
                           m_tempdata.m_RtcTateTime.day,
                           m_tempdata.m_RtcTateTime.hour,
                           m_tempdata.m_RtcTateTime.min,
                           m_tempdata.m_RtcTateTime.sec,

                           bcm_info.common.stdev,
                           bcm_info.common.ftd,
                           bcm_info.common.ft,
                           bcm_info.common.sw,

                           sensors_data.boardstemp_data,
                           sensors_data.boardsvolt_data,
                           sensors_data.boardsvolt_data,
                           bcm_info.common.work_temp_min,
                           bcm_info.common.work_temp_max,
                           bcm_info.common.boardsvolt_min,
                           bcm_info.common.boardsvolt_max,
                           m_defdata.m_baseinfo.sn
                           //����������Ĺ�������-----------------------
                                           );

    rlen +=sprintf((char *)rbuf+rlen,"QCPS%s,QCPM%s>\r\n",buf_QS,buf_QM);

    return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

}
/*==================================================================
*������             cmd_stat
*���ߣ�
*���ڣ�           2017-12-10
*���ܣ�          B.18����ȡ�豸����״̬����
         �豸��ʶ�����豸ID
         �����ڲ���·�¶ȡ��ڲ���·��ѹ����ӵ�Դ��ѹ������״̬�Զ���
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_stat(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}

    p = strtok(buf, ",");
    while(p)
    {
       /* if(1 == i)  {temp_di = p;}
        if(2 == i)
          {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
          }*/
        p = strtok(NULL, ",");
        i++;
    }
      if(i != 3) {goto err;}
    //1����֤
    //��֤DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
 
 	  //2 ���״̬ ���سɹ�
    rlen = sprintf((char *)rbuf,"<%s,%03d,%05d,%05d,%05d>\r\n",
                           sensor_di,
                           m_defdata.m_baseinfo.id,
                           sensors_data.boardstemp_data,
                           sensors_data.boardsvolt_data,
                           sensors_data.boardsvolt_data
                     //����������Ĺ���״̬-----------------------
                                           );
     return rlen;

err:
    rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

}
/*==================================================================
*������              cmd_at
*���ߣ�
*���ڣ�           2017-12-11
*���ܣ�  B.19�����û��ȡ�豸���������¶ȷ�Χ
    �豸��ʶ�����豸ID�����������¶�����ֵ�����������¶�����ֵ,-100.0��100.0
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_at(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int temp_min,temp_max;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
       /* {
          if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if((0 !=  strncmp(p,"-",1)) &&  (0 !=  strncmp(p,"0",1)))//��֤���ΪΪ - �� 0
        	 {goto err;}
        	if(-1 == check_digit(p+1, 4))//У�� ʣ���Ƿ�Ϊ����
           {goto err;}
        	temp_min=atoi(p);
          break;
        }  
        case 4:
        {
        	if((0 !=  strncmp(p,"-",1)) &&  (0 !=  strncmp(p,"0",1)))//��֤���ΪΪ - �� 0
        	 {goto err;}
        	if(-1 == check_digit(p+1, 4))//У�� ʣ���Ƿ�Ϊ����
           {goto err;}
      		temp_max=atoi(p);
        	break;
        }
        case 5: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 5) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
    /*if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%05d,%05d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.work_temp_min,
                                         bcm_info.common.work_temp_max
                                         );
       return rlen;
    }
    
    //2 У�� �����޷�Χ
    if((temp_min < -1000 ) || (temp_min > 1000 ))
    	{ goto err; }
    if((temp_max < -1000 ) || (temp_max > 1000 ))
    	{ goto err; }	
    if(temp_max < temp_min )
    	{ goto err; }		
    	
    //3 д��ṹ�� д��ʱ�� ������ȷ
    bcm_info.common.work_temp_min = temp_min;
    bcm_info.common.work_temp_max = temp_max;
    save_sys_cfg(&bcm_info);

    //4 ���¼���������
    board_temp_max = bcm_info.common.work_temp_max * 1.1;
    if(bcm_info.common.work_temp_min < 0)
    {
        board_temp_min = bcm_info.common.work_temp_min * 1.1;
    }
    else
    {
        board_temp_min = bcm_info.common.work_temp_min * 0.9;
    }

    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	
}
/*==================================================================
*������              cmd_vv
*���ߣ�
*���ڣ�           2017-12-11
*���ܣ�  B.20�����û��ȡ��������������ѹ��Χ
    �豸��ʶ�����豸ID������������ѹ����ֵ������������ѹ����ֵ,
    ֵΪ5λ����λ���㣬ȡֵ��ΧΪ0��30V
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_vv(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    int temp_min,temp_max;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2: break;//
       /* {
          if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(-1 == check_digit(p, 5))//У�� ʣ���Ƿ�Ϊ����
           {goto err;}
        	temp_min=atoi(p);
          break;
        }  
        case 4:
        {
        	if(-1 == check_digit(p, 5))//У�� ʣ���Ƿ�Ϊ����
           {goto err;}
      		temp_max=atoi(p);
        	break;
        }
        case 5: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 5) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%05d,%05d>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         bcm_info.common.boardsvolt_min,
                                         bcm_info.common.boardsvolt_max
                                         );
       return rlen;
    }
    
    //2 У�� �����޷�Χ
    if((temp_min < 0 ) || (temp_min > 300 ))
    	{ goto err; }
    if((temp_max < 0 ) || (temp_max > 300 ))
    	{ goto err; }	
    if(temp_max < temp_min )
    	{ goto err; }		
    	
    //3 д��ṹ�� д��ʱ�� ������ȷ
    bcm_info.common.boardsvolt_min = temp_min;
    bcm_info.common.boardsvolt_max = temp_max;
    save_sys_cfg(&bcm_info);

    board_volt_max = bcm_info.common.boardsvolt_max * 1.1;
    board_volt_min = bcm_info.common.boardsvolt_min * 0.9;

    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	
}
/*==================================================================
*������              cmd_sn
*���ߣ�
*���ڣ�           2017-12-11
*���ܣ�  B.21�����û��ȡ�豸���к�
    �豸��ʶ�����豸ID
    ������ҪȨ��
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_sn(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    char *temp_sn = NULL;
    unsigned char temp_chack_sn = 0, temp_chack_num = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}

    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2: break;//
        /*{
          if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(SN_NUM != strlen(p))	//36λSN װ�������9+����ʶ����10+����������14+У��3
           {goto err;}
        	temp_sn=p;
          break;
        }  
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    if(i == 3)  // ��ӡ ��ȡ
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%s>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         m_defdata.m_baseinfo.sn
                                         );
       return rlen;
    }
    
    if(1 != m_tempdata.SuperadMin) //�ж��Ƿ��в���Ȩ��
    {goto err;}

    //2 SN�� У��  ֻ��֤У����
    temp_chack_num = atoi(temp_sn + SN_NUM - 3);
	for(i = 0; i< SN_NUM-3; i++)
	    temp_chack_sn = temp_chack_sn^(temp_sn[i]);
	if(temp_chack_sn != temp_chack_num)
	    {goto err;}
    	
    //3 ����ṹ�� д��ṹ�� д��ʱ�� ������ȷ
    memset(m_defdata.m_baseinfo.sn, 0x00, sizeof(m_defdata.m_baseinfo.sn));
    strncpy((char *)&m_defdata.m_baseinfo.sn, temp_sn,SN_NUM);
    pamsave(&m_defdata);

    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	
}
/*==================================================================
*������              cmd_qcps
*���ߣ�
*���ڣ�           2017-12-11
*���ܣ�  B.22�����û��ȡ����ֵ�������Ʋ���
�豸��ʶ�����豸ID����ӦԪ����ʶ��ţ��豸������Χ���ޣ��豸������Χ���ޣ��������仯ֵ��
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_qcps(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    short temp_num;
    float temp_min,temp_max;
    char *temp_change = NULL;
    
    qs_t temp_qs;
    
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
        /*{
          if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(-1 == check_digit(p, 1))//У�� ��Ӧԭ�����
           {goto err;}
        	temp_num=atoi(p);
          break;
        }  
        case 4:
        {
            if((0 !=  strncmp(p,"-",1)) &&(-1 == check_format(p, strlen(p), ".", 1)))//��֤���ΪΪ - �� 0
            {goto err;}
            if((0 ==  strncmp(p,"-",1)) && (-1 == check_format(p+1, strlen(p+1), ".", 1)))//У�� ������Χ����
            {goto err;}
            temp_min=atof(p);
        	break;
        }
        case 5:
        {
        	if(-1 == check_format(p, strlen(p), ".", 1))//У�� ������Χ����
        	{goto err;} 
            temp_max=atof(p);
        	break;
        }
        case 6:
        {
            temp_change = p;
        	break;
        }
        case 7: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 7) && (i != 4))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
    /*if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }
     */
    if(-1 == check_sensor(temp_num, &temp_qs))
    	{ goto err;}
      
    if(i == 4)  // ��ӡ ��ȡ
    {
        for(i = 0; i < DEV_TYPE_MAX; i ++)
        {
            if(0 == strncmp((char *)&sensor_di,(char *)dev_type[i],4))
              break;
        }
        if((i == 6) && (temp_num == 2))   //����
        {
            rlen = sprintf((char *)rbuf,"<%s,%03d,%d,%.1f,%.1f,/>\r\n",
                                        sensor_di,
                                        m_defdata.m_baseinfo.id,
                                        temp_num,
                                        temp_qs.min,
                                        temp_qs.max
                                        );
        }
        else
        {
            rlen = sprintf((char *)rbuf,"<%s,%03d,%d,%.1f,%.1f,%.1f>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         temp_num,
                                         temp_qs.min,
                                         temp_qs.max,
                                         temp_qs.change
                                         );

        }
        return rlen;
    }
    
    
    //2 У���Ӧԭ�����
 	  if(-1 == save_sensor(temp_num, temp_min,temp_max, temp_change, &temp_qs))
    	{ goto err;} 

    //4 д��ṹ�� д��FLASH ������ȷ
    memcpy(&bcm_info.sensor.qs[temp_num-1],&temp_qs,sizeof(qs_t));
      
    save_sys_cfg(&bcm_info);	
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	
}
/*==================================================================
*������              cmd_qcpm
*���ߣ�
*���ڣ�           2017-12-11
*���ܣ� B.23�����û��ȡ˲ʱ����ֵ�������Ʋ���
�豸��ʶ�����豸ID���۲�Ҫ�ر�ţ�Ҫ�ؼ�ֵ���ޣ�Ҫ�ؼ�ֵ���ޣ����ɽ��ޣ�������ޣ���СӦ�ñ仯�����ʡ�
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_qcpm(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    float temp_min,temp_max;
    char temp_num,*temp_change = NULL,*temp_doubt = NULL,*temp_err = NULL;
    qm_t temp_qm;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2: break;//
        /*{
          if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
            if(-1 == check_digit(p, 2))//У�� �۲�Ҫ�ر��
           {goto err;}
            temp_num=atoi(p);
          break;
        }  
        case 4:
        {
            if((0 !=  strncmp(p,"-",1)) &&(-1 == check_format(p, strlen(p), ".", 1)))//��֤���ΪΪ - �� 0
              {goto err;}
            if((0 ==  strncmp(p,"-",1)) && (-1 == check_format(p+1, strlen(p+1), ".", 1)))//У�� ������Χ����
            {goto err;}
          temp_min=atof(p);
            break;
        }
        case 5:
        {
            if(-1 == check_format(p, strlen(p), ".", 1))//У�� ������Χ����
            {goto err;}
          temp_max=atof(p);
            break;
        }
        case 6:{temp_doubt = p; break;} // ��������
        case 7:{temp_err   = p; break;} // ��������
        case 8:{temp_change= p; break;} // ��СӦ����
        case 9: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 9) && (i != 4))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
   /* if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/

    if(-1 == check_element(temp_num, &temp_qm))
        { goto err;}

    if(i == 4)  // ��ӡ ��ȡ
    {
        for(i = 0; i < DEV_TYPE_MAX; i ++)
        {
            if(0 == strncmp((char *)&sensor_di,(char *)dev_type[i],4))
              break;
        }
        if(i == 6)    //��
        {//��Χ 11~14,21~24
            switch(temp_num)
           {
           case 11://����
           case 12:
           case 13:
           case 14: rlen = sprintf((char *)rbuf,"<%s,%03d,%d,%.1f,%.1f,%.1f,%.1f,/>\r\n",
                                   sensor_di,
                                   m_defdata.m_baseinfo.id,
                                   temp_num,
                                   temp_qm.min,
                                   temp_qm.max,
                                   temp_qm.doubtful,
                                   temp_qm.err
                                   );
                       break;
           case 21: //����
           case 22:
           case 23:
           case 24:rlen = sprintf((char *)rbuf,"<%s,%03d,%d,%.1f,%.1f,/,/,%.1f>\r\n",
                                  sensor_di,
                                  m_defdata.m_baseinfo.id,
                                  temp_num,
                                  temp_qm.min,
                                  temp_qm.max,
                                  temp_qm.changerate
                                  );
                      break;
           default:{goto err;}
           }
           return rlen;
        }
        else if(i == 9)    //����
        {
            rlen = sprintf((char *)rbuf,"<%s,%03d,%d,%.1f,%.1f,%.1f,%.1f,/>\r\n",
                                               sensor_di,
                                               m_defdata.m_baseinfo.id,
                                               temp_num,
                                               temp_qm.min,
                                               temp_qm.max,
                                               temp_qm.doubtful,
                                               temp_qm.err
                                               );
            return rlen;
        }
        else
        {
          rlen = sprintf((char *)rbuf,"<%s,%03d,%d,%.1f,%.1f,%.1f,%.1f,%.1f>\r\n",
                                         sensor_di,
                                         m_defdata.m_baseinfo.id,
                                         temp_num,
                                         temp_qm.min,
                                         temp_qm.max,
                                         temp_qm.doubtful,
                                         temp_qm.err,
                                         temp_qm.changerate
                                         );
          return rlen;
        }
    }
    
    
    //2 У��Ҫ�ر��
    if(-1 == save_element(temp_num, temp_min,temp_max, temp_doubt, temp_err,temp_change, &temp_qm))
        { goto err;}


    //4 д��ṹ�� д��FLASH ������ȷ
    for(i = 0; i < DEV_TYPE_MAX; i ++)
    {
        if(0 == strncmp((char *)&sensor_di,(char *)dev_type[i],4))
          break;
    }
    if(i == 6)    //��
    {//��Χ 11~14,21~24
        switch(temp_num)
       {
       case 11: memcpy(&bcm_info.sensor.qm[WIND_VELOCITY_3_S  ],&temp_qm,sizeof(qm_t)); break;
       case 12: memcpy(&bcm_info.sensor.qm[WIND_VELOCITY_1_M  ],&temp_qm,sizeof(qm_t)); break;
       case 13: memcpy(&bcm_info.sensor.qm[WIND_VELOCITY_2_M  ],&temp_qm,sizeof(qm_t)); break;
       case 14: memcpy(&bcm_info.sensor.qm[WIND_VELOCITY_10_M ],&temp_qm,sizeof(qm_t)); break;
       case 21: memcpy(&bcm_info.sensor.qm[WIND_DIRECTION_0_M ],&temp_qm,sizeof(qm_t)); break;
       case 22: memcpy(&bcm_info.sensor.qm[WIND_DIRECTION_1_M ],&temp_qm,sizeof(qm_t)); break;
       case 23: memcpy(&bcm_info.sensor.qm[WIND_DIRECTION_2_M ],&temp_qm,sizeof(qm_t)); break;
       case 24: memcpy(&bcm_info.sensor.qm[WIND_DIRECTION_10_M],&temp_qm,sizeof(qm_t)); break;
       default:{goto err;}
       }
    }
    else if((i == 9) || (i == 10))//����
    {
        memcpy(&bcm_info.sensor.qm[(temp_num/10)-1],&temp_qm,sizeof(qm_t));
    }
    else
    {
        memcpy(&bcm_info.sensor.qm[temp_num-11],&temp_qm,sizeof(qm_t));
    }



    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*������              cmd_cr
*���ߣ�
*���ڣ�           2017-12-11
*���ܣ�  B.24�����û��ȡ�豸��У�����춨��Ϣ
    �豸��ʶ�����豸ID,У��/�춨��ʶ����ʱ�䣬���ޣ���Ա��ţ�������
    ������ҪȨ��
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_cr(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    short temp_cak =0, j = 0, temp_term = 0;
    unsigned long temp_number = 0;
    int year, mon, day, num;
    char buffer[8][Flash_PAGE_SIZE/2-9]={0};    //128byte - 2byte(year)-1byte(mon)-1byte(day)-1byte(term)-4byte(number)
    s_check_cr_t    cfc[8];
    s_check_info_t  temp_check_info;
    float temp_a = 0, temp_b = 0, temp_c = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}

    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
        /*{
          if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
          {goto err;}
          temp_id = atoi(p);
          break;
        }*/
        case 3:
        {
        	if(-1 == check_digit(p, 1))//У�����춨��ʶ����1=У�� 2=�춨��
          {goto err;}
          temp_cak = atoi(p);
          break;
        }  
        case 4:
        {
        	if(8 != strlen(p))//���� YYYYMMDD
          {goto err;}
          if(sscanf(p,"%04d%02d%02d",&year,&mon,&day)!=3)
           {goto err;}
          break;
        }  
        case 5:
        {
        	if(-1 == check_digit(p, 2))//��Ч�� 2λ����
        	{goto err;}
        	temp_term=atoi(p);
        	break;
        }  
        case 6:
        {
        	if(-1 == check_digit(p, 5))//У��Ա 5λ����
        	{goto err;}
        	temp_number=atol(p);
        	break;
        }  
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        {
            if((Flash_PAGE_SIZE/2-7) <= strlen(p))//���� YYYYMMDD
            {goto err;}
            strcpy(buffer[i-7],p);
            break;
        }
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(!( ((i>=8) && (i<=15)) || (i==4)))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ,У����Ϣ����
    //��֤DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    //��֤��Ϣ���� 
    if((temp_cak != 1) && (temp_cak != 2))
      { goto err; } 
    if(i == 4)  // ��ӡ ��ȡ
    {
        if(temp_cak == 1)
        {
            if(m_tempdata.CfcFlag == true)
                 return 0;
            else
            {
                m_tempdata.CfcFlag = true;
                cfc_offset = 0;
            }
        }
        if(temp_cak == 2)
        {
           if(m_tempdata.CheckFlag == true)
                return 0;
           else
           {
               m_tempdata.CheckFlag = true;
               check_offset = 0;
           }
        }
       return 0;
    }
    
    if(1 != m_tempdata.SuperadMin) //�ж��Ƿ��в���Ȩ��
    {goto err;}
    //2 У�� д��ʱ��
    if(-1 == check_data(year, mon, day))
    	{goto err;}   	
    temp_check_info.year = year;
    temp_check_info.mon  = mon;
    temp_check_info.day  = day;
    //3 У�� д������	
    if((temp_term < 0) || (temp_term > 99))
    	{goto err;}   	
    temp_check_info.term = temp_term;
    //3 У�� д����Ա���	
    if(temp_number > 99999)
    	{goto err;}
    temp_check_info.number = temp_number;

    //4 �ж� У�� or �춨  ��Ϣ
    memset(cfc,0,sizeof(cfc));
    if(temp_cak == 1)   //У��
    {
        for(j=0;j<8;j++)
        {
            if(sscanf(buffer[j],"%d#%f:%f:%f",&num,&temp_a,&temp_b,&temp_c)!=4)
                {goto err;}
            else
            {
                bcm_info.sensor.cr[num-1].a = temp_a;
                bcm_info.sensor.cr[num-1].b = temp_b;
                bcm_info.sensor.cr[num-1].c = temp_c;

                cfc[j].number = num;
                cfc[j].cr.a   = temp_a;
                cfc[j].cr.b   = temp_b;
                cfc[j].cr.c   = temp_c;

                temp_check_info.len = j+1;
            }
            if(j == (i-8))
                break;
        }
        if(0 == save_cr(temp_cak,&temp_check_info,(char *)cfc))
            {goto err;}
    }
    if(temp_cak == 2)   //�춨
    {
        temp_check_info.len = strlen(buffer[0]);
        if(0 == save_cr(temp_cak,&temp_check_info,(char *)buffer))
            {goto err;}
    }

    //3 д��ṹ�� д��ʱ�� ������ȷ
    save_sys_cfg(&bcm_info);	
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;	

}
/*==================================================================
*������              cmd_readdata
*���ߣ�
*���ڣ�           2017-12-11
*���ܣ�  B.25��ʵʱ��ȡ����
�豸��ʶ��,�豸ID,֡��ʶ
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_readdata(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    unsigned long temp_time_now = 0;
    unsigned char temp_fi = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2: break;//
          /*{
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
            break;
          }      */
        case 3:
         {
            if(-1 == check_digit(p, 3))//У���Ƿ���֡��ʶ
            {goto err;}
            temp_fi = atoi(p);
            break;
          }
        case 4: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
      
    }
    if(( i != 4) && (i != 3))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    
    //2 ������ǰʱ����
    temp_time_now = get_softimer(&m_tempdata.m_RtcTateTime);
    
    //����ǰ������ʷ���ݷ���״̬ ��ִ�д������� ����Ӧ
    if(0 != m_tempdata.Count_read_history)
        return 0;
    
    if(i == 3)  // ��ӡ ��ȡ��ǰ������
    {
        m_tempdata.time_start_read = temp_time_now;
        m_tempdata.Count_read_history =  1;
        return 0;
    }
    
    //3 У��FI ��ȡֵ��Χ (0~83)(100~183)
 	if(temp_fi > 183)
  	{ goto err; }  
    if((temp_fi > 83) && (temp_fi < 100))
  	{ goto err; } 
    
    //4 ȡ������λ��ֵ
    temp_fi = temp_fi % 100;
 
    
    //5 �ж϶�ȡ�������ݻ���Сʱ����
    if(temp_fi <=60 )//����
  	{
  		m_tempdata.time_interval_read = temp_fi;
  	}
    else
  	{		
  		m_tempdata.time_interval_read = (temp_fi-60+1)*60;
  		
  	}
  	//6 ��ȡʱ�� ���ö�ȡ����������־
    m_tempdata.time_start_read = temp_time_now - (temp_time_now % m_tempdata.time_interval_read);
    m_tempdata.Count_read_history = 1;
    return 0;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*������              cmd_down
*���ߣ�
*���ڣ�           2017-12-11
*���ܣ�  B.26����ʷ��������
�豸��ʶ��,�豸ID,֡��ʶ
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_down(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    unsigned char temp_fi = 0;
    unsigned long temp_time_now = 0, temp_time_start = 0, temp_time_end = 0;
    s_RtcTateTime_t start,end;
    int temp_a = 0, temp_b = 0, temp_c = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;//{temp_di = p;break;}
        case 2:break;//
        /*  {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
            break;
          }     */
        case 3:
         {
          if(10 !=  strlen(p)) //У�鳤��
            {goto err;}
          if(-1 == check_format(p, 10, "-", 2))
            {goto err;}
          if(3 != sscanf(p,"%d-%d-%d",&temp_a, &temp_b, &temp_c) )
            {goto err;}
          if( -1 == check_data(temp_a, temp_b, temp_c))
       		{goto err; }
          start.year  = (unsigned int )  temp_a ;
          start.month = (unsigned char) (temp_b & 0x00ff);
 		  start.day   = (unsigned char) (temp_c & 0x00ff);
          break;
          }  
        case 4:
          {
			 if(8 !=  strlen(p)) //У�鳤��
          	 {goto err;}
          	 if(-1 == check_format(p, 8, ":", 2))
          	 {goto err;}
             if(3 != sscanf(p,"%d:%d:%d",&temp_a, &temp_b, &temp_c) )
             {goto err;}
            if(-1 == check_time(temp_a, temp_b, temp_c))
        		{goto err; } 
            start.hour = (unsigned char) (temp_a & 0x00ff);
            start.min  = (unsigned char) (temp_b & 0x00ff);
            start.sec  = (unsigned char) (temp_c & 0x00ff);
            break;
          }
        case 5:
        {
        	if(10 !=  strlen(p)) //У�鳤��
            {goto err;}
          if(-1 == check_format(p, 10, "-", 2))
            {goto err;}
          if(3 != sscanf(p,"%d-%d-%d", &temp_a, &temp_b, &temp_c) )
           {goto err;}
          if( -1 == check_data(temp_a, temp_b, temp_c))
       		 {goto err; }
          end.year  = (unsigned int )  temp_a ;
          end.month = (unsigned char) (temp_b & 0x00ff);
          end.day   = (unsigned char) (temp_c & 0x00ff);
          break;
        }  
        case 6:
          {
          	if((10 !=  strlen(p)) && (8 !=  strlen(p))) //У�鳤��
          	 {goto err;}
          	if(-1 == check_format(p, 8, ":", 2))
          	 {goto err;}
            if(3 != sscanf(p,"%d:%d:%d", &temp_a, &temp_b, &temp_c))
             {goto err;}
            if( -1 == check_time(temp_a, temp_b, temp_c))
        	{goto err; }
            end.hour = (unsigned char) (temp_a & 0x00ff);
            end.min  = (unsigned char) (temp_b & 0x00ff);
            end.sec  = (unsigned char) (temp_c & 0x00ff);
            break;
          }       
        case 7:
         {
            if(-1 == check_digit(p, 3))//У���Ƿ���֡��ʶ
            {goto err;}
            temp_fi = atoi(p);
            break;
          }
        case 8: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;
      
    }
    if(( i != 8) && (i != 7))   {goto err;}

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
    /*if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/
    
    //����ǰ������ʷ���ݷ���״̬ ��ִ�д������� ����Ӧ
    if(0 != m_tempdata.Count_down_history)
        return 0;

    //2 ������ǰʱ����
    temp_time_now   = get_softimer(&m_tempdata.m_RtcTateTime);
    temp_time_start = get_softimer(&start);
    temp_time_end   = get_softimer(&end);
    
    //3 У��ʱ��
    if(temp_time_end < temp_time_start)
        { goto err; }
    if(temp_time_now < temp_time_start)
        { goto err; }
    if(temp_time_end > temp_time_now)
        { temp_time_end = temp_time_now; }

    if(i == 7)  // ��ӡ ʱ�����ÿһ���ӵ�����
    {

        m_tempdata.time_start = temp_time_start;
        m_tempdata.Count_down_history =  temp_time_end - temp_time_start + 1;
        m_tempdata.time_interval = 1;
        return 0;
    }
    
    //4 У��FI ��ȡֵ��Χ (0~83)(100~183)
 	if(temp_fi > 183)
  	{ goto err; }  
    if((temp_fi > 83) && (temp_fi < 100))
  	{ goto err; } 
    
    //5 ȡ������λ��ֵ
    temp_fi = temp_fi % 100;
    
    //6 �ж϶�ȡ�������ݻ���Сʱ����
    if(temp_fi <=60 )//����
  	{
  		m_tempdata.time_interval = temp_fi;
  	}
    else
  	{		
  		m_tempdata.time_interval = (temp_fi - 60 + 1) * 60;
  		
  	}
  	//7 ��ȡʱ�� ���ö�ȡ����������־
    if(0 == (temp_time_start % 60))
    {
        m_tempdata.time_start = temp_time_start;
    }
    else
    {
        if(0 != ((temp_time_start % 60) % m_tempdata.time_interval))
            m_tempdata.time_start = temp_time_start + (m_tempdata.time_interval-((temp_time_start % 60) % m_tempdata.time_interval));
        else
            m_tempdata.time_start = temp_time_start;
    }

    m_tempdata.Count_down_history = (temp_time_end - m_tempdata.time_start) / m_tempdata.time_interval + 1;

    if(m_tempdata.Count_down_history > 720)
        m_tempdata.Count_down_history = 720;

    return 0;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;

}
/*==================================================================
*������              cmd_reset
*���ߣ�
*���ڣ�           2017-12-11
*���ܣ�  B.27�������豸
�豸��ʶ�����豸ID
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_reset(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
     /*   if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
        }*/
        p = strtok(NULL, ",");
        i++;
    }
    if(i != 3) {goto err;}
    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
/*    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }
*/
    //2 ���Ź�ֹͣ����    �豸����
    //WDTCTL=WDT_ARST_1000;
    //Stop_Timer0();
    while(1)
    {
        m_tempdata.alive_counter=250;
    }
 
    
err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*������              cmd_superadmin
*���ߣ�
*���ڣ�           2017-12-14
*���ܣ�  B.29������Ȩ������
�豸��ʶ�����豸ID
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_superadmin(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
       /* if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
        }*/
        p = strtok(NULL, ",");
        i++;
    }
    if(i != 3) {goto err;}
    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
 /*   if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/

    //2 ����Ȩ�� ׼��
    m_tempdata.SuperadMin = 1;
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
/*==================================================================
*������              cmd_unit
*���ߣ�
*���ڣ�           2017-12-25
*���ܣ�  B.30 �����û��ȡ������ѹ������������ݵĵ�λ   ��Ӧ��ѹ���ܹ���������17��
�豸��ʶ�����豸ID
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_unit(char *buf,char *rbuf)
{
    //char *temp_di = NULL;
    //unsigned short temp_id = 0;
    char *p = NULL, i = 0, rlen = 0;
    char temp_unit = 0;

    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        /*if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, 3))//У�� ID�Ƿ�Ϊ����
            {goto err;}
            temp_id = atoi(p);
        }*/
        if(3 == i)
        {
            if(-1 == check_digit(p, 1))//У�鵥λ�Ƿ�Ϊ����
            {goto err;}
            temp_unit = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }
    if((i != 3) && (i != 4)) {goto err;}
    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
  /*  if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }*/

    if(i == 3)  //��ȡ
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d,%d>\r\n",sensor_di,m_defdata.m_baseinfo.id,bcm_info.sensor.apunit);
        return rlen;
    }

    //2 д����ѹ��λ ������ ��ӡ
    if((temp_unit < 1) || (temp_unit > 4))
      { goto err; }

    bcm_info.sensor.apunit = temp_unit;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<%s,%03d,T>\r\n",sensor_di,m_defdata.m_baseinfo.id);
    return rlen;

err:
  rlen = sprintf((char *)rbuf,"<%s,%03d,F>\r\n",sensor_di,m_defdata.m_baseinfo.id);
  return rlen;
}
//================================================== ����������ú��� ===========================================
/*==================================================================
** �������� ��unsigned char Utils_CheckCmdStr(char *pStr)
** �������� ������յ��ַ�����ʽ�Ƿ����Ҫ��
** ��ڲ��� ��pStr��Ҫ�����ַ���ָ��
**
** ���ڲ��� ��0����ʽ����
**     ������ֵ������ַ���һ���м��Σ��Զ���Ϊ�ָ�����
==================================================================*/
int Utils_CheckCmdStr(char *pStr)
{
  char char_1;
  unsigned char sectionNum;
  char *pChar_1;

  if(pStr == 0) return 0;

  // �жϿ�ͷ�Ƿ��ж���
  char_1 = *pStr;
  if(char_1 == ',') return 0;

  // ���س��滻Ϊ�ַ���������
  pChar_1 = strchr(pStr,'\r');
  if(pChar_1 == 0) return 0;
  if(pChar_1 == pStr)  return 0;
  *pChar_1 = '\0';

  // �ж��Ƿ��������ķָ����ͽ�β�Ƿ��зָ���
  sectionNum = 1;
  while(1)
  {
    char_1 = *pStr++;
    if(char_1 == '\0')  return sectionNum;
    if(char_1 != ',')   continue;

    sectionNum++;

    char_1 = *pStr++;
    if(char_1 == '\0') return 0;  // �ַ�����β�зָ���
    if(char_1 == ',') return 0;   // ����ָ�������
  }
}
/*==================================================================
 *  ��   ��     ����check_digit
 *  ��         �ߣ�xhf
 *  ��         �ڣ�2014-05-23
 *  ��            �ܣ�����ַ����Ƿ�ȫΪ����
 *  ���������
 *         char *p (�����ַ�����
 *         uint8 len (�����ַ������ȣ�
 *  ��  ��    ֵ�� ���ͣ�int  )
 *          ����0        ��ʾȫΪ����
 *          ����-1      ��ʾ��������Ϊ���ַ�
 *  �޸ļ�¼��
==================================================================*/
int check_digit(char *p, uint8 len)
{
    uint8 i;
    for(i=0; i<len; i++)
    {
        if(0 == isdigit(*(p+i)))
        {
            return -1;
        }
    }
    return 0;
}
/*==================================================================
 *  ��   ��     ����check_format
 *  ��         �ߣ�
 *  ��         �ڣ�2017-12-13
 *  ��            �ܣ�����ַ����Ƿ�ȫΪ����,�ָ����Ƿ�����Ҫ��
 *  ���������
 *         char *p (�����ַ�����
 *         uint8 len (�����ַ������ȣ�
 *         char *buf �ָ���
 *         uint8 num �ָ�������
 *  ��  ��    ֵ�� ���ͣ�int  )
 *          ����0        ��ʾȫΪ����
 *          ����-1      ��ʾ��������Ϊ���ַ�
 *  �޸ļ�¼��
==================================================================*/
int check_format(char *p, uint8 len, char *buf, uint8 num)
{
    uint8 i = 0, j = 0;
    for(i=0; i<len; i++)
    {
        if(0 == strncmp((p+i),buf,1)) //���Ϊ 0
        {
            j++;
            if(j>num)
            {
                return -1;
            }
            continue;
        }
        if(0 == isdigit(*(p+i)))    //����0 ��ʾ������
        {
            return -1;
        }
    }
    return 0;
}

/*==================================================================
*������             check_data
*���ߣ�
*���ڣ�		2017-12-11
*���ܣ�   У������
*���룺   year, month, day		int�͵�������
*
 *  ��  ��    ֵ�� ���ͣ�int  )
 *          ����0        д�����ڳɹ�
 *          ����-1       д������ʧ��
*�޸ļ�¼��
==================================================================*/
int check_data(int year, int month, int day)
{
	unsigned char month_lab[]={0,31,28,31,30,31,30,31,31,30,31,30,31};


	  //У�������Ƿ����Ҫ��
	  if((((year%4)==0)&&((year%100)!=0))||((year%400)==0))//���꣬����2������
	  {
	      month_lab[2]=29;
	  }
	  if((year < 2000) || (year > 2100))
	      {goto err; }
	  if((month>12) || (month < 1))
	      {goto err; }
	  if((day > month_lab[month]) || (day < 1))
	      {goto err; }
	  return 0;
err:
    return -1;
}

/*==================================================================
*������             check_time
*���ߣ�
*���ڣ�2017-12-11
*���ܣ�У��ʱ��
*���룺  hour, min, sec  int�͵�ʱ����

 *  ��  ��    ֵ�� ���ͣ�int  )
 *          ����0        д�����ڳɹ�
 *          ����-1       д������ʧ��
*�޸ļ�¼��2017-12-10��LY���޸ķ���ֵ���޸�У��
==================================================================*/
int check_time(int hour, int min, int sec)
{

    //У��ʱ���Ƿ����Ҫ��
    if((hour>23) || (hour < 0))
        {goto err;}
    if((min>59) || (min < 0))
        {goto err;}
    if((sec>59) || (sec < 0))
        {goto err;}
        
    return 0;
err:
    return -1;
}
/*==================================================================
*������             check_datetime
*���ߣ�
*���ڣ�2017-12-11
*���ܣ�У��ʱ��
*���룺  hour, min, sec  int�͵�ʱ����

 *  ��  ��    ֵ�� ���ͣ�int  )
 *          ����0        д�����ڳɹ�
 *          ����-1       д������ʧ��
*�޸ļ�¼��2017-12-10��LY���޸ķ���ֵ���޸�У��
==================================================================*/
int check_datetime(s_RtcTateTime_t *datetime)
{
    unsigned long temp_time_now = 0, temp_time_chack = 0;
    unsigned long temp,timep;
    unsigned long pageIndex;
    unsigned long addr;
    unsigned char i;
    //У��ʱ���Ƿ����Ҫ��
 //   GPIO_setOutputLowOnPin(GPIO_PORT_P2,  GPIO_PIN3);

    temp_time_now   = get_softimer_sec(&m_tempdata.m_RtcTateTime);
    temp_time_chack = get_softimer_sec(datetime);

    temp = abs(temp_time_chack - temp_time_now);    //�����
    timep = temp_time_now/60;

    //1 ��ǰ���Ӳ���
    if((temp > 60) || (datetime->min != m_tempdata.m_RtcTateTime.min))
    {
        for(i = 0; i< 1; i++)                         //����sector��
       {
           pageIndex = ((timep-16*i) % Flash_PAGE_NUMBER) + FLASH_DATA_START;   // ����λ���ĸ�����
           addr = pageIndex << 8;
           W25Q128_Erase_Chip(W25Q128_ERS_SEC, addr);
       }
    }
    else    //��ǰ������
    {   //��ǰ�����ʱ  ���ҵ�ǰʱ��С��3s
       if((m_tempdata.m_RtcTateTime.sec <= 3) && (temp_time_chack > temp_time_now))
        {
            for(i = 0; i< 1; i++)                         //����sector��
            {
                pageIndex = ((timep-16*i) % Flash_PAGE_NUMBER) + FLASH_DATA_START;   // ����λ���ĸ�����
                addr = pageIndex << 8;
                if((pageIndex & 0x0F) == 0) // �ж��Ƿ�Ϊһ��sector�Ŀ�ʼ
                    W25Q128_Erase_Chip(W25Q128_ERS_SEC,addr);
            }
        }
    }

   // GPIO_setOutputHighOnPin(GPIO_PORT_P2,  GPIO_PIN3);
    return 0;
}

/*==================================================================
*������             check_sensor
*���ߣ�
*���ڣ�2017-12-16
*���ܣ���鴫������źʹ����������Ƿ����
���ش������� ������ �� ���仯ֵ
*���룺   *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
 *  ��  ��    ֵ�� ���ͣ�int  )
 *          ����0        д�����ڳɹ�
 *          ����-1       д������ʧ��
*�޸ļ�¼��2017-12-10��LY���޸ķ���ֵ���޸�У��
==================================================================*/
int check_sensor(short temp_num, qs_t *temp_qs)
{
    unsigned char value;

    for(value = 0; value < DEV_TYPE_MAX; value ++)
    {
      if(0 == strncmp((char *)&sensor_di,(char *)dev_type[value],4))
          break;
    }

	switch(value)
	{
		case 6 :	//��
		{
			if((temp_num < 1) || (temp_num >2))	{goto err;}
			memcpy(temp_qs,	(char *)&bcm_info.sensor.qs[temp_num - 1],sizeof(qs_t));
		}break;
		case 9:	//����
		{
			if((temp_num < 1) || (temp_num >MAX_SENSOR_NUM))	{goto err;}
			memcpy(temp_qs,	(char *)&bcm_info.sensor.qs[temp_num - 1],sizeof(qs_t));
		}break;
		case 10:	//����ˮ��
		{
		    if((temp_num < 1) || (temp_num >MAX_SENSOR_NUM)) {goto err;}
			memcpy(temp_qs,	(char *)&bcm_info.sensor.qs[temp_num - 1],sizeof(qs_t));
		}break;
		case 5:	//��ѹ
		{
		    if(temp_num != 1)  {goto err;}

		    if(bcm_info.sensor.apunit == 3)
		    {
		        temp_qs->change = bcm_info.sensor.qs[temp_num - 1].change * 0.750062;
		        temp_qs->max    = bcm_info.sensor.qs[temp_num - 1].max    * 0.750062;
		        temp_qs->min    = bcm_info.sensor.qs[temp_num - 1].min    * 0.750062;
		    }
		    else if(bcm_info.sensor.apunit == 4)
            {
                temp_qs->change = bcm_info.sensor.qs[temp_num - 1].change * 0.029530;
                temp_qs->max    = bcm_info.sensor.qs[temp_num - 1].max    * 0.029530;
                temp_qs->min    = bcm_info.sensor.qs[temp_num - 1].min    * 0.029530;
            }
		    else
		    {
		        memcpy(temp_qs, (char *)&bcm_info.sensor.qs[temp_num - 1],sizeof(qs_t));
		    }
		}break;
		case 12://����
		case 11://�ܷ���
		case 2:	//ʪ��
		case 3:	//�¶�
		case 8:	//����
		case 14:	//����
		{
			if(temp_num != 1)	{goto err;}
			memcpy(temp_qs,	(char *)&bcm_info.sensor.qs[temp_num - 1],sizeof(qs_t));
		}break;
		default:	{goto err;}
	}
	return 0;
	
err:	return -1;		
}

/*==================================================================
*������             save_sensor
*���ߣ�
*���ڣ�2017-12-16
*���ܣ���鴫������źʹ����������Ƿ����
���� �������� ������ �� ���仯ֵ
*���룺   *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
 *  ��  ��    ֵ�� ���ͣ�int  )
 *          ����0        д�����ڳɹ�
 *          ����-1       д������ʧ��
*�޸ļ�¼��2017-12-10��LY���޸ķ���ֵ���޸�У��
==================================================================*/
int save_sensor(short temp_num, float temp_min, float temp_max, char *temp_change, qs_t *temp_qs)
{
    unsigned char value;

       for(value = 0; value < DEV_TYPE_MAX; value ++)
       {
           if(0 == strncmp((char *)&sensor_di,(char *)dev_type[value],4))
             break;
       }

	switch(value)
	{
		case 6:	//��
		{
			if(1 == temp_num)//����
			{
				if(temp_min < 0)			{goto err;}
				if(temp_max > 200)			{goto err;}
				if(-1 == check_format(temp_change, strlen(temp_change), ".", 1))  {goto err;}
				if((atof(temp_change) < 0) || (atof(temp_change) > 200))		    {goto err;}
				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = atof(temp_change);
			}	
			else if(2 == temp_num)	//����
			{
				if(temp_min < 0)			{goto err;}
				if(temp_max > 360)		    {goto err;}
                if(0 != strcmp(temp_change,"/"))                {goto err;}
				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = INVALID_DATA;
			}	
			else
				{goto err;}	
		}break;
		case 9:	//����
		{
			if((0 < temp_num) && (MAX_SENSOR_NUM >= temp_num))
			{
				if(temp_min < -100) 		{goto err;}
				if(temp_max > 100)  		{goto err;}
				if(temp_max < temp_min)		{goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change), ".", 1))  {goto err;}
				if((atof(temp_change) > 10) || (atof(temp_change) < 0))		    {goto err;}
				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = atof(temp_change);
			}	
			else
				{goto err;}	
		}break;
		case 10:	//����ˮ��
		{
			if((0 < temp_num) && (MAX_SENSOR_NUM >= temp_num))
			{
				if(temp_min < 0) 			{goto err;}
				if(temp_max > 100)			{goto err;}
				if(temp_max < temp_min) 	{goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change), ".", 1))  {goto err;}
                if((atof(temp_change) > 20) || (atof(temp_change) < 0))          {goto err;}

				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = atof(temp_change);
			}	
			else
				{goto err;}	
		}break;
//		case 12,	//����
//		case 11,	//�ܷ���
		
		case 2:	//ʪ��
		{
			if(1 == temp_num)
			{
				if(temp_min < 0) 			{goto err;}
				if(temp_max > 100)			{goto err;}
				if(temp_max < temp_min) 	{goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change), ".", 1))  {goto err;}
                if((atof(temp_change) > 20) || (atof(temp_change) < 0))          {goto err;}

				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = atof(temp_change);
			}	
			else
				{goto err;}	
		}break;
		case 3:	//�¶�
		{
			if(1 == temp_num)
			{
				if(temp_min < -100) 		{goto err;}
				if(temp_max > 100)			{goto err;}
				if(temp_max < temp_min)	    {goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change), ".", 1))   {goto err;}
                if((atof(temp_change) > 10) || (atof(temp_change) < 0))           {goto err;}
				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = atof(temp_change);
			}	
			else
				{goto err;}	
		}break;
		case 5:	//��ѹ
		{
			if(1 == temp_num)
			{
			    if(3 == bcm_info.sensor.apunit) // * 0.750062
			    {
                    if(temp_min < 75.0076)          {goto err;}
                    if(temp_max > 1125.093)         {goto err;}
                    if(temp_max < temp_min)         {goto err;}
                    if(-1 == check_format(temp_change, strlen(temp_change), ".", 1)) {goto err;}
                    if((atof(temp_change) > 0.750062) || (atof(temp_change) < 0))       {goto err;}

                    temp_qs->min    = temp_min / 0.750062;
                    temp_qs->max    = temp_max / 0.750062;
                    temp_qs->change = atof(temp_change) / 0.750062;
			    }
			    else if(4 == bcm_info.sensor.apunit)// * 0.029530
			    {
                    if(temp_min < 2.953)          {goto err;}
                    if(temp_max > 44.295)         {goto err;}
                    if(temp_max < temp_min)       {goto err;}
                    if(-1 == check_format(temp_change, strlen(temp_change), ".", 1)) {goto err;}
                    if((atof(temp_change) > 0.029530) || (atof(temp_change) < 0))       {goto err;}

                    temp_qs->min    = temp_min / 0.029530;
                    temp_qs->max    = temp_max / 0.029530;
                    temp_qs->change = atof(temp_change) / 0.029530;
			    }
			    else
			    {
	                if(temp_min < 100)          {goto err;}
	                if(temp_max > 1500)         {goto err;}
	                if(temp_max < temp_min)     {goto err;}
	                if(-1 == check_format(temp_change, strlen(temp_change), ".", 1)) {goto err;}
	                if((atof(temp_change) > 1) || (atof(temp_change) < 0))       {goto err;}

	                temp_qs->min    = temp_min;
	                temp_qs->max    = temp_max;
	                temp_qs->change = atof(temp_change);
			    }
			}	
			else
				{goto err;}
		}break;
		case 8:	//����
		{
			if(1 == temp_num)
			{
				if(temp_min < 0) 			{goto err;}
				if(temp_max > 20)			{goto err;}
				if(temp_max < temp_min) 	{goto err;}
                if(0 != strcmp(temp_change,"/"))         {goto err;}

				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = INVALID_DATA;
			}	
			else
				{goto err;}
		}break;
		case 14:	//����
		{
			if(1 == temp_num)
			{
				if(temp_min < 0) 			{goto err;}
				if(temp_max > 20)			{goto err;}
				if(temp_max < temp_min)	    {goto err;}
                if(0 != strcmp(temp_change,"/"))         {goto err;}
				temp_qs->min    = temp_min;
				temp_qs->max    = temp_max;
				temp_qs->change = INVALID_DATA;
			}	
			else
				{goto err;}	
		}break;
		default:	{goto err;}
	}
	return 0;
	
err:	return -1;		
}
/*==================================================================
*������             check_element
*���ߣ�
*���ڣ�2017-12-16
*���ܣ���鴫������źʹ����������Ƿ����
���ش������� ������ �� ���仯ֵ
*���룺   *buf  (�������ݣ��������ݵ�ַ
*      *rbuf (������ݣ����ƽṹ��ַ
*
 *  ��  ��    ֵ�� ���ͣ�int  )
 *          ����0        д�����ڳɹ�
 *          ����-1       д������ʧ��
*�޸ļ�¼��2017-12-20��LY���޸ķ���ֵ���޸�У��
==================================================================*/
int check_element(short temp_num, qm_t *temp_qm)
{
    unsigned char value;

    memset(temp_qm,'/',sizeof(qm_t));
    for(value = 0; value < DEV_TYPE_MAX; value ++)
    {
      if(0 == strncmp((char *)&sensor_di,(char *)dev_type[value],4))
          break;
    }

    switch(value)
    {
        case 6 :    //��
        {//��Χ 11~14,21~24
            switch(temp_num)
            {
            case 11: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_VELOCITY_3_S],sizeof(qm_t));   break;
            case 12: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_VELOCITY_1_M],sizeof(qm_t));   break;
            case 13: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_VELOCITY_2_M],sizeof(qm_t));   break;
            case 14: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_VELOCITY_10_M],sizeof(qm_t));  break;
            case 21: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_DIRECTION_0_M],sizeof(qm_t));  break;
            case 22: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_DIRECTION_1_M],sizeof(qm_t));  break;
            case 23: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_DIRECTION_2_M],sizeof(qm_t));  break;
            case 24: memcpy(temp_qm, (char *)&bcm_info.sensor.qm[WIND_DIRECTION_10_M],sizeof(qm_t)); break;
            default: goto err;
            }
        }break;
        case 9: //����
        {
            switch(temp_num)
            {
            case 11:
            case 21:
            case 31:
            case 41:
            case 51:
            case 61:
            case 71:
            case 81:
                memcpy(temp_qm, (char *)&bcm_info.sensor.qm[(temp_num/10) - 1],sizeof(qm_t));  break;
            default: goto err;
            }
        }break;
        case 10:    //����ˮ��
        {
            switch(temp_num)
            {
            case 11:
            case 21:
            case 31:
            case 41:
            case 51:
                memcpy(temp_qm, (char *)&bcm_info.sensor.qm[(temp_num/10) - 1],sizeof(qm_t));
                break;
            default:   {goto err;}
            }
        }break;
        case 3: //�¶�
        {
            if((temp_num != 11) && (temp_num != 12))   {goto err;}
            memcpy(temp_qm, (char *)&bcm_info.sensor.qm[temp_num - 11],sizeof(qm_t));
        }break;
        case 12://����
        case 11://�ܷ���
        case 2: //ʪ��
        case 5: //��ѹ
        case 8: //����
        case 14://����
        {
            if(temp_num != 11)   {goto err;}
            memcpy(temp_qm, (char *)&bcm_info.sensor.qm[temp_num - 11],sizeof(qm_t));
        }break;
        default:    {goto err;}
    }
    return 0;
    
err:    return -1;
}
/*==================================================================
*������             save_sensor
*���ߣ�
*���ڣ�2017-12-16
*���ܣ���鴫������źʹ����������Ƿ����
���� �������� ������ �� ���仯ֵ
*���룺   *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
 *  ��  ��    ֵ�� ���ͣ�int  )
 *          ����0        д�����ڳɹ�
 *          ����-1       д������ʧ��
*�޸ļ�¼��2017-12-20��LY���޸ķ���ֵ���޸�У��
==================================================================*/
int save_element(short temp_num, float temp_min, float temp_max, char *temp_doubt, char *temp_err, char *temp_change, qm_t *temp_qm)
{
    unsigned char value;

       for(value = 0; value < DEV_TYPE_MAX; value ++)
       {
           if(0 == strncmp((char *)&sensor_di,(char *)dev_type[value],4))
             break;
       }

    switch(value)
    {
        case 6: //��
        {

            if((temp_num > 24) || (temp_num < 11))    {goto err;}
            if((temp_num > 14) && (temp_num < 21))    {goto err;}
            if(temp_min > temp_max)  {goto err;}
            //����  ��С�߽�
            if((temp_num > 21) && (temp_num < 24) && (temp_max > 360))  {goto err;}
            if((temp_num > 21) && (temp_num < 24) && (temp_min < 0))    {goto err;}
            //���� С�߽�
            if((temp_num > 11) && (temp_num < 14) && (temp_min < 0))    {goto err;}
            if((temp_num > 11) && (temp_num < 14) && (temp_max > 200))  {goto err;}

            //�ж����� ��������,����,��СӦ����
            if(2 == (temp_num /10))
            {
                if(0 != strcmp(temp_doubt,"/"))         {goto err;}
                if(0 != strcmp(temp_err  ,"/"))         {goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change), ".", 1))   {goto err;}

                if((0 > atof(temp_change) ) || (360 < atof(temp_change)))          {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = INVALID_DATA;
                temp_qm->err         = INVALID_DATA;
                temp_qm->changerate  = atof(temp_change);
            }
            //�ж����ٶ� ��������,����,��СӦ����
            if(1 == (temp_num /10))
            {

                if(-1 == check_format(temp_doubt, strlen(temp_doubt), ".", 1))  {goto err;}
                if(-1 == check_format(temp_err, strlen(temp_err), ".", 1))      {goto err;}
                if(0 != strcmp(temp_change,"/"))     {goto err;}

                if((0 > atof(temp_doubt)) || (200 < atof(temp_doubt)))        {goto err;}
                if((0 > atof(temp_err))   || (200 < atof(temp_err)))              {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = atof(temp_doubt);
                temp_qm->err         = atof(temp_err);
                temp_qm->changerate  = INVALID_DATA;
            }
         }break;
        case 9: //����
        {
            if((temp_num != 11) && (temp_num != 21) && (temp_num != 31) && (temp_num != 41) &&
                    (temp_num != 51)&& (temp_num != 61) && (temp_num != 71) && (temp_num != 81)) {goto err;}
            if(temp_min < -100)                 {goto err;}
            if(temp_max > 100)                  {goto err;}
            if(temp_max < temp_min)             {goto err;}
            if(-1 == check_format(temp_doubt, strlen(temp_doubt), ".", 1)) {goto err;}
            if(-1 == check_format(temp_err,   strlen(temp_err),   ".", 1)) {goto err;}
            if(0 != strncmp(temp_change  ,"/",1))    {goto err;}

            if((0 > atof(temp_doubt)) || (100 < atof(temp_doubt)))        {goto err;}
            if((0 > atof(temp_err))   || (100 < atof(temp_err)))              {goto err;}

            temp_qm->min         = temp_min;
            temp_qm->max         = temp_max;
            temp_qm->doubtful    = atof(temp_doubt);
            temp_qm->err         = atof(temp_err);
            temp_qm->changerate  = INVALID_DATA;
        }break;
        case 10:    //����ˮ��
        {
            if((temp_num != 11) && (temp_num != 21) && (temp_num != 31) && (temp_num != 41) && (temp_num != 51))  {goto err;}
            if(temp_max < temp_min)                {goto err;}
            if((temp_min < 0) || (temp_max > 100)) {goto err;}
            if(-1 == check_format(temp_doubt, strlen(temp_doubt), ".", 1)) {goto err;}
            if(-1 == check_format(temp_err,   strlen(temp_err),   ".", 1)) {goto err;}
            if(-1 == check_format(temp_change,   strlen(temp_change),   ".", 1)) {goto err;}

            if((atof(temp_doubt)  <0) || (atof(temp_doubt)  >100))  {goto err;}
            if((atof(temp_err)    <0) || (atof(temp_err)    >100))  {goto err;}
            if((atof(temp_change) <0) || (atof(temp_change) >100))  {goto err;}

            temp_qm->min         = temp_min;
            temp_qm->max         = temp_max;
            temp_qm->doubtful    = atof(temp_doubt);
            temp_qm->err         = atof(temp_err);
            temp_qm->changerate  = atof(temp_change);
        }break;
//      case 12,    //����
//      case 11,    //�ܷ���

        case 2: //ʪ��
        {
            if(11 == temp_num)
            {
                if(temp_min < 0)            {goto err;}
                if(temp_max > 100)          {goto err;}
                if(temp_max < temp_min)     {goto err;}

                if(-1 == check_format(temp_doubt, strlen(temp_doubt),   ".", 1))     {goto err;}
                if(-1 == check_format(temp_err,   strlen(temp_err),   ".", 1))     {goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change),   ".", 1))     {goto err;}

                if((atof(temp_doubt)  <0) || (atof(temp_doubt)  >100))  {goto err;}
                if((atof(temp_err)    <0) || (atof(temp_err)    >100))  {goto err;}
                if((atof(temp_change) <0) || (atof(temp_change) >100))  {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = atof(temp_doubt);
                temp_qm->err         = atof(temp_err);
                temp_qm->changerate  = atof(temp_change);
            }
            else
                {goto err;}
        }break;
        case 3: //�¶�
        {
            if((11 == temp_num) || (12 == temp_num))
            {
                if(temp_min < -100)         {goto err;}
                if(temp_max > 100)          {goto err;}
                if(temp_max < temp_min)     {goto err;}

                if(-1 == check_format(temp_doubt, strlen(temp_doubt),   ".", 1))     {goto err;}
                if(-1 == check_format(temp_err,   strlen(temp_err),   ".", 1))     {goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change),   ".", 1))     {goto err;}

                if((atof(temp_doubt)  <0) || (atof(temp_doubt)  >100))  {goto err;}
                if((atof(temp_err)    <0) || (atof(temp_err)    >100))  {goto err;}
                if((atof(temp_change) <0) || (atof(temp_change) >100))  {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = atof(temp_doubt);
                temp_qm->err         = atof(temp_err);
                temp_qm->changerate  = atof(temp_change);
            }
            else
                {goto err;}
        }break;
        case 5: //��ѹ
        {
            if(11 == temp_num)
            {
                if(temp_min < 100)          {goto err;}
                if(temp_max > 1500)         {goto err;}
                if(temp_max < temp_min)     {goto err;}

                if(-1 == check_format(temp_doubt, strlen(temp_doubt),   ".", 1))     {goto err;}
                if(-1 == check_format(temp_err,   strlen(temp_err),   ".", 1))     {goto err;}
                if(-1 == check_format(temp_change, strlen(temp_change),   ".", 1))     {goto err;}

                if((atof(temp_doubt)  <0) || (atof(temp_doubt)  >20))  {goto err;}
                if((atof(temp_err)    <0) || (atof(temp_err)    >20))  {goto err;}
                if((atof(temp_change) <0) || (atof(temp_change) >20))  {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = atof(temp_doubt);
                temp_qm->err         = atof(temp_err);
                temp_qm->changerate  = atof(temp_change);
            }
            else
                {goto err;}
        }break;
        case 8: //����
        {
            if(11 == temp_num)
            {
                if(temp_min < 0)            {goto err;}
                if(temp_max > 20)           {goto err;}
                if(temp_max < temp_min)     {goto err;}

                if(0 != strcmp(temp_doubt   ,"/"))    {goto err;}
                if(0 != strcmp(temp_err     ,"/"))    {goto err;}
                if(0 != strcmp(temp_change  ,"/"))    {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = INVALID_DATA;
                temp_qm->err         = INVALID_DATA;
                temp_qm->changerate  = INVALID_DATA;
            }
            else
                {goto err;}
        }break;
        case 14:    //����
        {
            if(11 == temp_num)
            {
                if(temp_min < 0)            {goto err;}
                if(temp_max > 20)           {goto err;}
                if(temp_max < temp_min)     {goto err;}

                if(-1 == check_digit(temp_doubt, 2)) {goto err;}
                if(-1 == check_digit(temp_err, 2))   {goto err;}
                if(0 != strcmp(temp_change  ,"/"))    {goto err;}

                if((atof(temp_doubt)  <0) || (atof(temp_doubt)  >20))  {goto err;}
                if((atof(temp_err)    <0) || (atof(temp_err)    >20))  {goto err;}

                temp_qm->min         = temp_min;
                temp_qm->max         = temp_max;
                temp_qm->doubtful    = atof(temp_doubt);
                temp_qm->err         = atof(temp_err);
                temp_qm->changerate  = INVALID_DATA;
            }
            else
                {goto err;}
        }break;
        default:    {goto err;}
    }
    return 0;

err:    return -1;
}
//============================================= ˽������ ====================================================
/*==================================================================
*������              cmd_debugon
*���ߣ�
*���ڣ�           2017-12-28
*���ܣ�  A.1 ���򿪵���ģʽ
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_debugon(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((18 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. DEBUGON,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//У�� ID�Ƿ�Ϊ����
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //��������ģʽ
    m_tempdata.DebugON = true;
    rlen = sprintf((char *)rbuf,"<Open debug mode.>\r\n");
    return rlen;
}
/*==================================================================
*������              cmd_debugoff
*���ߣ�
*���ڣ�           2017-12-28
*���ܣ�  A.2 ���رյ���ģʽ
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_debugoff(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((19 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. DEBUGOFF,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//У�� ID�Ƿ�Ϊ����
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    m_tempdata.DebugON = false;
    m_tempdata.DebugONCnt = 0;
    //m_tempdata.SecDataOut = false;    //�ر�˽������Ȩ��ʱ  �ر������ݷ���
    rlen = sprintf((char *)rbuf,"<Close debug mode.>\r\n");
    return rlen;
}
/*==================================================================
*������              cmd_version
*���ߣ�
*���ڣ�           2017-12-28
*���ܣ�  A.3���汾�Ų�ѯ
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_version(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((18 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. VERSION,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//У�� ID�Ƿ�Ϊ����
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //2����֤�豸�Ƿ�������ģʽ
    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }

    rlen = sprintf((char *)rbuf,"<VERSION:\r\nHardware: %s\r\nSoftware: %s>\r\n",m_defdata.m_baseinfo.hard_version,SOFT_VER);
    return rlen;
}
/*==================================================================
*������              cmd_restore
*���ߣ�
*���ڣ�           2017-12-28
*���ܣ�  A.4���ָ���������
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_restore(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((18 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. RESTORE,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//У�� ID�Ƿ�Ϊ����
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //2����֤�豸�Ƿ�������ģʽ
    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }

    Init_sys_cfg();
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<Resume factory setting success.>\r\n");
    return rlen;
}
/*==================================================================
*������              cmd_starttime
*���ߣ�
*���ڣ�           2017-12-28
*���ܣ�  A.5���豸��ʼ����ʱ��
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_starttime(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;
    time_t  power_start = 0;
    s_RtcTateTime_t rtc_temp;

    if((20 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. STARTTIME,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//У�� ID�Ƿ�Ϊ����
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //2����֤�豸�Ƿ�������ģʽ
    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }

    power_start = get_softimer(&m_tempdata.m_RtcTateTime) - m_tempdata.SysPowerON;
    Convert_Sec_to_Time(&rtc_temp, power_start*60);

    rlen = sprintf((char *)rbuf,"<Start time: %04d-%02d-%02d %02d:%02d:%02d.>\r\n",rtc_temp.year,rtc_temp.month,rtc_temp.day,
                                                                       rtc_temp.hour,rtc_temp.min,rtc_temp.sec);
    return 0;
}
/*==================================================================
*������              cmd_secdout_on
*���ߣ�
*���ڣ�           2017-12-28
*���ܣ�  A.6������뼶����
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_secdout_on(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((20 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. SECDOUTON,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//У�� ID�Ƿ�Ϊ����
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //2����֤�豸�Ƿ�������ģʽ
    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }

    m_tempdata.SecDataOut = true;
    rlen = sprintf((char *)rbuf,"<Open the seconds data sent automatically.>\r\n");
    return rlen;
}
/*==================================================================
*������              cmd_secdout_off
*���ߣ�
*���ڣ�           2017-12-28
*���ܣ�  A.7���ر��뼶�������
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_secdout_off(char *buf,char *rbuf)
{
    char *temp_di = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((21 != strlen(buf)) || (3 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. SECDOUTOFF,%s,%03d>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//У�� ID�Ƿ�Ϊ����
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //2����֤�豸�Ƿ�������ģʽ
    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }

    m_tempdata.SecDataOut = false;
    rlen = sprintf((char *)rbuf,"<Close the seconds data sent.>\r\n");
    return rlen;
}
/*==================================================================
*������              cmd_secdout_off
*���ߣ�
*���ڣ�           2017-12-28
*���ܣ�  A.7���ر��뼶�������
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_erase_check(char *buf,char *rbuf)
{
    char *temp_di = NULL, *temp_erase = NULL, *p = NULL, i = 0, rlen = 0;
    unsigned short temp_id = 0;

    if((21 != strlen(buf)) || (4 != Utils_CheckCmdStr(buf)))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error. ERASEOR,%s,%03d,XX>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        if(1 == i) {temp_di = p;}
        if(2 == i)
        {
            if(-1 == check_digit(p, strlen(p)))//У�� ID�Ƿ�Ϊ����
            {
                rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                return rlen;
            }
            temp_id = atoi(p);
        }
        if(3 == i)
        {
            temp_erase = p;
            if((0 != strcmp(temp_erase,"55"))  && (0 != strcmp(temp_erase,"AA")))
                { return 0; }
        }
        p = strtok(NULL, ",");
        i++;
    }

    //1����֤�豸��ʶ��,ID�Ƿ���ȷ
    //��֤DI
    if(0 != strcmp(temp_di,(char *)&sensor_di))
    { return 0; }
    //��֤ID
    if(temp_id != m_defdata.m_baseinfo.id)
    { return 0; }

    //2����֤�豸�Ƿ�������ģʽ
    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }

    if(0 == strcmp(temp_erase,"55"))
    {
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, FLASH_CFC_START    <<8);
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, (FLASH_CFC_START+16)<<8);
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, (FLASH_CFC_START+32)<<8);
        rlen = sprintf((char *)rbuf,"<Erase CFC OK.>\r\n");
    }
    if(0 == strcmp(temp_erase,"AA"))
    {
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, FLASH_CHECK_START    <<8);
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, (FLASH_CHECK_START+16)<<8);
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, (FLASH_CHECK_START+32)<<8);
        rlen = sprintf((char *)rbuf,"<Erase CHECK OK.>\r\n");
    }


    return rlen;
}

/*==================================================================
*������              cmd_cfc
*���ߣ�
*���ڣ�           2017-12-28
*���ܣ�  A.8���궨ϵ��
*�������������ţ����궨��ϵ��  aX^2 + bX + c
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_cfc(char *buf,char *rbuf)
{
    char i = 0, j = 0, rlen = 0, *temp_di = NULL, *p = NULL;
    int sensor_num = 0;
    float temp_a = 0, temp_b = 0, temp_c = 0;
    unsigned short temp_id = 0;

    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       return rlen;
    }
    j = Utils_CheckCmdStr(buf);
    if((4 != j) && (5 != j))
    {
        rlen = sprintf((char *)rbuf,"<Formatting error.\r\nRead:  CFC,%s,%03d,XX\r\nWrite: CFC,%s,%03d,XX,a:b:c>\r\n",
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id,
                                                        sensor_di,
                                                        m_defdata.m_baseinfo.id);
        return rlen;
    }
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
            case 0: break;
            case 1:
            {
                temp_di = p;
                if(0 != strcmp(temp_di,(char *)&sensor_di))
                    return 0;
            }break;
            case 2:
            {
                if(-1 == check_digit(p, strlen(p)))//У�� ID�Ƿ�Ϊ����
                {
                    rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                    return rlen;
                }
                temp_id = atoi(p);
                if(temp_id != m_defdata.m_baseinfo.id)
                    return 0;
            }break;
            case 3:
            {
                if((2 != strlen(p)) || (-1 == check_digit(p, 2)))
                {
                    rlen = sprintf((char *)rbuf,"<The sensor number must be two digits. Less than two use zero.>\r\n");
                    return rlen;
                }
                sensor_num = atoi(p);
            }break;
            case 4:
            {
                if(3 != sscanf(p,"%f:%f:%f",&temp_a, &temp_b, &temp_c) )
                {
                    rlen = sprintf((char *)rbuf,"<Have to put in three coefficients, a*X^2 + b*X + c.>\r\n");
                    return rlen;
                }
            }break;
            case 5: break;
            default: return 0;
        }
        p = strtok(NULL, ",");
        i++;
    }
    //1 ȷ�ϵ�ǰΪ���ִ�����  ��Ҫ�ǵڼ����������Ĳ���
    for(j = 0; j < DEV_TYPE_MAX; j ++)
    {
       if(0 == strncmp((char *)&sensor_di,(char *)dev_type[j],4))
         break;
    }
    if(j == 6)    //��
    {
       if((sensor_num > 2) || (sensor_num < 1))
       {
           rlen = sprintf((char *)rbuf,"<Only 1 or 2. 1 is wind speed, 2 is wind direction.>\r\n");
           return rlen;
       }
    }
    else if((j == 9) || (j == 10))   //����  ����ˮ��
    {
       if((sensor_num > bcm_info.sensor.SensorNum) || (sensor_num < 1))
       {
          rlen = sprintf((char *)rbuf,"<The sensor number cannot exceed the number of sensors.>\r\n");
          return rlen;
       }
    }
    else	//���������� �궨�������ò��ܴ���6
    {
       if((sensor_num > 6) || (sensor_num < 1))
       {
          rlen = sprintf((char *)rbuf,"<The sensor number cannot exceed 6.>\r\n");
          return rlen;
       }
    }

    //2 ������ǰ����
    if( i == 4)
    {
        rlen = sprintf((char *)rbuf,"<The sensor %d. Coefficient %f:%f:%f.>\r\n",sensor_num,
                                               bcm_info.sensor.cr[sensor_num-1].a,
                                               bcm_info.sensor.cr[sensor_num-1].b,
                                               bcm_info.sensor.cr[sensor_num-1].c);
        return rlen;
    }

    //3 �洢ϵ��
    bcm_info.sensor.cr[sensor_num-1].a = temp_a;
    bcm_info.sensor.cr[sensor_num-1].b = temp_b;
    bcm_info.sensor.cr[sensor_num-1].c = temp_c;
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<The coefficients are written successfully.>\r\n");
    return rlen;
}
/*==================================================================
*������              cmd_windh
*���ߣ�
*���ڣ�           2017-12-28
*���ܣ�  A.9���紫�������뿪������
*����������ֵ����Ӧ��˸߶�
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_windh(char *buf,char *rbuf)
{
    char *p = NULL, i = 0, rlen = 0;
    unsigned char  sensor_num = 0;
    char *temp_higt = NULL;

    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       goto err;
    }
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1:
        {
            if((2 < strlen(p)) || (-1 == check_digit(p, strlen(p))))
            {
                rlen = sprintf((char *)rbuf,"<The sensor number must be digits and between 4 to 13.>\r\n");
                goto err;
            }
            sensor_num = atoi(p);
            break;
        }
        case 2:
          {
              if(-1 == check_digit(p, strlen(p)))//У�� ID�Ƿ�Ϊ����
              {
                 rlen = sprintf((char *)rbuf,"<The sensor number must be all digits.>\r\n");
                 goto err;
              }
              temp_higt = p;
              break;
          }
        case 3: break;
        default:    {goto err;}
        }
        p = strtok(NULL, ",");
        i++;

    }
    if(( i != 2) && (i != 3))   {goto err;}

    //У�鴫��������Ƿ񳬷�Χ
    if(sensor_num > 13)
    {
        rlen = sprintf((char *)rbuf,"<The sensor number is too big.>\r\n");
        goto err;
    }
    //У�����������Χ
    if(atol(temp_higt) > 100000) //1000��
    {
        rlen = sprintf((char *)rbuf,"<The height of the wind rod is too high.>\r\n");
        goto err;
    }

    //д������
    if(sensor_num > 3)
    {
        strcpy(bcm_info.sensor.str_name[sensor_num-4],temp_higt);
        save_sys_cfg(&bcm_info);
        rlen = sprintf((char *)rbuf,"<Write to success.>\r\n");
    }
    else
        rlen = sprintf((char *)rbuf,"<Write failure. Default configuration.>\r\n");

    return rlen;

err:
  return rlen;
}
/*==================================================================
*������              cmd_stsensor
*���ߣ�
*���ڣ�           2017-12-28
*���ܣ�  A.10�����´���������
*���������´�����ʹ�ø�������Ӧ�Ĺ۲��
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_stsensor(char *buf,char *rbuf)
{
    char rlen = 0, *temp_di = NULL;
    short temp_id = 0;
    char *p = NULL, i = 0, j = 0;

    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       goto err;
    }
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
            case 0: break;
            case 1:
            {
                temp_di = p;
                if(0 != strcmp(temp_di,(char *)&sensor_di))
                    return 0;
            }break;
            case 2:
            {
                if(-1 == check_digit(p, strlen(p)))//У�� ID�Ƿ�Ϊ����
                {
                    rlen = sprintf((char *)rbuf,"<ID number is wrong.>\r\n");
                    return rlen;
                }
                temp_id = atoi(p);
                if(temp_id != m_defdata.m_baseinfo.id)
                    return 0;
            }break;
            case 3:
            {
                if((1 != strlen(p)) || (-1 == check_digit(p, 1)))
                {
                    rlen = sprintf((char *)rbuf,"<The sensor number must be one digits.>\r\n");
                    goto err;
                }
                bcm_info.sensor.SensorNum = atoi(p);
                bcm_info.sensor.data_num = atoi(p) * 2;
                //У�鴫���������Ƿ񳬷�Χ
                if((bcm_info.sensor.SensorNum == 9) || (bcm_info.sensor.SensorNum == 0))
                {
                    rlen = sprintf((char *)rbuf,"<The sensor number is between 0 and 9.>\r\n");
                    goto err;
                }
            }break;
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            {
                if((0 == strncmp(p,"A",1)) || (0 == strncmp(p,"B",1)))
                {
                    memset(&bcm_info.sensor.str_name[i-4][0],0,sizeof(bcm_info.sensor.str_name[i-4]));
                    strncpy(&bcm_info.sensor.str_name[i-4][0],"AB",2);
                    strncpy(&bcm_info.sensor.str_name[i-4][2],p,1);

                    memset(&bcm_info.sensor.str_name[i+4][0],0,sizeof(bcm_info.sensor.str_name[i+4]));
                    strncpy(&bcm_info.sensor.str_name[i+4][0],"AB",2);
                    strncpy(&bcm_info.sensor.str_name[i+4][2],p,1);
                    strncpy(&bcm_info.sensor.str_name[i+4][3],"l",1);
                }
                else if(0 == check_digit(p, strlen(p)))
                {
                    if(atoi(p) < 1000)
                    {
                        memset(&bcm_info.sensor.str_name[i-4][0],0,sizeof(bcm_info.sensor.str_name[i-4]));
                        strncpy(&bcm_info.sensor.str_name[i-4][0],"AB",2);
                        strncpy(&bcm_info.sensor.str_name[i-4][2],p,strlen(p));

                        memset(&bcm_info.sensor.str_name[i+4][0],0,sizeof(bcm_info.sensor.str_name[i+4]));
                        strncpy(&bcm_info.sensor.str_name[i+4][0],"ABl",3);
                        strncpy(&bcm_info.sensor.str_name[i+4][3],p,strlen(p));
                    }
                    else
                    {
                        rlen = sprintf((char *)rbuf,"<The sensor deep must be less than 999.>\r\n");
                        goto err;
                    }
                }
                else
                {
                    rlen = sprintf((char *)rbuf,"<The sensor deep must be A or B or from 1 to 999.>\r\n");
                    goto err;
                }
            }break;
            case 12: break;
            default: return 0;
        }
        p = strtok(NULL, ",");
        i++;
    }

    if(( i != 3) && (i != (bcm_info.sensor.SensorNum +4)))   {goto err;}

    if(i == 3)
    {
        rlen = sprintf((char *)rbuf,"<%s,%03d",sensor_di, m_defdata.m_baseinfo.id );

        for(j = 0; j < bcm_info.sensor.SensorNum; j++)
        {
            rlen += sprintf((char *)(rbuf+rlen),",%s",&bcm_info.sensor.str_name[j][0]);
        }

        rlen += sprintf((char *)(rbuf+rlen),">\r\n");

        return rlen;
    }
    //д������

    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<Write to success.>\r\n");
    return rlen;

err:
  return rlen;
}
/*==================================================================
*������              cmd_shsensor
*���ߣ�
*���ڣ�           2017-12-28
*���ܣ�  A.11������ˮ�ִ���������
*���������´�����ʹ�ø�������Ӧ�Ĺ۲��
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_shsensor(char *buf,char *rbuf)
{
    char *p = NULL, i = 0, rlen = 0;
    char sensor_num = 0, temp_deep = 0;

    if(m_tempdata.DebugON != true)
    {
       rlen = sprintf((char *)rbuf,"<Please open debug mode.>\r\n");
       goto err;
    }
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        if(i == 1)
        {
            if((2 < strlen(p)) || (-1 == check_digit(p, strlen(p))))
            {
                rlen = sprintf((char *)rbuf,"<The sensor number must be digits.>\r\n");
                goto err;
            }
            sensor_num = atoi(p);
            //У�鴫��������Ƿ񳬷�Χ
            if((sensor_num >10) || (sensor_num < 1))
            {
                rlen = sprintf((char *)rbuf,"<The sensor number is between 1 and 10.>\r\n");
                goto err;
            }
        }

        if ((1 < i)&& (i < sensor_num+2))
        {
            if(0 == check_digit(p, strlen(p)))
            {
                temp_deep = atoi(p);
                if(temp_deep < 1000)
                    strncpy(bcm_info.sensor.str_name[i-2],p,strlen(p));
                else
                {
                    rlen = sprintf((char *)rbuf,"<The sensor deep must be less than 999.>\r\n");
                    goto err;
                }
            }
            else
            {
                rlen = sprintf((char *)rbuf,"<The sensor deep must be digits.>\r\n");
                goto err;
            }
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 2) && (i != (sensor_num +2)))   {goto err;}

    //д������
    save_sys_cfg(&bcm_info);
    rlen = sprintf((char *)rbuf,"<Write to success.>\r\n");
    return rlen;

err:
  return rlen;
}

/*==================================================================
*������              cmd_shsensor
*���ߣ�
*���ڣ�           2017-12-28
*���ܣ�  A.11������ˮ�ִ���������
*���������´�����ʹ�ø�������Ӧ�Ĺ۲��
*���룺           *buf  (�������ݣ��������ݵ�ַ
*         *rbuf (������ݣ����ƽṹ��ַ
*
*���أ�           ��ӡ�ַ�������
*�޸ļ�¼��
==================================================================*/
int cmd_version_cfg(char *buf,char *rbuf)
{
    char *p = NULL, i = 0, rlen = 0, buffer[20];
    memset(buffer, 0x00, sizeof(buffer));
    
    if(0 == Utils_CheckCmdStr(buf))
    {goto err;}
    p = strtok(buf, ",");
    while(p)
    {
        switch(i)
        {
        case 0: break;
        case 1: break;
        case 2: break;
        case 3:
        {
            if(strlen(p) <= sizeof(buffer))
            {
                sprintf((char *)buffer, "%s", p);
            }
            else
            {
                goto err;
            }
            break;
        }
        default:    
            goto err;
        }
        p = strtok(NULL, ",");
        i++;
    }
    if(( i != 4))  
        goto err;
    
    //Ӳ���汾д��ṹ��
    memset(m_defdata.m_baseinfo.hard_version, 0x00, sizeof(m_defdata.m_baseinfo.hard_version));
    sprintf((char *)m_defdata.m_baseinfo.hard_version, "%s", buffer);

    //�ṹ��д��FLASH  
    pamsave(&m_defdata);
    
    //���سɹ�
    rlen = sprintf((char *)rbuf,"set hard version success");
    return rlen;
err:
    //����ʧ��
    rlen = sprintf((char *)rbuf,"set hard version fail");
    return rlen;
}
