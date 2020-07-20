/*
 * sensor_process.c
 *
 *  Created on: 2017Äê10ÔÂ28ÈÕ
 *      Author: lenovo
 */

#include "sensor_basic.h"


void time_task(void *pvParameters)
{
    static U16 p_msec=0xFF;
    static U8 p_sec=0xFF;
    static U8 p_min=0xFF;
    static U8 p_hour=0xFF;

	

    //sysinit();
        
    //startupprint();    //¿ªÊ¼ÐÅÏ¢  ´òÓ¡






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
        
            //Ã¿30·ÖÖÓ µÄµÚ50Ãë¶ÁÓ²Ê±ÖÓ¸øÈíÊ±ÖÓÐ£Ê±
            if((m_tempdata.m_RtcTateTime.min%30 == 0)&&(m_tempdata.m_RtcTateTime.sec == 50))
            {
                s_RtcTateTime_t time_struct_hardrtc_temp;
                //if(DS3231_ReadTime(&time_struct_hardrtc_temp)==1)  //Ê±¼äÐ£ÑéÕýÈ·²Å¸øÈíÊ±ÖÓÐ£Ê±
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

            if(m_tempdata.SuperadMin == true) //È¨ÏÞÃüÁî¿ªÆô ²¢¼ÆÊ±
            {
                m_tempdata.SuperadMinCnt ++;
                if(m_tempdata.SuperadMinCnt > 20)
                {
                    m_tempdata.SuperadMin = 0;
                    m_tempdata.SuperadMinCnt = 0;
                }
            }
            if(m_tempdata.DebugON == true)    //µ÷ÊÔÃüÁî¿ªÆô ²¢¼ÆÊ±
            {
                m_tempdata.DebugONCnt ++;
                if(m_tempdata.DebugONCnt > 20)
                {
                    m_tempdata.DebugON = false;
                    m_tempdata.DebugONCnt = 0;
                   // m_tempdata.SecDataOut = false;    //¹Ø±ÕË½ÓÐÃüÁîÈ¨ÏÞÊ±  ¹Ø±ÕÃëÊý¾Ý·¢ËÍ
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

        if(sensors_data.boardstemp_data>board_temp_max)//Æ«¸ß3
            sensor_state.board_temp = 3;
        else if(sensors_data.boardstemp_data<board_temp_min)//Æ«µÍ4
            sensor_state.board_temp = 4;
        else if((sensors_data.boardstemp_data >= bcm_info.common.work_temp_min) && (sensors_data.boardstemp_data <= bcm_info.common.work_temp_max)) //Õý³£
            sensor_state.board_temp = 0;

        //°åÑ¹²âÁ¿
        if(sensors_data.boardsvolt_data>board_volt_max)//Æ«¸ß3
        {
            sensor_state.board_volt = 3;
           /* sensor_state.bat_volt = 3;
            sensor_state.exter_volt = 3;
            sensor_state.board_current = 3;*/
        }
        else if(sensors_data.boardsvolt_data<board_volt_min)//Æ«µÍ4
        {
            sensor_state.board_volt = 4;
           /* sensor_state.bat_volt = 4;
            sensor_state.exter_volt = 4;
            sensor_state.board_current = 4;*/
        }
        else if((sensors_data.boardsvolt_data >= bcm_info.common.boardsvolt_min) && (sensors_data.boardsvolt_data <= bcm_info.common.boardsvolt_max)) //Õý³£
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

    sensor_state.state_num = data + 2;  // ×Ô¼ìz  Ö±Á÷xA
}

