/*
;************************************************************************************************************
;*										
;*
;*                                 	      	 
;*
;*--------------------------------------------- 文件信息 ----------------------------------------------------
;*
;* 文件名称 :
;* 文件功能 :
;*
;* 补充说明 :
;*-------------------------------------------- 最新版本信息 -------------------------------------------------
;* 修改作者 :
;* 修改日期 :
;* 版本声明 :
;*-------------------------------------------- 历史版本信息 -------------------------------------------------
;* 文件作者 : liujunjie_2008@126.com
;* 创建日期 :
;* 版本声明 :
;*-----------------------------------------------------------------------------------------------------------
;*-----------------------------------------------------------------------------------------------------------
;************************************************************************************************************
*/
#ifndef __SOILHUMI_HAL_H_
#define	__SOILHUMI_HAL_H_

extern void hal_sensor_init(void);
extern unsigned char Sensor_Init(void);
extern unsigned char SoilMoisture_engine(float result[5]); 
extern void Reset_Sensor(void);
extern void USART3_RX(void);

void SOILMOISTURE_USART1_Init(void);
unsigned char SOILMOISTURE_USART1_GetProcessingPhase(void);
void SOILMOISTURE_USART1_ProcessingPhaseReset(void);
unsigned char SOILMOISTURE_USART1_SendAudData(void);
unsigned char SOILMOISTURE_CalculateLRC( unsigned char *pd, unsigned char len);
unsigned char SOILMOISTURE_Char2Bin(char ucCharacter);
char SOILMOISTURE_Bin2Char(unsigned char ucByte);
unsigned char SOILMOISTURE_CharArray2BinArray(char *paa,unsigned char *pha,unsigned char aalen);
unsigned char SOILMOISTURE_BinArray2CharArray(unsigned char *binA,char *charA,unsigned char len);
static unsigned char SOILMOISTURE_TRSF_SetValid(void);


#endif
