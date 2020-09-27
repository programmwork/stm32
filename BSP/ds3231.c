/*
 * ds3231.c
 *
 *  Created on: 2017年8月1日
 *      Author: lenovo
 */

#include "ds3231.h"
#include <stdio.h>
#include <string.h>
#include "i2c.h"
#include "defpam.h"


#define BUF_SIZE 16
unsigned char SystemError;          	//
unsigned char I2cbuf[BUF_SIZE];

//extern unsigned char DS3231_ReadTime(s_RtcTateTime_t *prtc);
//extern unsigned char DS3231_SetTime(s_RtcTateTime_t *prtc);

/*
********************************************************************************
** 函数名称 ：void DS3231_Init(void)
** 函数功能 ：初始化RTC器件
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/


void HalDS3231_Init(void)
{

	
	 I2CInit(1);
	 delay_ms(100);
	 U8 sta=0;
		DS3231_ReadStatusReg(&sta);
	 DS3231_ReadCtrReg(&sta);
}

/*
********************************************************************************
** 函数名称 ：unsigned char DS3231_ReadStatusReg(unsigned char *pdat)
** 函数功能 ：读取状态寄存器
** 入口参数 ：pdat : 指向存放状态寄存器数据的指针
**
** 出口参数 ：
********************************************************************************
*/
unsigned char DS3231_ReadStatusReg(unsigned char *pdat)
{
	unsigned char mem[10];
	HAL_StatusTypeDef status = HAL_OK;
	
	status = I2C_ReadBuffer(I2C1BASE, DS3231_ADDR, 0x0F, 1, mem, 1);

	if (status != HAL_OK)		return 0;

	*pdat = mem[0];

	return	1;
}

/*
********************************************************************************
** 函数名称 ：unsigned char DS3231_WriteStatusReg(unsigned char dat)
** 函数功能 ：写状态寄存器
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/
unsigned char DS3231_WriteStatusReg(unsigned char dat)
{
		unsigned char mem[10];
		HAL_StatusTypeDef status = HAL_OK;	
	


	
		status = I2C_WriteBuffer(I2C1BASE, DS3231_ADDR, 0x0F, 1, (uint8_t *)dat, 1);

		if (status != HAL_OK)		return 0;

	return	1;
}

/*
********************************************************************************
** 函数名称 ：unsigned char DS3231_ReadCtrReg(unsigned char *pdat)
** 函数功能 ：读取控制寄存器
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/
unsigned char DS3231_ReadCtrReg(unsigned char *pdat)
{
	unsigned char mem[10];
	HAL_StatusTypeDef status = HAL_OK;

	status = I2C_ReadBuffer(I2C1BASE, DS3231_ADDR, 0x0E, 1, (uint8_t *)mem, 1);
	if (status != HAL_OK)		return 0;

	*pdat = mem[0];

	return	1;
}

/*
********************************************************************************
** 函数名称 ：unsigned char DS3231_WriteCtrReg(unsigned char dat)
** 函数功能 ：写控制寄存器
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/
unsigned char DS3231_WriteCtrReg(unsigned char dat)
{
	unsigned char mem[10];
	HAL_StatusTypeDef status = HAL_OK;
	

	status = I2C_ReadBuffer(I2C1BASE, DS3231_ADDR, 0x0E, 1, (uint8_t *)dat, 1);

	if (status != HAL_OK)	return 0;

	return	1;
}

//unsigned char BCD_to_DEC(unsigned char val)
//{
//unsigned char shijinzhi = 0;
//
//shijinzhi = val&0x0f;//按位与，shijinzhi得到低四位数
//
//val >>=4;//右移四位，将高四位移到低四位的位置，得到高四位码值
//val&=0x0f;//防止移位时高位补进1，只保留高四位码值
//val *=10;//高位码值乘以10
//
//shijinzhi +=val;//然后第四位码值相加
//return shijinzhi;//返回得到十进制数
//}

/*
BCD转成十进制数
*/



