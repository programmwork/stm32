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
#ifndef __AIRH_USART1_H_
#define	__AIRH_USART1_H_

#define	AirH_TX_RX_BUFF_LEN		110

/* ���ڴ���ADU����֡�Ľ׶����� */
#define	USART_PROCESSING_IDEL					0
#define	USART_PROCESSING_SENDING			  1
#define	USART_PROCESSING_RECEIVING		  2
#define	USART_PROCESSING_FINISH				3
#define	USART_PROCESSING_ERR					  4

extern unsigned char AirH_TxRxIndex;
extern unsigned char   AirH_UartProcessingPhase;
extern unsigned char AirH_RevStep;
extern char AirH_TxRxBuffer[];															// ������
extern unsigned char  AirH_TxRxLength;						          // �ֽڳ���

void USART3_RX(void);
extern void AirH_Init(void);
extern unsigned char AirH_GetProcessingPhase(void);
extern void AirH_ResetProcessingPhase(void);


#endif
