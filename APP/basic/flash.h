#ifndef __FLASH_H__
#define __FLASH_H__

#include "typedef.h"
#include "defpam.h"
#include "main.h"

#define SegmentStart 0x01800  //Segment = 0x01000~0x010FF  
//#define SegmentStart 0x020000
#define SegmentStart_II SegmentStart+0x80
#define SegmentStart_III SegmentStart+0x100
#define SegmentStart_IV  SegmentStart+0x180
#define SegmentSize_EACHSEG  0x80 //Ã¿¶Î 128char

#define SegmentSize 0x100       //SegmentA + SegmentB = 256  


extern void flash_writebuf(U8 *flash_ptr,U8 *buffer,U8 len) ;
extern void flash_readbuf(U8 *flash_ptr,U8 *buffer,U8 len) ;
extern void flash_clear(U8 *flash_ptr,U8 len) ;


extern void pamsave(s_defdata_t *m_defdata_t);
extern void pamclear(s_defdata_t *m_defdata_t);
extern U8 testpam_exist(s_defdata_t *m_defdata_t);

#endif
