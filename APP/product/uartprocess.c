
#include "string.h"
#include "stdio.h"
#include "stdlib.h"




#include "main.h"



uint8 rcvd_buffer[MAX_PKGLEN];
void checkuart(U8 uartno)
{
    uint16 len = 0, rd = 0, i = 0;
    uint8 rcvd_flag = 0;

    if(m_tempdata.m_uartrcv[uartno].RD != m_tempdata.m_uartrcv[uartno].WD)//±ØÐë´¦Àí
    {
        if(m_tempdata.m_uartrcv[uartno].WD >m_tempdata.m_uartrcv[uartno].RD)
        {
            len = m_tempdata.m_uartrcv[uartno].WD - m_tempdata.m_uartrcv[uartno].RD;
        }
        else
        {
            len = MAX_UARTRCV_LEN + m_tempdata.m_uartrcv[uartno].WD - m_tempdata.m_uartrcv[uartno].RD;
        }

        if(len > MAX_PKGLEN)
        {
            len = MAX_PKGLEN;
        }

        rd = m_tempdata.m_uartrcv[uartno].RD;
		memset(rcvd_buffer, 0, MAX_PKGLEN);
        for(i = 0;i < len;i++)
        {
            rcvd_buffer[i] = m_tempdata.m_uartrcv[uartno].buff[rd];

            rd = (rd + 1)%MAX_UARTRCV_LEN;

            if(rcvd_buffer[i] == '\n')
            {
                if(i > 0)
                {
                    if(rcvd_buffer[i - 1] == '\r')
                    {
                        rcvd_flag = true;
                    }
                }

                m_tempdata.m_uartrcv[uartno].RD = rd;
                break;
            }
        }

        if(rcvd_flag==true)
        {
            i++;
            uartdev_process(uartno, rcvd_buffer, i);
            //return 1;
        }
        else
        {
            if(i >= MAX_PKGLEN)
            {
                m_tempdata.m_uartrcv[uartno].RD = rd;
            }

            //return 0;
        }
    }
}



char rbuff[490];
U8 uartdev_process(U8 uartno,U8 *buff,U16 len)
{
    U16 rlen=0;

    memset(rbuff,0,490);

    if(len >= 2)
    {
        rlen= uartcmd_process(uartno,(char *)buff,len,(char *)rbuff);
        uartSendStr(uartno,(UINT8 *)rbuff,rlen);
    }
    return 0;
}


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


U8 uartdev_process_rs485(U8 uartno,U8 *buff,U8 len)
{
    return 0;
}

U8 uartdev_process_sensor(U8 uartno,U8 *buff,U8 len)
{

   return 0;
}

U8 uartdev_process_lora(U8 uartno,U8 *buff,U8 len)
{

    return 0;
}

