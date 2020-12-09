#include "ad5542a.h"

void AD5542A_init(void)
{
	GPIO_InitTypeDef  GPIO_Initure;
  __HAL_RCC_GPIOE_CLK_ENABLE();					//开启GPIOE时钟
	
	GPIO_Initure.Pin=AD5542A_CSpin|AD5542A_CLKpin|AD5542A_DATApin|AD5542A_LDACpin|AD5542A_CLRpin;			                //PI0.1
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		                //推挽输出
	GPIO_Initure.Pull=GPIO_NOPULL;         			                //上拉
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	                //高速
	HAL_GPIO_Init(AD5542A_PORT,&GPIO_Initure);

	AD5542A_CS(1);
	AD5542A_CLK(1);
	AD5542A_LDAC(1);
	AD5542A_CLR(1);
}

//恢复（清除）ADC输出至半参考电压
void AD5542A_CLRdata(void)
{
	AD5542A_CLR(1);
	delay_us(1);
	AD5542A_CLR(0);
	delay_us(1);
	AD5542A_CLR(1);
}

//设定输出电压，参数范围+-2.5，单位V
void AD5542A_write_data(double vol)
{
	u16 data=0;
	data=(vol+2.5)/5*65536;
	if(vol==2.5)
		data=65535;
	
	AD5542A_CS(1);
	AD5542A_CLK(0);
	AD5542A_LDAC(1);
	AD5542A_CLR(1);
	delay_us(1);
	AD5542A_CLK(1);
	delay_us(1);
	AD5542A_CS(0);
	delay_us(1);
	for(u8 i=0; i<16; i++)
	{
		AD5542A_CLK(0);
		AD5542A_DATA(data<<i&0x8000);
		delay_us(1);
		AD5542A_CLK(1);
		delay_us(1);		
	}
	AD5542A_CS(1);
	delay_us(1);
	AD5542A_LDAC(0);
	delay_us(1);
	AD5542A_LDAC(1);
}

