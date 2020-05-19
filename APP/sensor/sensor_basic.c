/*
 * sensor_basic.c
 *
 *  Created on: 2018年1月24日
 *      Author: HYSD
 */

//#include <msp430.h>
#include "sensor_basic.h"
#include <stdio.h>
#include <string.h>

sensor_state_t sensor_state;

uint8 Flag_nodata = 0;//没有数据标志位

FLASH_Store_MinData_t  FLASH_Store_MinData;//存储结构钵
FLASH_Store_MinData_t  FLASH_Read_MinData;//读数据结构体
sensors_data_t sensors_data;
//板温 板压  上下限
float board_temp_max = 0;
float board_temp_min = 0;
float board_volt_max = 0;
float board_volt_min = 0;

#define V_R_L                   4.7
#define V_R_H                   51.0

uint32  check_offset = 0;   //检定读取偏移地址
uint32  cfc_offset = 0;     //校正都取偏移地址

/*********************************************************************************
** 函数名称 ：BanWenBanYa_engine
** 函数功能 ：测量板温板压
** 入口参数 ：
** 出口参数 ：
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

 /* 采集板温 */
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
        // 计算板温
        value = valueTSum / counter;
        *pT = (float)value ;        //求出温度
        flag = 1;
    }
    else
    {
        flag &= ~1;
    }
    valueTSum = 0;
    counter = 0;

  /* 采集板压 */
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
        // 计算板压
        value = valueVSum / counter;
        fp_1 = (3.3 * (float)value) / 4096;                                     // 求出电压值
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

    //每秒钟调用执行
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
            else//运行模式
            {
                if(bcm_info.common.mo==0)//无线
                {

                }
                else if(bcm_info.common.mo==1)//本地串口
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
  //执行一次存储操作
    static uint32 time_mindata_store;

    Convert_Time_to_min(&time_mindata_store);

    //转换成分钟数  调用此函数时已到了下一分钟，所以需要减去1
    //若是秒数据存储就需要另外开辟一个时间结构体标志位到就先保存此结构体
    FLASH_Store_MinData.time = time_mindata_store;
    save_record_min(time_mindata_store,(unsigned char *)&FLASH_Store_MinData,sizeof(FLASH_Store_MinData_t));
}

/***********************************************************************************************************
** 函数名称 ：unsigned char save_record_min(unsigned long timep, unsigned char dat[],unsigned char len)
** 函数功能 ：将数据存储到FLASH中
** 入口参数 ：
**
** 出口参数 ：0:存储失败  1:存储成功
***********************************************************************************************************/
unsigned char save_record_min(unsigned long timep, unsigned char dat[], unsigned char len)
{
    unsigned long pageIndex;
    unsigned long addr;
    unsigned char flag;

    if(len > (Flash_PAGE_SIZE / 2))  return 0;
    pageIndex = (timep % Flash_PAGE_NUMBER) + FLASH_DATA_START;   // 数据位于哪个扇区

    //  pageIndex = timep % Flash_PAGE_NUMBER;      // 数据位于哪个扇区
    addr = pageIndex << 8;

    if((pageIndex & 0x0F) == 0)                         // 判断是否为一个sector的开始
    {
        W25Q128_Erase_Chip(W25Q128_ERS_SEC, addr);
        W25Q128_Data_Prog(addr, dat, len);              // 向FLASH中存储数据
    }
    else
    {
        W25Q128_Data_Read(addr, &flag, 1);  // 读取主存储区
        if(flag == 0xFF)
        {
            W25Q128_Data_Prog(addr, dat, len);               // 向FLASH中存储数据
        }
        else
        {
            // 主存储区有数据则读取备份存储区，看是否空闲
            addr += (Flash_PAGE_SIZE / 2);
            W25Q128_Data_Read(addr, &flag, 1);
            if(flag == 0xFF)
            {
                W25Q128_Data_Prog(addr, dat, len);   // 向FLASH中存储数据
            }
            else return 0;                          // 写入失败
        }
    }
    return 1;                                   // 正确
}

