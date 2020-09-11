/*
;************************************************************************************************************
;*
;*
;*                                 	      	 
;*
;*--------------------------------------------- �ļ���Ϣ ----------------------------------------------------
;*
;* �ļ����� :
;* �ļ����� :
;*
;* ����˵�� :
;*-------------------------------------------- ���°汾��Ϣ -------------------------------------------------
;* �޸����� :
;* �޸����� :
;* �汾���� :
;*-------------------------------------------- ��ʷ�汾��Ϣ -------------------------------------------------
;* �ļ����� : liujunjie_2008@126.com
;* �������� :
;* �汾���� :
;*-----------------------------------------------------------------------------------------------------------
;*-----------------------------------------------------------------------------------------------------------
;************************************************************************************************************
*/

#include "sensor_basic.h"

extern unsigned char TxRxIndex;

extern unsigned char UartProcessingPhase;


/********************************************************************************
** �������� ��void Sensor_Init(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ��
********************************************************************************/


void hal_sensor_init(void)
{
  Sensor_Init();

  SampleData_Init(&sensors_data);
}

/********************************************************************************
** �������� ��void Sensor_Init(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ��
********************************************************************************/

unsigned char Sensor_Init(void)
{
  AirH_USART3_Init();
  return 1;
}


/*
********************************************************************************
** �������� ��void AirP_rotronic(char *buf,float *result)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ��0:������Ч  1:������Ч
********************************************************************************
*/

void AirP_rotronic(char *buf,float *result)
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


/*
********************************************************************************
** �������� ��unsigned char Airp_engine(float *result)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ��0:������Ч  1:������Ч
********************************************************************************
*/

unsigned char Airp_engine(float *result)
{ 
    UINT8 buffer[10];
    UINT32 count = 50;


    unsigned char phase = AirP_USART1_GetProcessingPhase();

    if(phase == USART_PROCESSING_IDEL)                                              // 閸掋倖鏌囬弰顖氭儊缁屾椽妫�
    {
        TxRxIndex = 0;
        TxRxLength = 0;
        strcpy((char *)buffer,"01:R\r\n");
        RevStep = 1;
        
        uartSendStr(UARTDEV_1, (UINT8 *)&buffer, sizeof(buffer));

        UartProcessingPhase = USART_PROCESSING_SENDING;

        while(count)
        {                
            if(USART_PROCESSING_FINISH == UartProcessingPhase)
            {
                Airp_ResetProcessingPhase();

                if(TxRxLength != 104)  
                {
                    result[0] = INVALID_DATA;
                    result[1] = INVALID_DATA;
                    result[2] = INVALID_DATA;

                }
                else
                {                    
                    AirP_rotronic(TxRxBuffer, result);
                }

                TxRxIndex = 0;
                TxRxLength = 0;

                break;
            }
            
            if(USART_PROCESSING_ERR == UartProcessingPhase)
            {
                TxRxIndex = 0;
                TxRxLength = 0;

                Airp_ResetProcessingPhase();

                result[0] = INVALID_DATA;
                result[1] = INVALID_DATA;
                result[2] = INVALID_DATA;

                break;
            }
            
            count--;

            vTaskDelay(20);                       
        }

        if(count <= 0)
        {
            TxRxIndex = 0;
            TxRxLength = 0;
	
            Airp_ResetProcessingPhase();

            result[0] = INVALID_DATA;
            result[1] = INVALID_DATA;
            result[2] = INVALID_DATA;
        }
    }
    else           
    {
        TxRxIndex = 0;
        TxRxLength = 0;

        Airp_ResetProcessingPhase();

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
    Sensor_Init();
}

