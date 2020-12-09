#ifndef __SPI_H
#define __SPI_H
#include "matrixos.h"

#define DIN_H GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define DIN_L GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define SCLK_H GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define SCLK_L GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define CLR_H GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define CLR_L GPIO_ResetBits(GPIOB,GPIO_Pin_7)
#define SYNC_H GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define SYNC_L GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define LDAC_H GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define LDAC_L GPIO_ResetBits(GPIOB,GPIO_Pin_9)

typedef struct
{
  enum state dac8563_state;   
	u16 dac_data;
}DAC8563_state_space; //结构体名是LED_state_space

void DAC8563_Configuration_init(void);
void DAC8563_WRITE(u8 cmd,u16 data);
void DACDelay(vu32 nCount);///
void dac8563_event(DAC8563_state_space *DAC_state);



#endif

