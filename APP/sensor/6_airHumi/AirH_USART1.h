/*
;*--------------------------------------------- 文件信息 ----------------------------------------------------
;* 文件名称 : 
;* 文件功能 : 
;* 补充说明 :
;*-------------------------------------------- 最新版本信息 -------------------------------------------------
;* 修改作者 :
;* 修改日期 :
;* 版本声明 :
;*-------------------------------------------- 历史版本信息 -------------------------------------------------
;* 文件作者 : liujunjie_2008@126.com
;* 创建日期 :
;* 版本声明 :
;************************************************************************************************************
*/
#ifndef __AIRH_USART1_H_
#define	__AIRH_USART1_H_

#define	AirP_TX_RX_BUFF_LEN		110

/* 串口处理ADU数据帧的阶段描述 */
#define	AirP_USART1_PROCESSING_IDEL					0
#define	AirP_USART1_PROCESSING_SENDING			  1
#define	AirP_USART1_PROCESSING_RECEIVING		  2
#define	AirP_USART1_PROCESSING_FINISH				3
#define	AirP_USART1_PROCESSING_ERR					  4

unsigned char AirP_TxRxIndex;

extern unsigned char   AirP_UartProcessingPhase;
extern unsigned char AirP_RevStep;

static unsigned int AirP_T3IntCounter;
extern char AirP_TxRxBuffer[];															// 缓冲区
extern unsigned char  AirP_TxRxLength;						          // 字节长度
extern unsigned char T3Timer_start;

#define AirP_T3_START_COUNTING()         T3Timer_start = 1; AirP_T3IntCounter = 0

#define AirP_T3_RESTART_COUNTING()   AirP_T3IntCounter = 0

#define AirP_T3_STOP_COUNTING()      T3Timer_start = 0; AirP_T3IntCounter = 0


void USART1_RX(void);
extern void AirP_USART1_Init(void);
extern unsigned char AirP_USART1_SendBytes(void);
extern unsigned char AirP_USART1_GetProcessingPhase(void);
extern void AirP_USART1_ResetProcessingPhase(void);

extern unsigned char Airp_USART1_TK_Check(unsigned long *pAirp);

#endif
