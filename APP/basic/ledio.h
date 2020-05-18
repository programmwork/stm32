/*
 * ledio.h
 *
 *  Created on: 2016��3��7��
 *      Author: lenovo
 */

#ifndef SOURCE_BASIC_LEDIO_H_
#define SOURCE_BASIC_LEDIO_H_

#include "typedef.h"
#include "driverlib.h"
enum{
led0=0,
RUN_LED=led0,
led1,
NET_LED=led1,
led2,
led3,
led4,
RUN2_LED=led4,//2�����еƣ�������һ����������һ��
LEDNUM,
};
enum{
LED_OFF=0,
LED_ON,
LED_SLOW_FLASH,//��˸
LED_FAST_FLASH,
};

typedef struct{
	U8 ledstate;
	U8 ledflashflag;
	U8 ledflashcounter;
}LEDFLASHSATAUS;//��˸״̬


#define LED_0_STATE(x)   {if(x==1)P11OUT &=~BIT0;else P11OUT|=BIT0;LED_4_STATE(x)}
#define LED_1_STATE(x)   {if(x==1)P10OUT &=~BIT7;else P10OUT|=BIT7;}
#define LED_2_STATE(x)   {if(x==1)P10OUT &=~BIT6;else P10OUT|=BIT6;}
#define LED_3_STATE(x)   {if(x==1)P10OUT &=~BIT3;else P10OUT|=BIT3;}
#define LED_4_STATE(x)   {if(x==1)P5OUT &=~BIT2;else P5OUT|=BIT2;}



void Init_Ledio(void);

void ledOffAll(void);
void ledOnAll(void);

void ledSetState(U8 ledno,U8 state);
void checkLedflash(U8 ledno);

void Init_Pwr(void);

void updatetime_LED();
void checkLEDS(void);



void Init_Port();
#endif
