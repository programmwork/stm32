/************************************************************************************************************
;*--------------------------------------------- �ļ���Ϣ ----------------------------------------------------
;* �ļ����� :   flash.c
;* �ļ����� :   Ƭ�ڴ洢
;* ����˵�� :
;*-------------------------------------------- ���°汾��Ϣ -------------------------------------------------
;* �޸����� :
;* �޸����� :
;* �汾���� :
;*-------------------------------------------- ��ʷ�汾��Ϣ -------------------------------------------------
;* �ļ����� :
;* �������� :
;* �汾���� :
***********************************************************************************************************/
#include "flash.h"
#include "typedef.h"   
#include "string.h"
    
//*********************************************************  
//дƬ�ڴ洢������
//*********************************************************
void flash_writebuf(U8 *flash_ptr,U8 *buffer,U8 len)
{  
    __disable_interrupt();                      // ��ֹ�ж�  
    FCTL3 = FWKEY;                              // ����д��Կ������  
    FCTL1 = FWKEY+ERASE;                        // �β���  
    *(unsigned int *)flash_ptr = 0;             // ����  
    FCTL1 = FWKEY+WRT;                          // �ֽ�д��  
  U8 i;
    for (   i = 0; i < len; i++)
    {  
        *flash_ptr++ = *buffer++;  
    }  
  
    FCTL1 = FWKEY;                              // ����д��Կ  
    FCTL3 = FWKEY+LOCK;                         // ������ס  
    __enable_interrupt();                       // �ָ��ж�  
} 

//*********************************************************
//��ȡƬ�ڴ洢������
//*********************************************************
void flash_readbuf(U8 *flash_ptr,U8 *buffer,U8 len)
{  
    __disable_interrupt();  
   U8 i;
    for(   i = 0 ; i < len ; i++ )
    {  
        *buffer++ =  *flash_ptr++;  
    }  
  
    __enable_interrupt();  
}

//*********************************************************
//����Ƭ�ڴ洢������
//*********************************************************
void flash_clear(U8 *flash_ptr,U8 len)
{  
    __disable_interrupt();                      // ��ֹ�ж�  
    FCTL3 = FWKEY;                              // ����д��Կ������  
    FCTL1 = FWKEY+ERASE;                        // �β���  
    *(unsigned int *)flash_ptr = 0;             // ����  
    FCTL1 = FWKEY+WRT;                          // �ֽ�д��  
  U8 i;
    for (   i = 0; i < len; i++)
    {  
        *flash_ptr++ = 0xFF;//*buffer++;  
    }  
  
    FCTL1 = FWKEY;                              // ����д��Կ  
    FCTL3 = FWKEY+LOCK;                         // ������ס  
    __enable_interrupt();                       // �ָ��ж�  
} 

//*********************************************************
//�����ļ�����
//*********************************************************
void pamsave(s_defdata_t *p_defdata_t)
{
    U16 size=sizeof(s_defdata_t);
    U8 num_section;//�ܹ����ٶ�
    U8 len_section=128;//ÿ�γ���
    U8 len_lastsection=128;//���һ�γ���
    U8 i;

    p_defdata_t->save_start_flag = SAVEPAM_STARTFLAG_CHAR;
    p_defdata_t->save_end_flag = SAVEPAM_ENDFLAG_CHAR;

    num_section=size/128;
    if(size%128 !=0)
    {
        num_section++;
        len_lastsection=size%128;
    }

    for(  i=0;i<num_section;i++)
    {
        if((i+1)==num_section)//���һ��
        {
            flash_writebuf((U8*)(SegmentStart+i*len_section),(U8 *)p_defdata_t+i*len_section,len_lastsection);
        }
        else
        {
            flash_writebuf((U8*)(SegmentStart+i*len_section),(U8 *)p_defdata_t+i*len_section,len_section);
        }
    }
}

//*********************************************************
//�����ļ�����
//*********************************************************
void pamclear(s_defdata_t *p_defdata_t)
{
    //����ɾ����ֻ��Ҫɾ����һ���ֽھ���
    p_defdata_t->save_start_flag=0xFF;
    p_defdata_t->save_end_flag=0xFF;
    flash_writebuf((U8*)SegmentStart,(U8 *)p_defdata_t,10);
}

//*********************************************************
//�����ļ���ʼ�����
//*********************************************************
U8 testpam_exist(s_defdata_t *p_defdata_t)
{
    U16 size=sizeof(s_defdata_t);
    U8 num_section;//�ܹ����ٶ�
    U8 len_section=128;//ÿ�γ���
    U8 len_lastsection=128;//���һ�γ���
    U8 i;

    num_section=size/128;
    if(size%128 !=0)
    {
        num_section++;
        len_lastsection=size%128;
    }

    for(  i=0;i<num_section;i++)
    {
        if((i+1)==num_section)//���һ��
        {
            flash_readbuf((U8*)(SegmentStart+i*len_section),(U8*)p_defdata_t+i*len_section,len_lastsection);
        }
        else
        {
            flash_readbuf((U8*)(SegmentStart+i*len_section),(U8*)p_defdata_t+i*len_section,len_section);
        }
    }

    if((p_defdata_t->save_start_flag == SAVEPAM_STARTFLAG_CHAR)
            &&(p_defdata_t->save_end_flag == SAVEPAM_ENDFLAG_CHAR))
    {
        return true;
    }
    else
    {
        flash_clear((U8*)SegmentStart+0,128);
        flash_clear((U8*)SegmentStart+128,128);
        flash_clear((U8*)SegmentStart+256,128);

        return false;
    }
}


