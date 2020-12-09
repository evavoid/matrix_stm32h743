#include "hmc830.h"

const u32 REG0 = 0x400A7975;    //X  RD a5-a0 d23-d0   0 1 00_0000 0000 1010 0111 1001 0111 0101
const u32 REG1 = 0x01000000;//康威上最后一位是2
const u32 REG2 = 0x02000000;//康威上最后一位是1
const u32 REG3 = 0x03000000;
const u32 REG4 = 0x04000000;
const u32 REG5 = 0x05000000;
const u32 REG6 = 0x06000000;
const u32 REG7 = 0x07000000;
const u32 REG8 = 0x08000000;
const u32 REG9 = 0x09000000;
const u32 REGa = 0x0a000000;
const u32 REGb = 0x0b000000;
const u32 REGc = 0x0c000000;
const u32 REGd = 0x0d000000;
const u32 REGe = 0x0e000000;
const u32 REGf = 0x0f000000;

const u32 READ = 0x40000000;
const u32 WRITE = 0X00000000;

void HMC830_Write(u32 dat)
{
	u16 i,j;
	
	SCK(0);
	SEN(1);
	dat<<=1;//remove first bit，具体看协议，不要删
	for(j=0;j<4;j++)
	{
		for(i=0;i<8;i++)//write data
		{
			if(dat & 0x80000000)
				SDI(1);
			else
				SDI(0);
			dat<<=1;
			SCK(0);
			delay_us(50);
			SCK(1);
			delay_us(50);
			SCK(0);
			delay_us(50);
		}
	}
	SEN(0);
	delay_us(50);
	SEN(1);
	delay_us(50);
	SEN(0);
	delay_us(50);
}
	
u32 HMC830_Read(u32 dat)
{
	u16 i;
	u32 dat_read=0x00000000;//return value
	
	SCK(0);
	SEN(1);
	delay_us(50);
	dat<<=1;//remove first data
	for(i=0;i<7;i++)
	{
		if(dat & 0x80000000)
			SDI(1);
		else
			SDI(0);
		dat<<=1;
		
		SCK(1);
		delay_us(50);
		SCK(0);
		delay_us(50);		
		if(READ_SDO)
			dat_read |= 0x00000001;
		else
			dat_read &= ~0x00000001;
		dat_read<<=1;
	}
	
	delay_us(50);
	
	for(i=0;i<24;i++)
	{
		SCK(1);
		delay_us(50);
		SCK(0);
		delay_us(50);	
		if(READ_SDO)
			dat_read |= 0x00000001;
		else
			dat_read &= ~0x00000001;
		if(i<23)
			dat_read<<=1;
	}
	SEN(0);
	delay_us(50);
	SEN(1);
	delay_us(50);
	SEN(0);
	delay_us(50);
	
	return dat_read;
}
#if !(MODE_FRA)
static void HMC830_int(void)
{
//初始化后输出100MHz
//HMC830_Write(REG0|0x20|WRITE); 不能用可以把这句加上试试
	HMC830_Write(WRITE|REG1|0x2);
//CEN is controlled by host
	HMC830_Write(WRITE|REG2|0x2);
//R devider = 2，本振分频系数，适用于100M晶振
	HMC830_Write(WRITE|REG5|0x6A10);
//[3:1] = 000 HMC830LP6GE
//[7:4] = 0010 VCO_Reg02h
//[12:8] = 001000 vco output devider
//[14:13] = 11, max gain
	HMC830_Write(WRITE|REG5|0x898);
//single output RF_N
	HMC830_Write(WRITE|REG5|0x0);
//enable all bands
	HMC830_Write(WRITE|REG6|0x303ca);
//PFD:int ,mode B	来自官方调试器
	HMC830_Write(WRITE|REG7|0x14D);
//lock detect window   来自官方调试器
	HMC830_Write(WRITE|REG9|0x153fff);
//PFD gain  来自官方调试器
	HMC830_Write(WRITE|REGa|0x2046);
//来自官方/康威
	HMC830_Write(WRITE|REGb|0x7c061);
//
	HMC830_Write(WRITE|REGf|0x81);
//locked_sign output  锁定标志开启
	HMC830_Write(WRITE|REG3|0x28);
//Ndevider=53  VCO分频（N分频）
}
#endif
#if MODE_FRA
static void HMC830_fra(void)
{
	//初始化后输出25MHz
	//HMC830_Write(REG0|0x20|WRITE); 不能用可以把这句加上试试
	HMC830_Write(WRITE|REG1|0x2);
//CEN is controlled by host
	HMC830_Write(WRITE|REG2|0x2);
//R devider = 2，本振分频系数，适用于100M晶振
	HMC830_Write(WRITE|REG5|0x7e90);
//[3:1] = 000 HMC830LP6GE
//[7:4] = 0010 VCO_Reg02h
//[12:8] = 001000 vco output devder
//[14:13] = 11, max gain
	HMC830_Write(WRITE|REG5|0x898);
//single output RF_N
	HMC830_Write(WRITE|REG5|0x0);
//enable all bands
	HMC830_Write(WRITE|REG6|0x30f4a);
//PFD:int ,mode B	来自官方调试器
	HMC830_Write(WRITE|REG7|0x14D);
//lock detect window   来自官方调试器
	HMC830_Write(WRITE|REG9|0x5cbfff);
//PFD gain  来自官方调试器
	HMC830_Write(WRITE|REGa|0x2046);
//来自官方/康威
	HMC830_Write(WRITE|REGb|0x7c061);
//
	HMC830_Write(WRITE|REGf|0x81);
//locked_sign output  锁定标志开启
	HMC830_Write(WRITE|REG3|0x1e);
//Ndevider=53  VCO分频（N分频）
	HMC830_Write(WRITE|REG4|0x800000);
}
#endif

