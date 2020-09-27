/*
 * sensor_process.c
 *
 *  Created on: 2017��10��28��
 *      Author: lenovo
 */


#include "sensor_basic.h"

extern uint8 Check_RDY(uint8 state, uint32 timeout);
#define PARA_A 				3.9083E-3 	   	            // �¶�ϵ��
#define PARA_B 				-5.775E-7 
#define STD_R_VALUE 	100.0       		            // ��׼����ֵ


void autosend()
{
    //check ft time


    if(m_tempdata.event.autosendevent==true)
    {
    
        if(bcm_info.common.sw == 1)
        {
            check_event_autosend();
        }
        else
        {
            m_tempdata.event.autosendevent=false;
        }
    }

}
float result[2];
void time_task(void *pvParameters)
{
    static U16 p_msec=0xFF;
    static U8 p_sec=0xFF;
    static U8 p_min=0xFF;
    static U8 p_hour=0xFF;

    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);


    uint32_t i = 0, j = 0, k = 0;
    uint8_t str[256];
    uint8_t readstr[256];
    uint32_t error = 0;



    uint8 count = 50, readAD[2], readSTAT = 0;
    unsigned long result_0, result_1;
    float fp32_1;

    
    
while(1)
{/*//ʵʱʱ��оƬ
    int value = 0;

    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);
    
    //DS3231_ReadTemperature(&value);


    DS3231_SetTime(&m_tempdata.m_RtcTateTime);

    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
    

    vTaskDelay(20);

    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);

    DS3231_ReadTime(&m_tempdata.m_RtcTateTime);

     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
    

    vTaskDelay(20);*/
		
////////////////////flashоƬ//////////////////////

    /*for (i = 0; i < 10; i++)
        {
            for (j = 0; j < 256; j++)
            {
                if ((j % 2) == 0)
                {
                    str[j] = i;
                }
                else
                {
                    str[j] = (j / 2) + 1;
                }
            }


            
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);
    W25Q128_Erase_Chip(W25Q128_ERS_SEC, 0);//adr = 0x00 || 2560 << 8 || 25600 << 8 
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
    
    vTaskDelay(20);

    //HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);
    W25Q128_Data_Prog(0, str, sizeof(str));
    //HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
    
    vTaskDelay(20);

    //HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);
    W25Q128_Data_Read(0, readstr, sizeof(readstr));
    //HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
    
    vTaskDelay(20);

    for(k = 0;k < 256; k++)
    {
        if(str[k] == readstr[k])
        {
           
        }
        else
        {
           error++;//��һ��
        }
    }

    vTaskDelay(20);
    }*/

        


////////////////////ad7792оƬ///////////////////////
HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);



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
                      |AD7792_MODE_RATE_17
                      );      
       

        if(Check_RDY(0, 200)) //0��ת�����
        {
            AD7792_Red_Reg( AD7792_REG_DATA, readAD, 2 );
            
            //ͨ��1�Ĳ���ֵ
            result_0 = (readAD[0] << 8) + readAD[1];
            HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);

            //�Ӽ���Ƿ��ȡ�ɹ��ˣ��÷�ŵĺ���
            //�жϳɹ��Ժ�ſ�ʼ AD7792ת����1ͨ��������
            if(Check_RDY(AD7792_STAT_NRDY, 10))
            {
                AD7792_Set_Cfg( AD7792_CFG_VBIAS_DIS
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


                if(Check_RDY(0, 200))
                {
                    AD7792_Red_Reg( AD7792_REG_DATA, readAD, 2 );


                    //ͨ��2�Ĳ���ֵ
                    result_1 = (readAD[0] << 8) + readAD[1];       
                
                    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);

                    // �������ֵ
                    fp32_1 = (((float)result_0) * STD_R_VALUE) / result_1;

                    // �����¶�ֵ
                    fp32_1 = (float)(((-PARA_A+sqrt(PARA_A*PARA_A-4*PARA_B*(1-(fp32_1)/100)))/(2*PARA_B)));

                    result[0] = fp32_1;

                            
                }        
            }
        }
    }
    else
    {
            AD7792_Init();
    }
    
    
}


    while(1)
    {
			
        if(p_msec!=m_tempdata.m_RtcTateTime.msec)
        {
            p_msec=m_tempdata.m_RtcTateTime.msec;


            if(m_tempdata.m_RtcTateTime.msec == 0)
            {
                
            }
        }

        if(p_sec!=m_tempdata.m_RtcTateTime.sec)
        {
            p_sec=m_tempdata.m_RtcTateTime.sec;

            startupprint();
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);

    
#if (SENSOR != 2)   //·ç 
					  if(m_tempdata.m_RtcTateTime.sec%2 == 0)
            {
                m_tempdata.event.secevent = true;
            }
#endif


#if (SENSOR == 2)   //·ç
						m_tempdata.event.secevent = true;
						m_tempdata.event.Flag_1s = true;
#endif
        
            //ÿ30���� �ĵ�50���Ӳʱ�Ӹ���ʱ��Уʱ
            if((m_tempdata.m_RtcTateTime.min%30 == 0)&&(m_tempdata.m_RtcTateTime.sec == 50))
            {
                s_RtcTateTime_t time_struct_hardrtc_temp;
                if(DS3231_ReadTime(&time_struct_hardrtc_temp)==1)  //ʱ��У����ȷ�Ÿ���ʱ��Уʱ
                {
                    m_tempdata.m_RtcTateTime = time_struct_hardrtc_temp;
                }
            }
        }

         if((p_min!=m_tempdata.m_RtcTateTime.min)
           &&(m_tempdata.m_RtcTateTime.sec<3))
        {
            p_min= m_tempdata.m_RtcTateTime.min;
          
            m_tempdata.event.minevent = true;   
        
            m_tempdata.SysPowerON++;

            if(m_tempdata.SuperadMin == true) //Ȩ������� ����ʱ
            {
                m_tempdata.SuperadMinCnt ++;
                if(m_tempdata.SuperadMinCnt > 20)
                {
                    m_tempdata.SuperadMin = 0;
                    m_tempdata.SuperadMinCnt = 0;
                }
            }
            if(m_tempdata.DebugON == true)    //��������� ����ʱ
            {
                m_tempdata.DebugONCnt ++;
                if(m_tempdata.DebugONCnt > 20)
                {
                    m_tempdata.DebugON = false;
                    m_tempdata.DebugONCnt = 0;
                   // m_tempdata.SecDataOut = false;    //�ر�˽������Ȩ��ʱ  �ر������ݷ���
                    uartSendStr(0,"Close debug mode.\r\n",19);
                }
            }
        }

        if((p_hour!=m_tempdata.m_RtcTateTime.hour)
            &&(m_tempdata.m_RtcTateTime.min==0)
            &&(m_tempdata.m_RtcTateTime.sec>5))
        {
            p_hour= m_tempdata.m_RtcTateTime.hour;
        }

        autosend();
        vTaskDelay(20);
    }
        

     
}



