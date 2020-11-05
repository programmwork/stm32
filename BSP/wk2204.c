/************************************************************************************************************
;*--------------------------------------------- 文件信息 ----------------------------------------------------
;* 文件名称 : wk2204.c
;* 文件功能 :wk2204芯片驱动
;* 补充说明 :
;*-------------------------------------------- 最新版本信息 -------------------------------------------------
;* 修改作者 :
;* 修改日期 :
;* 版本声明 :
;*-------------------------------------------- 历史版本信息 -------------------------------------------------
;* 文件作者 : guozikun
;* 创建日期 : 2020.10.28
;* 版本声明 : wk2204芯片底层驱动
***********************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <wk2204.h>
s_U3rc hu3;


/*************************************************************************************/
/******************************WK2204Init*******************************************/
//函数功能：本函数主要会初始化一些芯片基本寄存器；
/*********************************************************************************/
void WK2204Init( UINT8 port)
{
    uint8_t gena = 0,grst = 0,gier = 0,sier = 0,scr = 0;
    //使能子串口时钟
    gena=WK2204ReadReg(port,WK2XXX_GENA);
    switch (port)
    {
        case 1://使能子串口 1 的时钟
            gena|=WK2XXX_UT1EN;
            WK2204WriteReg(port,WK2XXX_GENA,gena);
            break;
        case 2://使能子串口 2 的时钟
            gena|=WK2XXX_UT2EN;
            WK2204WriteReg(port,WK2XXX_GENA,gena);
            break;
        case 3://使能子串口 3 的时钟
            gena|=WK2XXX_UT3EN;
            WK2204WriteReg(port,WK2XXX_GENA,gena);
            break;
        case 4://使能子串口 4 的时钟
            gena|=WK2XXX_UT4EN;
            WK2204WriteReg(port,WK2XXX_GENA,gena);
            break;
    }
    //软件复位子串口
    grst=WK2204ReadReg(port,WK2XXX_GRST);
    switch (port)
    {
        case 1://软件复位子串口 1
        grst|=WK2XXX_UT1RST;
        WK2204WriteReg(port,WK2XXX_GRST,grst);
        break;
        case 2://软件复位子串口 2
        grst|=WK2XXX_UT2RST;
        WK2204WriteReg(port,WK2XXX_GRST,grst);
        break;
        case 3://软件复位子串口 3
        grst|=WK2XXX_UT3RST;
        WK2204WriteReg(port,WK2XXX_GRST,grst);
        break;
        case 4://软件复位子串口 4
        grst|=WK2XXX_UT4RST;
        WK2204WriteReg(port,WK2XXX_GRST,grst);
        break;
    }
    //使能子串口中断，包括子串口总中断和子串口内部的接收中断，和设置中断触点
    gier=WK2204ReadReg(port,WK2XXX_GIER);
    switch (port)
    {
        case 1://软件复位子串口 1
        gier|=WK2XXX_UT1RST;
        WK2204WriteReg(port,WK2XXX_GIER,gier);
        break;
        case 2://软件复位子串口 2
        gier|=WK2XXX_UT2RST;

        WK2204WriteReg(port,WK2XXX_GIER,gier);
        break;
        case 3://软件复位子串口 3
        gier|=WK2XXX_UT3RST;
        WK2204WriteReg(port,WK2XXX_GIER,gier);
        break;
        case 4://软件复位子串口 4
        gier|=WK2XXX_UT4RST;
        WK2204WriteReg(port,WK2XXX_GIER,gier);
        break;
    }
    //使能子串口接收触点中断和超时中断
    sier=WK2204ReadReg(port,WK2XXX_SIER);
    sier |= WK2XXX_RFTRIG_IEN|WK2XXX_RXOUT_IEN;
    WK2204WriteReg(port,WK2XXX_SIER,sier);
    // 初始化 FIFO 和设置固定中断触点
    WK2204WriteReg(port,WK2XXX_FCR,0XFF);
    //设置任意中断触点，如果下面的设置有效，那么上面 FCR 寄存器中断的固定中断触点将失效
    WK2204WriteReg(port,WK2XXX_SPAGE,1);//切换到 page1
    WK2204WriteReg(port,WK2XXX_RFTL,0X40);//设置接收触点为 64 个字节
    WK2204WriteReg(port,WK2XXX_TFTL,0X10);//设置发送触点为 16 个字节
    WK2204WriteReg(port,WK2XXX_SPAGE,0);//切换到 page0
    //使能子串口的发送和接收使能
    scr=WK2204ReadReg(port,WK2XXX_SCR);
    scr|=WK2XXX_TXEN|WK2XXX_RXEN;
    WK2204WriteReg(port,WK2XXX_SCR,scr);
}


