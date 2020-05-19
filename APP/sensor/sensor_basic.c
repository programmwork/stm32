/*
 * sensor_basic.c
 *
 *  Created on: 2018��1��24��
 *      Author: HYSD
 */

//#include <msp430.h>
#include "sensor_basic.h"
#include <stdio.h>
#include <string.h>

sensor_state_t sensor_state;

uint8 Flag_nodata = 0;//û�����ݱ�־λ

FLASH_Store_MinData_t  FLASH_Store_MinData;//�洢�ṹ��
FLASH_Store_MinData_t  FLASH_Read_MinData;//�����ݽṹ��
sensors_data_t sensors_data;
//���� ��ѹ  ������
float board_temp_max = 0;
float board_temp_min = 0;
float board_volt_max = 0;
float board_volt_min = 0;

#define V_R_L                   4.7
#define V_R_H                   51.0

uint32  check_offset = 0;   //�춨��ȡƫ�Ƶ�ַ
uint32  cfc_offset = 0;     //У����ȡƫ�Ƶ�ַ

/*********************************************************************************
** �������� ��BanWenBanYa_engine
** �������� ���������°�ѹ
** ��ڲ��� ��
** ���ڲ��� ��
*********************************************************************************/
unsigned char BanWenBanYa_engine(float *pT, float *pV)
{
    static char counter = 0, i = 0;
    long valueTSum = 0;
    unsigned long valueVSum = 0;
    U8 flag = 0;
    int value = 0;
    unsigned int buffer = 0;

    float fp_1=0.0;

    while(1)
    {
        Start_ADC(2);
        if(Check_ADC(&buffer)) 
        {

        }
        Start_ADC(1);
        if(Check_ADC(&buffer))
        {
        }
        Start_ADC(6);
        if(Check_ADC(&buffer))
        {
        }
            
    }

 /* �ɼ����� */
    for(i = 0 ; i < 10 ; i++)
    {
        //if(DS3231_ReadTemperature(&value) == 1)
        {
            valueTSum += value;
            counter++;
        }
    }
    if(counter >= 5)
    {
        // �������
        value = valueTSum / counter;
        *pT = (float)value ;        //����¶�
        flag = 1;
    }
    else
    {
        flag &= ~1;
    }
    valueTSum = 0;
    counter = 0;

  /* �ɼ���ѹ */
    for(i = 0 ; i < 10 ; i++)
    {
        //Start_ADC(0);
        //if(Check_ADC(&buffer))
        {
            valueVSum = valueVSum + buffer;
            counter++;
        }
    }

    if(counter >= 5)
    {
        // �����ѹ
        value = valueVSum / counter;
        fp_1 = (3.3 * (float)value) / 4096;                                     // �����ѹֵ
        *pV = fp_1 * (V_R_L + V_R_H) / V_R_L;
        flag = flag | 0x2;
    }
    else
    {
        flag = flag & ~0x2;
    }

    counter = 0;
    valueVSum = 0;

    return flag;
}

void check_event_autosend(void)
{
    unsigned int send_time = 0;
    s_RtcTateTime_t temp;
    UINT32 data = 0;

    //ÿ���ӵ���ִ��
    Convert_Time_to_min(&data);
    Convert_Sec_to_Time(&temp, data*60);

    if((temp.min % bcm_info.common.ftd) == 0)
    {
        send_time = m_tempdata.m_RtcTateTime.sec*10 + m_tempdata.m_RtcTateTime.msec/100;

        if(send_time >= bcm_info.common.ft)
        {
            m_tempdata.event.autosendevent=false;
            
            if(m_tempdata.DebugON==true)
            {
                uartSendStr(0,(unsigned char *)sensors_data.sendpacket,sensors_data.sendpacketlen);
            }
            else//����ģʽ
            {
                if(bcm_info.common.mo==0)//����
                {

                }
                else if(bcm_info.common.mo==1)//���ش���
                    uartSendStr(0,(unsigned char *)sensors_data.sendpacket,sensors_data.sendpacketlen);
                else if(bcm_info.common.mo == 2)
                {
                    uartSendStr(0,(unsigned char *)sensors_data.sendpacket,sensors_data.sendpacketlen);
                }
            }
        }
    }
}

