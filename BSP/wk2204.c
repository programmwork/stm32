/************************************************************************************************************
;*--------------------------------------------- �ļ���Ϣ ----------------------------------------------------
;* �ļ����� : wk2204.c
;* �ļ����� :wk2204оƬ����
;* ����˵�� :
;*-------------------------------------------- ���°汾��Ϣ -------------------------------------------------
;* �޸����� :
;* �޸����� :
;* �汾���� :
;*-------------------------------------------- ��ʷ�汾��Ϣ -------------------------------------------------
;* �ļ����� : guozikun
;* �������� : 2020.10.28
;* �汾���� : wk2204оƬ�ײ�����
***********************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <wk2204.h>


/*************************************************************************************/
/******************************WK2204Init*******************************************/
//�������ܣ���������Ҫ���ʼ��һЩоƬ�����Ĵ�����
/*********************************************************************************/
void WK2204Init( UINT8 port)
{
    uint8_t gena = 0,grst = 0,gier = 0,sier = 0,scr = 0;
    //ʹ���Ӵ���ʱ��
    gena=WK2204ReadReg(port,WK2XXX_GENA);
    switch (port)
    {
        case 1://ʹ���Ӵ��� 1 ��ʱ��
            gena|=WK2XXX_UT1EN;
            WK2204WriteReg(port,WK2XXX_GENA,gena);
            break;
        case 2://ʹ���Ӵ��� 2 ��ʱ��
            gena|=WK2XXX_UT2EN;
            WK2204WriteReg(port,WK2XXX_GENA,gena);
            break;
        case 3://ʹ���Ӵ��� 3 ��ʱ��
            gena|=WK2XXX_UT3EN;
            WK2204WriteReg(port,WK2XXX_GENA,gena);
            break;
        case 4://ʹ���Ӵ��� 4 ��ʱ��
            gena|=WK2XXX_UT4EN;
            WK2204WriteReg(port,WK2XXX_GENA,gena);
            break;
    }
    //�����λ�Ӵ���
    grst=WK2204ReadReg(port,WK2XXX_GRST);
    switch (port)
    {
        case 1://�����λ�Ӵ��� 1
        grst|=WK2XXX_UT1RST;
        WK2204WriteReg(port,WK2XXX_GRST,grst);
        break;
        case 2://�����λ�Ӵ��� 2
        grst|=WK2XXX_UT2RST;
        WK2204WriteReg(port,WK2XXX_GRST,grst);
        break;
        case 3://�����λ�Ӵ��� 3
        grst|=WK2XXX_UT3RST;
        WK2204WriteReg(port,WK2XXX_GRST,grst);
        break;
        case 4://�����λ�Ӵ��� 4
        grst|=WK2XXX_UT4RST;
        WK2204WriteReg(port,WK2XXX_GRST,grst);
        break;
    }
    //ʹ���Ӵ����жϣ������Ӵ������жϺ��Ӵ����ڲ��Ľ����жϣ��������жϴ���
    gier=WK2204ReadReg(port,WK2XXX_GIER);
    switch (port)
    {
        case 1://�����λ�Ӵ��� 1
        gier|=WK2XXX_UT1RST;
        WK2204WriteReg(port,WK2XXX_GIER,gier);
        break;
        case 2://�����λ�Ӵ��� 2
        gier|=WK2XXX_UT2RST;

        WK2204WriteReg(port,WK2XXX_GIER,gier);
        break;
        case 3://�����λ�Ӵ��� 3
        gier|=WK2XXX_UT3RST;
        WK2204WriteReg(port,WK2XXX_GIER,gier);
        break;
        case 4://�����λ�Ӵ��� 4
        gier|=WK2XXX_UT4RST;
        WK2204WriteReg(port,WK2XXX_GIER,gier);
        break;
    }
    //ʹ���Ӵ��ڽ��մ����жϺͳ�ʱ�ж�
    sier=WK2204ReadReg(port,WK2XXX_SIER);
    sier |= WK2XXX_RFTRIG_IEN|WK2XXX_RXOUT_IEN;
    WK2204WriteReg(port,WK2XXX_SIER,sier);
    // ��ʼ�� FIFO �����ù̶��жϴ���
    WK2204WriteReg(port,WK2XXX_FCR,0XFF);
    //���������жϴ��㣬��������������Ч����ô���� FCR �Ĵ����жϵĹ̶��жϴ��㽫ʧЧ
    WK2204WriteReg(port,WK2XXX_SPAGE,1);//�л��� page1
    WK2204WriteReg(port,WK2XXX_RFTL,0X40);//���ý��մ���Ϊ 64 ���ֽ�
    WK2204WriteReg(port,WK2XXX_TFTL,0X10);//���÷��ʹ���Ϊ 16 ���ֽ�
    WK2204WriteReg(port,WK2XXX_SPAGE,0);//�л��� page0
    //ʹ���Ӵ��ڵķ��ͺͽ���ʹ��
    scr=WK2204ReadReg(port,WK2XXX_SCR);
    scr|=WK2XXX_TXEN|WK2XXX_RXEN;
    WK2204WriteReg(port,WK2XXX_SCR,scr);
}


/**************************************BaudAdaptive***********************************/
//�������ܣ�����������Ӧ������ 0x55 ʵ�����ӿ� uart ����������Ӧ
//*************************************************************************/
void BaudAdaptive()
{
     unsigned char cmd = 0x55;

    // ����������Ӧƥ�䡣���� 0X55 ������ƥ�䲨����
    uartSendStr(UARTDEV_3,(unsigned char *)cmd,sizeof(cmd));

    delay_ms(1000); 
}

