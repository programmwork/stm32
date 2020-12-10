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

TIM_HandleTypeDef htim1;


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
    AirP_Init();
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
    AirP_Init();
}

/*********************************************************************************
** 閸戣姤鏆熼崥宥囆�? 閿涙nsigned char AirP_engine(float *result)
** 閸戣姤鏆熼崝鐔诲�? 閿涙俺顕伴崣鏍ㄧ毜閸樺鏆熼幑锟�?
** 閸忋儱褰涢崣鍌涙�? 閿涳�?
**
** 閸戝搫褰涢崣鍌涙�? 閿涳�?0閿涙碍顒滈崷銊嚢閸欐牔鑵� 1閿涙俺顕伴崣鏍ㄥ灇閸旓拷  2閿涙俺顕伴崣鏍у毉闁匡�?
*********************************************************************************/
unsigned char AirP_engine(float *result)
{
    // 閸欐牕锟借壈瀵栭崶锟�?0-5,0:PTB301,1:PTB210,2:PTB330,3:閸楀簼绨▔鎵�?
    uint32 count = 50, i = 0;
    unsigned char buffer[10], buf_temp[32];

    //��ȡ��ǰ�ź�״̬
    unsigned char phase = AirP_GetProcessingPhase();
    if(bcm_info.sensor.ce == 0)//PTB201
    {        
        memset(buf_temp, 0x00, sizeof(buf_temp));
        if(phase == USART_PROCESSING_IDEL)// 閸掋倖鏌囬弰顖氭儊缁屾椽妫�
        {        
            TxRxIndex = 0;
            TxRxLength = 0;
            strcpy((char *)buffer,".P\r\n");
            RevStep = 1;
            uartSendStr(UARTDEV_3, (UINT8 *)&buffer, sizeof(".P\r\n") - 1);

            UartProcessingPhase = USART_PROCESSING_SENDING;
            while(count)
            {                
                if(USART_PROCESSING_FINISH == UartProcessingPhase)
                {
                    AirP_ResetProcessingPhase();

                    if((TxRxLength != 10) && (TxRxLength != 11)) //���ȴ��� 
                    {
                        result[0] = INVALID_DATA;
                    }
                    else
                    {                    
                        while(0x20 != TxRxBuffer[i])
                        {
                            buf_temp[i] = TxRxBuffer[i];
                            i++;
                        }
                        result[0] = atof(buf_temp);                  
                    }

                    TxRxIndex = 0;
                    TxRxLength = 0;

                    break;
                }
                
                if(USART_PROCESSING_ERR == UartProcessingPhase)
                {
                    TxRxIndex = 0;
                    TxRxLength = 0;

                    AirP_ResetProcessingPhase();
                    result[0] = INVALID_DATA;
                    
                    break;
                }
                
                count--;

                vTaskDelay(20);                       
            }
            if(count <= 0)
            {
                TxRxIndex = 0;
                TxRxLength = 0;

                AirP_ResetProcessingPhase();

                result[0] = INVALID_DATA;
            }
        }
        else           
        {
            TxRxIndex = 0;
            TxRxLength = 0;

            AirP_ResetProcessingPhase();

            result[0] = INVALID_DATA;
        }
        
        return 1;
    }        
    else if(bcm_info.sensor.ce == 3)//PTB301
    {
        if(phase == USART_PROCESSING_IDEL)// 閸掋倖鏌囬弰顖氭儊缁屾椽妫�
        {        
            TxRxIndex = 0;
            TxRxLength = 0;
            strcpy((char *)buffer, "01:R\r\n");
            RevStep = 1;
            
            uartSendStr(UARTDEV_3, (UINT8 *)&buffer, sizeof(buffer));

            UartProcessingPhase = USART_PROCESSING_SENDING;
            while(count)
            {                
                if(USART_PROCESSING_FINISH == UartProcessingPhase)
                {
                    AirP_ResetProcessingPhase();

                    if((TxRxLength != 9) && (TxRxLength != 10)) //���ȴ��� 
                    {
                        result[0] = INVALID_DATA;
                    }
                    else
                    {                    
                        if(memcmp("01:",TxRxBuffer,3))
                        {
                            result[0] = INVALID_DATA;
                        }
                        else
                        {
                            *result = atof(&TxRxBuffer[3]);
                        }                         
                    }

                    TxRxIndex = 0;
                    TxRxLength = 0;

                    break;
                }
                
                if(USART_PROCESSING_ERR == UartProcessingPhase)
                {
                    TxRxIndex = 0;
                    TxRxLength = 0;

                    AirP_ResetProcessingPhase();
                    result[0] = INVALID_DATA;
                    
                    break;
                }
                
                count--;

                vTaskDelay(20);                       
            }
            if(count <= 0)
            {
                TxRxIndex = 0;
                TxRxLength = 0;

                AirP_ResetProcessingPhase();

                result[0] = INVALID_DATA;
            }
        }
        else           
        {
            TxRxIndex = 0;
            TxRxLength = 0;

            AirP_ResetProcessingPhase();

            result[0] = INVALID_DATA;
        }
        
        return 1;
    }
}



