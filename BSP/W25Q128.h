
#ifndef SOURCE_MODULE_FLASH_W25X16_H_
#define SOURCE_MODULE_FLASH_W25X16_H_

#ifndef __W25Q128_H
#define __W25Q128_H


//W25Q128   ��256�飬ÿ��16������ÿ����16ҳ      ��65536ҳ
//W25X16    ��32�飬ÿ��16������ÿ����16ҳ       ��8192ҳ
//��32���ַ����������������ַ��ÿ����16������
//  Block: 64K
// Sector: 4K
//   Page: 256B
#define BLOCK_ADDR_0        0X000000            //��0������ʼ��ַ
#define BLOCK_ADDR_1        0X010000            //��1������ʼ��ַ
#define BLOCK_ADDR_2        0X020000            //��2������ʼ��ַ
#define BLOCK_ADDR_3        0X030000            //��3������ʼ��ַ
#define BLOCK_ADDR_4        0X040000            //��4������ʼ��ַ
#define BLOCK_ADDR_5        0X050000            //��5������ʼ��ַ
#define BLOCK_ADDR_6        0X060000            //��6������ʼ��ַ
#define BLOCK_ADDR_7        0X070000            //��7������ʼ��ַ
#define BLOCK_ADDR_8        0X080000            //��8������ʼ��ַ
#define BLOCK_ADDR_9        0X090000            //��9������ʼ��ַ
#define BLOCK_ADDR_10       0X0A0000            //��10������ʼ��ַ
#define BLOCK_ADDR_11       0X0B0000            //��11������ʼ��ַ
#define BLOCK_ADDR_12       0X0C0000            //��12������ʼ��ַ
#define BLOCK_ADDR_13       0X0D0000            //��13������ʼ��ַ
#define BLOCK_ADDR_14       0X0E0000            //��14������ʼ��ַ
#define BLOCK_ADDR_15       0X0F0000            //��15������ʼ��ַ
#define BLOCK_ADDR_16       0X100000            //��16������ʼ��ַ
#define BLOCK_ADDR_17       0X110000            //��17������ʼ��ַ
#define BLOCK_ADDR_18       0X120000            //��18������ʼ��ַ
#define BLOCK_ADDR_19       0X130000            //��19������ʼ��ַ
#define BLOCK_ADDR_20       0X140000            //��20������ʼ��ַ
#define BLOCK_ADDR_21       0X150000            //��21������ʼ��ַ
#define BLOCK_ADDR_22       0X160000            //��22������ʼ��ַ
#define BLOCK_ADDR_23       0X170000            //��23������ʼ��ַ
#define BLOCK_ADDR_24       0X180000            //��24������ʼ��ַ
#define BLOCK_ADDR_25       0X190000            //��25������ʼ��ַ
#define BLOCK_ADDR_26       0X1A0000            //��26������ʼ��ַ
#define BLOCK_ADDR_27       0X1B0000            //��27������ʼ��ַ
#define BLOCK_ADDR_28       0X1C0000            //��28������ʼ��ַ
#define BLOCK_ADDR_29       0X1D0000            //��29������ʼ��ַ
#define BLOCK_ADDR_30       0X1E0000            //��30������ʼ��ַ
#define BLOCK_ADDR_31       0X1F0000            //��31������ʼ��ַ
//���� ����������ַ
#define SECTOR_ADDR_0       0X000000            //��0������ʼ��ַ
#define SECTOR_ADDR_1       0X001000            //��1������ʼ��ַ
#define SECTOR_ADDR_2       0X002000            //��2������ʼ��ַ
#define SECTOR_ADDR_3       0X003000            //��3������ʼ��ַ
#define SECTOR_ADDR_4       0X004000            //��4������ʼ��ַ
#define SECTOR_ADDR_5       0X005000            //��5������ʼ��ַ
#define SECTOR_ADDR_6       0X006000            //��6������ʼ��ַ
#define SECTOR_ADDR_7       0X007000            //��7������ʼ��ַ
#define SECTOR_ADDR_8       0X008000            //��8������ʼ��ַ
#define SECTOR_ADDR_9       0X009000            //��9������ʼ��ַ
#define SECTOR_ADDR_10      0X00A000            //��10������ʼ��ַ
#define SECTOR_ADDR_11      0X00B000            //��11������ʼ��ַ
#define SECTOR_ADDR_12      0X00C000            //��12������ʼ��ַ
#define SECTOR_ADDR_13      0X00D000            //��13������ʼ��ַ
#define SECTOR_ADDR_14      0X00E000            //��14������ʼ��ַ
#define SECTOR_ADDR_15      0X00F000            //��15������ʼ��ַ