/***********************************************************************************************************
** 函数名称 ：unsigned char read_record_min(unsigned long timep, unsigned char dat[], unsigned char len)
** 函数功能 ：读取分钟数据
** 入口参数 ：
**
** 出口参数 ：0:读取失败  1:读取成功
***********************************************************************************************************/
unsigned char read_record_min(unsigned long timep, unsigned char dat[], unsigned char len)
{
    unsigned long pageIndex;
    unsigned long addr;
    unsigned char flag;


    if(len > (Flash_PAGE_SIZE / 2))  return 0;
    pageIndex = (timep % Flash_PAGE_NUMBER) + FLASH_DATA_START;   // 数据位于哪个扇区
    addr = (pageIndex << 8) + (Flash_PAGE_SIZE / 2);

    W25Q128_Data_Read(addr, &flag, 1);     // 读取备份存储区

    if(flag == 0xFF)                   // 备份存储区中没有数据
    {
        addr -= (Flash_PAGE_SIZE / 2);
        W25Q128_Data_Read(addr, &flag, 1);  // 读取主存储区

        if(flag == 0xFF)
            return 0;      // 主存储区中没有数据
        else
            W25Q128_Data_Read(addr, dat, len);// 主存储区中有数据
    }
    else
    {
        W25Q128_Data_Read(addr, dat, len); // 备份存储区中有数据
    }
    return 1;
}

unsigned char read_history_mindata(void)
{
    if(m_tempdata.Count_read_history>0)
    {
        if(read_record_min(m_tempdata.time_start_read,(unsigned char *)&FLASH_Read_MinData,sizeof(FLASH_Read_MinData))!=1)//读一条数据出来
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
        Flag_nodata = 0;//无数据标志位清0
        m_tempdata.time_start_read = m_tempdata.time_start_read+m_tempdata.time_interval_read ;//起始时间指向下一分钟
        m_tempdata.Count_read_history--;
    }
    return m_tempdata.Count_read_history;
}

