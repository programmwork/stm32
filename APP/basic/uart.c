
#include "string.h"
#include "stdio.h" 
#include "stdlib.h"

#include  <msp430.h>
#include "typedef.h"
#include "uart.h" 
#include "main.h" 
/*************************************************************************
** �������ƣ����ڳ�ʼ������
** ��������: IAR 4.11B 
** ����:    
** ��������: 2008.09.10
** ����:��ʼ������
        ���䷽ʽ���ã��������ã�����������
** ����ļ�:
** �޸���־��

*************************************************************************/
const BPS_CFG TableBPS[] =
{
    {6666,5,0},
    {3333,2,0},
    {1666,5,0},
    {833,2,0},//	BPS_9600
    {416,6,0},//	BPS_19200
    {208,3,0},//	BPS_38400
    {138,7,0},//	BPS_57600
    {69,4,0}  //	BPS_115200
};

const BPS2_CFG TableBPS2[] =
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


/*
 * RS485�շ����ƽŶ���
 * */

    #define RS485_RD_UART_1  {P5OUT &=~BIT5;}
    #define RS485_WD_UART_1  {P5OUT|=BIT5;}

    #define RS485_RD_UART_2  //{P9OUT &=~BIT6;}
    #define RS485_WD_UART_2  //{P9OUT|=BIT6;}
     

void rs485_ctrl_init(UINT8 uartno)
{
//Ŀǰֻ��uart1 �߱�RS485 ����
  if(uartno==MODBUSRS485_UARTNO)
  {
  //P5.5
    P5DIR |=BIT5;
    P5OUT |=BIT5;
  }
  else if(uartno==DEBUG_UARTNO)
   {
   //P9.6
   //  P9DIR |=BIT6;
   //  P9OUT |=BIT6;
   }
}
/*******************************************************************************
*�������ƣ�uartInit
*�������ܣ����ڳ�ʼ��
*��ڲ�����UINT8 uartno   ���ں�
        s_uartinfo_t *uartinfo  ��������
*���ڲ����� ��
*��д���ڣ�
*******************************************************************************/
void uartInit(UINT8 uartno,s_uartinfo_t *uartinfo)
{ 
    U32   baud=9600;
    UINT8 databit=8;
    UINT8 parity='N';
    UINT8 stopbit=1;

    if( uartinfo->uartdevtype==UART_DEVICE_DEBUG)
    {
    }
    else  if( uartinfo->uartdevtype==UART_DEVICE_SENSOR)
    {
    }
    else  if( uartinfo->uartdevtype==UART_DEVICE_RS485)
    {
    }
    else  if( uartinfo->uartdevtype==UART_DEVICE_LORA)
    {
    }
    else
    {
    }
   
    switch (uartno)
    {
        case 0:
        {
            baud    = bcm_info.common.se.baudrate;
            databit = bcm_info.common.se.datasbit;
            parity  = bcm_info.common.se.parity ;
            stopbit = bcm_info.common.se.stopbits;
            uart0Init(baud,databit,parity,stopbit);
        }
        break;
        case 1:
            uart1Init(baud,databit,parity,stopbit);
        break;
        case 2:
            uart2Init(baud,databit,parity,stopbit);
        break;
        case 3:
            uart3Init(baud,databit,parity,stopbit);
        break;

        default:break;
    }
    
 
}
/*******************************************************************************
*�������ƣ�uartXInit
*�������ܣ����ڳ�ʼ��
*��ڲ����� U32 bps          ������
        UINT8 databit    ����λ
        UINT8 parity     У��
        UINT8 stopbit    ֹͣλ
*���ڲ����� ��
*��д���ڣ�
*******************************************************************************/
void uart0Init(U32 bps,UINT8 databit,UINT8 parity,UINT8 stopbit)
{
	U8 i;
	U8 bps_index=0;

    for(i=0;i<BPS_MAX;i++)
    {
        if(bps == TableBPS2[i].bps)
        {
            bps_index = TableBPS2[i].bpsIndex;
            break;
        }
    }

    if(bps_index>BPS_MAX)
    return;

    //UART0:P3.4--TXD,P3.5--RXD
    P3SEL|=BIT4+BIT5;                     // ѡ�񴮿ڹ���
    UCA0CTL0 = UCMODE_0;                  // ѡ�񴮿ڹ���

    UCA0CTL1 |= UCSSEL__SMCLK;            // CLK =  8M
    UCA0BRW = TableBPS[bps_index].UBR0;
    UCA0MCTL = (TableBPS[bps_index].UMCTL<<4) + (TableBPS[bps_index].UBR1<<1);

    UCA0CTL1 &=~UCSWRST;         //�˳�USCI����

    UCA0IE |= UCRXIE;              //ʹ���շ��ж�

#if 1
    /*******DATABIT*********************************************/
    UCA0CTL0=0x00;
    if(databit==7)
    {
        UCA0CTL0 |=UC7BIT;
    }
    else //databit==8
    {}
    /*******PARITYBIT*********************************************/
    if(parity=='E')
    {
        UCA0CTL0 |=(UCPEN|UCPAR);
    }
    else if(parity=='O')
    {
        UCA0CTL0 |=UCPEN;
    }
    else //��NONE"
    {
    }
    /*******STOPBIT*********************************************/
    if(stopbit==2)
    {
        UCA0CTL0|=UCSPB;
    }
    else
    {
    }
#endif

    _BIS_SR(GIE);
}

