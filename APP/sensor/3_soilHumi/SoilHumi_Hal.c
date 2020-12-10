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

/**********************************************************************************************/
/***********************************����1��������**********************************************/
/**********************************************************************************************/
#define	SOILMOISTURE_ADU_BUFF_LEN		64

/* ���ڴ���ADU����֡�Ľ׶����� */
#define	SOILMOISTURE_ADU_PROCESSING_IDEL         0
#define	SOILMOISTURE_ADU_PROCESSING_SENDING      1
#define	SOILMOISTURE_ADU_PROCESSING_RECEIVING    2
#define	SOILMOISTURE_ADU_PROCESSING_FINISH       3
#define	SOILMOISTURE_ADU_PROCESSING_ERR          4

char SOILMOISTURE_AduBuffer[SOILMOISTURE_ADU_BUFF_LEN];               // ADU������
unsigned char SOILMOISTURE_AduLength;                                 // ADU�ֽڳ���

unsigned char SOILMOISTURE_TxRxIndex;
unsigned volatile char  SOILMOISTURE_AduProcessingPhase;      //


#define RS485_BIT //BIT4         //P5.4
#define RS485_IO()          //P5SEL &=~ RS485_BIT
#define RS485_IO_OUT()     // P5DIR |=  RS485_BIT
#define RS485_TX_EN()      // P5OUT |=  RS485_BIT
#define RS485_RX_EN()       //P5OUT &=~ RS485_BIT

TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart3;



/*
*********************************************************************************************************
** �������� ��void USART_Init(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ����
*********************************************************************************************************
*/
void SOILMOISTURE_USART1_Init(void)
{

    UART_Init(3, 9600, 8, 'N', 1, 1);

    RS485_IO();
    RS485_IO_OUT();

    SOILMOISTURE_AduProcessingPhase = SOILMOISTURE_ADU_PROCESSING_IDEL;
}




/*
*********************************************************************************************************
** �������� ��unsigned char USART1_GetProcessingPhase(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ����
*********************************************************************************************************
*/
unsigned char SOILMOISTURE_USART1_GetProcessingPhase(void)
{
  return SOILMOISTURE_AduProcessingPhase;
}

/*
*********************************************************************************************************
** �������� ��void USART1_ProcessingPhaseReset(void)
** �������� ����λ���ڴ���״̬��
** ��ڲ��� ��
**
** ���ڲ��� ����
*********************************************************************************************************
*/
void SOILMOISTURE_USART1_ProcessingPhaseReset(void)
{


    SOILMOISTURE_AduProcessingPhase = SOILMOISTURE_ADU_PROCESSING_IDEL;
}

/*
*********************************************************************************************************
** �������� ��void USART1_SendAudData(void)
** �������� �������жϷ���SOILMOISTURE_AduBuffer�е�����
** ��ڲ��� ��
**
** ���ڲ��� ����
*********************************************************************************************************
*/
unsigned char SOILMOISTURE_USART1_SendAudData(void)
{
  unsigned char  i, buffer[256] = {0};
  
  if(SOILMOISTURE_AduProcessingPhase != SOILMOISTURE_ADU_PROCESSING_IDEL)  return 0;    // ����æ

  if((SOILMOISTURE_AduLength < 10) || (SOILMOISTURE_AduLength > 200)) return 0;

  SOILMOISTURE_AduBuffer[SOILMOISTURE_AduLength] = 0x0D;
  SOILMOISTURE_AduBuffer[SOILMOISTURE_AduLength+1] = 0x0A;
  SOILMOISTURE_AduBuffer[SOILMOISTURE_AduLength+2] = 0xFF;
  
  SOILMOISTURE_AduLength = SOILMOISTURE_AduLength + 3;
  
  for(i=0;i<SOILMOISTURE_AduLength;i++)  SOILMOISTURE_AduBuffer[i] |= 0x80;  // ��Ҫ���͵�Ascii������λ��һ

  SOILMOISTURE_TxRxIndex = 0;

  buffer[0] = ':' | 0x80;
  memcpy(&buffer[1], &SOILMOISTURE_AduBuffer[0], SOILMOISTURE_AduLength);
  uartSendStr(UARTDEV_3, (UINT8 *)&buffer, SOILMOISTURE_AduLength + 1);

  SOILMOISTURE_AduProcessingPhase = SOILMOISTURE_ADU_PROCESSING_SENDING;    // ���ڷ�������
  
  return 1;
}