void Convert_Sec_to_Time(s_RtcTateTime_t *rtc_temp, time_t sec_data)
{
    struct tm *temp;

    temp = localtime(&sec_data);
    rtc_temp->year = temp->tm_year + 1900;
    rtc_temp->month = temp->tm_mon + 1;
    rtc_temp->day = temp->tm_mday;
    rtc_temp->hour = temp->tm_hour;
    rtc_temp->min = temp->tm_min;
    rtc_temp->sec= temp->tm_sec;

}

void Convert_Time_to_min(UINT32 *data)
{
    time_t sec = 0;
    struct tm temp;

    temp.tm_year = m_tempdata.m_RtcTateTime.year - 1900;
    temp.tm_mon = m_tempdata.m_RtcTateTime.month - 1;
    temp.tm_mday = m_tempdata.m_RtcTateTime.day;
    temp.tm_hour = m_tempdata.m_RtcTateTime.hour;
    temp.tm_min = m_tempdata.m_RtcTateTime.min;
    temp.tm_sec = m_tempdata.m_RtcTateTime.sec;
    sec = mktime(&temp);
/*
    temp1.tm_year = m_tempdata.m_RtcTateTime.year - 1900;
    temp1.tm_mon = m_tempdata.m_RtcTateTime.month - 1;
    temp1.tm_mday = m_tempdata.m_RtcTateTime.day;
    temp1.tm_hour = m_tempdata.m_RtcTateTime.hour;
    temp1.tm_min = m_tempdata.m_RtcTateTime.min;
    temp1.tm_sec = m_tempdata.m_RtcTateTime.sec;
    base_sec = mktime(&temp1);

    diff_sec = difftime(sec, base_sec);
*/
    *data = sec / 60;
}

void check_event_storage(void)
{
  //ִ��һ�δ洢����
    static uint32 time_mindata_store;

    Convert_Time_to_min(&time_mindata_store);

    //ת���ɷ�����  ���ô˺���ʱ�ѵ�����һ���ӣ�������Ҫ��ȥ1
    //���������ݴ洢����Ҫ���⿪��һ��ʱ��ṹ���־λ�����ȱ���˽ṹ��
    FLASH_Store_MinData.time = time_mindata_store;
    save_record_min(time_mindata_store,(unsigned char *)&FLASH_Store_MinData,sizeof(FLASH_Store_MinData_t));
}

/***********************************************************************************************************
** �������� ��unsigned char save_record_min(unsigned long timep, unsigned char dat[],unsigned char len)
** �������� �������ݴ洢��FLASH��
** ��ڲ��� ��
**
** ���ڲ��� ��0:�洢ʧ��  1:�洢�ɹ�
***********************************************************************************************************/
unsigned char save_record_min(unsigned long timep, unsigned char dat[], unsigned char len)
{
    unsigned long pageIndex;
    unsigned long addr;
    unsigned char flag;

    if(len > (Flash_PAGE_SIZE / 2))  return 0;
    pageIndex = (timep % Flash_PAGE_NUMBER) + FLASH_DATA_START;   // ����λ���ĸ�����

    //  pageIndex = timep % Flash_PAGE_NUMBER;      // ����λ���ĸ�����
    addr = pageIndex << 8;

    if((pageIndex & 0x0F) == 0)                         // �ж��Ƿ�Ϊһ��sector�Ŀ�ʼ
    {
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, addr);
        W25Q128_Data_Prog(addr, dat, len);              // ��FLASH�д洢����
    }
    else
    {
        W25Q128_Data_Read(addr, &flag, 1);  // ��ȡ���洢��
        if(flag == 0xFF)
        {
            W25Q128_Data_Prog(addr, dat, len);               // ��FLASH�д洢����
        }
        else
        {
            // ���洢�����������ȡ���ݴ洢�������Ƿ����
            addr += (Flash_PAGE_SIZE / 2);
            W25Q128_Data_Read(addr, &flag, 1);
            if(flag == 0xFF)
            {
                W25Q128_Data_Prog(addr, dat, len);   // ��FLASH�д洢����
            }
            else return 0;                          // д��ʧ��
        }
    }
    return 1;                                   // ��ȷ
}