void uart1Init(U32 bps,UINT8 databit,UINT8 parity,UINT8 stopbit)
{
    U8 i;
    U8 bps_index=0;

    for(i=0;i<BPS_MAX;i++)
    {
        if(bps==TableBPS2[i].bps)
        {
            bps_index=TableBPS2[i].bpsIndex;
            break;
        }
    }

    if(bps_index>BPS_MAX)
    return;

    //UART1:P5.6--TXD,P5.7--RXD
    P5SEL|=BIT6+BIT7;                     // ѡ�񴮿ڹ���
    UCA1CTL0 = UCMODE_0;                  // ѡ�񴮿ڹ���

    UCA1CTL1 |= UCSSEL__SMCLK;            // CLK =  8M
    UCA1BRW = TableBPS[bps_index].UBR0;
    UCA1MCTL = (TableBPS[bps_index].UMCTL<<4) + (TableBPS[bps_index].UBR1<<1);

    UCA1CTL1 &=~UCSWRST;           //�˳�USCI����

    UCA1IE |= UCRXIE;              //ʹ���շ��ж�

 #if 1
    //*******DATABIT*********************************************
    UCA1CTL0=0x00;
    if(databit==7)
    {
        UCA1CTL0 |=UC7BIT;
    }
    else //databit==8
    {}
    //*******PARITYBIT*******************************************
    if(parity=='E')
    {
        UCA1CTL0 |=(UCPEN|UCPAR);
    }
    else if(parity=='O')
    {
        UCA1CTL0 |=UCPEN;
    }
    else //��NONE"
    {
    }
    //*******STOPBIT*********************************************
    if(stopbit==2)
    {
        UCA1CTL0|=UCSPB;
    }
    else
    {
    }
#endif

    _BIS_SR(GIE);
}

void uart2Init(U32 bps,UINT8 databit,UINT8 parity,UINT8 stopbit)
{
    U8 i;
    U8 bps_index=0;

    for(i=0;i<BPS_MAX;i++)
    {
        if(bps==TableBPS2[i].bps)
        {
            bps_index=TableBPS2[i].bpsIndex;
            break;
        }
    }

    if(bps_index>BPS_MAX)
    return;

    //UART2:P9.4--TXD,P9.5--RXD
    P9SEL|=BIT4+BIT5;             // ѡ�񴮿ڹ���
    UCA2CTL0 = UCMODE_0;            // ѡ�񴮿ڹ���

    UCA2CTL1 |= UCSSEL__SMCLK;            // CLK =  8M
    UCA2BRW = TableBPS[bps_index].UBR0;
    UCA2MCTL = (TableBPS[bps_index].UMCTL<<4) + (TableBPS[bps_index].UBR1<<1);

    UCA2CTL1 &=~UCSWRST;         //�˳�USCI����

    UCA2IE |= UCRXIE;              //ʹ���շ��ж�

#if 1
    /*******DATABIT*********************************************/
    UCA2CTL0=0x00;
    if(databit==7)
    {
        UCA2CTL0 |=UC7BIT;
    }
    else //databit==8
    {}
    /*******PARITYBIT*********************************************/
    if(parity=='E')
    {
        UCA2CTL0 |=(UCPEN|UCPAR);
    }
    else if(parity=='O')
    {
        UCA2CTL0 |=UCPEN;
    }
    else //��NONE"
    {
    }
    /*******STOPBIT*********************************************/
    if(stopbit==2)
    {
        UCA2CTL0|=UCSPB;
    }
    else
    {
    }
#endif

    _BIS_SR(GIE);
}