/*
*********************************************************************************************************
** �������� ��
** �������� �����ڽ����жϷ������
** ��ڲ��� ��
**
** ���ڲ��� ��
*********************************************************************************************************
*/
void USART3_RX(void)
{ 
    static unsigned char revStep;
  
    unsigned char td;
  
    //URX1IF = 0;
  
    td = (uint8_t)(huart3.Instance->RDR);  // ��ȡ����������,���λ����(��7,N,2ģʽ��Ϊ8,N,1ģʽ)
    
    if(td == ':')                          // ���յ���ʼ�ַ�":"
    {
        SOILMOISTURE_TxRxIndex = 0;
        revStep = 1;                        // ״̬���� "��������״̬"
        return;
    }
  
    switch(revStep)
    {
        case 1:                               // ��������״̬
            if(td == 0x0D)  revStep = 2;        // �л������ջ��з���״̬
            else  
            {
                if(SOILMOISTURE_TxRxIndex >= SOILMOISTURE_ADU_BUFF_LEN)
                {
                    revStep = 0;

                    SOILMOISTURE_AduProcessingPhase = SOILMOISTURE_ADU_PROCESSING_ERR;
                }
                else
                    SOILMOISTURE_AduBuffer[SOILMOISTURE_TxRxIndex++] = td;
            }
        break;

        case 2:                                 // ���ջ��з���״̬
            if(td == 0x0A)                        // �ɹ�����һ֡����
            {
                SOILMOISTURE_AduBuffer[SOILMOISTURE_TxRxIndex] = 0;           // �����յ�����ת�����ַ������Ǳ��룩
                SOILMOISTURE_AduLength = SOILMOISTURE_TxRxIndex;              // ADU����֡���ȣ��������ַ�������0
                SOILMOISTURE_AduProcessingPhase = SOILMOISTURE_ADU_PROCESSING_FINISH;    // �Ѿ����յ���ȷ��ADU����֡
            }
            else
            {
                SOILMOISTURE_AduProcessingPhase = SOILMOISTURE_ADU_PROCESSING_ERR;      // ���յ�����֡���󣬴��ڿ���
            }

            revStep = 0;                          // ��λ״̬�����ȴ���ʼ�ַ�":"״̬
        break;

        default : break;
    }    
}

/**********************************************************************************************/
/**********************************����ˮ����������********************************************/
/**********************************************************************************************/

static unsigned char SOILMOISTURE_AduBufferBin[25];  

static unsigned char SOILMOISTURE_SensorProcessingPhase;      //

/*
*********************************************************************************************************
** �������� ��unsigned char CalculateLRC( unsigned char* pd, unsigned char len)
** �������� ������LRC
** ��ڲ��� �� 
** ���ڲ��� ��
*********************************************************************************************************
*/ 
unsigned char SOILMOISTURE_CalculateLRC( unsigned char *pd, unsigned char len)
{
  unsigned char   ucLRC = 0;  
  
  while(len--)
    ucLRC += *pd++;       
  
  ucLRC = (unsigned char) (-((char) ucLRC));
  
  return ucLRC;
}

/*
*********************************************************************************************************
** �������� ��
** �������� ���ַ���16������ת������غ���
** ��ڲ��� ��
**        
** ���ڲ��� ��
*********************************************************************************************************
*/ 
unsigned char SOILMOISTURE_Char2Bin(char ucCharacter)
{
  if( ( ucCharacter >= '0' ) && ( ucCharacter <= '9' ) )
  {
    return ( unsigned char )( ucCharacter - '0' );
  }
  else if( ( ucCharacter >= 'A' ) && ( ucCharacter <= 'F' ) )
  {
    return ( unsigned char )( ucCharacter - 'A' + 0x0A );
  }
  else
  {
    return 0xFF;
  }
}

