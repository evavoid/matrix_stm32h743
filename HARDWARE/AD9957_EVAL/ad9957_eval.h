#ifndef __AD9957EVAL_H
#define __AD9957EVAL_H
#include "matrixos.h"

/**
* @pinconfig
*		PH6		IO_RESET
*		PH7 	CS~
*		PH8		SCLK
*		PH9 	SDO
*		PH10	SDIO
*		PH11	IO_UPDATE
*/

#define AD9957_IO_RESET(n) 				  (n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_RESET))
#define AD9957_CS(n) 								(n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_RESET))
#define AD9957_SCLK(n) 							(n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET))
//#define AD9957_SDO(n) 							(n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET))
#define AD9957_SDIO(n) 							(n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET))
#define AD9957_IO_UPDATE(n) 				(n?HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_RESET))
#define AD9957_SDO_READ    					HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_5)
//////////////////////////////////////////////// copy from ADI ExampleCode

// Control Function register 1
#define RAM_ENABLE									0x80000000
#define RAM_PLAYBACK_IQPATH					0x10000000
#define MANUAL_OSK_CONTROL					0x00800000
#define INVERSE_SINC_ENABLE					0x00400000
#define CLEAR_CCI										0x00200000
#define SINE_OUTPUT									0x00010000
#define ATCLR_PHSE_ACC							0x00002000
#define CLR_PHSE_ACC								0x00000800
#define ARR_IO_UP										0x00000400
#define OSK_ENABLE                  0x00000200
#define AUTO_OSK										0x00000100
#define DIG_POW_DOWN                0x00000080
#define DAC_POW_DOWN                0x00000040
#define REFCLK_POW_DWN              0x00000020
#define AUX_DAC_POW_DWN             0x00000010
#define EXT_PDWN_CTRL               0x00000008
#define AUTO_PDWN_ENABLE						0x00000004
#define SDIO_IP											0x00000002
#define LSB_FIRST										0x00000001

// Control Function register 2
#define BLACKFIN_MODE_EN						0x80000000
#define BLACKFIN_BIT_ORDER					0x40000000
#define BLACKFIN_EARLY_FRAME_EN			0x20000000
#define PROF_REG_ASF_SOURCE					0x01000000
#define INT_IO_UPDATE								0x00800000
#define SYNC_CLK_EN									0x00400000
#define EFFEC_FTW										0x00010000
#define PDCLK_RATE									0x00002000
#define OFFSET_BINARY								0x00001000
#define PDCLK_ENABLE								0x00000800
#define PDCLK_INVERT								0x00000400
#define TX_INVERT										0x00000200
#define Q_FIRST_EN									0x00000100
#define DATA_ASSEM_HLD							0x00000040
#define SYNC_TIM_DIS								0x00000020

// Control Function register 3
#define REFCLK_INP_DIV_BYPASS				0x00008000
#define REFCLK_INP_DIV_RST					0x00004000
#define PLL_ENABLE									0x00000100

// Operating modes
#define QUADRATURE_MOD							0x00000000
#define SINGLE_TONE									0x01000000
#define INTERPOLATING_DAC						0x02000000

// REFCLK OUT buffer control
#define LOW_OUT_CURRENT							0x10000000
#define MEDIUM_OUT_CURRENT					0x20000000
#define HIGH_OUT_CURRENT						0x30000000

// Multichip Synchronisation register
#define SYNC_REC_ENABLE							0x08000000
#define SYNC_GEN_ENABLE							0x04000000
#define SYNC_GEN_POL								0x02000000

// Profile QDUC register
#define SPECTRAL_INVERT							0x02000000
#define INVERSE_CCI_BYPASS					0x01000000

// RAM segment register
#define RAM_SEGMENT_REG_0						0x05
#define RAM_SEGMENT_REG_1						0x06

// Profile registers
#define PROFILE_0										0x0E
#define PROFILE_1										0x0F
#define PROFILE_2										0x10
#define PROFILE_3										0x11
#define PROFILE_4										0x12
#define PROFILE_5										0x13
#define PROFILE_6										0x14
#define PROFILE_7										0x15