void uart3Init(U32 bps,UINT8 databit,UINT8 parity,UINT8 stopbit)
{
    U8 i;
    U8 bps_index=0;

    for(i=0;i<BPS_MAX;i++)
    {
        if(bps==TableBPS2[i].bps)
        {
            bps_index=TableBPS2[i].bpsIndex;
            break;
        }
    }

    if(bps_index>BPS_MAX)
    return;

    //UART3:P10.4--TXD,P10.5--RXD
    P10SEL|=BIT4+BIT5;// ѡ�񴮿ڹ���
    UCA3CTL0 = UCMODE_0;                                         // ѡ�񴮿ڹ���

    UCA3CTL1 |= UCSSEL__SMCLK;            // CLK =  8M
    UCA3BRW = TableBPS[bps_index].UBR0;
    UCA3MCTL = (TableBPS[bps_index].UMCTL<<4) + (TableBPS[bps_index].UBR1<<1);

    UCA3CTL1 &=~UCSWRST;         //�˳�USCI����
    UCA3IE |= UCRXIE;              //ʹ���շ��ж�

#if 1

    /*******DATABIT*********************************************/
    UCA3CTL0=0x00;
    if(databit==7)
    {
        UCA3CTL0 |=UC7BIT;
    }
    else //databit==8
    {}
    /*******PARITYBIT*********************************************/
    if(parity=='E')
    {
        UCA3CTL0 |=(UCPEN|UCPAR);
    }
    else if(parity=='O')
    {
        UCA3CTL0 |=UCPEN;
    }
    else //��NONE"
    {
    }
    /*******STOPBIT*********************************************/
    if(stopbit==2)
    {
        UCA3CTL0|=UCSPB;
    }
    else
    {
    }
#endif

    _BIS_SR(GIE);
}
//------------------------- end uart init -------------------------

#pragma vector=USCI_A0_VECTOR 
__interrupt void USCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV,4))
    {
        case 0:break;                             // Vector 0 - no interrupt
        case 2:                                   // Vector 2 - RXIFG
        if(((m_tempdata.m_uartrcv[UARTDEV_0].WD + 1) % MAX_UARTRCV_LEN) != m_tempdata.m_uartrcv[UARTDEV_0].RD)//�ɽ��н��մ���
        {
             m_tempdata.m_uartrcv[UARTDEV_0].buff[m_tempdata.m_uartrcv[UARTDEV_0].WD]=UCA0RXBUF;
             m_tempdata.m_uartrcv[UARTDEV_0].WD = (m_tempdata.m_uartrcv[UARTDEV_0].WD + 1) % MAX_UARTRCV_LEN;
        }
        break;
        case 4:break;                             // Vector 4 - TXIFG
        default: break;
    }
}

#pragma vector=USCI_A1_VECTOR 
__interrupt void USCI_A1_ISR(void)
{
    switch(__even_in_range(UCA1IV,4))
    {
        case 0:break;                             // Vector 0 - no interrupt
        case 2:                                   // Vector 2 - RXIFG
        USART1_RX();
        break;
        case 4:break;                             // Vector 4 - TXIFG
        default: break;
    }
}

#pragma vector=USCI_A2_VECTOR 
__interrupt void USCI_A2_ISR(void)
{
switch(__even_in_range(UCA2IV,4))
{
    case 0:break;                             // Vector 0 - no interrupt
    case 2:                                   // Vector 2 - RXIFG
    break;
    case 4:break;                             // Vector 4 - TXIFG
    default: break;
    }
}

#pragma vector=USCI_A3_VECTOR 
__interrupt void USCI_A3_ISR(void)
{
    switch(__even_in_range(UCA3IV,4))
    {
        case 0:break;                             // Vector 0 - no interrupt
        case 2:                                   // Vector 2 - RXIFG
        break;
        case 4:break;                             // Vector 4 - TXIFG
        default: break;
    }
}

/*********************************************************************************************************
** Function name:	    uartSendStr1
** Descriptions:	    �򴮿ڷ����ַ���
** input parameters:    pucStr:  Ҫ���͵��ַ���ָ�� len���͵ĳ��ȣ����255,Ϊ0ʱֱ�ӷ�������Ƭ��ַ����
** output parameters:   ��
** Returned value:      ����ָ�����͵��ַ�����
*********************************************************************************************************/
void uartSendStr(UINT8 uartno,UINT8 *pucStr,UINT16 len)
{

    if(uartno == 0)
    {
        if(bcm_info.common.mo == 2)
        {
            
            
            delay_us(500);
            
            UartWrite(uartno, pucStr, len);
            
            delay_ms(2);
            
        }
        else
        {
            UartWrite(uartno, pucStr, len);
        }
    }
    else
    {
        UartWrite(uartno, pucStr, len);
    }
    
}

