/*
 * ledio.c
 *
 *  Created on: 2016年3月7日
 *      Author: lenovo
 */



#include "ledio.h"

#include "main.h"

#define MAX_FLASHCOUNTER 50  //传感器休眠需要，32ms定时唤醒，这里的 counter 20一个flash周期
LEDFLASHSATAUS ledstatus[LEDNUM];

void Init_Ledio(void)
{

    GPIO_setAsOutputPin( GPIO_PORT_P2,  GPIO_PIN1 );
    GPIO_setAsOutputPin( GPIO_PORT_P2,  GPIO_PIN2 );
    GPIO_setAsOutputPin( GPIO_PORT_P2,  GPIO_PIN3 );
    GPIO_setAsOutputPin( GPIO_PORT_P2,  GPIO_PIN4 );
}

void ledOnAll(void)
{
ledSetState(led0,LED_ON);
ledSetState(led1,LED_ON);
ledSetState(led2,LED_ON);
ledSetState(led3,LED_ON);
ledSetState(led4,LED_ON);


}
void ledOffAll(void)
{
ledSetState(led0,LED_OFF);
ledSetState(led1,LED_OFF);
ledSetState(led2,LED_OFF);
ledSetState(led3,LED_OFF);
ledSetState(led4,LED_OFF);

}
void ledSetState(U8 ledno,U8 state)
{
  if((state==LED_ON)||(state==LED_OFF))
  {
  switch (ledno)
    {
    case led0:
    LED_0_STATE(state);
    break;

    case led1:
    LED_1_STATE(state);
    break;

    case led2:
    LED_2_STATE(state);
    break;

    case led3:
    LED_3_STATE(state);
    break;

    case led4:
    LED_4_STATE(state);
    break;
    default:
    break;
    }
  }
  else
  {
    switch (ledno)
    {
    case led0:
    case led1:
    case led2:
    case led3:
    case led4:
   ledstatus[ledno].ledstate=state;//快闪、慢闪，只做标识
   ledstatus[ledno].ledflashflag=true;
   ledstatus[ledno].ledflashcounter=0;
    break;

    default:
    break;
    }

  }

}
/*
闪烁，持续时间lastMS--MS
*/
void checkLedflash(U8 ledno)
{

  if(ledstatus[ledno].ledflashflag==false)
  {
  ledstatus[ledno].ledflashcounter=0;
  return;
  }

  if(ledstatus[ledno].ledflashcounter<MAX_FLASHCOUNTER)
  {
    ledstatus[ledno].ledflashcounter++;
    if(ledstatus[ledno].ledstate==LED_SLOW_FLASH)//慢闪
    {

       if((ledstatus[ledno].ledflashcounter%30)==1)
       {
         ledSetState(ledno,LED_ON);
       }
       else if((ledstatus[ledno].ledflashcounter%30)==10)
       {
        ledSetState(ledno,LED_OFF);
       }

    }
    else  if(ledstatus[ledno].ledstate==LED_FAST_FLASH)//快闪
    {
       if(ledstatus[ledno].ledflashcounter%10==1)
       {
         ledSetState(ledno,LED_ON);
       }
       else if((ledstatus[ledno].ledflashcounter%10)==5)
       {
        ledSetState(ledno,LED_OFF);
       }

    }

  }
  else
  {
  ledstatus[ledno].ledflashcounter=0;
  ledstatus[ledno].ledflashflag=false;

     ledstatus[ledno].ledstate=LED_OFF;
     ledSetState(ledno,ledstatus[ledno].ledstate);
  }

}
void updatetime_LED()
{

checkLEDS();

}
void checkLEDS(void)
{
	U8 i;
    for(i=0;i<LEDNUM;i++)
    checkLedflash(i);
}





void Init_Port()
{
 //无用的IO 电平固定，降功耗
    P1OUT = 0;
    P1DIR =  0xFF;

    P2OUT = 0;
    P2DIR =  0xFF;

    P3OUT = 0;
    P3DIR =  0xFF;

    P4OUT = 0;
    P4DIR =  0xFF;

    P5OUT = 0;
    P5DIR =  0xFF;

    P6OUT = 0;
    P6DIR =  0xFF;

    P7OUT = 0;
    P7DIR =  0xFF;

    P8OUT = 0;
    P8DIR =  0xFF;

    P9OUT = 0x00;
    P9DIR =  0xFF;

    P10OUT = 0;
    P10DIR =  0xFF;
}


void Init_Pwr(void)
{

}


