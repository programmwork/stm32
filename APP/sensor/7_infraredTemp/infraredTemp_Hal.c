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

TIM_HandleTypeDef htim1;



// ͨ��1���üĴ�����ֵ��ƫ�ý��á������Ա��롢4�����桢�ڲ���׼��ѹ������ʹ�ܡ�ͨ��1����
#define DI_WEN_CHL0_CFG_REG_VAL 		0x1290

// ͨ��2���üĴ�����ֵ��ƫ�ý��á������Ա��롢4�����桢�ڲ���׼��ѹ������ʹ�ܡ�ͨ��2����
#define DI_WEN_CHL1_CFG_REG_VAL 		0x1291

// ͨ���ܽ�����
#define SWITCH0_BIT   BIT4 //P9.4
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
#define SWITCH2_HIGH()               P9OUT |=  SWITCH2_BIT
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
{		


    AD7792_Init();
    //Init_ADC_temp();
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

}
/*
********************************************************************************
** �������� ��unsigned char infraredTemp_engine(float Result[5]) 
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ��0:������Ч  1:������Ч
********************************************************************************
*/ 

unsigned char infraredTemp_engine(float result[MAX_SENSOR_NUM])
{
    unsigned long result_0, result_1;
  
    unsigned long temp_r = 0;
    uint8 count = 50, readAD[2], readSTAT = 0;

    float v_out = 0, R_result = 0, A_t = 0.001129241, B_t = 0.0002341077, C_t = 0.00000008775468;
    float m_t = 0, b_t = 0, tc = 0;

    
    // AD7792ת����0ͨ��������
    AD7792_Set_Cfg(	AD7792_CFG_VBIAS_DIS
    				|AD7792_CFG_POR_U
    				|AD7792_CFG_GAIN_4
    				|AD7792_CFG_REF_EXT
    				|AD7792_CFG_BUFFER
    				|AD7792_CFG_SEL_CH0
    				);

    AD7792_Set_Mode(AD7792_MODE_CONV_ONCE
					|AD7792_MODE_CLK_INT64
					|AD7792_MODE_RATE_17
					);

    AD7792_Red_Reg( AD7792_REG_STAT, &readSTAT, 1 );

    while(count--)
    {
        vTaskDelay(10);
        
        if((readSTAT & AD7792_STAT_NRDY) == 0)
        {
            break;        
        }

        AD7792_Red_Reg( AD7792_REG_STAT, &readSTAT, 1 );
    }


    if(count > 0)
    {

        AD7792_Red_Reg( AD7792_REG_DATA, readAD, 2 );

        result_0 = (readAD[0] << 8) + readAD[1];

        // AD7792ת����0ͨ��������
        AD7792_Set_Cfg(	AD7792_CFG_VBIAS_DIS
        				|AD7792_CFG_POR_U
        				|AD7792_CFG_GAIN_4
        				|AD7792_CFG_REF_EXT
        				|AD7792_CFG_BUFFER
        				|AD7792_CFG_SEL_CH1
        				);

        AD7792_Set_Mode(AD7792_MODE_CONV_ONCE
    					|AD7792_MODE_CLK_INT64
    					|AD7792_MODE_RATE_17
    					);

        AD7792_Red_Reg( AD7792_REG_STAT, &readSTAT, 1 );

        count = 50;
        while(count--)
        {
            vTaskDelay(10);
            
            if((readSTAT & AD7792_STAT_NRDY) == 0)
            {
                break;        
            }

            AD7792_Red_Reg( AD7792_REG_STAT, &readSTAT, 1 );
        }
        if(count > 0)
        {            
            AD7792_Red_Reg( AD7792_REG_DATA, readAD, 2 );
        
            result_1 = (readAD[0] << 8) + readAD[1];

            v_out = (3.3 * (float)result_1) / 65535;
            // �����¶�ֵ
            R_result = 24900 * (2.5 / v_out - 1);

            //���㵼��
            R_result = logf(R_result);
            tc = 1 / (A_t + B_t * R_result + C_t * powf(R_result, 3));

            //
            m_t = bcm_info.sensor.cr[0].a * powf(tc, 2) + bcm_info.sensor.cr[0].b * tc + bcm_info.sensor.cr[0].c;
            b_t = bcm_info.sensor.cr[1].a * powf(tc, 2) + bcm_info.sensor.cr[1].b * tc + bcm_info.sensor.cr[1].c;

            result[0] = powf((powf(tc, 4) + ((result_0 * 1.17) * m_t) / 65535 * 4 + b_t), 0.25) - 273.15;

            /*m_t = 65874.8*powf(tc, 2) + 6593650 * tc + 1285780000;
            b_t = 3977.69*powf(tc, 2) + 111083 * tc - 7004620;

            result[0] = powf((powf(tc, 4) + ((result_0 * 2.5) * m_t)/65535 * 4 + b_t),0.25);
            result[0] = result[0] - 273.15;*/
            
            return 1;
        }
  }
  
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