void HMC830_Init()
{
	/////////////////////
	////init IO ports////      Port_E Pin_2.3.4.5.6
	/////////////////////
	GPIO_InitTypeDef  GPIO_InitStructure;

 __HAL_RCC_GPIOE_CLK_ENABLE();
	//init SEN
	
	//init output pins
	GPIO_InitStructure.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
   GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStructure.Pull =GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	//init input pin
  GPIO_InitStructure.Pin = GPIO_PIN_2;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull =GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	CEN(0);
	SEN(1);
	delay_us(100);
	SCK(1);
	delay_us(300);
	CEN(1);
	SDI(1);
	delay_us(100);
	////init IO ports end////
	#if MODE_FRA
		HMC830_fra();
	#else
		HMC830_int();
	#endif
}

//频率设定整数分频，参数单位Hz，支持小数输入
u8 HMC830_freq_def(double freq_out_Hz)
{
	const double div_RF_test[33]={30,28,32,26,34,24,36,22,38,20,40,18,42,16,
													44,14,46,12,48,10,50,8,52,6,54,54,56,4,58,2,60, 1,62} ;
	double freq_div, div_N_test; 
	u32 flag_cal=0, i=0, div_RF, div_N;
	freq_div=freq_out_Hz/50000000;
	for(i=0; i < 33;i++)
	{
		div_N_test=freq_div*div_RF_test[i];
		if(div_N_test>=30&&div_N_test<=60)
		{
			flag_cal=1;
			div_RF=div_RF_test[i];
			break;
		}
	}
	if(flag_cal==0)
		return 1;
	div_N=(u32)(div_N_test+0.5);
	if(div_RF==1||div_RF==2)
	  HMC830_Write(WRITE|REG5|0xE010|(div_RF<<=7));
	else
		HMC830_Write(WRITE|REG5|0x6010|(div_RF<<=7));
	HMC830_Write(WRITE|REG5|0x0);
	HMC830_Write(WRITE|REG3|div_N);
	return 0;
}

//频率设定小数分频，参数单位Hz，支持小数输入
u8 HMC830_freq_def_fra(double freq_out_Hz)
{  
	u32 n,f,rf;
	u32 div_n,div_f,div_rf;
  freq_set(&n,&f,&rf,freq_out_Hz);
	
	div_n=(u32)(n+0.5);
	div_f=(u32)(f+0.5);
	div_rf=(u32)(rf+0.5);
	
	HMC830_Write(WRITE|REG5|0x6010|(div_rf<<=7));
	HMC830_Write(WRITE|REG5|0x0);
	HMC830_Write(WRITE|REG3|div_n);
	HMC830_Write(WRITE|REG4|div_f);
	return 0;
}

//频率设定，小数分频
//nint:整数
//decimal:小数
//rf_reg:输出分频参数
//frew_out_hz:输出频率
void freq_set(u32* nint,u32* decimal,u32* rf_reg,double frew_out_hz)
{
	if(frew_out_hz>3000000000||frew_out_hz<25000000)return;
	
	double ref_freq=frew_out_hz/50000000;
	double rfmin=1,rfmax=31;
	double rf=(rfmin+rfmax)/2;//分频参数，从1->31
	
	while(1)
	{
		if(ref_freq*rf*2<60&&ref_freq*rf*2>30)
			break;
		else if(ref_freq*rf*2>=60)
		{
			rfmax=(rf+0.5);
			rf=(u32)((rfmin+rfmax)/2+0.5);
		}
		else if(ref_freq*rf*2<=30)
		{
			rfmin=(rf+0.5);
			rf=(u32)((rfmin+rfmax)/2+0.5);
		}
	}
	*nint=(u32)(ref_freq*rf*2);
	*decimal=(ref_freq*rf*2-*nint)*16777216;
	*rf_reg=rf*2;
}


