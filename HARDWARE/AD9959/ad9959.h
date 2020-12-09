#ifndef __AD9959_H
#define __AD9959_H 
#include "matrixos.h"

/**
	when WRRG_OPEN is 1, the function of AD9959_IOQ_Output can 
	be used, otherwise the function of others can be used.
	The parameter is designed for multiple channels.
****/
#define WRRG_OPEN 1

//应用参考值
#define FRE_REF    8.589934592f
#define POW_REF    45.51111111f

//晶振频率:25M
#define Crystal  25000000
//系统时钟:500M
#define System_Clock 500000000

#define AD9959_SDIO0(n)		(n?HAL_GPIO_WritePin(GPIOI,GPIO_PIN_0,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOI,GPIO_PIN_0,GPIO_PIN_RESET))
#define AD9959_SDIO3(n)		(n?HAL_GPIO_WritePin(GPIOI,GPIO_PIN_1,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOI,GPIO_PIN_1,GPIO_PIN_RESET)) 

#define AD9959_UP(n)		(n?HAL_GPIO_WritePin(GPIOI,GPIO_PIN_4,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOI,GPIO_PIN_4,GPIO_PIN_RESET))
#define AD9959_CS(n)		(n?HAL_GPIO_WritePin(GPIOI,GPIO_PIN_5,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOI,GPIO_PIN_5,GPIO_PIN_RESET))
#define AD9959_SCK(n)		(n?HAL_GPIO_WritePin(GPIOI,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOI,GPIO_PIN_6,GPIO_PIN_RESET))
#define AD9959_RST(n)		(n?HAL_GPIO_WritePin(GPIOI,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOI,GPIO_PIN_7,GPIO_PIN_RESET))


#define CSR  0x00    //通道选择寄存器
#define FR1  0x01    //功能控制寄存器1
#define FR2  0x02    //功能控制寄存器2
#define CFR  0x03    //通道功能寄存器
#define CFTW0 0x04   //通道频率控制字0
#define CPOW0 0x05   //通道相位补偿字0
#define ACR   0x06   //幅度控制寄存器
#define LSRR  0x07   //线性扫描率,[7:0]Rise,[15:8]fall
#define RDW   0x08   //线性扫描步进字rising
#define FDW   0x09   //线性扫描步进字falling
#define CW1   0x0a   //通道字1
#define CW2   0x0b
#define CW3   0x0c
#define CW4   0x0d
#define CW5   0x0e
#define CW6   0x0f
#define CW7   0x10
#define CW8   0x11
#define CW9   0x12
#define CW10  0x13
#define CW11  0x14
#define CW12  0x15
#define CW13  0x16
#define CW14  0x17
#define CW15  0x18

/***ERROR****/
//void AD9959__Sweep_Trigger(u8 Channel);

/***HARDWARE FUNCTION****/
void AD9959GPIOInit(void);
void AD9959_IO_UpDate(void);
void AD9959_Reset(void);
void AD9959_Start(void);
void AD9959_WByte(unsigned char byte);
void AD9959_WRrg(u8 reg, u32 data);

/***MID LAYER FUNCTION***/
void AD9959_Init(void);

void AD9959_Set_Fre(u32 Fout);
void AD9959_Set_Amp(u16 amp);
void AD9959_Set_Pha(float Pout);
void AD9959_Ch(u16 Channel);
void AD9959_cos_sin(u8 cos_sin);

/***HIGH LAYER APPLICATION FUNCTION***/
void AD9959_Single_Output(u8 Channel,u32 Fout,float Pout,u16 amp);
void AD9959__Sweep_Fre(u8 Channel,u32 FreS,u32 FreE,float FTstep,float RTstep,u32 FFstep,u32 RFstep,u8 DWELL);
void AD9959__Sweep_Amp(u8 Channel,u16 ampS,u32 ampE,float FTstep,float RTstep,u32 FFstep,u32 RFstep,u8 DWELL);
void AD9959__Sweep_Phase(u8 Channel,u16 phaseS,u32 phaseE,float FTstep,float RTstep,u32 FFstep,u32 RFstep,u8 DWELL);
void AD9959_IOQ_Output(u8 CHI,u8 CHO,u8 CHQ,u32 Fout,u16 amp);

/**
example:
	AD9959_Init();
  AD9959_Single_Output(2,3000,0,1000);//3000hz 1000amp sin function
**/

/**
example:
	AD9959_Init();
  AD9959_Single_Output(2,3000,0,1000);//3000hz 1000amp sin function
	/.../
	AD9959_Ch(2);
	AD9959_Set_Amp(500);
	AD9959_IO_UpDate();
**/




#endif

