static unsigned char bcd_trans_char(unsigned char bcd_data)
{
  char i,j;
  char trans_data;
  j=bcd_data;
	i=bcd_data;
  i=i&0x0f;
  j>>=4;
	j&=0x0f;
  trans_data=j*10+i;
  return(trans_data);
}
/*

*/



unsigned char char_trans_bcd(unsigned char char_data)
{
  char i,j;
  unsigned char trans_data;
  i=char_data/10;
	i=i<<4;
	i&=0xf0;
  j=char_data%10;
  trans_data=i+j;
  return(trans_data);
}



/*
********************************************************************************
** 函数名称 ：unsigned char DS3231_SetTime(RTCTIME *prtc)
** 函数功能 ：设置时间
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/
unsigned char DS3231_SetTime(s_RtcTateTime_t *prtc)
{
  short year;
	unsigned char mem[10];
  HAL_StatusTypeDef status = HAL_OK;


	mem[0] = char_trans_bcd(prtc->sec);
	mem[1] = char_trans_bcd(prtc->min);
	mem[2] = char_trans_bcd(prtc->hour);
	//temp = char_trans_bcd(prtc->wday);
	mem[4] = char_trans_bcd(prtc->day);
	mem[5] = char_trans_bcd(prtc->month);

	year = prtc->year - 2000;
	mem[6] = char_trans_bcd(year);
	
	


	status = I2C_WriteBuffer(I2C1BASE, DS3231_ADDR, 0, 1, (uint8_t *)mem, 7);
	if (status != HAL_OK)		return 0;
	else	return	1;
}

/*
********************************************************************************
** 函数名称 ：unsigned char DS3231_ReadTime(RTCTIME *prtc)
** 函数功能 ：读取时间
** 入口参数 ：
**
** 出口参数 ：
********************************************************************************
*/
unsigned char DS3231_ReadTime(s_RtcTateTime_t *prtc)
{
		unsigned char mem[10];
		HAL_StatusTypeDef status = HAL_OK;


	status = I2C_ReadBuffer(I2C1BASE, DS3231_ADDR, 0, 1, (uint8_t *)mem, 7);
	if (status != HAL_OK)			return 0;
			

	prtc->sec 	= 	bcd_trans_char(mem[0]);
	prtc->min 	= 	bcd_trans_char(mem[1]);
	prtc->hour 	= 	bcd_trans_char(mem[2]);
//	temp 	= 	I2cbuf[3];
	prtc->day 	= 	bcd_trans_char(mem[4]);
	prtc->month = 	bcd_trans_char(mem[5]);
	prtc->year 	= 	bcd_trans_char(mem[6]) + 2000;
	return	1;
}

/*********************************************************************************
** 函数名称 ：unsigned char DS3231_ReadTemperature(int *pdat)
** 函数功能 ：读取温度
** 入口参数 ：
**
** 出口参数 ：
*********************************************************************************/

unsigned char DS3231_ReadTemperature(int *pdat)
{
    float fTempNum=0, fTempNum0=0;
    unsigned char mem[10];
		HAL_StatusTypeDef status = HAL_OK;

    status = I2C_ReadBuffer(I2C1BASE, DS3231_ADDR, 0x11, 1, (uint8_t *)mem, 2);

    if (status != HAL_OK) 			return 0;

    fTempNum0 = (mem[1]&0xc0)>>6;  //小数部分
    fTempNum0 *= 0.25;  //小数部分
    if( (mem[0]&0x80) != 0 ) //判断符号位，负数
    {
    fTempNum = -1*((256.0-mem[0])+fTempNum0);  //补码到原码绝对值转换
    }
    else
    {
    fTempNum = mem[0]+fTempNum0;
    }

    *pdat = (int)((fTempNum+0.05)*10);//四舍五入，放大十倍输出，如16.22输出162,16.26输出163
    return 1;
}