unsigned char show_history_mindata(void)
{
    if(m_tempdata.Count_down_history>0)
    {
        if(read_record_min(m_tempdata.time_start,(unsigned char *)&FLASH_Read_MinData,sizeof(FLASH_Read_MinData))!=1)//读一条数据出来
            Flag_nodata = 1;
        else
        {
            if(FLASH_Read_MinData.time != m_tempdata.time_start)
            Flag_nodata = 1;
        }

        memset(&sensors_data.historyPacket,0,MAX_PKG_LEN);
        sensors_data.historyPacketLen=GetMinData((char *)sensors_data.historyPacket,&sensors_data,0);

        uartSendStr(0,(unsigned char *)sensors_data.historyPacket,sensors_data.historyPacketLen);
        Flag_nodata = 0;//无数据标志位清0
        m_tempdata.time_start = m_tempdata.time_start+m_tempdata.time_interval ;//起始时间指向下一分钟
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
 * 函数名称： S32ToStrAlignRight
 * 函数功能：数值转换为字符串的函数
 * (CharNum的长度应大于数据的总长度,如果不满足将忽略CharNum,按实际长度显示)
 * 入口参数：   signed long dat   待转换的整数
            unsigned char charNum 转换后字符串的长度(数字前面不足CharNum的位置用指定字符填充)
            char  *str_out_1  输出字符串
 * 出口参数：    无
 * 编写时间：
 * 修改时间：
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

    // 计算一共有几位字符(包括小数点和负号)
    if(dat < 0)
    {
        fh = 1;
        dat = -dat;
    }
    tc = t_i_in = dat;
    while(t_i_in /= 10)        total++;                           // 判断有效数字一共有几位
    k = total;
    if(dot >= total) total =  dot+2;                      // 当小数点位数大于等于有效数字位数时
    // 规律:字符的总位数为小数点的位置+2,即加一个".",加一个"0"
    else if(0 != dot) total++;                                    // 加一个"."

    if(fh) total++;                                                           // 是负数再加1,total为总的字符的数

    if(charNum < total)
    {
        d = total - charNum;
        charNum = total;                                  // 判断入口参数的合法性
    }

    if(fh)     *str_out++ ='-';                                       // 如果是负数在空格后加"-"

    for(i=0;i<charNum-total;i++)                              // 在不足的位置填充空格
        *str_out++ = charFill;

    if(dot >= k)                                                              // 小数点的位置在最高位有效数字的前面,即为纯小数
    {
        *str_out++ ='0';
        *str_out++ ='.';
        for(i=0;i<dot-k;i++)    *str_out++ = '0';       // 规律:小数点后有DotPosition-k个0
        str_out = str_out+k;                                            // 先在最末尾加'\0'
        *str_out-- ='\0';
        for(i=0;i<k;i++)                                                    // 在之间添加有效数字
        {
            *str_out-- = tc % 10+'0';
            tc /= 10;
        }
    }
    else                                                                              // 小数点的位置在最高位有效数字的后面,即不为纯小数
    {
        if(0 != dot) str_out = str_out + k + 1;     // 为小数
        else str_out = str_out + k;                             // 为整数
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
 * 函数名称： Data_Copy
 * 函数功能：分钟数据拷贝
 *
 * 入口参数：    qcfalshdata_t *FLASH_Store  存储到flash的数据及qc
 *          qcfalshdata_t *FLASH_Read   从flash读取的数据及qc
 *          qcdata_t *sensor            传感器采集到的数据及qc
 *          qcfalshdata_t *Mindata_temp 用于显示的数据及qc
 *          unsigned char mode          判定为读取flash 还是写flash 以及当前显示
 *          unsigned char multiple      比例因子， 10的multiple次幂
 * 出口参数：  无
 * 编写时间：
 * 修改时间：
==================================================================*/
void Data_Copy(qcfalshdata_t *FLASH_Store, qcfalshdata_t *FLASH_Read, qcdata_t *sensor, qcfalshdata_t *Mindata_temp, unsigned char mode, unsigned char multiple)
{
    float temp = 0, min_data = 0;
    double integer = 0, dot = 0;

    temp = pow(10,multiple);

    if(mode == 1)//需要存储分钟数据，来自主动调用
    { //数据保存至flash结构体   存储时数据均放大100倍取整

      if(sensor->data == INVALID_DATA)
      {
        FLASH_Store->data = INVALID_DATA;
        Mindata_temp->data = INVALID_DATA;
      }
      else
      {
          min_data = (sensor->data)*temp;

          //四舍五入
          dot = modf(min_data, &integer);
          if(0.5 <= dot)            min_data = integer+1;
          else if(-0.5 >= dot)      min_data = integer-1;

        FLASH_Store->data = (long)((min_data*100)/temp);
        Mindata_temp->data = min_data;
      }

      FLASH_Store->qc = sensor->qc;
      Mindata_temp->qc = sensor->qc;
    }

    else if((mode == 0)||(mode == 2))//不需要存储的分钟数据，来自历史数据读取调用
    {
        if(FLASH_Read->data == INVALID_DATA)
            Mindata_temp->data = INVALID_DATA;
        else
        {
            if(multiple >= 2)
            {
                temp = pow(10,multiple-2);
                Mindata_temp->data = (float)((float)FLASH_Read->data*temp);//比例因子
            }
            else
            {
                temp = pow(10,2-multiple);
                Mindata_temp->data = (float)((float)FLASH_Read->data/temp);//比例因子
            }
        }

        Mindata_temp->qc   = FLASH_Read->qc;
    }
    else ;    //error
}

/*==================================================================
 * 函数名称： Pack_Data
 * 函数功能：打包数据
 *
 * 入口参数：    qcfalshdata_t *Mindata_temp 用于显示的数据及qc
 *          unsigned int *len           本数据每个字符所在打包函数的地址
 *          char *Header_packet         打包函数字符串
 *          unsigned char data_len      字节长度
 * 出口参数：    无
 * 编写时间：
 * 修改时间：
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
        temp_data = (long)(Mindata_temp->data);   //放大1倍
        S32ToStrAlignRight(temp_data, data_len, Header_packet+*len);
    }
    *len=strlen((char *)Header_packet);
    Header_packet[*len]=',';
    (*len)++;
}

/*==================================================================
 * 函数名称： Pack_Datal
 * 函数功能：标差的打包数据
 *
 * 入口参数：    long *Mindata_temp          用于显示数据
 *          unsigned int *len           本数据每个字符所在打包函数的地址
 *          char *Header_packet         打包函数字符串
 *          unsigned char data_len      字节长度
 * 出口参数：    无
 * 编写时间：
 * 修改时间：
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
** 函数名称 ：float StandardDeviation( float *pData, unsigned char *pQC, unsigned int len)
** 函数功能 ：计算标准差
** 入口参数 ：
** 出口参数 ：无
**********************************************************************************************************/
float StandardDeviation( float *pData, unsigned char *pQC, unsigned int len)
{
  float sum = 0, sum_f = 0, average = 0;
  unsigned int i,validNumber;

  // 计算有效样本的平均值
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

  // 计算标准差
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