#define MAX_ADDR        0x1FFFFF    // ����оƬ�ڲ�����ַ
#define SEC_MAX         1024        // �������������
#define SEC_SIZE        0x1000      // ������С

#define W25Q128_ENA_RST		0X66
#define W25Q128_EXC_RST		0X99

#define W25Q128_DIS_WR		0X04
#define W25Q128_ENA_WR		0X06
#define W25Q128_ENA_VOL		0X50

#define W25Q128_ERS_SEC		0X20
#define W25Q128_ERS_32K		0X52
#define W25Q128_ERS_64K		0XD8
#define W25Q128_ERS_CHIP	0X60

#define W25Q128_DATA_READ	0X03
#define W25Q128_DATA_PROG	0X02


#define W25Q128_SR1_BUSY	0X01
#define W25Q128_SR1_WEL		0X02
#define W25Q128_SR1_BP0		0X04
#define W25Q128_SR1_BP1		0X08
#define W25Q128_SR1_BP2		0X10
#define W25Q128_SR1_TB		0X20
#define W25Q128_SR1_SEC		0X40
#define W25Q128_SR1_SRP		0X80

#define W25Q128_SR2_SRL		0X01
#define W25Q128_SR2_QE		0X02
#define W25Q128_SR2_RSV		0X04
#define W25Q128_SR2_LB1		0X08
#define W25Q128_SR2_LB2		0X10
#define W25Q128_SR2_LB3		0X20
#define W25Q128_SR2_CMP		0X40
#define W25Q128_SR2_SUS		0X80

#define W25Q128_SR3_WPS		0X04
#define W25Q128_SR3_DR0		0X20
#define W25Q128_SR3_DR1		0X40


#define W25Q128_READ_SR1	0X05
#define W25Q128_READ_SR2	0X35
#define W25Q128_READ_SR3	0X15
#define W25Q128_SET_SR1		0X01
#define W25Q128_SET_SR2		0X31
#define W25Q128_SET_SR3		0X11


#define W25Q128_GET_DID		0XAB
#define W25Q128_GET_MID		0X90
#define W25Q128_GET_UID		0X4B


void W25Q128_Init( void );
void W25Q128_Reset(void);
uint8_t W25Q128_Data_Read( uint32_t addr, uint8_t *rbuf, uint16_t len );
uint8_t W25Q128_Data_Prog( uint32_t addr, uint8_t *tbuf, uint16_t len );

void W25Q128_WriteEnable( void );
void W25Q128_WriteDisable( void );
void W25Q128_WriteVolatile( void );
void W25Q128_WriteVolatile( void );
uint8_t W25Q128_SET_SR( uint8_t cmd, uint8_t val );
void W25Q128_Erase_Chip( uint8_t cmd, uint32_t addr );


uint8_t W25Q128_IS_Busy( void );

void W25Q128_Get_DID( void );
void W25Q128_Get_MID( void );
void W25Q128_Get_UID( void );
void W25Q128_Send_Buf( uint8_t *buf, uint16_t len, uint8_t over );
void W25Q128_Recv_Buf( uint8_t *buf, uint16_t len );
void W25Q128_Read_Reg( uint8_t cmd, uint8_t *buf, uint16_t len );


#endif
#endif