char SOILMOISTURE_Bin2Char(unsigned char ucByte)
{
  if( ucByte <= 0x09 )
  {
    return ( unsigned char )( '0' + ucByte );
  }
  else if( ( ucByte >= 0x0A ) && ( ucByte <= 0x0F ) )
  {
    return ( unsigned char )( ucByte - 0x0A + 'A' );
  }
  else
  {
    return 0xFF;
  }    
}

unsigned char SOILMOISTURE_CharArray2BinArray(char *paa,unsigned char *pha,unsigned char aalen)
{
  unsigned char i;
  unsigned char td_1,td_2;
  
  if((aalen & 0x01) || (aalen < 2))  return 0;      // Ascii���������
  
  for(i=0;i<aalen;i+=2)
  {
    td_1 = SOILMOISTURE_Char2Bin(paa[i]);
    if(td_1 == 0xff)  return 0;
    
    td_2 = SOILMOISTURE_Char2Bin(paa[i+1]);
    if(td_2 == 0xff)  return 0;
    
    pha[i>>1] = (td_1<<4) | td_2;  
  }
  return 1;
}

unsigned char SOILMOISTURE_BinArray2CharArray(unsigned char *binA,char *charA,unsigned char len)
{
  unsigned char i,j;
  
  for(i=0;i<len;i++)
  {
    j = i << 1;
    
    charA[j++] = SOILMOISTURE_Bin2Char(binA[i] >> 4);
    
    charA[j] = SOILMOISTURE_Bin2Char(binA[i] & 0x0F);    
  }
  
  return 1;
}

/*
********************************************************************************
** �������� ��
** �������� �����ô�����(1-5��)��Ч
** ��ڲ��� ��
**
** ���ڲ��� ��
********************************************************************************
*/ 
static unsigned char SOILMOISTURE_TRSF_SetValid(void)
{
    UINT32 count = 50;
    unsigned char phase = 0;
  
    phase = SOILMOISTURE_USART1_GetProcessingPhase();
  
    if(phase == SOILMOISTURE_ADU_PROCESSING_IDEL)                        // �ж��Ƿ����
    {
        strcpy(SOILMOISTURE_AduBuffer,"01100001000408001F000000000000C3");

        SOILMOISTURE_AduLength = sizeof("01100001000408001F000000000000C3") - 1;

        SOILMOISTURE_USART1_SendAudData(); // ����ADU����֡

        while(count)
        {
            if(phase == SOILMOISTURE_ADU_PROCESSING_FINISH)                      // �ж��Ƿ�������
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();

                if(SOILMOISTURE_AduLength != 14) return 2;
                if(memcmp("011000010004EA",SOILMOISTURE_AduBuffer,14)) return 2;    // ֱ�ӱȽ�    // ֱ�ӱȽ�

                return 1;                                                           // ���óɹ�
            }

            if(phase == SOILMOISTURE_ADU_PROCESSING_ERR)                          // �ж��Ƿ����
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();        
                return 2;                                                           // ����
            }
            
            count--;

            vTaskDelay(20);        
        }

        return 0;                                                             // æ
    }
}

/*
********************************************************************************
** �������� ��
** �������� ����������
** ��ڲ��� ��
**
** ���ڲ��� ��
********************************************************************************
*/ 
static unsigned char SOILMOISTURE_TRSF_StartMeasuring(void)
{
    UINT32 count = 50;

    unsigned char phase = SOILMOISTURE_USART1_GetProcessingPhase();

    if(phase == SOILMOISTURE_ADU_PROCESSING_IDEL)                        // �ж��Ƿ����
    {
        strcpy(SOILMOISTURE_AduBuffer,"010600000001F8");

        SOILMOISTURE_AduLength = sizeof("010600000001F8") - 1;

        SOILMOISTURE_USART1_SendAudData();     // ����ADU����֡

        while(count)
        {

            if(phase == SOILMOISTURE_ADU_PROCESSING_FINISH)                      // �ж��Ƿ�������
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();

                if(SOILMOISTURE_AduLength != 14) return 2;

                if(memcmp("010600000001F8",SOILMOISTURE_AduBuffer,14)) return 2;    // ֱ�ӱȽ�

                return 1;                                              // ���óɹ�
            }

            if(phase == SOILMOISTURE_ADU_PROCESSING_ERR)   // �ж��Ƿ����
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();        
                return 2;     // ����
            }
            
            count--;
            
            vTaskDelay(20);         
        }     
    }

    return 0;                                                // æ
}

