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
    AirH_Init();
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
** 閸戣姤鏆熼崥宥囆�? 閿涙nsigned char AirH_engine(float *result)
** 閸戣姤鏆熼崝鐔诲�? 閿涙俺顕伴崣鏍ㄧ毜閸樺鏆熼幑锟�?
** 閸忋儱褰涢崣鍌涙�? 閿涳�?
**
** 閸戝搫褰涢崣鍌涙�? 閿涳�?0閿涙碍顒滈崷銊嚢閸欐牔鑵� 1閿涙俺顕伴崣鏍ㄥ灇閸旓拷  2閿涙俺顕伴崣鏍у毉闁匡�?
*********************************************************************************/
unsigned char AirH_engine(float *result)
{ 
    UINT8 buffer[10];
    UINT32 count = 50;


    unsigned char phase = AirH_USART3_GetProcessingPhase();

    if(phase == USART_PROCESSING_IDEL)												// 閸掋倖鏌囬弰顖氭儊缁屾椽妫�
    {
        TxRxIndex = 0;
        TxRxLength = 0;
        strcpy((char *)buffer,"{F00RDD}\r\n");
        RevStep = 1;
        uartSendStr(UARTDEV_3, (UINT8 *)&buffer, sizeof("{F00RDD}\r\n") - 1);

        UartProcessingPhase = USART_PROCESSING_SENDING;

        while(count--)
        {                
            if(USART_PROCESSING_FINISH == UartProcessingPhase)
            {
                AirH_USART3_ResetProcessingPhase();

                if(TxRxLength != 104)  
                {
                    result[0] = INVALID_DATA;
                    result[1] = INVALID_DATA;
                    result[2] = INVALID_DATA;

                }
                else
                {                    
                    AirH_rotronic(TxRxBuffer, result);
                }

                TxRxIndex = 0;
                TxRxLength = 0;

                break;
            }
            
            if(USART_PROCESSING_ERR == UartProcessingPhase)
            {
                TxRxIndex = 0;
                TxRxLength = 0;

                AirH_USART3_ResetProcessingPhase();

                result[0] = INVALID_DATA;
                result[1] = INVALID_DATA;
                result[2] = INVALID_DATA;

                break;
            }
        }

        if(count <= 0)
        {
            TxRxIndex = 0;
            TxRxLength = 0;

            AirH_USART3_ResetProcessingPhase();

            result[0] = INVALID_DATA;
            result[1] = INVALID_DATA;
            result[2] = INVALID_DATA;
        }
    }
    else           
    {
        TxRxIndex = 0;
        TxRxLength = 0;

        AirH_USART3_ResetProcessingPhase();

        result[0] = INVALID_DATA;
        result[1] = INVALID_DATA;
        result[2] = INVALID_DATA;
    }
    
    return 1;
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
    AirH_Init();
}


