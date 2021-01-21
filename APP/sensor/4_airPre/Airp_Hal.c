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

#include "ctype.h"




TIM_HandleTypeDef htim1;



int filter_str(char str[], unsigned int len)
{
    int i = 0;
    for(i = 0; i < len; i++)
    {
        if(!(isdigit(str[i])) && (str[i] != 0x2E) && (str[i] != 0x20) && (str[i] != 0x2B) && (str[i] != 0x2D))
        {
            return 0;
        }
    }

    return 1;
}



int Get_P_Value(char *buf, float *airP, float *airT, unsigned char *con)
{
    char *p = NULL, i = 0;

    p = strtok(buf, ",");
    
    while(p)
    {
        switch(i)
        {
            case 0:
            {
                if(filter_str(p, strlen(p)))//if p is number
                {
                    *airP = atof(p);
                }
                else
                {
                    return 0;
                }
            }break;
            case 1:
            {
                if(filter_str(p, strlen(p)))//if p is number
                {
                    *airT = atof(p);
                }
                else
                {
                    return 0;
                }
            }break;
            case 2:
            {
                if(filter_str(p, strlen(p)))//if p is number
                {
                    *con = atoi(p);
                }
                else
                {
                    return 0;
                }
            }break;
            default: return 0;
        }
        p = strtok(NULL, ",");
        i++;
    }   
    
    return 1;
}



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
    unsigned char buffer[10], stat = 0;;
    float airp = 0, airT = 0;
    char buf_temp[32];

    if(bcm_info.sensor.ce == 1)//PTB201
    {        
        memset(buf_temp, 0x00, sizeof(buf_temp));
        if(AirP_UartProcessingPhase == AIRP_USART_PROCESSING_IDEL)// 閸掋倖鏌囬弰顖氭儊缁屾椽妫�
        {        
            AirP_TxRxIndex = 0;
            AirP_TxRxLength = 0;
            strcpy((char *)buffer,".P\r\n");
            AirP_RevStep = 1;
            uartSendStr(UARTDEV_3, (UINT8 *)&buffer, sizeof(".P\r\n") - 1);

            AirP_UartProcessingPhase = AIRP_USART_PROCESSING_SENDING;
            while(count)
            {                
                if(AIRP_USART_PROCESSING_FINISH == AirP_UartProcessingPhase)
                {
                    AirP_ResetProcessingPhase();

                    if((AirP_TxRxLength != 10) && (AirP_TxRxLength != 11)) //���ȴ��� 
                    {
                        result[0] = INVALID_DATA;
                    }
                    else
                    {
                        for(i = 0; i < 7; i++)
                        {
                            buf_temp[i] = AirP_TxRxBuffer[i];
                        }

                        if(1 != (filter_str(buf_temp, strlen(buf_temp))))
                        {
                            result[0] = INVALID_DATA;
                        }
                        else
                        {
                            result[0] = atof(buf_temp);
                        }
                    }

                    AirP_TxRxIndex = 0;
                    AirP_TxRxLength = 0;

                    break;
                }
                
                if(AIRP_USART_PROCESSING_ERR == AirP_UartProcessingPhase)
                {
                    AirP_TxRxIndex = 0;
                    AirP_TxRxLength = 0;

                    AirP_ResetProcessingPhase();
                    result[0] = INVALID_DATA;
                    
                    break;
                }
                
                count--;

                vTaskDelay(20);                       
            }
            if(count <= 0)
            {
                AirP_TxRxIndex = 0;
                AirP_TxRxLength = 0;

                AirP_ResetProcessingPhase();

                result[0] = INVALID_DATA;
            }
        }
        else           
        {
            AirP_TxRxIndex = 0;
            AirP_TxRxLength = 0;

            AirP_ResetProcessingPhase();

            result[0] = INVALID_DATA;
        }
        
        return 1;
    }        
    else if(bcm_info.sensor.ce == 2)//baro-1
    {
        if(AirP_UartProcessingPhase == AIRP_USART_PROCESSING_IDEL)// 閸掋倖鏌囬弰顖氭儊缁屾椽妫�
        {        
            AirP_TxRxIndex = 0;
            AirP_TxRxLength = 0;
            strcpy((char *)buffer, "send\r\n");
            AirP_RevStep = 1;
            
            uartSendStr(UARTDEV_3, (UINT8 *)&buffer, sizeof("send\r\n") - 1);

            AirP_UartProcessingPhase = AIRP_USART_PROCESSING_SENDING;
            while(count)
            {                
                if(AIRP_USART_PROCESSING_FINISH == AirP_UartProcessingPhase)
                {
                    AirP_ResetProcessingPhase();

                    if(AirP_TxRxLength != 18) 
                    {
                        result[0] = INVALID_DATA;
                        result[1] = INVALID_DATA;
                    }
                    else
                    {
                        if(Get_P_Value(AirP_TxRxBuffer, &airp, &airT, &stat))
                        {
                            if(0 == stat)
                            {
                                result[0] = airp;
                                result[1] = airT;
                            }
                            else
                            {
                                result[0] = -991;
                                result[1] = -991;
                            }                    
                        }
                        else
                        {
                            result[0] = INVALID_DATA;
                            result[1] = INVALID_DATA;
                        }  
                    }

                    AirP_TxRxIndex = 0;
                    AirP_TxRxLength = 0;

                    break;
                }
                
                if(AIRP_USART_PROCESSING_ERR == AirP_UartProcessingPhase)
                {
                    AirP_TxRxIndex = 0;
                    AirP_TxRxLength = 0;

                    AirP_ResetProcessingPhase();
                    
                    result[0] = INVALID_DATA;
                    result[1] = INVALID_DATA;
                    
                    break;
                }
                
                count--;

                vTaskDelay(20);                       
            }
            if(count <= 0)
            {
                AirP_TxRxIndex = 0;
                AirP_TxRxLength = 0;

                AirP_ResetProcessingPhase();

                result[0] = INVALID_DATA;
                result[1] = INVALID_DATA;
            }
        }
        else           
        {
            AirP_TxRxIndex = 0;
            AirP_TxRxLength = 0;

            AirP_ResetProcessingPhase();

            result[0] = INVALID_DATA;
            result[1] = INVALID_DATA;

        }
            
        return 1;
    }
}