/*
********************************************************************************
** �������� ��
** �������� ������Ƿ�������
** ��ڲ��� ��
**
** ���ڲ��� ��
********************************************************************************
*/ 
static unsigned char SOILMOISTURE_TRSF_CheckMeasuringOK(void)
{
    UINT32 count = 50;

    unsigned char phase = SOILMOISTURE_USART1_GetProcessingPhase();

    if(phase == SOILMOISTURE_ADU_PROCESSING_IDEL)                        // �ж��Ƿ����
    {
        strcpy(SOILMOISTURE_AduBuffer,"010400010001F9");

        SOILMOISTURE_AduLength = sizeof("010400010001F9") - 1;

        SOILMOISTURE_USART1_SendAudData();                                 // ����ADU����֡

        while(count)
        {

            if(phase == SOILMOISTURE_ADU_PROCESSING_FINISH)                      // �ж��Ƿ�������
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();

                if(!memcmp("0104020002F7",SOILMOISTURE_AduBuffer,12)) return 1;    // ת����ɣ���û�д���

                if(!memcmp("0104020003F6",SOILMOISTURE_AduBuffer,12)) return 2;    // ת����ɣ����д���

                if(!memcmp("0104020000F9",SOILMOISTURE_AduBuffer,12)) return 3;    // û�в���

                return 0;
                // if(!memcmp("0104020001F8",SOILMOISTURE_AduBuffer,12)) return 0;    // ���ڲ�����

                //  return 2;                                                          // Ӧ�����        
            }

            if(phase == SOILMOISTURE_ADU_PROCESSING_ERR)                         // �ж��Ƿ����
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();        
                return 2;                                                          // ����
            }
            
            count--;

            vTaskDelay(20);  
        }
    }

    return 0;                                                            // æ
}


