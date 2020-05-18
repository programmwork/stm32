/************************************************************************************************************
;*--------------------------------------------- 文件信息 ----------------------------------------------------
;* 文件名称 :   flash.c
;* 文件功能 :   片内存储
;* 补充说明 :
;*-------------------------------------------- 最新版本信息 -------------------------------------------------
;* 修改作者 :
;* 修改日期 :
;* 版本声明 :
;*-------------------------------------------- 历史版本信息 -------------------------------------------------
;* 文件作者 :
;* 创建日期 :
;* 版本声明 :
***********************************************************************************************************/
#include "flash.h"
#include "typedef.h"   
#include "string.h"
    
//*********************************************************  
//写片内存储的数据
//*********************************************************
void flash_writebuf(U8 *flash_ptr,U8 *buffer,U8 len)
{  
    __disable_interrupt();                      // 禁止中断  
    FCTL3 = FWKEY;                              // 设置写密钥并解锁  
    FCTL1 = FWKEY+ERASE;                        // 段擦除  
    *(unsigned int *)flash_ptr = 0;             // ？？  
    FCTL1 = FWKEY+WRT;                          // 字节写入  
  U8 i;
    for (   i = 0; i < len; i++)
    {  
        *flash_ptr++ = *buffer++;  
    }  
  
    FCTL1 = FWKEY;                              // 设置写密钥  
    FCTL3 = FWKEY+LOCK;                         // 重新锁住  
    __enable_interrupt();                       // 恢复中断  
} 

//*********************************************************
//读取片内存储的数据
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
//擦除片内存储的数据
//*********************************************************
void flash_clear(U8 *flash_ptr,U8 len)
{  
    __disable_interrupt();                      // 禁止中断  
    FCTL3 = FWKEY;                              // 设置写密钥并解锁  
    FCTL1 = FWKEY+ERASE;                        // 段擦除  
    *(unsigned int *)flash_ptr = 0;             // ？？  
    FCTL1 = FWKEY+WRT;                          // 字节写入  
  U8 i;
    for (   i = 0; i < len; i++)
    {  
        *flash_ptr++ = 0xFF;//*buffer++;  
    }  
  
    FCTL1 = FWKEY;                              // 设置写密钥  
    FCTL3 = FWKEY+LOCK;                         // 重新锁住  
    __enable_interrupt();                       // 恢复中断  
} 

//*********************************************************
//参数文件保存
//*********************************************************
void pamsave(s_defdata_t *p_defdata_t)
{
    U16 size=sizeof(s_defdata_t);
    U8 num_section;//总共多少段
    U8 len_section=128;//每段长度
    U8 len_lastsection=128;//最后一段长度
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
        if((i+1)==num_section)//最后一段
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
//参数文件擦除
//*********************************************************
void pamclear(s_defdata_t *p_defdata_t)
{
    //参数删除，只需要删除第一个字节就行
    p_defdata_t->save_start_flag=0xFF;
    p_defdata_t->save_end_flag=0xFF;
    flash_writebuf((U8*)SegmentStart,(U8 *)p_defdata_t,10);
}

//*********************************************************
//参数文件初始化检查
//*********************************************************
U8 testpam_exist(s_defdata_t *p_defdata_t)
{
    U16 size=sizeof(s_defdata_t);
    U8 num_section;//总共多少段
    U8 len_section=128;//每段长度
    U8 len_lastsection=128;//最后一段长度
    U8 i;

    num_section=size/128;
    if(size%128 !=0)
    {
        num_section++;
        len_lastsection=size%128;
    }

    for(  i=0;i<num_section;i++)
    {
        if((i+1)==num_section)//最后一段
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