/***********************************************************************************************************
** �������� ��unsigned char read_record_min(unsigned long timep, unsigned char dat[], unsigned char len)
** �������� ����ȡ��������
** ��ڲ��� ��
**
** ���ڲ��� ��0:��ȡʧ��  1:��ȡ�ɹ�
***********************************************************************************************************/
unsigned char read_record_min(unsigned long timep, unsigned char dat[], unsigned char len)
{
    unsigned long pageIndex;
    unsigned long addr;
    unsigned char flag;


    if(len > (Flash_PAGE_SIZE / 2))  return 0;
    pageIndex = (timep % Flash_PAGE_NUMBER) + FLASH_DATA_START;   // ����λ���ĸ�����
    addr = (pageIndex << 8) + (Flash_PAGE_SIZE / 2);

    W25Q128_Data_Read(addr, &flag, 1);     // ��ȡ���ݴ洢��

    if(flag == 0xFF)                   // ���ݴ洢����û������
    {
        addr -= (Flash_PAGE_SIZE / 2);
        W25Q128_Data_Read(addr, &flag, 1);  // ��ȡ���洢��

        if(flag == 0xFF)
            return 0;      // ���洢����û������
        else
            W25Q128_Data_Read(addr, dat, len);// ���洢����������
    }
    else
    {
        W25Q128_Data_Read(addr, dat, len); // ���ݴ洢����������
    }
    return 1;
}

unsigned char read_history_mindata(void)
{
    if(m_tempdata.Count_read_history>0)
    {
        if(read_record_min(m_tempdata.time_start_read,(unsigned char *)&FLASH_Read_MinData,sizeof(FLASH_Read_MinData))!=1)//��һ�����ݳ���
        {
            Flag_nodata = 1;
        }
        else
        {
            if(FLASH_Read_MinData.time != m_tempdata.time_start_read)
            Flag_nodata = 1;
        }

        memset(&sensors_data.historyPacket,0,MAX_PKG_LEN);
        sensors_data.historyPacketLen=GetMinData((char *)sensors_data.historyPacket,&sensors_data,2);

        uartSendStr(0,(unsigned char *)sensors_data.historyPacket,sensors_data.historyPacketLen);
        Flag_nodata = 0;//�����ݱ�־λ��0
        m_tempdata.time_start_read = m_tempdata.time_start_read+m_tempdata.time_interval_read ;//��ʼʱ��ָ����һ����
        m_tempdata.Count_read_history--;
    }
    return m_tempdata.Count_read_history;
}

unsigned char show_history_mindata(void)
{
    if(m_tempdata.Count_down_history>0)
    {
        if(read_record_min(m_tempdata.time_start,(unsigned char *)&FLASH_Read_MinData,sizeof(FLASH_Read_MinData))!=1)//��һ�����ݳ���
            Flag_nodata = 1;
        else
        {
            if(FLASH_Read_MinData.time != m_tempdata.time_start)
            Flag_nodata = 1;
        }

        memset(&sensors_data.historyPacket,0,MAX_PKG_LEN);
        sensors_data.historyPacketLen=GetMinData((char *)sensors_data.historyPacket,&sensors_data,0);

        uartSendStr(0,(unsigned char *)sensors_data.historyPacket,sensors_data.historyPacketLen);
        Flag_nodata = 0;//�����ݱ�־λ��0
        m_tempdata.time_start = m_tempdata.time_start+m_tempdata.time_interval ;//��ʼʱ��ָ����һ����
        m_tempdata.Count_down_history--;
    }
    return m_tempdata.Count_down_history;
}

void sample_data_save(char *dat ,uint16 data_num)
{
    unsigned long addr = 16ul << 8;

    W25Q128_Erase_Chip(W25Q128_ERS_SEC, addr);

    while(data_num > 256)
    {
        W25Q128_Data_Prog(addr, (unsigned char *)dat, 256);
        data_num -= 256;
        addr += 256;
        dat += 256;
    }

    if(data_num != 0)
    {
        W25Q128_Data_Prog(addr, (unsigned char *)dat, data_num);
    }
}

void sample_data_read(unsigned int index, char *dat ,unsigned int data_num)
{
    unsigned long addr = (16ul << 8) + index;

    W25Q128_Data_Read(addr, (unsigned char *)dat, data_num);
}

