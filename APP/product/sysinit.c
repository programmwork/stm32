/*
 * sysinit.c
 *
 *  Created on: 2016年3月7日
 *      Author: lenovo
 */


#include "main.h"

#define SYS_CFG_ADDR 0


const BPS_CFG TableBPS[] =
{
    {BPS_1200,1200},
    {BPS_2400,2400},
    {BPS_4800,4800},
    {BPS_9600,9600},
    {BPS_19200,19200},
    {BPS_38400,38400},
    {BPS_57600,57600},
    {BPS_115200,115200}
};





//******************************************************************************
//
//初始化信息打印
//
//******************************************************************************
void startupprint(void)
{
    char buf[256];
    sprintf(buf,"VERSION INFORMATION:\r\nHardware: %s\r\nSoftware: %s\r\n...System start...\r\n",m_defdata.m_baseinfo.hard_version,SOFT_VER);

    uartSendStr(UARTDEV_1,(unsigned char *)buf,strlen(buf));
    uartSendStr(UARTDEV_2,(unsigned char *)buf,strlen(buf));
    
    
    return;
}



void Init_Port()
{
 //无用的IO 电平固定，降功耗
    

    
}



void sysinit(void)
{    
    /*Configure GPIO pin : PC0 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    //Init_Clock();//初始化系统时钟

    Init_Port();//IO 初始化  包括一些供电控制引脚

    harddog_init();    //外部硬狗 初始化


    /* GPIO Ports Clock Enable */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);  //LED 初始化
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_SET);
    





    HalDS3231_Init();//RTC
    W25Q128_Init();//extern FLASH
    
    pam_init(&m_defdata);      //系统固定默认参数初始化
    sys_cfg_init();     //系统参数从FLASH或内存中获取
    tempdata_init(&m_tempdata);//临时变量初始化，全局参数初始化
    
    Uart_CFG(1, 1);
    Uart_CFG(2, 1);
    
    //主串口初始化
    uart_CBD CBD = {0};

    CBD.cid = 0;

    CBD.rw |= UART_WR;
    CBD.pri_t = COM_SEND_PRIORITY;
    CBD.heap_t_size = TASK_SEND_STACK_SIZE;    
    CBD.Mes_queue_Send_size = MES_SEND_SIZE;

    CBD.rw |= UART_RD;
    CBD.pri_r = COM_RECV_PRIORITY;
    CBD.heap_r_size = TASK_RECV_STACK_SIZE;
    
    UartOpen( &CBD );
    
    //串口2初始化
    uart_CBD CBD1 = {0};

    CBD1.cid = 1;

    CBD1.rw |= UART_WR;
    CBD1.pri_t = COM_SEND_PRIORITY;
    CBD1.heap_t_size = TASK_SEND_STACK_SIZE;      
    CBD1.Mes_queue_Send_size = MES_SEND_SIZE;

    CBD1.rw |= UART_RD;
    CBD1.pri_r = COM_RECV_PRIORITY;
    CBD1.heap_r_size = TASK_RECV_STACK_SIZE;

    UartOpen( &CBD1 );//
    
    DS3231_ReadTime(&m_tempdata.m_RtcTateTime);
	Init_ADC();

    hal_sensor_init();    //传感器对应IO 初始化

    Self_test();
}

//******************************************************************************
//
//串口初始化
//
//******************************************************************************

uint8_t Uart_CFG(uint8_t num, uint8_t msp)
{
    U8 i;
    U8 bps_index = 0;
    uint32_t bps = 9600;
    uint8_t databit = 8;
    char parity_temp = 'N';//奇偶检验
    uint8_t stopbit = 1;
    uint8_t temp = 0, parity = 0;

    if((num !=1)&&(num !=2))
    {
        return 0;
    }


    bps         = bcm_info.common.se[num - 1].baudrate;
    databit     = bcm_info.common.se[num - 1].datasbit;
    parity_temp = bcm_info.common.se[num - 1].parity;
    stopbit     = bcm_info.common.se[num - 1].stopbits;


    if(parity_temp == 'N')
    {
        parity = 0;
    }
    else
    {
        parity = 1;
    }    

    for(i=0;i<BPS_MAX;i++)
    {
        if(bps == TableBPS[i].bps)
        {
            bps_index = TableBPS[i].bpsIndex;
            break;
        }
    }


    if(bps_index > BPS_MAX)

    {
        return 0;
    }

    temp = UART_Init(num, bps, databit, parity, stopbit, msp);
    
    
    return temp;

}



