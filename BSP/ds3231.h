/*
* ds3231.h
*
*  Created on: 2017年8月1日
*      Author: lenovo
*/

#ifndef SOURCE_MODULE_RTC_DS3231_H_
#define SOURCE_MODULE_RTC_DS3231_H_


#include "typedef.h"
#include "defpam.h"

#define DS3231_ADDR		0xD0

/*
//RTC3231
#define RTC3231_SCL_BIT BIT2 //P10.2
#define RTC3231_SDA_BIT BIT1 //P10.1
//#define RTC3231_POWER_BIT BIT3 //P1.3

#define RTC3231_SCK_FUNC_IO()   P10SEL &= ~RTC3231_SCL_BIT
#define RTC3231_SCL_IN()        P10DIR &=~ RTC3231_SCL_BI
#define RTC3231_SCL_OUT()       P10DIR |= RTC3231_SCL_BIT
#define RTC3231_SCL_HIGH()      P10OUT |= RTC3231_SCL_BIT
#define RTC3231_SCL_LOW()      P10OUT &=~ RTC3231_SCL_BI

#define RTC3231_SDA_FUNC_IO()   P10SEL &= ~RTC3231_SDA_BIT      // SDA和SCK为普通IO口
#define RTC3231_SDA_IN()        P10DIR &=~ RTC3231_SDA_BIT
#define RTC3231_SDA_OUT()       P10DIR |= RTC3231_SDA_BIT
#define RTC3231_SDA_HIGH()      P10OUT |= RTC3231_SDA_BIT
#define RTC3231_SDA_LOW()       P10OUT &=~ RTC3231_SDA_BIT
#define RTC3231_SDA_VALUE()     (P10IN & RTC3231_SDA_BIT)
*/
/*
#define RTC3231_POWER_FUNC_IO()     P1SEL &=~ RTC3231_POWER_BIT
#define RTC3231_POWER_OUT()         P1DIR |= RTC3231_POWER_BIT
#define RTC3231_POWER_ON()          P1OUT |= RTC3231_POWER_BIT
#define RTC3231_POWER_OFF()         P1OUT &=~ RTC3231_POWER_BIT
*/
/*
#define SCK_DIR_OUT()   RTC3231_SCL_OUT()
#define SCK_H()			RTC3231_SCL_HIGH()
#define SCK_L()			RTC3231_SCL_LOW()

#define SDA_DIR_OUT()   RTC3231_SDA_OUT()
#define SDA_H()			RTC3231_SDA_HIGH()
#define SDA_L()			RTC3231_SDA_LOW()

#define SDA_DIR_IN()	RTC3231_SDA_IN()							//
#define SDA_READ_IN()   RTC3231_SDA_VALUE()



#define _nop_(n)	{unsigned int dly; for(dly=0; dly<(10*n); dly++);}
*/
//
//IIC
void IICinit(void);
void iic_start(void);
void iic_stop(void);
unsigned char IICSendByte(unsigned char ch);
unsigned char IICreceiveByte(unsigned char ack);
//UTCTimeStruct RTC;
unsigned char DS3231_WriteStatusReg(unsigned char dat);
unsigned char DS3231_ReadCtrReg(unsigned char *pdat);
unsigned char DS3231_WriteCtrReg(unsigned char dat);
unsigned char DS3231_ReadTemperature(int *pdat);

void IICreceiveNbyte(unsigned char exn, unsigned char n);
void IICwriteNbyte(unsigned char n);

void HalDS3231_Init(void);

unsigned char DS3231_ReadStatusReg(unsigned char *pdat);
unsigned char DS3231_ReadCtrReg(unsigned char *pdat);

unsigned char char_trans_bcd(unsigned char char_data);

unsigned char DS3231_SetTime(s_RtcTateTime_t *prtc);
unsigned char DS3231_ReadTime(s_RtcTateTime_t *prtc);

#endif /* SOURCE_MODULE_RTC_DS3231_H_ */
