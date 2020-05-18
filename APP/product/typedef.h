/*
 * typedef.h
 *
 *  Created on: 2016年3月7日
 *      Author: lenovo
 */

#ifndef  SOURCE_BASIC_TYPEDEF_H_
#define  SOURCE_BASIC_TYPEDEF_H_

typedef unsigned char U8;
typedef unsigned int  U16;
typedef unsigned long U32;
typedef signed char   S8;
typedef signed int    S16;
typedef signed long   S32;

typedef  unsigned char   UINT8;
typedef  unsigned short  UINT16;
typedef  unsigned long   UINT32;

typedef  unsigned char   uint8;
typedef  unsigned short  uint16;
typedef  unsigned long   uint32;

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef TRUE
#define TRUE  0x01
#endif

#ifndef FALSE
#define FALSE 0x00
#endif
#ifndef VALID
#define VALID 1
#endif
#ifndef INVALID
#define INVALID 0
#endif

#ifndef OFF
#define OFF 0
#endif

#ifndef ON
#define ON 1
#endif

#ifndef UNALIVE
#define UNALIVE 0
#endif

#ifndef ALIVE
#define ALIVE 1
#endif

/*
系統延時函數
*/
#define CPU_F ((double)8000000) //假設系統時鐘頻率8M
#define delay_us(x) //__delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) //__delay_cycles((long)(CPU_F*(double)x/1000.0))

#define UcharToLONG(LONG24,LONG16,LONG8,LONG0)     ((UINT32)( ((UINT32)LONG24<< 24) | ((UINT32)LONG16<< 16)| ((UINT32)LONG8<< 8) | (UINT32)LONG0))

#endif
