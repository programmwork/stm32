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

#define	TX_RX_BUFF_LEN		32

/* ���ڴ���ADU����֡�Ľ׶����� */
#define	USART_PROCESSING_IDEL				  0
#define	USART_PROCESSING_SENDING			  1
#define	USART_PROCESSING_RECEIVING		  2
#define	USART_PROCESSING_FINISH			  3
#define	USART_PROCESSING_ERR				  4


extern unsigned char   UartProcessingPhase;
extern unsigned char RevStep;
extern unsigned char TxRxIndex;
extern char TxRxBuffer[];															// ������
extern unsigned char  TxRxLength;						          // �ֽڳ���

void USART3_RX(void);
extern void AirP_Init(void);
extern unsigned char AirP_GetProcessingPhase(void);
extern void AirP_ResetProcessingPhase(void);

#endif
