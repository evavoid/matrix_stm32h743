#include "adf4351.h"

#define PFD 20

u16 ADF4351_INT =0;
u16 ADF4351_FRAC=0;
u16 ADF4351_MOD =0;
u16 ADF4351_Div =16;

static u32 value_to_reg[6]={0,0,0,0,0,0};

void ADF_Output_GPIOInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitStructure.Pin   =  GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStructure.Mode  =  GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed =  GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Pull  =  GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void ADF_Input_GPIOInit(void)
{
	/* Read function may have some problems, it can not and write function, 
	so do not initialize the input IO first.*/
	GPIO_InitTypeDef  GPIO_InitStructure;	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitStructure.Pin= GPIO_PIN_4;
	GPIO_InitStructure.Speed =  GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Mode  =  GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void delay (int length)
{
	while (length >0)
    	length--;
}
/**********对ADF4351进行读和写**************/
void WriteToADF4351(u8 count, u8 *buf)
{
	u8 ValueToWrite = 0;
	u8 i = 0;
	u8 j = 0;
	
//	ADF_Output_GPIOInit();
	
	ADF4351_CE(1);
	delay_us(1);
	ADF4351_CLK(0);
	ADF4351_LE(0);
	delay_us(1);
	
	for(i = count; i>0; i--)
	{
		ValueToWrite = *(buf+i-1);
		for(j=0; j<8; j++)
		{
			if(0x80 == (ValueToWrite & 0x80))
			{
				ADF4351_OUTPUT_DATA(1);
			}
			else
			{
				ADF4351_OUTPUT_DATA(0);
			}
			delay_us(1);
			ADF4351_CLK(1);
			delay_us(1);
			ValueToWrite <<= 1;
			ADF4351_CLK(0);	
		}
	}
	ADF4351_OUTPUT_DATA(0);
	delay_us(1);
	ADF4351_LE(1);
	delay_us(1);
	ADF4351_LE(0);
}


void ReadToADF4351(u8 count, u8 *buf)
{
	u8 i = 0;
	u8 j = 0;
	u8 iTemp = 0;
	u8 RotateData = 0;
	
//	ADF_Input_GPIOInit();
	ADF4351_CE(1);
	delay_us(1);
	ADF4351_CLK(0);
	ADF4351_LE(0);
	delay_us(1);
	
	for(i = count; i>0; i--)
	{
		for(j = 0; j<8; j++)
		{
			RotateData <<=1;
			delay_us(1);
			iTemp = ADF4351_INPUT_DATA;
			ADF4351_CLK(1);
			if(0x01 == (iTemp&0x01))
			{
				RotateData |= 1;
			}
			delay_us(1);
			ADF4351_CLK(0);
		}
		*(buf+i-1) = RotateData;
	}
	delay_us(1);
	ADF4351_LE(1);
	delay_us(1);
	ADF4351_LE(1);
}

/******以下是对ADF4351芯片的初始化***********/
void ADF4351Init(void)
{
	/* Read function may have some problems, it can not and write function, 
	so do not initialize the input IO first.*/
//	u8 buf[4] = {0,0,0,0};
	
//	//控制寄存器5
//	buf[3] = 0x00;				//此处配置固定输出265M
//	buf[2] = 0x58;
//	buf[1] = 0x00;				//write communication register 0x00580005 to control the progress 
// 	buf[0] = 0x05;				//to write Register 5 to set digital lock detector
//	WriteToADF4351(4,buf);		

//	//控制寄存器4
//	buf[3] = 0x00;
//	buf[2] = 0xAA;				//(DB23=1)The signal is taken from the VCO directly;(DB22-20:4H)the RF divider is 16;(DB19-12:50H)R is 80
//	buf[1] = 0x01;				//(DB11=0)VCO powerd up;
// 	buf[0] = 0xFC;				//(DB5=1)RF output is enabled;(DB4-3=3H)Output power level is 5
//	WriteToADF4351(4,buf);		

//	//控制寄存器3
//	buf[3] = 0x00;
//	buf[2] = 0x00;
//	buf[1] = 0x04;				//(DB14-3:96H)clock divider value is 150.
// 	buf[0] = 0xB3;
//	WriteToADF4351(4,buf);	

//  //控制寄存器2
//	buf[3] = 0x00;
//	buf[2] = 0x00;				//(DB6=1)set PD polarity is positive;(DB7=1)LDP is 6nS;
//	buf[1] = 0x4E;				//(DB8=0)enable fractional-N digital lock detect;
// 	buf[0] = 0x42;				//(DB12-9:7H)set Icp 2.50 mA;
//	WriteToADF4351(4,buf);		//(DB23-14:1H)R counter is 1

//	//控制寄存器1
//	buf[3] = 0x08;
//	buf[2] = 0x00;
//	buf[1] = 0x80;			   //(DB14-3:6H)MOD counter is 6;
// 	buf[0] = 0x11;			   //(DB26-15:6H)PHASE word is 1,neither the phase resync 
//	WriteToADF4351(4,buf);	   //nor the spurious optimization functions are being used
//							   //(DB27=1)prescaler value is 8/9
//	
//	//控制寄存器0
//	buf[3] = 0x00;
//	buf[2] = 0x64;
//	buf[1] = 0x00;
// 	buf[0] = 0x00;				//(DB14-3:0H)FRAC value is 0;
//	WriteToADF4351(4,buf);		//(DB30-15:140H)INT value is 320;	

	ADF_Output_GPIOInit();
	value_to_reg[5] = 0x580005;
	value_to_reg[4] = 0xCA01FC;
	value_to_reg[3] = 0x4B3;
	value_to_reg[2] = 0x4E42;
	value_to_reg[1] = 0x8008011;
	value_to_reg[0] = 0x6A0000;
  ADF4351Reset();
}

void WriteToADF4351_Reg(u32 value_reg, u8 reg)
{
	u8 buf[4] = {0,0,0,0};
	value_reg = value_reg | reg;
	value_to_reg[reg] = value_reg;
	buf[3] = value_reg>>3*8;				
	buf[2] = value_reg>>2*8;
	buf[1] = value_reg>>1*8;				
 	buf[0] = value_reg;				
	WriteToADF4351(4,buf);		
}

void ADF4351Reset(void)
{
	WriteToADF4351_Reg(value_to_reg[5], 0x05);
	WriteToADF4351_Reg(value_to_reg[4], 0x04);
	WriteToADF4351_Reg(value_to_reg[3], 0x03);
	WriteToADF4351_Reg(value_to_reg[2], 0x02);
	WriteToADF4351_Reg(value_to_reg[1], 0x01);
	WriteToADF4351_Reg(value_to_reg[0], 0x00);
}

void Aux_Output(u8 mode)
{
	if(mode == 1)	value_to_reg[4] |= (1<<8);	//赋值
	if(mode == 0)	value_to_reg[4] &= ~(1<<8);//清除该位
	ADF4351Reset();
}

void Dbm_Output(u8 mode)
{
	value_to_reg[4] &= ~(0x3<<3);//清除该位
	if(mode == 3)	value_to_reg[4] |= (0x3<<3);	//+5dBm
	if(mode == 2)	value_to_reg[4] |= (0x2<<3);	//+2dBm
	if(mode == 1)	value_to_reg[4] |= (0x1<<3);	//-1dBm
	if(mode == 0)	value_to_reg[4] |= (0x0<<3); //-4dBm
	ADF4351Reset();
}
void Prescaler(u8 mode)
{
	value_to_reg[1] &= ~(1<<27);//清除该位
	if(mode == 1)	value_to_reg[1] |= (1<<27);	//8/9
	if(mode == 0)	value_to_reg[1] |= (0<<27); //4/5
	ADF4351Reset();
}

void Freq_Output(u32 Freqency,int channel_spacing)
{
//	u8 div_Judge = 0;
	double N_Judge = 0;
	double FRAC_MOD_Judge = 0;
	
//	div_Judge = ceil((float)Freqency/68750000);
//	ADF4351_Div = 64 / pow(2,div_Judge-1);
	
	if((Freqency>=  35000)&&(Freqency<   68750))	ADF4351_Div = 64;
	if((Freqency>=  68750)&&(Freqency<  137500))	ADF4351_Div = 32;
	if((Freqency>= 137500)&&(Freqency<  275000))	ADF4351_Div = 16;
	if((Freqency>= 275000)&&(Freqency<  550000))	ADF4351_Div =  8;
	if((Freqency>= 550000)&&(Freqency< 1100000))	ADF4351_Div =  4;
	if((Freqency>=1100000)&&(Freqency< 2200000))	ADF4351_Div =  2;
	if((Freqency>=2200000)&&(Freqency<=4400000))	ADF4351_Div =  1;
	N_Judge = Freqency * ADF4351_Div / 20000.0f ;
	ADF4351_INT = N_Judge;
	FRAC_MOD_Judge = N_Judge - ADF4351_INT;

//	if(channel_spacing==1000000) ADF4351_MOD =10;
//	if(channel_spacing==100000 ) ADF4351_MOD =50;
//	if(channel_spacing==10000  ) ADF4351_MOD =250;
//	if(channel_spacing==1000   ) ADF4351_MOD =1250;
//	if(channel_spacing==500    ) ADF4351_MOD =2500;
	ADF4351_MOD = 4095;
	ADF4351_FRAC = ADF4351_MOD * FRAC_MOD_Judge;
	
	if(FRAC_MOD_Judge == 0) 
	{
		ADF4351_FRAC=0;
		ADF4351_MOD =2;
	}
	
	value_to_reg[0] &= ~(0xFFFF<<15);//清除该位 INT
	value_to_reg[0] &= ~(0xFFF <<3);//清除该位	FRAC
	value_to_reg[1] &= ~(0xFFF <<3);//清除该位	MOD
	value_to_reg[4] &= ~(0x07 <<20);//清除该位	DIV
	
	switch(ADF4351_Div)
	{
		case 1:value_to_reg[4] |= (0x00<<20);break;	//div = 1
		case 2:value_to_reg[4] |= (0x01<<20);break;	//div = 2
		case 4:value_to_reg[4] |= (0x02<<20);break;	//div = 4
		case 8:value_to_reg[4] |= (0x03<<20);break;	//div = 8
		case 16:value_to_reg[4] |= (0x04<<20);break;	//div = 16
		case 32:value_to_reg[4] |= (0x05<<20);break;	//div = 32
		case 64:value_to_reg[4] |= (0x06<<20);break;	//div = 64
	}
	
	value_to_reg[1] |= (ADF4351_MOD<<3); //MOD
	value_to_reg[0] |= (ADF4351_FRAC<<3); //FRAC
	value_to_reg[0] |= (ADF4351_INT<<15);//INT
	ADF4351Reset();
}


