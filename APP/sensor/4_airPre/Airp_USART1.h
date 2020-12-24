/*
;*--------------------------------------------- �ļ���Ϣ ----------------------------------------------------
;* �ļ����� : 
;* �ļ����� : 
;* ����˵�� :
;*-------------------------------------------- ���°汾��Ϣ -------------------------------------------------
;* �޸����� :
;* �޸����� :
;* �汾���� :
;*-------------------------------------------- ��ʷ�汾��Ϣ -------------------------------------------------
;* �ļ����� : liujunjie_2008@126.com
;* �������� :
;* �汾���� :
;************************************************************************************************************
*/
#ifndef __AIRP_USART1_H_
#define	__AIRP_USART1_H_

#define	AIRP_TX_RX_BUFF_LEN		32

/* ���ڴ���ADU����֡�Ľ׶����� */
#define	AIRP_USART_PROCESSING_IDEL				  0
#define	AIRP_USART_PROCESSING_SENDING			  1
#define	AIRP_USART_PROCESSING_RECEIVING		  2
#define	AIRP_USART_PROCESSING_FINISH			  3
#define	AIRP_USART_PROCESSING_ERR				  4


extern unsigned char   AirP_UartProcessingPhase;
extern unsigned char AirP_RevStep;
extern unsigned char AirP_TxRxIndex;
extern char AirP_TxRxBuffer[];															// ������
extern unsigned char  AirP_TxRxLength;						          // �ֽڳ���

void USART3_RX(void);
extern void AirP_Init(void);
extern unsigned char AirP_GetProcessingPhase(void);
extern void AirP_ResetProcessingPhase(void);

#endif