/**************************************BaudAdaptive***********************************/
//函数功能：波特率自适应，发送 0x55 实现主接口 uart 波特率自适应
//*************************************************************************/
void BaudAdaptive()
{
     unsigned char cmd = 0x55;

    // 波特率自适应匹配。发送 0X55 用来是匹配波特率
    uartSendStr(UARTDEV_3,(unsigned char *)cmd,sizeof(cmd));

    delay_ms(1000); 
}

/**************************************WK2204WriteReg********************************/
//函数功能：写寄存器函数(前提是该寄存器可写)
//参数：port:为子串口的数(C0C1)
// reg:为寄存器的地址(A3A2A1A0)
// dat:为写入寄存器的数据
//注意：在子串口被打通的情况下，向 FDAT 写入的数据会通过 TX 引脚输出
//*************************************************************************/
void WK2204WriteReg(unsigned char port,unsigned char reg,unsigned char dat)
{
    unsigned char cmd = 0x00;
    
    //写指令，对于指令的构成见数据手册
    cmd = ((port-1)<<4)+reg;

    //发送写指令
    uartSendStr(UARTDEV_3, (unsigned char *)cmd, sizeof(cmd));
}


/*************************************WK2204ReadReg**********************************
/
//函数功能：读寄存器函数
//参数：port:为子串口的数(C0C1)
// reg:为寄存器的地址(A3A2A1A0)
// rec_data:为读取到的寄存器值
//注意：在子串口被打通的情况下，读 FDAT，实际上就是读取 uart 的 rx 接收的数据
/*************************************************************************/
unsigned char WK2204ReadReg(unsigned char port,unsigned char reg, unsigned char *data)
{
    uint8_t  cmd = 0x00, str[8], buffer[MAX_PKGLEN], len;
    uint16_t len = 0, rd = 0, i = 10, j = 256;

    memset(str, 0x00, sizeof(str));
    //写指令，对于指令的构成见数据手册
    cmd = 0x40+((port-1)<<4)+reg;
    uartSendStr(UARTDEV_3, (unsigned char *)cmd, sizeof(cmd));
    while(i--)
    {
        //接收返回的寄存器值
        if((hu3.WD ) != hu3.RD)//必须处理
        {
            data = hu3.buff[hu3.RD];            
            hu3.RD = (hu3.RD + 1)%MAXLEN;            
            return success;
        }
        HAL_Delay(1);
    }
        
    return fail;  
} 

/**************************************WK2204writeFIFO*******************************/
//函数功能：写 FIFO 函数（该函数写入的数据会通过 uart 的 TX 发送出去)
//参数：port:为子串口的数(C0C1)
// *dat：为写入数据指针
// num：为写入数据的个数，不超过 16 个字节（N3N2N1N0）
/*************************************************************************/
void WK2204WriteFIFO(unsigned char port,unsigned char *send_da,unsigned char num)
{
    unsigned char i, cmd = 0x00;
    cmd = 0x80+((port-1)<<4)+(num-1); //写指令,对于指令构成见数据手册
    uartSendStr(UARTDEV_3, (unsigned char *)cmd, sizeof(cmd));

    for(i=0; i<num; i++)
    {
        uartSendStr(UARTDEV_3, (unsigned char *)*send_da, sizeof(*send_da));
    }
}

/***************************************WK2204readFIFO*******************************/
//函数功能：读 FIFO 函数(该函数读取的数据是 FIFO 缓存中的数据，实际上是 uart 的 rx 接收的数据)
//参数：port:为子串口的数(C0C1)
// *dat：为读到数据指针
// num：为读出数据的个数，不超过 16 个字节（N3N2N1N0）
/*************************************************************************/
unsigned char WK2204ReadFIFO(unsigned char port,unsigned char num)
{

}