//******************************************************************************
//
//硬件看门狗初始化、硬件看门狗喂狗
//
//******************************************************************************
void harddog_init(void)
{
    /* GPIO Ports Clock Enable */

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
}
void harddog_feed(void)
{
    uint32_t i = 10;
    
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
    while(i--);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
}



/*******************************************************************************
*函数名称：sys_cfg_init
*函数功能：系统配置初始化，根据FLASH内有无数据，视从何处配置系统默认参数
*入口参数：无
*出口参数： 0-从内存启动
        1-从外部FLASH启动
*编写日期：
*******************************************************************************/
unsigned char sys_cfg_init(void)
{
    bcm_info_t info;
    unsigned char int8u_1,int8u_2;
    unsigned int i;

    // 读取Flag标识并判断
    W25Q128_Data_Read(SYS_CFG_ADDR, &int8u_1, 1);

    if(int8u_1 != 0x7E)
    {
        for(i=0; i<10000; i++);         // 延时一小段时间

        W25Q128_Data_Read(SYS_CFG_ADDR, &int8u_2, 1);
        if(int8u_2 != 0x7E)
        {
            Init_sys_cfg();     //若FLASH中无数据 ，则执行本段初始化
            return 0;
        }
    }

    memset(&info, 0, sizeof(bcm_info_t));
    read_sys_cfg(&info);
    memcpy(&bcm_info, &info, sizeof(bcm_info_t));

    return 1;
}

/*******************************************************************************
*函数名称：read_sys_cfg
*函数功能：FLASH中系统控制信息初始化
*入口参数：bcm_info_t *p_bcm_info
*出口参数： 无
*编写日期：
*******************************************************************************/
void read_sys_cfg(bcm_info_t *p_bcm_info)
{
    W25Q128_Data_Read(SYS_CFG_ADDR, (uint8 *)p_bcm_info, sizeof(bcm_info_t));
}

/*******************************************************************************
*函数名称：save_sys_cfg
*函数功能：系统参数信息写入FLASH
*入口参数：bcm_info_t *p_bcm_info
*出口参数： 无
*编写日期：
*******************************************************************************/
void save_sys_cfg(bcm_info_t *p_bcm_info)
{//擦除系统配置扇区，写新的配置
    p_bcm_info->flag = 0x7e;
    W25Q128_Erase_Chip(W25Q128_ERS_SEC, SYS_CFG_ADDR);
    W25Q128_Data_Prog(SYS_CFG_ADDR,(uint8 *)p_bcm_info,256);
    W25Q128_Data_Prog(SYS_CFG_ADDR+256,((uint8 *)p_bcm_info)+256,256);
    W25Q128_Data_Prog(SYS_CFG_ADDR+512,((uint8 *)p_bcm_info)+512,(sizeof(bcm_info_t)-512));
}

/*******************************************************************************
*函数名称：pam_init
*函数功能：参数文件初始化，这些参数只可读，不可随意修改，并且恢复出厂设置的时候仍然保存
*入口参数：s_defdata_t *p_defdata_t
*出口参数： 无
*编写日期：
*******************************************************************************/
unsigned char pam_init(s_defdata_t *p_defdata_t)
{
    
    unsigned char int8u_1,int8u_2;
    unsigned int i;
    unsigned long addr;
    addr = FLASH_SN << 8;

    // 读取Flag标识并判断
    W25Q128_Data_Read(addr, &int8u_1, 1);

    if(int8u_1 != SAVEPAM_STARTFLAG_CHAR)
    {
        for(i=0; i<10000; i++);         // 延时一小段时间

        W25Q128_Data_Read(addr, &int8u_2, 1);
        if(int8u_2 != SAVEPAM_STARTFLAG_CHAR)
        {
            memset(&p_defdata_t->m_baseinfo.sn,0,sizeof(p_defdata_t->m_baseinfo.sn));     //若FLASH中无数据 ，则执行本段初始化
					  strncpy(sensor_di, DI,4);  //设备标识位
					  //
					  //
            return 0;
				}
			}
				
      W25Q128_Data_Read(addr, (uint8 *)p_defdata_t, sizeof(s_defdata_t));
			strncpy(sensor_di, DI,4);  //设备标识位
}
		