#define PARA_A  0.1957
#define PARA_B  0.404
#define PARA_C  0.02852
/*
********************************************************************************
** �������� ��
** �������� ����ȡ��������1-5�ţ�����������
** ��ڲ��� ��
**
** ���ڲ��� ��
********************************************************************************
*/ 
static unsigned char SOILMOISTURE_TRSF_ReadMeasuringResult(float *pResult)
{
    UINT32 count = 50;

    unsigned char phase = SOILMOISTURE_USART1_GetProcessingPhase();
    union 
    {
        float fp32_1;
        unsigned char int8u_1[4];
    }union_1;


    if(phase == SOILMOISTURE_ADU_PROCESSING_IDEL)                        // �ж��Ƿ����
     {
        strcpy(SOILMOISTURE_AduBuffer,"01040100000AF0");

        SOILMOISTURE_AduLength = sizeof("01040100000AF0") - 1;

        SOILMOISTURE_USART1_SendAudData();                                  // ����ADU����֡

        while(count)
        {
            if(phase == SOILMOISTURE_ADU_PROCESSING_FINISH)                      // �ж��Ƿ�������
            {
                SOILMOISTURE_USART1_ProcessingPhaseReset();

                if(SOILMOISTURE_AduLength != 48) 
                {
                    pResult[0] = INVALID_DATA;
                    pResult[1] = INVALID_DATA;
                    pResult[2] = INVALID_DATA;
                    pResult[3] = INVALID_DATA;
                    pResult[4] = INVALID_DATA;
                    
                    return 2;
                }
                SOILMOISTURE_CharArray2BinArray(SOILMOISTURE_AduBuffer,SOILMOISTURE_AduBufferBin,48);

                if(SOILMOISTURE_CalculateLRC(SOILMOISTURE_AduBufferBin, 24) == 0)  // ����У��
                {
                    union_1.int8u_1[3] = SOILMOISTURE_AduBufferBin[5];
                    union_1.int8u_1[2] = SOILMOISTURE_AduBufferBin[6];
                    union_1.int8u_1[1] = SOILMOISTURE_AduBufferBin[3];
                    union_1.int8u_1[0] = SOILMOISTURE_AduBufferBin[4];
                    // ������������Ĺ�һ��Ƶ��ת��Ϊ�����ˮ��
                    // if(union_1.fp32_1 < PARA_C) pResult[0] = 0;
                    //   else 
                    //   pResult[0] = pow(((union_1.fp32_1 - PARA_C) / PARA_A), (1 / PARA_B));  

                     pResult[0] = union_1.fp32_1;  

                    union_1.int8u_1[3] = SOILMOISTURE_AduBufferBin[9];
                    union_1.int8u_1[2] = SOILMOISTURE_AduBufferBin[10];
                    union_1.int8u_1[1] = SOILMOISTURE_AduBufferBin[7];
                    union_1.int8u_1[0] = SOILMOISTURE_AduBufferBin[8];
                    // ������������Ĺ�һ��Ƶ��ת��Ϊ�����ˮ��
                    //  if(union_1.fp32_1 < PARA_C) pResult[1] = 0;
                    ///  else 
                    //    pResult[1] = pow(((union_1.fp32_1 - PARA_C) / PARA_A), (1 / PARA_B)); 


                    pResult[1] = union_1.fp32_1;


                    union_1.int8u_1[3] = SOILMOISTURE_AduBufferBin[13];
                    union_1.int8u_1[2] = SOILMOISTURE_AduBufferBin[14];
                    union_1.int8u_1[1] = SOILMOISTURE_AduBufferBin[11];
                    union_1.int8u_1[0] = SOILMOISTURE_AduBufferBin[12];
                    // ������������Ĺ�һ��Ƶ��ת��Ϊ�����ˮ��
                    //  if(union_1.fp32_1 < PARA_C) pResult[2] = 0;
                    // else 
                    //   pResult[2] = pow(((union_1.fp32_1 - PARA_C) / PARA_A), (1 / PARA_B)); 


                    pResult[2] = union_1.fp32_1;

                    union_1.int8u_1[3] = SOILMOISTURE_AduBufferBin[17];
                    union_1.int8u_1[2] = SOILMOISTURE_AduBufferBin[18];
                    union_1.int8u_1[1] = SOILMOISTURE_AduBufferBin[15];
                    union_1.int8u_1[0] = SOILMOISTURE_AduBufferBin[16];
                    // ������������Ĺ�һ��Ƶ��ת��Ϊ�����ˮ��
                    // if(union_1.fp32_1 < PARA_C) pResult[3] = 0;
                    // else 
                    //   pResult[3] = pow(((union_1.fp32_1 - PARA_C) / PARA_A), (1 / PARA_B)); 
                    pResult[3] = union_1.fp32_1;


                    union_1.int8u_1[3] = SOILMOISTURE_AduBufferBin[21];
                    union_1.int8u_1[2] = SOILMOISTURE_AduBufferBin[22];
                    union_1.int8u_1[1] = SOILMOISTURE_AduBufferBin[19];
                    union_1.int8u_1[0] = SOILMOISTURE_AduBufferBin[20];
                    // ������������Ĺ�һ��Ƶ��ת��Ϊ�����ˮ��
                    // if(union_1.fp32_1 < PARA_C) pResult[4] = 0;
                    // else 
                    //   pResult[4] = pow(((union_1.fp32_1 - PARA_C) / PARA_A), (1 / PARA_B)); 
                    pResult[4] = union_1.fp32_1;
                    
                    return 1;                                            // ���óɹ�
                }
                pResult[0] = INVALID_DATA;
                pResult[1] = INVALID_DATA;
                pResult[2] = INVALID_DATA;
                pResult[3] = INVALID_DATA;
                pResult[4] = INVALID_DATA;

                return 2;                                              // У��ʧ��
            }

            if(phase == SOILMOISTURE_ADU_PROCESSING_ERR)                          // �ж��Ƿ����
            {                
                SOILMOISTURE_USART1_ProcessingPhaseReset(); 
                
                pResult[0] = INVALID_DATA;
                pResult[1] = INVALID_DATA;
                pResult[2] = INVALID_DATA;
                pResult[3] = INVALID_DATA;
                pResult[4] = INVALID_DATA;
                
                return 2;// ����
            }
            
            count--;

            vTaskDelay(20); 
        }
    }
    return 0;                                                // æ
}

