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
#ifndef __AIRP_USART1_H_
#define	__AIRP_USART1_H_

#define	AIRP_TX_RX_BUFF_LEN		32

/* 串口处理ADU数据帧的阶段描述 */
#define	AIRP_USART_PROCESSING_IDEL				  0
#define	AIRP_USART_PROCESSING_SENDING			  1
#define	AIRP_USART_PROCESSING_RECEIVING		  2
#define	AIRP_USART_PROCESSING_FINISH			  3
#define	AIRP_USART_PROCESSING_ERR				  4


extern unsigned char   AirP_UartProcessingPhase;
extern unsigned char AirP_RevStep;
extern unsigned char AirP_TxRxIndex;
extern char AirP_TxRxBuffer[];															// 缓冲区
extern unsigned char  AirP_TxRxLength;						          // 字节长度

void USART3_RX(void);
extern void AirP_Init(void);
extern unsigned char AirP_GetProcessingPhase(void);
extern void AirP_ResetProcessingPhase(void);

#endif