/*******************************************************************************
*函数名称：save_sys_cfg
*函数功能：系统参数信息写入FLASH
*入口参数：bcm_info_t *p_bcm_info
*出口参数： 无
*编写日期：
*******************************************************************************/
void pamsave(s_defdata_t *p_defdata_t)
{//擦除系统配置扇区，写新的配置
	  unsigned long addr;
    p_defdata_t->save_start_flag=SAVEPAM_STARTFLAG_CHAR;
    
	  addr = FLASH_SN << 8;
	  W25Q128_Erase_Chip(W25Q128_ERS_SEC, addr);
	  W25Q128_Data_Prog(addr,(uint8 *)p_defdata_t,sizeof(s_defdata_t));
}
/******************************************************************************
*系统参数 内部flash初始化
******************************************************************************/
void defdata_init(s_defdata_t *p_defdata_t)
{
    U8 i, sn_buf[40];

    p_defdata_t->save_start_flag=SAVEPAM_STARTFLAG_CHAR;

    memcpy(sn_buf, (char *)p_defdata_t->m_baseinfo.sn, 36);
    memset(p_defdata_t,0,sizeof(s_defdata_t));

    memcpy((char *)p_defdata_t->m_baseinfo.sn, sn_buf, 36);

    p_defdata_t->m_baseinfo.id = 0;

#if 0       //串口功能  初始化时配置
    //串口功能
    //接SENSOR
    i=UARTDEV_0;
    p_defdata_t->m_uartinfo[i].uartdevtype=UART_DEVICE_DEBUG;
    p_defdata_t->m_uartinfo[i].comworktype= COMTYPE_RS232;

    //接485
    i=UARTDEV_1;
    p_defdata_t->m_uartinfo[i].uartdevtype=UART_DEVICE_RS485;
    p_defdata_t->m_uartinfo[i].comworktype= COMTYPE_RS485;

    //接DEBUG通信口
    i=UARTDEV_2;
    p_defdata_t->m_uartinfo[i].uartdevtype=UART_DEVICE_SENSOR;
    p_defdata_t->m_uartinfo[i].comworktype= COMTYPE_RS232;

    //接OUT通信口
    i=UARTDEV_3;
    p_defdata_t->m_uartinfo[i].uartdevtype=UART_DEVICE_LORA;
    p_defdata_t->m_uartinfo[i].comworktype= COMTYPE_RS232;
#endif

    p_defdata_t->save_end_flag=SAVEPAM_ENDFLAG_CHAR;
}

/******************************************************************************
*临时变量初始化
******************************************************************************/
void tempdata_init(s_tempdata_t *p_tempdata_t)
{
    U8 i;

    //系统时间
    p_tempdata_t->m_RtcTateTime.year=2000;
    p_tempdata_t->m_RtcTateTime.month=1;
    p_tempdata_t->m_RtcTateTime.day=1;
    p_tempdata_t->m_RtcTateTime.hour=0;
    p_tempdata_t->m_RtcTateTime.min=0;
    p_tempdata_t->m_RtcTateTime.sec=0;
    p_tempdata_t->m_RtcTateTime.msec=0;

    //串口接收队列初始化
    for(i=0;i<MAX_UARTNUM;i++)
    {
        p_tempdata_t->m_uartrcv[i].WD=0;
        p_tempdata_t->m_uartrcv[i].RD=0;
    }

    //串口函数映射
    for(i=0;i<MAX_UARTNUM;i++)
    {
        //p_tempdata_t->m_uartrcv[i].process=&uartdev_process;//处理通信串口命令
    }

    //调试输出
    p_tempdata_t->DebugON=false;
    p_tempdata_t->m_debuguint_t.sample=false;

    //串口透传
    p_tempdata_t->m_opencomset_t.flag=false;
    p_tempdata_t->m_opencomset_t.mainuartno=0;
    p_tempdata_t->m_opencomset_t.slaveuartno=0;
    p_tempdata_t->m_opencomset_t.counter=0;


    m_tempdata.alive_counter=0;

    //传感器状态值
    memset(&sensor_state,0,sizeof(sensor_state));

    //板温、板压，偏高低判定值
    board_temp_max = bcm_info.common.work_temp_max * 1.1;
    if(bcm_info.common.work_temp_min < 0)
    {
        board_temp_min = bcm_info.common.work_temp_min * 1.1;
    }
    else
    {
        board_temp_min = bcm_info.common.work_temp_min * 0.9;
    }
    board_volt_max = bcm_info.common.boardsvolt_max * 1.1;
    board_volt_min = bcm_info.common.boardsvolt_min * 0.9;
}


