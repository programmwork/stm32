/*
;*--------------------------------------------- 閺傚洣娆㈡穱鈩冧�? ----------------------------------------------------
;* 閺傚洣娆㈤崥宥囆�? : 1111111
;* 閺傚洣娆㈤崝鐔诲�? :
;* 鐞涖儱鍘栫拠瀛樻�? :
;*-------------------------------------------- 閺堬拷閺傛壆澧楅張顑夸繆閹�? -------------------------------------------------
;* 娣囶喗鏁兼担婊嗭拷锟�? :
;* 娣囶喗鏁奸弮銉︽�? :
;* 閻楀牊婀版竟鐗堟�? :
;*-------------------------------------------- 閸樺棗褰堕悧鍫熸拱娣団剝浼� -------------------------------------------------
;* 閺傚洣娆㈡担婊嗭拷锟�? : liujunjie_2008@126.com
;* 閸掓稑缂撻弮銉︽�? :
;* 閻楀牊婀版竟鐗堟�? :
;************************************************************************************************************
*/
#include "sensor_basic.h"

/********************************************************************************
** 閸戣姤鏆熼崥宥囆�? 閿涳�?
** 閸戣姤鏆熼崝鐔诲�? 閿涳�?
** 閸忋儱褰涢崣鍌涙�? 閿涳�?
**
** 閸戝搫褰涢崣鍌涙�? 閿涳�?
********************************************************************************
*/
void hal_sensor_init(void)
{
  Sensor_Init();

  SampleData_Init(&sensors_data);
}

/*
********************************************************************************
** 閸戣姤鏆熼崥宥囆�? 閿涙nsigned char Sensor_Init(void)
** 閸戣姤鏆熼崝鐔诲�? 閿涙碍鐨甸崢瀣╃炊閹扮喎娅掗崚婵嗩潗閸栵�?
** 閸忋儱褰涢崣鍌涙�? 閿涳�?
**
** 閸戝搫褰涢崣鍌涙�? 閿涳�?
********************************************************************************
*/
unsigned char Sensor_Init(void)
{
  AirP_USART1_Init();
  return 1;
}

/*==================================================================
*鍑芥暟锛�?              AirH_rotronic
*浣滆�咃�?    ly
*鏃ユ湡锛�?           2018-01-11
*鍔熻兘锛�? 澶勭悊缃�?崜灏煎厠�?垮害浼犳劅鍣紝涓插彛鏁版嵁
*杈撳叆锛�?          char *buf (杈撳叆鏁版嵁)杈撳叆鏁版嵁瀛楃涓�?
*        float *result        (杈撳嚭鏁版嵁)鎺у埗缁撴瀯鍦板潃
*杩斿洖锛�?         鏃�
*淇敼璁板綍锛�
==================================================================*/
void AirH_rotronic(char *buf,float *result)
{
    char *p = NULL, i = 0;
    float temp[3];

    p = strtok(buf, ";");
    while(p)
    {
       if(1 == i)  {temp[0] = atof(p);} //�?垮害
       if(5 == i)  {temp[1] = atof(p);} //娓╁�?
       if(10 == i) {temp[2] = atof(p);} //闇茬�?

       p = strtok(NULL, ";");
       i++;
    }

    *result = temp[0];
    *(++result) = temp[2];
    *(++result) = temp[1];
}