// Data mode
#define WRITE 											0x00
#define READ  											0x01

//-----------------------------------------------------
//				   评估板例程给出的基础函数组A
//-----------------------------------------------------
//void WriteReg(unsigned int Data, unsigned int NumBytes);
//void ReadReg(unsigned int ReadBuff[], unsigned int NumBytes);
//unsigned int ArrToByte(unsigned int arr[]);

//-----------------------------------------------------
//				     对评估板例程基础函数进行对应替换,为B
//-----------------------------------------------------
void AD9957_WriteOneByte(u8 Data);
uint8_t AD9957_ReadOneByte(void);

void Init_AD9957_EVAL(void);
void AD9957_WriteReg(unsigned int Data, unsigned int NumBytes);
void AD9957_ReadReg(unsigned int ReadBuff[], unsigned int NumBytes);

//-----------------------------------------------------
//				   顶层函数，未被注释的函数，内部函数组已经由A替换为了B
//-----------------------------------------------------
void IO_Update(void);
void ControlFunctionReg1(unsigned char mode, unsigned int Data, unsigned int InputBuff[]);
void ControlFunctionReg2(unsigned char mode, unsigned int Data, unsigned int IOUpdateRate, unsigned int InputBuff[]);
void ControlFunctionReg3(unsigned char mode, unsigned int VCOSel, unsigned int CurrSett, unsigned int NDivide, unsigned int Data, unsigned int InputBuff[]);
void AuxDacControlReg(unsigned char mode, unsigned int FSC, unsigned int InputBuff[]);
void IOUpdateRateReg(unsigned char mode, unsigned int Data, unsigned int InputBuff[]);
//void RAMSegmentReg(unsigned char mode, unsigned char SegmentReg, unsigned int StepRate, unsigned int EndAddr, unsigned int StartAddr, unsigned char RAMPlayback, unsigned int InputBuff1[], unsigned int InputBuff2[]);
//void AmpScaleReg(unsigned char mode, unsigned int RampRate, unsigned int ScaleFactor, unsigned int StepSize, unsigned int InputBuff[]);
//void MultiChipSyncReg(unsigned char mode, unsigned char SyncValDelay, unsigned char SyncStatePreset, unsigned char SyncGenDelay, unsigned char SyncRecDelay, unsigned int Data, unsigned int InputBuff[]); 
void ProfSingleToneReg(unsigned char mode, unsigned char ProfNum, unsigned int ASF, unsigned int POW, unsigned int FTW, unsigned int InputBuff1[], unsigned int InputBuff2[]);
//void ProfQDUCReg(unsigned char mode, unsigned char ProfNum, unsigned char InterpolRate, unsigned int OSF, unsigned int POW, unsigned int FTW, unsigned char settings, unsigned int InputBuff1[], unsigned int InputBuff2[]);
//void RAMReg(unsigned char mode, unsigned int Data, unsigned int InputBuff[]);
//void GPIOReg(unsigned char mode, unsigned int Data, unsigned int InputBuff[]);
//void GPIODataReg(unsigned char mode, unsigned int Data, unsigned int InputBuff[]);

//-----------------------------------------------------
//	典型顶层应用
//-----------------------------------------------------

/*
	Init_AD9957_EVAL();
	unsigned int inpbuff4[4]={0x00,0x00,0x00,0x00};
	unsigned int inpbuff5[4]={0x00,0x00,0x00,0x00};
	//Write to Profile registers，写入配置表
	ProfSingleToneReg(WRITE,PROFILE_0,0x08B5,0x0000,0x193DD97F,inpbuff4,inpbuff5);
	delay_us(10);
*/

/*	
	Init_AD9957_EVAL();
	unsigned int inpbuff1[4]={0x00,0x00,0x00,0x00};
	//AD9957_ReadReg(inpbufftest,4);
	ControlFunctionReg1(READ,CFR1,inpbuff1);//回读寄存器值
	delay_us(10);
*/

#endif