uint8               buf_cfc_check[200];
uint8               sbuf[220];
unsigned char read_cfc_data(void)
{
    uint32              addr = 0;
    s_check_info_t      temp_check_info;
    s_check_cr_t        cfc[8];
    uint8               i, len = 0;
    

    addr =  (FLASH_CFC_START<<8) + cfc_offset * Flash_PAGE_SIZE/2;


      W25Q128_Data_Read(addr, (unsigned char *)&temp_check_info, sizeof(s_check_info_t));

      if(temp_check_info.flag != true)
      {
          m_tempdata.CfcFlag = false;
          cfc_offset = 0;
      }
      else
      {
          memset(cfc,0,sizeof(cfc));
          W25Q128_Data_Read(addr+sizeof(s_check_info_t),  (unsigned char *)&cfc, temp_check_info.len*sizeof(s_check_cr_t));
          if(temp_check_info.len>8) return 0;
          memset(buf_cfc_check,0,MAX_PKGLEN/2);
          for(i = 0; i<temp_check_info.len; i++)
          {
              len += sprintf((char *)buf_cfc_check+len ,",%d#%.02f:%.02f:%.02f",cfc[i].number, cfc[i].cr.a, cfc[i].cr.b, cfc[i].cr.c);
          }
            memset(sbuf,0,MAX_PKGLEN);
            sprintf((char *)sbuf,"<%s,%03d,1,%04d%02d%02d,%02d,%ld%s>\r\n",
                                                     sensor_di,
                                                     m_defdata.m_baseinfo.id,

                                                     temp_check_info.year,
                                                     temp_check_info.mon,
                                                     temp_check_info.day,
                                                     temp_check_info.term,
                                                     temp_check_info.number,
                                                     buf_cfc_check
                                                     );
            uartSendStr(0, sbuf, strlen((char *)sbuf));
            cfc_offset++;
            if(cfc_offset == 90)
            {
                cfc_offset = 0;
                m_tempdata.CfcFlag = false;
            }
      }

  return 0;
}
unsigned char read_check_data(void)
{
    uint32              addr = 0;
    s_check_info_t      temp_check_info;
    
    

    addr =  (FLASH_CHECK_START<<8) + check_offset * Flash_PAGE_SIZE/2;

      W25Q128_Data_Read(addr, (unsigned char *)&temp_check_info, sizeof(s_check_info_t));

      if(temp_check_info.flag != true)
      {
          m_tempdata.CheckFlag = false;
          check_offset = 0;
      }
      else
      {
          memset(buf_cfc_check,0,MAX_PKGLEN/2);
          W25Q128_Data_Read(addr+sizeof(s_check_info_t), buf_cfc_check, temp_check_info.len);

            memset(sbuf,0,MAX_PKGLEN);
            sprintf((char *)sbuf,"<%s,%03d,2,%04d%02d%02d,%02d,%05ld,%s>\r\n",
                                                     sensor_di,
                                                     m_defdata.m_baseinfo.id,

                                                     temp_check_info.year,
                                                     temp_check_info.mon,
                                                     temp_check_info.day,
                                                     temp_check_info.term,
                                                     temp_check_info.number,
                                                     buf_cfc_check
                                                     );
            uartSendStr(0, sbuf, strlen((char *)sbuf));
            check_offset++;
      }
  return 0;
}
unsigned char save_cr(uint8 num, s_check_info_t * check_info, char * data)
{
    uint8 i = 0, flag = 0;
    uint32 addr = 0;
    uint8 result = 0;
    uint8 len = 0;

    if(num == 1 )
    {
        addr = (FLASH_CFC_START<< 8);
        len = check_info->len * 13;
    }
    else if (num == 2)
    {
        addr = (FLASH_CHECK_START<< 8);
        len = check_info->len;
    }
    else
        return 0;

    for(i = 0;i < 90;i++)
    {
        W25Q128_Data_Read(addr, &flag, 1);

        if(flag == 0xff)
        {
            check_info->flag = 1;
            W25Q128_Data_Prog(addr, (unsigned char *)check_info, sizeof(s_check_info_t));
            W25Q128_Data_Prog(addr + sizeof(s_check_info_t), (unsigned char *)data, len);
            result = 1;
            break;
        }

        addr += Flash_PAGE_SIZE / 2;
    }

    return result;
}
/*==================================================================
 * �������ƣ� S32ToStrAlignRight
 * �������ܣ���ֵת��Ϊ�ַ����ĺ���
 * (CharNum�ĳ���Ӧ�������ݵ��ܳ���,��������㽫����CharNum,��ʵ�ʳ�����ʾ)
 * ��ڲ�����   signed long dat   ��ת��������
            unsigned char charNum ת�����ַ����ĳ���(����ǰ�治��CharNum��λ����ָ���ַ����)
            char  *str_out_1  ����ַ���
 * ���ڲ�����    ��
 * ��дʱ�䣺
 * �޸�ʱ�䣺
==================================================================*/
void  S32ToStrAlignRight(signed long dat, unsigned char charNum, char  *str_out_1)
{
    unsigned long t_i_in,tc;
    unsigned char  i,k,total=1;
    unsigned char  fh = 0;
    unsigned char charFill = '0';
    unsigned char dot = 0;

    unsigned char tempChar[12];
    unsigned char d = 0;
    unsigned char *str_out = tempChar;

    // ����һ���м�λ�ַ�(����С����͸���)
    if(dat < 0)
    {
        fh = 1;
        dat = -dat;
    }
    tc = t_i_in = dat;
    while(t_i_in /= 10)        total++;                           // �ж���Ч����һ���м�λ
    k = total;
    if(dot >= total) total =  dot+2;                      // ��С����λ�����ڵ�����Ч����λ��ʱ
    // ����:�ַ�����λ��ΪС�����λ��+2,����һ��".",��һ��"0"
    else if(0 != dot) total++;                                    // ��һ��"."

    if(fh) total++;                                                           // �Ǹ����ټ�1,totalΪ�ܵ��ַ�����

    if(charNum < total)
    {
        d = total - charNum;
        charNum = total;                                  // �ж���ڲ����ĺϷ���
    }

    if(fh)     *str_out++ ='-';                                       // ����Ǹ����ڿո���"-"

    for(i=0;i<charNum-total;i++)                              // �ڲ����λ�����ո�
        *str_out++ = charFill;

    if(dot >= k)                                                              // С�����λ�������λ��Ч���ֵ�ǰ��,��Ϊ��С��
    {
        *str_out++ ='0';
        *str_out++ ='.';
        for(i=0;i<dot-k;i++)    *str_out++ = '0';       // ����:С�������DotPosition-k��0
        str_out = str_out+k;                                            // ������ĩβ��'\0'
        *str_out-- ='\0';
        for(i=0;i<k;i++)                                                    // ��֮�������Ч����
        {
            *str_out-- = tc % 10+'0';
            tc /= 10;
        }
    }
    else                                                                              // С�����λ�������λ��Ч���ֵĺ���,����Ϊ��С��
    {
        if(0 != dot) str_out = str_out + k + 1;     // ΪС��
        else str_out = str_out + k;                             // Ϊ����
        *str_out-- = '\0';
        for(i=0;i<k;i++)
        {
            *str_out-- = tc % 10+'0';
            if(i==dot-1) *str_out-- ='.';
            tc /= 10;
        }
    }
    strcpy(str_out_1,(char *)(tempChar+d));               //
}