/*********************************************************************************
** 閸戣姤鏆熼崥宥囆�? 閿涙nsigned char Airp_engine(float *result)
** 閸戣姤鏆熼崝鐔诲�? 閿涙俺顕伴崣鏍ㄧ毜閸樺鏆熼幑锟�?
** 閸忋儱褰涢崣鍌涙�? 閿涳�?
**
** 閸戝搫褰涢崣鍌涙�? 閿涳�?0閿涙碍顒滈崷銊嚢閸欐牔鑵� 1閿涙俺顕伴崣鏍ㄥ灇閸旓拷  2閿涙俺顕伴崣鏍у毉闁匡�?
*********************************************************************************/
unsigned char Airp_engine(float *result)
{// 閸欐牕锟借壈瀵栭崶锟�?0-5,0:PTB301,1:PTB210,2:PTB330,3:閸楀簼绨▔鎵�?

    unsigned char buffer[10];

  if(bcm_info.sensor.ce == 0)//PTB301
  {
    unsigned char phase = AirP_USART1_GetProcessingPhase();
    
    if(phase == AirP_USART1_PROCESSING_IDEL)												// 閸掋倖鏌囬弰顖氭儊缁屾椽妫�
    {
        AirP_TxRxIndex = 0;
        AirP_TxRxLength = 0;
        strcpy((char *)buffer,"{F00RDD}\r\n");
        AirP_RevStep = 1;
        uartSendStr(UARTDEV_3, (UINT8 *)&buffer, sizeof("{F00RDD}\r\n") - 1);

        AirP_UartProcessingPhase = AirP_USART1_PROCESSING_SENDING;
        AirP_T3_START_COUNTING();

        return 0;																							        // 韫囷�?
    }
    
    if(phase == AirP_USART1_PROCESSING_FINISH)											// 閸掋倖鏌囬弰顖氭儊閹恒儲鏁圭�瑰本鍨�?
    {    
      AirP_USART1_ResetProcessingPhase();
      
      if(AirP_TxRxLength != 104)    //閿欒�?
      {
          AirP_TxRxIndex = 0;
          AirP_TxRxLength = 0;
          return 2;
      }
      else
      {
          AirP_TxRxIndex = 0;
          AirP_TxRxLength = 0;
      }
      
      //缃楀崜灏煎厠涓插彛鏁版嵁澶勭悊鍑芥暟
      AirH_rotronic(AirP_TxRxBuffer, result);
      
      if((phase == AirP_USART1_PROCESSING_IDEL) && (Num_sample < (SAMPLE_COUNT-1)))                                         // 閸掋倖鏌囬弰顖氭儊缁屾椽妫�
      {
          AirP_TxRxIndex = 0;
          AirP_TxRxLength = 0;
          strcpy((char *)buffer,"{F00RDD}\r\n");
          AirP_RevStep = 1;
          uartSendStr(UARTDEV_3, (UINT8 *)&buffer, sizeof("{F00RDD}\r\n") - 1);

          AirP_UartProcessingPhase = AirP_USART1_PROCESSING_SENDING;
          AirP_T3_START_COUNTING();                                                                                                // 韫囷�?
      }

      return 1;																							    // 鐠佸墽鐤嗛幋鎰�?
    }
    
    if(phase == AirP_USART1_PROCESSING_ERR)											// 閸掋倖鏌囬弰顖氭儊閸戞椽鏁�
    {
        AirP_TxRxIndex = 0;
        AirP_TxRxLength = 0;

      AirP_USART1_ResetProcessingPhase();
      return 2;																							// 閸戞椽鏁�?
    }
    return 0;		
  }
  else if(bcm_info.sensor.ce == 3)//閸楀簼绨▔鎵�?
  {//ljj濞ｈ濮為敍�?冪箲閸ョ偛锟介棿璐熺拠璇插絿閸掓壆娈戞导鐘冲妳閸ｃ劌锟藉ジ娅�?10閿涘苯褰囨稉锟芥担宥呯毈閺侊�?
    unsigned long Airp;
    
    if(Airp_USART1_TK_Check(&Airp))  //閺嶏繝鐛欓幋鎰�?
    {
      *result = (float)Airp / 10.0;
      return 1;
    }
    else return 0;
  }
  else
  {
    return 0;
  }
  
  // 韫囷�?
}

/*******************************************************************************
** 閸戣姤鏆熼崥宥囆�? 閿涙oid Reset_Sensor(void)
** 閸戣姤鏆熼崝鐔诲�? 閿涳�?
** 閸忋儱褰涢崣鍌涙�? 閿涳�?
**
** 閸戝搫褰涢崣鍌涙�? 閿涳�?
*******************************************************************************/
void Reset_Sensor(void)
{
  AirP_USART1_Init();
}

void Init_Timer1(unsigned short delayMS)
{
    //Start timer
    UINT16 Period=(UINT16)(1000.0*(float)delayMS);
    Timer_B_clearTimerInterrupt(TIMER_B0_BASE);

    Timer_B_initUpModeParam param = {0};
    param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_8;
    param.timerPeriod = Period;
    param.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    param.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_B_CAPTURECOMPARE_INTERRUPT_ENABLE;
    param.timerClear = TIMER_B_DO_CLEAR;
    param.startTimer = true;
    Timer_B_initUpMode(TIMER_B0_BASE, &param);
}

/*
********************************************************************************
** 闁告垼濮ら弳鐔煎触�?�ュ泦锟�? 闁挎冻鎷�?
** 闁告垼濮ら弳鐔煎礉閻旇鍘� 闁挎稒鑹鹃悾楣冨籍鐠虹儤鐝3濞戞搩鍘介弻鍥嫉�?�ュ懎顫ょ紒瀣儏缁拷
** 闁稿繈鍎辫ぐ娑㈠矗閸屾稒娈� 闁挎冻鎷�?
**
** 闁告垵鎼ぐ娑㈠矗閸屾稒娈� 闁挎冻鎷�?
********************************************************************************
*/ 
#pragma vector=TIMERB0_VECTOR
__interrupt void TIMERB0_ISR(void)
{
    if(T3Timer_start != 0)
    {
        AirP_T3IntCounter++;

        if(AirP_T3IntCounter > 19)  //1绉�
        {
            AirP_UartProcessingPhase = AirP_USART1_PROCESSING_ERR;

            AirP_T3_STOP_COUNTING();
        }
    }
}