/*
********************************************************************************
** �������� ��void TuRangShuiFen_Init(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ��
********************************************************************************
*/ 
unsigned char Sensor_Init(void)
{
    SOILMOISTURE_USART1_Init();                // ���ڳ�ʼ��

    SOILMOISTURE_SensorProcessingPhase = 0;
    return 1;
}


/*
********************************************************************************
** �������� ��
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ��
********************************************************************************
*/
void hal_sensor_init(void)
{
  Sensor_Init();

  SampleData_Init(&sensors_data);
}

/*
********************************************************************************
** �������� ��unsigned char SoilMoisture_engine(float *pResult)
** �������� ������ˮ�ִ�����Ѳ�캯��
** ��ڲ��� ��Result[5]:ָ�򸡵������ָ�룬��Ч���ݽ�������ڴ�ָ��ָ���������
**                      ��������ΪС��0.x����ʽ����0.34����34%����ˮ��
** ���ڲ��� ��0��æ  1��������Ч 2������
********************************************************************************
*/ 
unsigned char SoilMoisture_engine(float Result[5]) 
{
    unsigned char result;
  
    /* ���ô�������Ч */
    if(SOILMOISTURE_SensorProcessingPhase == 0) 
    {
        result = SOILMOISTURE_TRSF_SetValid();
        if(result == 1)                        // ���óɹ�
        {
            SOILMOISTURE_SensorProcessingPhase = 1;
        }
        else
        {
            Result[0] == -991;
        }    
    }
  
  /* ��鴫�����Ƿ���� */
    if(SOILMOISTURE_SensorProcessingPhase == 1) 
    {
        result = SOILMOISTURE_TRSF_CheckMeasuringOK();
        if((result == 1) || (result == 3))
        {
          SOILMOISTURE_SensorProcessingPhase = 2;          // ������һ����Ĳ���
        }
        else if(result == 2)
        {
          SOILMOISTURE_SensorProcessingPhase = 0;          // ת���������¿�ʼʹ�ܴ�����
          Result[0] = -992;
        }
        else 
        {
            Result[0] = -993;
        }  
    }
  
  /* ����ת�� */
    if(SOILMOISTURE_SensorProcessingPhase == 2) 
    {
        result = SOILMOISTURE_TRSF_StartMeasuring();
        if(result == 1)
        {
          SOILMOISTURE_SensorProcessingPhase = 3;          // ������һ����Ĳ���
        }
        else if(result == 2)                               // Ӧ��������¿�ʼʹ�ܴ�����
        {
          SOILMOISTURE_SensorProcessingPhase = 0;
          Result[0] = -994;
        }
        else 
        {
            Result[0] = -995;
        }  
    }
  
  /* �ȴ�ת����� */
    if(SOILMOISTURE_SensorProcessingPhase == 3) 
    {
        result = SOILMOISTURE_TRSF_CheckMeasuringOK();
        if(result == 1)
        {
            SOILMOISTURE_SensorProcessingPhase = 4;          // ������һ����Ĳ���
        }
        else if(result == 2)
        {
            SOILMOISTURE_SensorProcessingPhase = 0;          // ת����ɣ�������
            Result[0] = -996;
        }
        else if(result == 3)
        {
            SOILMOISTURE_SensorProcessingPhase = 1;          // û�в���
            Result[0] = -997;
        }
        else 
        {
            Result[0] = -998;
        }
    }
  
  /* ��ȡת����� */
    if(SOILMOISTURE_SensorProcessingPhase == 4) 
    {
        result = SOILMOISTURE_TRSF_ReadMeasuringResult(Result);
        if(result == 1)
        {
            SOILMOISTURE_SensorProcessingPhase = 1; 
        }
        else if(result == 2)
        {
            SOILMOISTURE_SensorProcessingPhase = 0; 
            Result[0] = -999;
        }
       
        //return result;
    }
  
  return 1;
}

/*
********************************************************************************
** �������� ��void Reset_Sensor(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ��
********************************************************************************
*/ 
void Reset_Sensor(void)
{
  Sensor_Init();
}