/***************************************WK2204readFIFO*******************************/
//函数功能：读 FIFO 函数(该函数读取的数据是 FIFO 缓存中的数据，实际上是 uart 的 rx 接收的数据)
//参数：port:为子串口的数(C0C1)
// *dat：为读到数据指针
// num：为读出数据的个数，不超过 16 个字节（N3N2N1N0）
/*************************************************************************/
unsigned char WK2204ReadFIFO_byte(unsigned char port,unsigned char* data, uint16_t len)
{
    uint8_t rec_data, cmd = 0x00, reg;
    uint32_t timeout = 10, fifo_len = 512;

    //查询FSR是否有数据     
    reg= WK2204ReadReg(PORT_1, WK2204_FSR);    
    
    //0x08 == bit0001000   FSR子串口FIFO状态寄存器和第四bit进行逻辑或
    while(0x08 | reg)
    {
        cmd = WK2204_FDAT+((port-1)<<4);
        //发送读取子串口 FIFO 数据寄存器命令
        uartSendStr(UARTDEV_3, (unsigned char *)cmd, sizeof(cmd));

        //接收返回的寄存器值
        //len = sprintf(data, "%s", WK2204ReadReg(UARTDEV_3, WK2204_FDAT));
    }
    return len;
}

/**************************WK2204SetBaud*********************************************

 函数功能：设置子串口波特率函数、此函数中波特率的匹配值是根据 11.0592Mhz 下的外部晶振计算的
 port:子串口号
 baud:波特率大小.波特率表示方式，

*************************WK2204SetBaud******************************************/
void WK2204SetBaud(UINT8 port,int baud)
{
UINT8 baud1 = 0, baud0 = 0, pres = 0, scr = 0;
//如下波特率相应的寄存器值，是在外部时钟为 11.0592 的情况下计算所得，如果使用其他晶振，需要重新计算
switch (baud)
{
     case 600:
        baud1=0x4;
        baud0=0x7f;
        pres=0;
     break;
     case 1200:
        baud1=0x2;
        baud0=0x3F;
        pres=0;
     break;
     case 2400:
        baud1=0x1;
        baud0=0x1f;
        pres=0;
     break;
     case 4800:
        baud1=0x00;
        baud0=0x8f;
        pres=0;
     break;
     case 9600:
        baud1=0x00;
        baud0=0x47;
        pres=0;
     break;
     case 19200:
        baud1=0x00;
        baud0=0x23;
        pres=0;
     break;
     case 38400:
        baud1=0x00;
        baud0=0x11;
        pres=0;
     break;
     case 76800:
        baud1=0x00;
        baud0=0x08;
        pres=0;
     break;

     case 1800:
        baud1=0x01;
        baud0=0x7f;
        pres=0;
     break;
     case 3600:
        baud1=0x00;
        baud0=0xbf;
        pres=0;
     break;
     case 7200:
        baud1=0x00;
        baud0=0x5f;
        pres=0;
     break;
     case 14400:
        baud1=0x00;
        baud0=0x2f;
        pres=0;
     break;
     case 28800:
        baud1=0x00;
        baud0=0x17;
        pres=0;
    break;
     case 57600:
        baud1=0x00;
        baud0=0x0b;
        pres=0;
     break;
     case 115200:
        baud1=0x00;
        baud0=0x05;
        pres=0;
     break;
     case 230400:
        baud1=0x00;
        baud0=0x02;
        pres=0;
     break;
     default:
        baud1=0x00;
        baud0=0x00;
        pres=0;
     }
    //关掉子串口收发使能
    scr=WK2204ReadReg(port,WK2XXX_SCR); 
    WK2204WriteReg(port,WK2XXX_SCR,0);
    //设置波特率相关寄存器
    WK2204WriteReg(port,WK2XXX_SPAGE,1);//切换到 page1
    WK2204WriteReg(port,WK2XXX_BAUD1,baud1);
    WK2204WriteReg(port,WK2XXX_BAUD0,baud0);
    WK2204WriteReg(port,WK2XXX_PRES,pres);
    WK2204WriteReg(port,WK2XXX_SPAGE,0);//切换到 page0
    //使能子串口收发使能
    WK2204WriteReg(port,WK2XXX_SCR,scr);
}



/*
*********************************************************************************************************
** 函数名称 ：
** 函数功能 ：串口接收中断服务程序
** 入口参数 ：
**
** 出口参数 ：
*********************************************************************************************************
*/

void WK2204_RX(void)
{
    //可进行接收处理
    if(((hu3.WD + 1) % MAXLEN) != hu3.RD)
    {
        hu3.buff[hu3.WD] = (uint8_t)(huart3.Instance->RDR);	
        hu3.WD = (hu3.WD + 1) % MAXLEN;
    }
    return;
}