void uartprocess_task( void *pvParameters )
{


    while(1)
    {
        uart_rcv(UARTDEV_1);
        uart_rcv(UARTDEV_2);
        
        checkuart(UARTDEV_1);
        checkuart(UARTDEV_2);
        vTaskDelay(20);
    }
        

}

void check_event_sample_task( void *pvParameters )
{
    //sensors_data_t sensors_data;
    //sensor_data_t sensor_data;


    SampleData_Init(&sensors_data);


    
    while(1)
    {
        if((m_tempdata.event.secevent==true) && (m_tempdata.event.minevent == false))
        {
            m_tempdata.event.secevent = false;

            Element_SecSample(&sensors_data);
        }
				Element_MsSample(&sensors_data);
        if(m_tempdata.event.minevent==true)
        {
            m_tempdata.event.minevent = false;

            Element_MinSample(&sensors_data);
         
            memset(&sensors_data.sendpacket,0,MAX_PKG_LEN);
            sensors_data.sendpacketlen=GetMinData((char *)sensors_data.sendpacket,&sensors_data,1);

            check_event_storage();

            m_tempdata.event.autosendevent=true;
        }
        
        vTaskDelay(20);
   } 

}



void readdata_task(void *pvParameters)
{


    while(1)
    {
        if(m_tempdata.Count_read_history > 0)
        {
            read_history_mindata();
        }
        else if(m_tempdata.Count_down_history > 0)
        {
            show_history_mindata();
        }
        else if(m_tempdata.CfcFlag == true)
        {
            read_cfc_data();
        }
        else if(m_tempdata.CheckFlag == true)
        {
            read_check_data();
        }


        vTaskDelay(1000);

    }

}

void Self_test()
{
    float banWen = 0, banYa = 0;
    unsigned char data = 0;

    data = BanWenBanYa_engine(&banWen, &banYa);

    if(data != 0)
    {
        if((data & 0x1) == 0x1)
        {
            sensors_data.boardstemp_data = (int)banWen;
        }

        if((data & 0x2)  == 0x2)
        {
            sensors_data.boardsvolt_data = (int)(banYa*10);
        }

        if(sensors_data.boardstemp_data>board_temp_max)//ƫ��3
            sensor_state.board_temp = 3;
        else if(sensors_data.boardstemp_data<board_temp_min)//ƫ��4
            sensor_state.board_temp = 4;
        else if((sensors_data.boardstemp_data >= bcm_info.common.work_temp_min) && (sensors_data.boardstemp_data <= bcm_info.common.work_temp_max)) //Õý³£
            sensor_state.board_temp = 0;

        //��ѹ����
        if(sensors_data.boardsvolt_data>board_volt_max)//ƫ��3
        {
            sensor_state.board_volt = 3;
           /* sensor_state.bat_volt = 3;
            sensor_state.exter_volt = 3;
            sensor_state.board_current = 3;*/
        }
        else if(sensors_data.boardsvolt_data<board_volt_min)//ƫ��4
        {
            sensor_state.board_volt = 4;
           /* sensor_state.bat_volt = 4;
            sensor_state.exter_volt = 4;
            sensor_state.board_current = 4;*/
        }
        else if((sensors_data.boardsvolt_data >= bcm_info.common.boardsvolt_min) && (sensors_data.boardsvolt_data <= bcm_info.common.boardsvolt_max)) //����
        {
        {
            sensor_state.board_volt = 0;
         /*   sensor_state.bat_volt = 0;
            sensor_state.exter_volt = 0;
            sensor_state.board_current = 0;*/
        }
    }

    sensor_state.exter_power = 7;

    data = 0;
    if(0 != sensor_state.board_temp)       data++;
    if(0 != sensor_state.board_volt)       data++;
   //if(0 != sensor_state.bat_volt)         data++;
    if(0 != sensor_state.exter_volt)       data++;
    if(0 != sensor_state.board_current)    data++;

    if(data == 0)
    {
        sensor_state.self_test = 0;
    }
    else
    {
        sensor_state.self_test = 1;
    }

    sensor_state.state_num = data + 2;  // �Լ�z  ֱ��xA
    }
}

