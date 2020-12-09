#ifndef __ADF4351_H
#define __ADF4351_H
#include "matrixos.h"

//LE//加载使能
//CE//芯片使能
#define ADF4351_CLK(n)          		(n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_RESET))
#define ADF4351_OUTPUT_DATA(n)      (n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET))
#define ADF4351_LE(n)               (n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET))
#define ADF4351_CE(n)               (n?HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET))
#define ADF4351_INPUT_DATA          HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4)//串行数据输出（到单片机）

//Underlying communication function,It only decides what information to send, not where to send and what to send.
void ReadToADF4351(u8 count, u8 *buf);
void WriteToADF4351(u8 count, u8 *buf);

//A middle-level communication function that writes specific data to a register
void WriteToADF4351_Reg(u32 value_to_reg, u8 reg);
void ADF4351Reset(void);//Update the value of the memory area to the chip.

//Application function
void Aux_Output(u8 mode);
void Dbm_Output(u8 mode);
void Prescaler(u8 mode);
void Freq_Output(u32 Freqency,int channel_spacing);

//Init function
	/* Read function may have some problems, it can not and write function, 
	so do not initialize the input IO first.*/
void ADF4351Init(void);
void ADF_Output_GPIOInit(void);
//void ADF_Input_GPIOInit(void);
/**example:
	ADF4351Init();
	Freq_Output(***,***);
	//If you want to modify parameters at runtime,here are two ways.
	//1.Prescaler(**);Dbm_Output(**);Aux_Output(**);
	//2.ADF4351_INT=**;value_to_reg[3]=**;ADF4351Reset();
**/
#endif