/**************************************WK2204WriteReg********************************/
//�������ܣ�д�Ĵ�������(ǰ���ǸüĴ�����д)
//������port:Ϊ�Ӵ��ڵ���(C0C1)
// reg:Ϊ�Ĵ����ĵ�ַ(A3A2A1A0)
// dat:Ϊд��Ĵ���������
//ע�⣺���Ӵ��ڱ���ͨ������£��� FDAT д������ݻ�ͨ�� TX �������
//*************************************************************************/
void WK2204WriteReg(unsigned char port,unsigned char reg,unsigned char dat)
{
    unsigned char cmd = 0x00;
    
    //дָ�����ָ��Ĺ��ɼ������ֲ�
    cmd = ((port-1)<<4)+reg;

    //����дָ��
    uartSendStr(UARTDEV_3, (unsigned char *)cmd, sizeof(cmd));
}


/*************************************WK2204ReadReg**********************************
/
//�������ܣ����Ĵ�������
//������port:Ϊ�Ӵ��ڵ���(C0C1)
// reg:Ϊ�Ĵ����ĵ�ַ(A3A2A1A0)
// rec_data:Ϊ��ȡ���ļĴ���ֵ
//ע�⣺���Ӵ��ڱ���ͨ������£��� FDAT��ʵ���Ͼ��Ƕ�ȡ uart �� rx ���յ�����
/*************************************************************************/
unsigned char WK2204ReadReg(unsigned char port,unsigned char reg)
{
    uint8_t rec_data = 0x00, cmd = 0x00, buf[256], str[8];
    uint32 i = 256;
    memset(buf, 0x00, sizeof(buf));
    memset(str, 0x00, sizeof(str));
    //дָ�����ָ��Ĺ��ɼ������ֲ�
    cmd = 0x40+((port-1)<<4)+reg;
    uartSendStr(UARTDEV_3, (unsigned char *)cmd, sizeof(cmd));

    //���շ��صļĴ���ֵ
    if(HAL_OK == HAL_UART_Receive(&huart3 , (uint8_t *)m_tempdata.m_uartrcv[UARTDEV_3].buff, 1, 0xFFFF))
    {
        return (uint8_t )m_tempdata.m_uartrcv[UARTDEV_3].buff;
    }
    else
    {
        rec_data = INVALID_BUF;
        return rec_data;
    }
}

/**************************************WK2204writeFIFO*******************************/
//�������ܣ�д FIFO �������ú���д������ݻ�ͨ�� uart �� TX ���ͳ�ȥ)
//������port:Ϊ�Ӵ��ڵ���(C0C1)
// *dat��Ϊд������ָ��
// num��Ϊд�����ݵĸ����������� 16 ���ֽڣ�N3N2N1N0��
/*************************************************************************/
void WK2204WriteFIFO(unsigned char port,unsigned char *send_da,unsigned char num)
{
    unsigned char i, cmd = 0x00;
    cmd = 0x80+((port-1)<<4)+(num-1); //дָ��,����ָ��ɼ������ֲ�
    uartSendStr(UARTDEV_3, (unsigned char *)cmd, sizeof(cmd));

    for(i=0; i<num; i++)
    {
        uartSendStr(UARTDEV_3, (unsigned char *)*send_da, sizeof(*send_da));
    }
}

/***************************************WK2204readFIFO*******************************/
//�������ܣ��� FIFO ����(�ú�����ȡ�������� FIFO �����е����ݣ�ʵ������ uart �� rx ���յ�����)
//������port:Ϊ�Ӵ��ڵ���(C0C1)
// *dat��Ϊ��������ָ��
// num��Ϊ�������ݵĸ����������� 16 ���ֽڣ�N3N2N1N0��
/*************************************************************************/
unsigned char WK2204ReadFIFO(unsigned char port,unsigned char num)
{
    unsigned char rec_data, n, cmd = 0x00;
    unsigned char dat[256];

    memset(dat, 0x00, sizeof(dat));
    
    cmd = 0xc0+((port-1)<<4)+(num-1);
    uartSendStr(UARTDEV_3, (unsigned char *)cmd, sizeof(cmd));

    //���շ��صļĴ���ֵ//////////////////////////
    rec_data = INVALID_BUF;

    return rec_data;
}

/**************************WK2204SetBaud*********************************************

 �������ܣ������Ӵ��ڲ����ʺ������˺����в����ʵ�ƥ��ֵ�Ǹ��� 11.0592Mhz �µ��ⲿ��������
 port:�Ӵ��ں�
 baud:�����ʴ�С.�����ʱ�ʾ��ʽ��

*************************WK2204SetBaud******************************************/
void WK2204SetBaud(UINT8 port,int baud)
{
UINT8 baud1 = 0, baud0 = 0, pres = 0, scr = 0;
//���²�������Ӧ�ļĴ���ֵ�������ⲿʱ��Ϊ 11.0592 ������¼������ã����ʹ������������Ҫ���¼���
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
    //�ص��Ӵ����շ�ʹ��
    scr=WK2204ReadReg(port,WK2XXX_SCR); 
    WK2204WriteReg(port,WK2XXX_SCR,0);
    //���ò�������ؼĴ���
    WK2204WriteReg(port,WK2XXX_SPAGE,1);//�л��� page1
    WK2204WriteReg(port,WK2XXX_BAUD1,baud1);
    WK2204WriteReg(port,WK2XXX_BAUD0,baud0);
    WK2204WriteReg(port,WK2XXX_PRES,pres);
    WK2204WriteReg(port,WK2XXX_SPAGE,0);//�л��� page0
    //ʹ���Ӵ����շ�ʹ��
    WK2204WriteReg(port,WK2XXX_SCR,scr);
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

void delay(void)
{
    uint32_t i = 0;
    for(i = 9999; i < 0 ; i--);

}