/*==================================================================
 * �������ƣ� Data_Copy
 * �������ܣ��������ݿ���
 *
 * ��ڲ�����    qcfalshdata_t *FLASH_Store  �洢��flash�����ݼ�qc
 *          qcfalshdata_t *FLASH_Read   ��flash��ȡ�����ݼ�qc
 *          qcdata_t *sensor            �������ɼ��������ݼ�qc
 *          qcfalshdata_t *Mindata_temp ������ʾ�����ݼ�qc
 *          unsigned char mode          �ж�Ϊ��ȡflash ����дflash �Լ���ǰ��ʾ
 *          unsigned char multiple      �������ӣ� 10��multiple����
 * ���ڲ�����  ��
 * ��дʱ�䣺
 * �޸�ʱ�䣺
==================================================================*/
void Data_Copy(qcfalshdata_t *FLASH_Store, qcfalshdata_t *FLASH_Read, qcdata_t *sensor, qcfalshdata_t *Mindata_temp, unsigned char mode, unsigned char multiple)
{
    float temp = 0, min_data = 0;
    double integer = 0, dot = 0;

    temp = pow(10,multiple);

    if(mode == 1)//��Ҫ�洢�������ݣ�������������
    { //���ݱ�����flash�ṹ��   �洢ʱ���ݾ��Ŵ�100��ȡ��

      if(sensor->data == INVALID_DATA)
      {
        FLASH_Store->data = INVALID_DATA;
        Mindata_temp->data = INVALID_DATA;
      }
      else
      {
          min_data = (sensor->data)*temp;

          //��������
          dot = modf(min_data, &integer);
          if(0.5 <= dot)            min_data = integer+1;
          else if(-0.5 >= dot)      min_data = integer-1;

        FLASH_Store->data = (long)((min_data*100)/temp);
        Mindata_temp->data = min_data;
      }

      FLASH_Store->qc = sensor->qc;
      Mindata_temp->qc = sensor->qc;
    }

    else if((mode == 0)||(mode == 2))//����Ҫ�洢�ķ������ݣ�������ʷ���ݶ�ȡ����
    {
        if(FLASH_Read->data == INVALID_DATA)
            Mindata_temp->data = INVALID_DATA;
        else
        {
            if(multiple >= 2)
            {
                temp = pow(10,multiple-2);
                Mindata_temp->data = (float)((float)FLASH_Read->data*temp);//��������
            }
            else
            {
                temp = pow(10,2-multiple);
                Mindata_temp->data = (float)((float)FLASH_Read->data/temp);//��������
            }
        }

        Mindata_temp->qc   = FLASH_Read->qc;
    }
    else ;    //error
}

