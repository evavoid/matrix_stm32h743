#include "dac8563.h"

void DACDelay(vu32 nCount) 
{ 
  for(; nCount != 0; nCount--); 
} 

void DAC8563_Configuration_init(void) 
{ 
  GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
} 

void DAC8563_WRITE(u8 cmd,u16 data)
{
	u8 s=0;
  SYNC_H;
  DACDelay(0x01);
  SYNC_L;
  SCLK_L;
  for(s=0;s<8;s++)
  {
    if((cmd&0x80)==0x80){DIN_H;}
    else{DIN_L;}
    DACDelay(0x01);
    SCLK_H;
    DACDelay(0x01);
    cmd<<=1;
    SCLK_L;
    DACDelay(0x01);
  }
  for(s=0;s<16;s++)
  {
    if((data&0x8000)==0x8000){DIN_H;}
    else{DIN_L;}
    DACDelay(0x01);
    SCLK_H;
    DACDelay(0x01);
    data<<=1;
    SCLK_L;
    DACDelay(0x01);
  }
}

void dac8563_event(DAC8563_state_space *DAC_state)
{
	if(DAC_state->dac8563_state==prepare_state)
	{
		DAC8563_Configuration_init();
		CLR_L;
		LDAC_H;
		DAC8563_WRITE(0x28,0x0001);
		DAC8563_WRITE(0x20,0x0003);
		DAC8563_WRITE(0x38,0x0001);
		DAC_state->dac8563_state=run_state0;
		DAC_state->dac_data=0;
	}
	else if(DAC_state->dac8563_state==run_state0)
	{
		DAC8563_WRITE(0x18,DAC_state->dac_data);
    DAC8563_WRITE(0x19,DAC_state->dac_data);
    LDAC_L;
    DACDelay(0x01);
    LDAC_H;
    DAC_state->dac_data+=200;
	}
}




	/*sleep_state=1,prepare_state=2,run_state0=3,
					run_state1=4,run_state2=5,run_state3=6,
					run_state4=7,dead_state=8};
	*/


