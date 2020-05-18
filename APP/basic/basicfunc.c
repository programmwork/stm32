/*
 * basicfunc.c
 *
 *  Created on: 2016年3月7日
 *      Author: lenovo
 */



#include "basicfunc.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
unsigned int  CRCCheck(unsigned char *pp,unsigned int CRClen)
{
unsigned char a;
unsigned char i,j;
unsigned int CRC16=0xa001;   //密钥
unsigned int crc=0xffff;
if(CRClen>512)
return crc;
for(i=0;i<CRClen;i++)
  {
    crc^=pp[i];
    for(j=0;j<8;j++)
      {
       a=crc&0x0001;
       crc>>=1;
       if(a)
	   	 {crc^=CRC16;}
   	  }
  }
  return crc;
}



int days_of_year_after2017(U16 year, U8 month, U8 date)
{
        U8 month_data[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
        U8 i;
        U16 days=date;
        if((((year%4)==0)&&((year%100)!=0))
            ||((year%400)==0))
	{
           //闰月
            month_data[2]=29;
	}
        if((month>0)&&(month<13))
        {
        	for(i=1;i<month;i++)
        	{
        		days+=month_data[i];
        	}
        }
     return days+(year-2017)*365;
}
int minutes_of_day_now(U8 hour,U8 min,U8 sec)
{

	return hour*60+min;

}