/*==================================================================
 * �������ƣ� Pack_Data
 * �������ܣ��������
 *
 * ��ڲ�����    qcfalshdata_t *Mindata_temp ������ʾ�����ݼ�qc
 *          unsigned int *len           ������ÿ���ַ����ڴ�������ĵ�ַ
 *          char *Header_packet         ��������ַ���
 *          unsigned char data_len      �ֽڳ���
 * ���ڲ�����    ��
 * ��дʱ�䣺
 * �޸�ʱ�䣺
==================================================================*/
void Pack_Data(qcfalshdata_t *Mindata_temp, unsigned int *len, char *Header_packet, unsigned char data_len)
{
    long temp_data = 0;
    unsigned char i = 0;

    if((Mindata_temp->data ==INVALID_DATA) || (Flag_nodata == 1))
    {
        for(i = 0;i<data_len;i++)
        {
            Header_packet[*len]='/';
            (*len)++;
        }
    }
    else
    {
        temp_data = (long)(Mindata_temp->data);   //�Ŵ�1��
        S32ToStrAlignRight(temp_data, data_len, Header_packet+*len);
    }
    *len=strlen((char *)Header_packet);
    Header_packet[*len]=',';
    (*len)++;
}

/*==================================================================
 * �������ƣ� Pack_Datal
 * �������ܣ����Ĵ������
 *
 * ��ڲ�����    long *Mindata_temp          ������ʾ����
 *          unsigned int *len           ������ÿ���ַ����ڴ�������ĵ�ַ
 *          char *Header_packet         ��������ַ���
 *          unsigned char data_len      �ֽڳ���
 * ���ڲ�����    ��
 * ��дʱ�䣺
 * �޸�ʱ�䣺
==================================================================*/
void Pack_Datal(long *Mindata_temp, unsigned int *len, char *Header_packet, unsigned char data_len)
{
    long temp_data = 0;
    unsigned char i = 0;

    if((*Mindata_temp == INVALID_DATA) || (Flag_nodata == 1))
    {
        for(i = 0;i<data_len;i++)
        {
            Header_packet[*len]='/';
            (*len)++;
        }
    }
    else
    {
        temp_data = *Mindata_temp;
        S32ToStrAlignRight(temp_data, data_len, Header_packet+*len);
    }
    *len=strlen((char *)Header_packet);
    Header_packet[*len]=',';
    (*len)++;
}

/**********************************************************************************************************
** �������� ��float StandardDeviation( float *pData, unsigned char *pQC, unsigned int len)
** �������� �������׼��
** ��ڲ��� ��
** ���ڲ��� ����
**********************************************************************************************************/
float StandardDeviation( float *pData, unsigned char *pQC, unsigned int len)
{
  float sum = 0, sum_f = 0, average = 0;
  unsigned int i,validNumber;

  // ������Ч������ƽ��ֵ
  sum = 0;
  validNumber = 0;
  for(i=0; i<len; i++)
  {
    if(CHECK_QC_IS_OK(pQC,i))
    {
      sum += pData[i];
      validNumber++;
    }
  }

  if(validNumber == 0) return 0;

  average = sum / validNumber;

  // �����׼��
  sum_f = 0;
  for(i=0; i<len; i++)
  {
    if(CHECK_QC_IS_OK(pQC,i))
    {
      sum_f += (pData[i] - average) * (pData[i] - average);
    }
  }

  sum_f = sum_f / validNumber;

  return sqrt(sum_f);
}

