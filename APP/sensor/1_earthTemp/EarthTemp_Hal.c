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

TIM_HandleTypeDef htim1;


static unsigned char DiWenChlNumber;							// �ڼ�ͨ���ĵ���
//static unsigned char AD7792ChlNumber;							// AD7792�ڼ�ͨ��


// ͨ��1���üĴ�����ֵ��ƫ�ý��á������Ա��롢4�����桢�ڲ���׼��ѹ������ʹ�ܡ�ͨ��1����
//0x1290

// ͨ��2���üĴ�����ֵ��ƫ�ý��á������Ա��롢4�����桢�ڲ���׼��ѹ������ʹ�ܡ�ͨ��2����
//0x1291

// ͨ���ܽ�����
#define SWITCH0_LOW()                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
#define SWITCH0_HIGH()               HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

#define SWITCH1_LOW()                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
#define SWITCH1_HIGH()               HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

#define SWITCH2_LOW()                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
#define SWITCH2_HIGH()               HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
/********************************************************************************
** �������� ��void Sensor_Init(void)
** �������� ��
** ��ڲ��� ��
**
** ���ڲ��� ��
********************************************************************************
*/ 
unsigned char Sensor_Init(void) 
{	
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

    /*Configure GPIO pin : PA4 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
    
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
void USART3_RX(void)
{

}

/*
********************************************************************************
** �������� ��uint8 Check_RDY(uint8 state, uint32 timeout)
** �������� ��
** ��ڲ��� ��state 1:δ���         0�����
**
** ���ڲ��� ��
********************************************************************************
*/ 
uint8 Check_RDY(uint8 state, uint32 timeout)
{
    uint8 readSTAT = 0;

    while(timeout--)
    {        
        
        AD7792_Red_Reg( AD7792_REG_STAT, &readSTAT, 1 );//�ѼĴ���״̬д��readSTAT  
        
        if((readSTAT & AD7792_STAT_NRDY) == state)
        {
            return 1;//success
        }

        vTaskDelay(1);

    }
    return 0;//error
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
unsigned char EarthTemp_engine(float result[MAX_SENSOR_NUM])
{
    unsigned long result_0;
    unsigned char i = 0, ChlNumber = 0;
    unsigned long result_1;
    uint8 count = 50, readAD[2], readSTAT = 0;
  
    float fp32_1;

    ChlNumber = bcm_info.sensor.SensorNum;

    if(ChlNumber > MAX_SENSOR_NUM)
        ChlNumber = MAX_SENSOR_NUM;

    for(i = 0;i < ChlNumber; i++)
    {
        if(i == 0)		     {SWITCH0_LOW();  SWITCH1_LOW();  SWITCH2_LOW();}
        else if(i == 1)  	 {SWITCH0_HIGH(); SWITCH1_LOW();  SWITCH2_LOW();}
        else if(i == 2)   	 {SWITCH0_LOW();  SWITCH1_HIGH(); SWITCH2_LOW();}
        else if(i == 3)   	 {SWITCH0_HIGH(); SWITCH1_HIGH(); SWITCH2_LOW();}
        else if(i == 4)      {SWITCH0_LOW();  SWITCH1_LOW();  SWITCH2_HIGH();}
        else if(i == 5)      {SWITCH0_HIGH(); SWITCH1_LOW();  SWITCH2_HIGH();}
        else if(i == 6)      {SWITCH0_LOW();  SWITCH1_HIGH(); SWITCH2_HIGH();}
        else if(i == 7)      {SWITCH0_HIGH(); SWITCH1_HIGH(); SWITCH2_HIGH();}

        if(Check_RDY(AD7792_STAT_NRDY, 10))
        {

            // AD7792ת����0ͨ��������
            AD7792_Set_Cfg( AD7792_CFG_VBIAS_DIS
                          |AD7792_CFG_POR_U
                          |AD7792_CFG_GAIN_4
                          |AD7792_CFG_REF_EXT
                          |AD7792_CFG_BUFFER
                          |AD7792_CFG_SEL_CH0
                          );
        
            AD7792_Set_Mode(AD7792_MODE_CONV_ONCE
                          |AD7792_MODE_CLK_INT64
                          |AD7792_MODE_RATE_19
                          );
            vTaskDelay( 100 );
            if(Check_RDY(0, 10))
            {            
                AD7792_Red_Reg( AD7792_REG_DATA, readAD, 2 );
                
                //ͨ��1�Ĳ���ֵ
                result_0 = (readAD[0] << 8) + readAD[1];
                
                if(Check_RDY(AD7792_STAT_NRDY, 10))
                {
                    // AD7792ת����1ͨ��������
                    AD7792_Set_Cfg( AD7792_CFG_VBIAS_DIS
                              |AD7792_CFG_POR_U
                              |AD7792_CFG_GAIN_4
                              |AD7792_CFG_REF_EXT
                              |AD7792_CFG_BUFFER
                              |AD7792_CFG_SEL_CH1
                              );

                    AD7792_Set_Mode(AD7792_MODE_CONV_ONCE
                              |AD7792_MODE_CLK_INT64
                              |AD7792_MODE_RATE_19
                              );

                    vTaskDelay( 100 );

                    if(Check_RDY(0, 10))
                    {
                        AD7792_Red_Reg( AD7792_REG_DATA, readAD, 2 );

                        //ͨ��2�Ĳ���ֵ
                        result_1 = (readAD[0] << 8) + readAD[1];

                         // �������ֵ
                        fp32_1 = (((float)result_0) * STD_R_VALUE) / result_1;
                        
                        // �����¶�ֵ
                        fp32_1 = (float)(((-PARA_A+sqrt(PARA_A*PARA_A-4*PARA_B*(1-(fp32_1)/100)))/(2*PARA_B)));

                        result[i] = fp32_1;
                    }
                }
            }
        }        
        result[i] = INVALID_DATA;
    }
    return 1;																												// ������Ч
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


