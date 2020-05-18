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

#define PARA_A 				3.9083E-3 	   	            // �¶�ϵ��
#define PARA_B 				-5.775E-7 
#define STD_R_VALUE 	100.0       		            // ��׼����ֵ


static unsigned char DiWenChlNumber;							// �ڼ�ͨ���ĵ���
static unsigned char AD7792ChlNumber;							// AD7792�ڼ�ͨ��


// ͨ��1���üĴ�����ֵ��ƫ�ý��á������Ա��롢4�����桢�ڲ���׼��ѹ������ʹ�ܡ�ͨ��1����
#define DI_WEN_CHL0_CFG_REG_VAL 		0x1290

// ͨ��2���üĴ�����ֵ��ƫ�ý��á������Ա��롢4�����桢�ڲ���׼��ѹ������ʹ�ܡ�ͨ��2����
#define DI_WEN_CHL1_CFG_REG_VAL 		0x1291

// ͨ���ܽ�����
/*#define SWITCH0_BIT   BIT4 //P9.4
#define SWITCH1_BIT   BIT5 //P9.5
#define SWITCH2_BIT   BIT6 //P9.6

#define SWITCH0_FUNC_IO()            P9SEL &= ~SWITCH0_BIT
#define SWITCH0_PIN_OUT()            P9DIR |=  SWITCH0_BIT
#define SWITCH0_LOW()                P9OUT &= ~SWITCH0_BIT
#define SWITCH0_HIGH()               P9OUT |=  SWITCH0_BIT

#define SWITCH1_FUNC_IO()            P9SEL &= ~SWITCH1_BIT
#define SWITCH1_PIN_OUT()            P9DIR |=  SWITCH1_BIT
#define SWITCH1_LOW()                P9OUT &= ~SWITCH1_BIT
#define SWITCH1_HIGH()               P9OUT |=  SWITCH1_BIT

#define SWITCH2_FUNC_IO()            P9SEL &= ~SWITCH2_BIT
#define SWITCH2_PIN_OUT()            P9DIR |=  SWITCH2_BIT
#define SWITCH2_LOW()                P9OUT &= ~SWITCH2_BIT
#define SWITCH2_HIGH()               P9OUT |=  SWITCH2_BIT*?
/*
********************************************************************************
** �������� ��void Sensor_Init(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ��
********************************************************************************
*/ 
unsigned char Sensor_Init(void) 
{		/*
    SWITCH0_FUNC_IO();
    SWITCH0_PIN_OUT();

    SWITCH1_FUNC_IO();
    SWITCH1_PIN_OUT();

    SWITCH2_FUNC_IO();
    SWITCH2_PIN_OUT();*/

    AD7792_Init();
  return 1;																						// ��ʼ���ɹ�
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
void hal_sensor_init()
{

  Sensor_Init();
  SampleData_Init(&sensors_data);
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
void USART1_RX(void)
{

}
/*
********************************************************************************
** �������� ��unsigned char EarthTemp_engine(float Result[5]) 
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ��0:������Ч  1:������Ч
********************************************************************************
*/ 

unsigned char EarthTemp_engine(float result[MAX_SENSOR_NUM])
{
  static float earthTemp_1[MAX_SENSOR_NUM];        // ���ǰ�ĸ�ͨ�����¶�����
  static unsigned long result_0;
  unsigned char i = 0, ChlNumber = 0;
  unsigned long result_1;
  
  float fp32_1;

  ChlNumber = bcm_info.sensor.SensorNum;

  if(ChlNumber > MAX_SENSOR_NUM)
      ChlNumber = MAX_SENSOR_NUM;
/*
  if(DiWenChlNumber == 0)		    {SWITCH0_LOW();  SWITCH1_LOW();  SWITCH2_LOW();}
  else if(DiWenChlNumber == 1)  	{SWITCH0_HIGH(); SWITCH1_LOW();  SWITCH2_LOW();}
  else if(DiWenChlNumber == 2)  	{SWITCH0_LOW();  SWITCH1_HIGH(); SWITCH2_LOW();}
  else if(DiWenChlNumber == 3)  	{SWITCH0_HIGH(); SWITCH1_HIGH(); SWITCH2_LOW();}
  else if(DiWenChlNumber == 4)  	{SWITCH0_LOW();  SWITCH1_LOW();  SWITCH2_HIGH();}
  else if(DiWenChlNumber == 5)      {SWITCH0_HIGH(); SWITCH1_LOW();  SWITCH2_HIGH();}
  else if(DiWenChlNumber == 6)      {SWITCH0_LOW();  SWITCH1_HIGH();  SWITCH2_HIGH();}
  else if(DiWenChlNumber == 7)      {SWITCH0_HIGH();  SWITCH1_HIGH();  SWITCH2_HIGH();}

  // AD7792ת����1ͨ��������
  if(AD7792ChlNumber == 0)																					
  {
    if(AD7792_SingleConversion(DI_WEN_CHL0_CFG_REG_VAL,&result_0))	// ������Ч
      AD7792ChlNumber = 1;   
    else return 0;
  }
  
  
  // AD7792ת����2ͨ��������
  if(AD7792_SingleConversion(DI_WEN_CHL1_CFG_REG_VAL,&result_1))	  // ������Ч
  {
    AD7792ChlNumber = 0;		      
    
    // �������ֵ
    fp32_1 = (((float)result_0) * STD_R_VALUE) / result_1;
    
    // �����¶�ֵ
    fp32_1 = (float)(((-PARA_A+sqrt(PARA_A*PARA_A-4*PARA_B*(1-(fp32_1)/100)))/(2*PARA_B)));
    
    if(DiWenChlNumber < (ChlNumber-1))
    {
      earthTemp_1[DiWenChlNumber] = fp32_1;
      DiWenChlNumber++;
    }
    else 
    {
        for(i = 0;i < ChlNumber;i++)
        {
            if(i < (ChlNumber - 1))
            {
                result[i] = earthTemp_1[i];
            }
            else
            {
                result[i] = fp32_1;
            }
        }

        DiWenChlNumber = 0;
      
      return 1;																						
    }		
  }
  */
  return 0;																													// ������Ч
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


