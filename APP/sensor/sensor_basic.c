/*
 * sensor_basic.c
 *
 *  Created on: 2018��1��24��
 *      Author: HYSD
 */

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
    uint16_t buffer = 0;

    float fp_1=0.0;

 /* �ɼ����� */
    for(i = 0 ; i < 10 ; i++)
    {
        if(DS3231_ReadTemperature(&value) == 1)
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
        Start_ADC(6);
        if(Check_ADC(&buffer))
        {
            valueVSum = valueVSum + buffer;
            counter++;
        }
    }

    if(counter >= 5)
    {
        // �����ѹ
        value = valueVSum / counter;
        fp_1 = (3.3f * (float)value) / 4096;                                     // �����ѹֵ
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
                uartSendStr(UARTDEV_1,(unsigned char *)sensors_data.sendpacket,sensors_data.sendpacketlen);
            }
            else//����ģʽ
            {
                if(bcm_info.common.mo==0)//����
                {

                }
                else if(bcm_info.common.mo==1)//���ش���
                    uartSendStr(UARTDEV_1,(unsigned char *)sensors_data.sendpacket,sensors_data.sendpacketlen);
                else if(bcm_info.common.mo == 2)
                {
                    uartSendStr(UARTDEV_1,(unsigned char *)sensors_data.sendpacket,sensors_data.sendpacketlen);
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

unsigned char clear_data(short num)//����豸��У׼���춨��Ϣ
{
    uint32           addr = 0, addr_var = 0;
    s_check_info_t   temp_check_info;
    unsigned char    wd;

    //�ж�flash
    if(num == 1 )
    {
        //addr = (FLASH_CFC_START<< 8);
        W25X16_SPI_FastRead_nBytes( FLASH_CFC_WD_MAIN<<8,  (unsigned char *)&flash_flag_info_main, sizeof(FLASH_flag_Info_t) );
        W25X16_SPI_FastRead_nBytes( FLASH_CFC_WD_BACK<<8,  (unsigned char *)&flash_flag_info_back, sizeof(FLASH_flag_Info_t) );
    }
    else if (num == 2)
    {
        //addr = (FLASH_CHECK_START<< 8);
        W25X16_SPI_FastRead_nBytes( FLASH_CHECK_WD_MAIN<<8,  (unsigned char *)&flash_flag_info_main, sizeof(FLASH_flag_Info_t) );
        W25X16_SPI_FastRead_nBytes( FLASH_CHECK_WD_BACK<<8,  (unsigned char *)&flash_flag_info_back, sizeof(FLASH_flag_Info_t) );
    }

    //��ȡָ��
    if( flash_flag_info_main.flag == 1)
    {
        wd = flash_flag_info_main.WD; //��ֵ����ʱ����
    }
    else
    {
        if(flash_flag_info_back.flag == 1)
        {
            wd = flash_flag_info_back.WD; //��ֵ����ʱ����
        }
        else
        {
            if( (flash_flag_info_main.flag == 0xFF) && (flash_flag_info_back.flag == 0xFF) )
            {
                /*if(num == 1 )
                {
                    W25X16_Flash_Erase_Sector( ( FLASH_CFC_START ) <<8); //��������
                    W25X16_Flash_Erase_Sector( FLASH_CFC_WD_MAIN<<8 );
                    delay_ms(100);//100ms
                    W25X16_Flash_Erase_Sector( FLASH_CFC_WD_BACK<<8 );
                }
                else if(num == 2 )
                {
                    W25X16_Flash_Erase_Sector( ( FLASH_CHECK_START ) <<8); //��������
                    W25X16_Flash_Erase_Sector( FLASH_CHECK_WD_MAIN<<8 );
                    delay_ms(100);//100ms
                    W25X16_Flash_Erase_Sector( FLASH_CHECK_WD_BACK<<8 );
                }*/
                wd = 0;
                //return 1; //û����
            }
            else
            {
                return 2; //ָ�� err
            }
        }
    }

    //����
    if(num == 1)
    {
        addr_var  = FLASH_CFC_START;
    }
    else if(num == 2)
    {
        addr_var  = FLASH_CHECK_START;
    }

    addr = (addr_var + wd * 16)<< 8;
    W25X16_SPI_FastRead_nBytes(addr,  (unsigned char *)&temp_check_info, sizeof(s_check_info_t));

    if(temp_check_info.flag == 1)
    {
        W25X16_Flash_Erase_Sector( ( addr_var + wd * 16 ) <<8); //��������
    }
    else
    {
        W25X16_Flash_Erase_Sector( ( addr_var + wd * 16 ) <<8); //��������
    }

    //��д
    if(wd == 0)
    {
        wd = 9;
    }
    else
    {
        wd-- ;
    }

    addr = (addr_var + wd * 16)<< 8;
    //W25X16_SPI_FastRead_nBytes(addr,  (unsigned char *)&temp_check_info, sizeof(s_check_info_t));

    //if(temp_check_info.flag == 1)
    //{
        flash_flag_info_main.WD = wd;
        flash_flag_info_main.flag = 1;
        flash_flag_info_back.WD = wd;
        flash_flag_info_back.flag = 1;
        //��ָ��
        if(num == 1 )
        {
            W25X16_Flash_Erase_Sector( FLASH_CFC_WD_MAIN<<8 );
            W25X16_SPI_Write_nBytes( FLASH_CFC_WD_MAIN<<8 ,  (unsigned char *)&flash_flag_info_main, sizeof(FLASH_flag_Info_t) );
            delay_ms(100);//100ms
            W25X16_Flash_Erase_Sector( FLASH_CFC_WD_BACK<<8 );
            W25X16_SPI_Write_nBytes( FLASH_CFC_WD_BACK<<8 ,  (unsigned char *)&flash_flag_info_back, sizeof(FLASH_flag_Info_t) );
        }
        else if(num == 2 )
        {
            W25X16_Flash_Erase_Sector( FLASH_CHECK_WD_MAIN<<8 );
            W25X16_SPI_Write_nBytes( FLASH_CHECK_WD_MAIN<<8,  (unsigned char *)&flash_flag_info_main, sizeof(FLASH_flag_Info_t) );
            delay_ms(100);//100ms
            W25X16_Flash_Erase_Sector( FLASH_CHECK_WD_BACK<<8 );
            W25X16_SPI_Write_nBytes( FLASH_CHECK_WD_BACK<<8,  (unsigned char *)&flash_flag_info_back, sizeof(FLASH_flag_Info_t) );
        }
    //}
    /*else
    {
        if(num == 1 )
        {
            W25X16_Flash_Erase_Sector( FLASH_CFC_WD_MAIN<<8 );
            //W25X16_SPI_Write_nBytes( FLASH_CFC_WD_MAIN<<8) ,  (unsigned char *)&flash_flag_info_main, sizeof(FLASH_flag_Info_t) );
            delay_ms(100);//100ms
            W25X16_Flash_Erase_Sector( FLASH_CFC_WD_BACK<<8 );
            //W25X16_SPI_Write_nBytes( FLASH_CFC_WD_BACK<<8) ,  (unsigned char *)&flash_flag_info_back, sizeof(FLASH_flag_Info_t) );
        }
        if(num == 2 )
        {
            W25X16_Flash_Erase_Sector( FLASH_CHECK_WD_MAIN<<8);
            //W25X16_SPI_Write_nBytes( FLASH_CHECK_WD_MAIN<<8),  (unsigned char *)&flash_flag_info_main, sizeof(FLASH_flag_Info_t) );
            delay_ms(100);//100ms
            W25X16_Flash_Erase_Sector( FLASH_CHECK_WD_BACK<<8 );
            //W25X16_SPI_Write_nBytes( FLASH_CHECK_WD_BACK<<8),  (unsigned char *)&flash_flag_info_back, sizeof(FLASH_flag_Info_t) );
        }
    }*/

    return 0;
}

unsigned char read_cfc_data(void) //У������
{
    uint32              addr = 0;
    s_check_info_t      temp_check_info;
    //s_check_cr_t        cfc[8];
    uint8               buf[MAX_PKGLEN];
    uint8               sbuf[MAX_PKGLEN];

    //1:����
    if ( m_tempdata.Cfc_temp.count > 9 )
    {
        m_tempdata.Cfc_temp.WD = 0;
        m_tempdata.Cfc_temp.count = 0;
        m_tempdata.Cfc_temp.Flag = false;
        return 0;
    }

    //1:����
    if(m_tempdata.Cfc_temp.count == 0)
    {
        W25X16_SPI_FastRead_nBytes( FLASH_CFC_WD_MAIN<<8,  (unsigned char *)&flash_flag_info_main, sizeof(FLASH_flag_Info_t) );
        W25X16_SPI_FastRead_nBytes( FLASH_CFC_WD_BACK<<8,  (unsigned char *)&flash_flag_info_back, sizeof(FLASH_flag_Info_t) );
        if( flash_flag_info_main.flag == 1)
        {
            m_tempdata.Cfc_temp.WD = flash_flag_info_main.WD;
            m_tempdata.Cfc_temp.count = 0;
        }
        else
        {
            if(flash_flag_info_back.flag == 1)
            {
                m_tempdata.Cfc_temp.WD = flash_flag_info_back.WD;
                m_tempdata.Cfc_temp.count = 0;
            }
            else
            {
                m_tempdata.Cfc_temp.WD = 0;
                m_tempdata.Cfc_temp.count = 0;
                m_tempdata.Cfc_temp.Flag = false;
                memset(sbuf,0,MAX_PKGLEN);
                sprintf( (char *)sbuf, "<%s>\r\n", "crc no record" );
                uartSendStr(0, sbuf, strlen((char *)sbuf));
                return 0;
            }
        }
    }

    //2����flash��
    memset(buf,0,MAX_PKGLEN);
    memset(sbuf,0,MAX_PKGLEN);
    addr = (FLASH_CFC_START + m_tempdata.Cfc_temp.WD * 16)<< 8;
    W25X16_SPI_FastRead_nBytes(addr,  (unsigned char *)&temp_check_info, sizeof(s_check_info_t));
    if(temp_check_info.flag == 1)
    {
        addr += sizeof(s_check_info_t);
        W25X16_SPI_FastRead_nBytes(addr,  (unsigned char *)&buf, temp_check_info.len);
        sprintf((char *)sbuf,"<%s,%03d,1,%s>\r\n",sensor_di, m_defdata.m_baseinfo.id, buf);
        uartSendStr(0, sbuf, strlen((char *)sbuf));
    }
    else
    {
        memset(sbuf,0,MAX_PKGLEN);
        if( m_tempdata.Cfc_temp.count == 0)
        {
            sprintf( (char *)sbuf, "<%s>\r\n", "crc no record" );
            uartSendStr(0, sbuf, strlen((char *)sbuf));
        }

        m_tempdata.Cfc_temp.WD = 0;
        m_tempdata.Cfc_temp.count = 0;
        m_tempdata.Cfc_temp.Flag = false;

        return 0;
    }

    if(m_tempdata.Cfc_temp.WD == 0)
    {
        m_tempdata.Cfc_temp.WD = 9;
    }
    else
    {
        m_tempdata.Cfc_temp.WD--;
    }

    m_tempdata.Cfc_temp.count++;

  return 0;
}
unsigned char read_check_data(void) //�춨
{
    uint32              addr = 0;
    s_check_info_t      temp_check_info;
    uint8               buf[MAX_PKGLEN];
    uint8               sbuf[MAX_PKGLEN];

    //1:����
    if ( m_tempdata.Check_temp.count > 9 )
    {
        m_tempdata.Check_temp.WD = 0;
        m_tempdata.Check_temp.count = 0;
        m_tempdata.Check_temp.Flag = false;
        return 0;
    }

    //1:����
    if(m_tempdata.Check_temp.count == 0)
    {
        W25X16_SPI_FastRead_nBytes( FLASH_CHECK_WD_MAIN<<8,  (unsigned char *)&flash_flag_info_main, sizeof(FLASH_flag_Info_t) );
        W25X16_SPI_FastRead_nBytes( FLASH_CHECK_WD_BACK<<8,  (unsigned char *)&flash_flag_info_back, sizeof(FLASH_flag_Info_t) );
        if( flash_flag_info_main.flag == 1)
        {
            m_tempdata.Check_temp.WD = flash_flag_info_main.WD;
            m_tempdata.Check_temp.count = 0;
        }
        else
        {
            if(flash_flag_info_back.flag == 1)
            {
                m_tempdata.Check_temp.WD = flash_flag_info_back.WD;
                m_tempdata.Check_temp.count = 0;
            }
            else
            {
                m_tempdata.Check_temp.WD = 0;
                m_tempdata.Check_temp.count = 0;
                m_tempdata.Check_temp.Flag = false;
                memset(sbuf,0,MAX_PKGLEN);
                sprintf( (char *)sbuf, "<%s>\r\n", "check no record" );
                uartSendStr(0, sbuf, strlen((char *)sbuf));
                return 0;
            }
        }
    }

    //2����flash��
    memset(buf,0,MAX_PKGLEN);
    memset(sbuf,0,MAX_PKGLEN);
    addr = (FLASH_CHECK_START + m_tempdata.Check_temp.WD * 16)<< 8;
    W25X16_SPI_FastRead_nBytes(addr,  (unsigned char *)&temp_check_info, sizeof(s_check_info_t));
    if(temp_check_info.flag == 1)
    {
        addr += sizeof(s_check_info_t);
        W25X16_SPI_FastRead_nBytes(addr,  (unsigned char *)&buf, temp_check_info.len);
        sprintf((char *)sbuf,"<%s,%03d,2,%s>\r\n",sensor_di, m_defdata.m_baseinfo.id, buf);
        uartSendStr(0, sbuf, strlen((char *)sbuf));
    }
    else
    {
        memset(sbuf,0,MAX_PKGLEN);
        if( m_tempdata.Check_temp.count == 0)
        {
            sprintf( (char *)sbuf, "<%s>\r\n", "check no record" );
            uartSendStr(0, sbuf, strlen((char *)sbuf));
        }

        m_tempdata.Check_temp.WD = 0;
        m_tempdata.Check_temp.count = 0;
        m_tempdata.Check_temp.Flag = false;

        return 0;
    }

    if(m_tempdata.Check_temp.WD == 0)
    {
        m_tempdata.Check_temp.WD = 9;
    }
    else
    {
        m_tempdata.Check_temp.WD--;
    }

    m_tempdata.Check_temp.count++;

  return 0;
}
unsigned char save_cr(uint8 num, s_check_info_t * check_info, char * data)
{
    uint8 var = 0;
    uint32 addr = 0, addr_capture_data = 0;
    uint8 result = 0;
    uint32 len = 0;

    if(num == 1 )
    {
        addr = FLASH_CFC_START;
        len = check_info->len;
        W25X16_SPI_FastRead_nBytes( FLASH_CFC_WD_MAIN<<8,  (unsigned char *)&flash_flag_info_main, sizeof(FLASH_flag_Info_t) );
        W25X16_SPI_FastRead_nBytes( FLASH_CFC_WD_BACK<<8,  (unsigned char *)&flash_flag_info_back, sizeof(FLASH_flag_Info_t) );
    }
    else if (num == 2)
    {
        addr = FLASH_CHECK_START;
        len = check_info->len;
        W25X16_SPI_FastRead_nBytes( FLASH_CHECK_WD_MAIN<<8,  (unsigned char *)&flash_flag_info_main, sizeof(FLASH_flag_Info_t) );
        W25X16_SPI_FastRead_nBytes( FLASH_CHECK_WD_BACK<<8,  (unsigned char *)&flash_flag_info_back, sizeof(FLASH_flag_Info_t) );
    }
    else
        return 0;

    if(flash_flag_info_main.flag == 1)
    {
        var = flash_flag_info_main.WD;
        if( ((int)(flash_flag_info_main.WD) >= 0) && (flash_flag_info_main.WD <= 9) )
        {
            if(flash_flag_info_main.WD == 9)
            {
                var = 0;
            }
            else
            {
                var ++;
            }
        }
    }
    else
    {
        if(flash_flag_info_back.flag == 1)
        {
            var = flash_flag_info_back.WD;
            if( ((int)(flash_flag_info_back.WD) >= 0) && (flash_flag_info_back.WD <= 9) )
            {
                if(flash_flag_info_back.WD == 9)
                {
                    var = 0;
                }
                else
                {
                    var ++;
                }
            }
        }
        else
        {
            if( (flash_flag_info_back.flag == 0xFF) && (flash_flag_info_main.flag == 0xFF) )
            {
                var = 0;
            }
            else
            {
                return 2;
            }
        }
    }

    addr = (addr + var * 16)<< 8;

    //W25X16_SPI_FastRead_nBytes(addr, &flag, 1);

    W25X16_Flash_Erase_Sector( addr );
    check_info->flag = 1;
    W25X16_SPI_Write_nBytes(addr, (unsigned char *)check_info, sizeof(s_check_info_t)); //��
    addr += sizeof(s_check_info_t); //��ַƫ��
    //if(256 )
    //{
    if(len > (256-sizeof(s_check_info_t)) )
    {
        W25X16_SPI_Write_nBytes( addr, (unsigned char *)data + addr_capture_data, 256-sizeof(s_check_info_t) );
        addr = addr + (256-sizeof(s_check_info_t));
        len = len - (256-sizeof(s_check_info_t));
        addr_capture_data = addr_capture_data + (256-sizeof(s_check_info_t));

        while(1)
        {
            if(len > 256)
            {
                W25X16_SPI_Write_nBytes( addr, (unsigned char *)data + addr_capture_data, 256);
                addr += 256;
                len -= 256;
                addr_capture_data += 256;
            }
            else
            {
                W25X16_SPI_Write_nBytes(addr, (unsigned char *)data + addr_capture_data, len);
                break;
            }
       }
    }
    else
    {
        W25X16_SPI_Write_nBytes(addr, (unsigned char *)data + addr_capture_data, len);
    }

    flash_flag_info_main.flag = 1;
    flash_flag_info_back.flag = 1;
    flash_flag_info_main.WD = var;
    flash_flag_info_back.WD = var;
    if(num == 1 )
    {
        W25X16_Flash_Erase_Sector( FLASH_CFC_WD_MAIN << 8 );
        W25X16_SPI_Write_nBytes( FLASH_CFC_WD_MAIN << 8,  (unsigned char *)&flash_flag_info_main, sizeof(FLASH_flag_Info_t) );
        delay_ms(100);//100ms
        W25X16_Flash_Erase_Sector( FLASH_CFC_WD_BACK << 8 );
        W25X16_SPI_Write_nBytes( FLASH_CFC_WD_BACK << 8,  (unsigned char *)&flash_flag_info_back, sizeof(FLASH_flag_Info_t) );
    }
    if(num == 2 )
    {
        W25X16_Flash_Erase_Sector( FLASH_CHECK_WD_MAIN << 8 );
        W25X16_SPI_Write_nBytes( FLASH_CHECK_WD_MAIN << 8,  (unsigned char *)&flash_flag_info_main, sizeof(FLASH_flag_Info_t) );
        delay_ms(100);//100ms
        W25X16_Flash_Erase_Sector( FLASH_CHECK_WD_BACK << 8 );
        W25X16_SPI_Write_nBytes( FLASH_CHECK_WD_BACK << 8,  (unsigned char *)&flash_flag_info_back, sizeof(FLASH_flag_Info_t) );
    }

    result = 0;

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

