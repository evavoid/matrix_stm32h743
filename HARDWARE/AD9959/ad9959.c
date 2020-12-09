#include "ad9959.h"

//寄存器长度
u8 Reg_Len[25]={1,3,2,3,4,2,3,2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4};		//对应不同寄存器位数,1*8,2*8,3*8,或者4*8

void AD9959GPIOInit()
{
	GPIO_InitTypeDef  GPIO_Initure;
 	   
    __HAL_RCC_GPIOI_CLK_ENABLE();					//开启GPIOI时钟
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1;			                //PI0.1
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		                //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;         			                //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	                //高速
    HAL_GPIO_Init(GPIOI,&GPIO_Initure);     		                //初始化GPIOI.0
    
    GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;   //PI4,5,6,7
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		                //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;         			                //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	                //高速
    HAL_GPIO_Init(GPIOI,&GPIO_Initure);     		                //初始化GPIOI.4,5,6,7
}

void AD9959_Start()
{
    AD9959_CS(1);//AD9959_CS_H;
    delay_us(20);
    AD9959_CS(0);//AD9959_CS_L;
    delay_us(20);
} 

void AD9959_Reset()
{
	AD9959_RST(0);//AD9959_RST_L;
	delay_us(20);
	AD9959_RST(1);//AD9959_RST_H;
	delay_us(20);
	AD9959_RST(0);//AD9959_RST_L;
}

/*
**********************************************************************
*每次对寄存器操作之后必须执行此段代码才能够将数据从缓存区写入寄存器
**********************************************************************
*/
void AD9959_IO_UpDate()
{
	AD9959_UP(0);//AD9959_UP_L;
    delay_us(20);
	AD9959_UP(1);//AD9959_UP_H;
	delay_us(20);
	AD9959_UP(0);//AD9959_UP_L;
	delay_us(20);
	AD9959_UP(1);//AD9959_UP_H;
		
}


void AD9959_Init()
{
  AD9959GPIOInit();
  AD9959_Start();                        //CS置地，使能DDS
  AD9959_Reset();                        //给DDS复位
  AD9959_WRrg(CSR,0x00);       //配置CSR：通道选择，串行通信模式选择Single-bit serial mode (2-wire mode)
  AD9959_WRrg(FR1,0xD00000);   //配置FR1：系统时钟大于225M,PLL：20,75uA
  AD9959_WRrg(FR2,0x0000);     //配置FR2 
  AD9959_WRrg(CFR,0x000300);    //配置CFR,9.455mA电流
  AD9959_IO_UpDate();
}




/*********************************************************************************
向AD9959发送一个字节数据
*********************************************************************************/

void AD9959_WByte(unsigned char byte)   
{
   u8 bit_ctr;   
   for(bit_ctr=0;bit_ctr<8;bit_ctr++)   //output 8-bit
   {
       if(byte&0x80)  
          AD9959_SDIO0(1);//AD9959_SDIO0_H;
       else
          AD9959_SDIO0(0);//AD9959_SDIO0_L; 
       AD9959_SCK(1);//AD9959_SCK_H;
       delay_us(10);
       byte=(byte<<1);
       AD9959_SCK(0);//AD9959_SCK_L;
       delay_us(10);
    }   
}


void AD9959_WRrg(u8 reg, u32 data)
{
  u8 i, nbyte;
  nbyte= Reg_Len[reg];
	
	#if (!WRRG_OPEN)
		AD9959_SDIO3(0);//AD9959_SDIO3_L;//同步d
	#endif	
	
  delay_us(10);
  AD9959_CS(0);//AD9959_CS_L;
  delay_us(10);
  AD9959_WByte(reg);
  for(i=0;i<nbyte;i++)
    AD9959_WByte((data>>((nbyte-1-i)*8))&0xFF);
  delay_us(10);
  AD9959_CS(1);//AD9959_CS_H;
  delay_us(10);
	
	#if (!WRRG_OPEN)
		AD9959_SDIO3(1);//AD9959_SDIO3_H;
	#endif
	
  delay_us(10);
}


//设置频率，32bit精度的Fout:0~(2^32-1),控制输出频率按步进均匀变化
//输出频率从0到100多MHz连续可调
void AD9959_Set_Fre(u32 Fout)
{
  u32 FTW;
  FTW = (u32)( Fout * FRE_REF ); 
  AD9959_WRrg(0x04,FTW);
}

//设置相位，14bit精度的Phase:0~(2^14-1),控制4个通道的输出任意相位步进均匀变化
//输出相位差可在0~360度内可调
void AD9959_Set_Pha(float Pout)
{
  int POW;
  POW = (int)( Pout * POW_REF );
  AD9959_WRrg(0x05,POW);
}


//设置输出幅度，10bit精度的amp:0~1023,控制输出幅度按步进均匀变化
//输出幅度约在0~10V峰值可调
void AD9959_Set_Amp(u16 amp)															  
{
  u32 dat;
  dat = 0x00001000+amp;	  //Bit 12??,Amplitude multiplier enable
  AD9959_WRrg(0x06,dat);
}

//通道选择：0 1  2  3分别对应不同输出通道
void AD9959_Ch(u16 Channel)
{
  switch(Channel)
  {
  case 0:AD9959_WRrg(0x00,0x10);break;
  case 1:AD9959_WRrg(0x00,0x20);break;
  case 2:AD9959_WRrg(0x00,0x40);break;
  case 3:AD9959_WRrg(0x00,0x80);break;
  case 10:AD9959_WRrg(0x00,0x30);break;
  case 20:AD9959_WRrg(0x00,0x50);break;
  case 30:AD9959_WRrg(0x00,0x90);break;
  case 21:AD9959_WRrg(0x00,0x60);break;
  case 31:AD9959_WRrg(0x00,0xA0);break;
  case 32:AD9959_WRrg(0x00,0xC0);break;
  case 321:AD9959_WRrg(0x00,0xE0);break;
  case 310:AD9959_WRrg(0x00,0xB0);break;
  case 210:AD9959_WRrg(0x00,0x70);break;
  case 320:AD9959_WRrg(0x00,0xD0);break;
  case 3210:AD9959_WRrg(0x00,0xF0);break;
  default:break;
  }
}
//正交信号
void AD9959_IOQ_Output(u8 CHI,u8 CHO,u8 CHQ,u32 Fout,u16 amp)
{
		AD9959_SDIO3(0);//AD9959_SDIO3_L;//同步
		
		AD9959_Ch(CHI);
		AD9959_Set_Fre(Fout);
    AD9959_Set_Pha(0);
    AD9959_Set_Amp(amp);
    AD9959_cos_sin(0);//cos I
		
		AD9959_Ch(CHO);
		AD9959_Set_Fre(Fout);
    AD9959_Set_Pha(0);
    AD9959_Set_Amp(amp);
    AD9959_cos_sin(1);//sin O
	
		AD9959_Ch(CHQ);
		AD9959_Set_Fre(Fout);
    AD9959_Set_Pha(0);
    AD9959_Set_Amp(amp);
    AD9959_cos_sin(1);//sin Q

		AD9959_IO_UpDate();
    AD9959_SDIO3(1);//AD9959_SDIO3_H;//同步
}

//改变输出相位cos/sin
void AD9959_cos_sin(u8 cos_sin)
{
    switch(cos_sin)
    {
        case 0: AD9959_WRrg(CFR,0x000300);//cos
            break;
        case 1: AD9959_WRrg(CFR,0x000301);//sin
            break;
    }
}


//单个波形输出模式，可设置单个波形输出的通道，频率，幅度，以及输出的初始相位
void AD9959_Single_Output(u8 Channel,u32 Fout,float Pout,u16 amp)
{
	AD9959_Ch(Channel);
    AD9959_Set_Fre(Fout);
    AD9959_Set_Pha(Pout);
    AD9959_Set_Amp(amp);
	AD9959_IO_UpDate();
}

/*
***********************************************************************************
*扫描设置函数：输入参数：Channel  通道选择
*                        FreS     频率开始值        FreE    频率结束值                     
*                        FTstep   下降扫描时间      RTstep  上升扫描时间
*                        FFstep   下降扫描频率      RFstep  上升扫描频率
*                        DWELL     1 使能 0 不能
***********************************************************************************
*/
void AD9959__Sweep_Fre(u8 Channel,u32 FreS,u32 FreE,float FTstep,float RTstep,u32 FFstep,u32 RFstep,u8 DWELL)
{  
   u32 FTW0,CW_1,RDW0,FDW0;
   u16 RSRR0,FSRR0;
   FTW0 = (u32)( FreS * FRE_REF ); 
   CW_1 = (u32)( FreE * FRE_REF ); 
   RDW0 = (u32)( RFstep * FRE_REF ); 
   FDW0 = (u32)( FFstep * FRE_REF ); 
   RSRR0 = (u16)(RTstep*0.4f);
   FSRR0 = (u16)(FTstep*0.4f);
   AD9959_Ch(Channel); 
   AD9959_WRrg(0x06,0x0003ff);
   if(DWELL)
     //AD9959_WRrg(0x03,0x80C300);    // dwell enable[23:22] 01 amp sweep 10 fre sweep 11 phase sweep
     AD9959_WRrg(0x03,0x80c314);//auto clear accumulator
   else
   	 AD9959_WRrg(0x03,0x804314);
     //AD9959_WRrg(0x03,0x804300);   //配置CFR:频率扫频，扫频使能。DWELL  DISABLED
   AD9959_WRrg(0x01,0xD00000); 
   AD9959_WRrg(0x04,FTW0);
   AD9959_WRrg(0x0A,CW_1);
   AD9959_WRrg(0x08,RDW0);
   AD9959_WRrg(0x09,FDW0);
   AD9959_WRrg(0x07,FSRR0*256+RSRR0);

   AD9959_IO_UpDate();
}

void AD9959__Sweep_Amp(u8 Channel,u16 ampS,u32 ampE,float FTstep,float RTstep,u32 FFstep,u32 RFstep,u8 DWELL)
{
	 u32 RDW0,FDW0;
	 u32 SDW,EDW;
   u16 RSRR0,FSRR0;
   SDW = ampS;
   EDW = ampE<<22;
   RDW0 = (u32)RFstep; 
   FDW0 = (u32)FFstep; 
   RSRR0 = (u16)(RTstep*0.4f);
   FSRR0 = (u16)(FTstep*0.4f);
   AD9959_Ch(Channel); 
   AD9959_WRrg(0x06,0x0003ff);
   if(DWELL)
     //AD9959_WRrg(0x03,0x80C300);    // dwell enable[23:22] 01 amp sweep 10 fre sweep 11 phase sweep
     AD9959_WRrg(0x03,0x40c314);//auto clear accumulator
   else
   	 AD9959_WRrg(0x03,0x404314);
     //AD9959_WRrg(0x03,0x804300);   
   AD9959_WRrg(0x01,0xD00000); 
   AD9959_WRrg(0x06,SDW);
   AD9959_WRrg(0x0A,EDW);
   AD9959_WRrg(0x08,RDW0);
   AD9959_WRrg(0x09,FDW0);
   AD9959_WRrg(0x07,FSRR0*256+RSRR0);
}

void AD9959_Sweep_Phase(u8 Channel,u16 phaseS,u32 phaseE,float FTstep,float RTstep,u32 FFstep,u32 RFstep,u8 DWELL)
{
	 u32 RDW0,FDW0;
	 u32 SDW,EDW;
   u16 RSRR0,FSRR0;
   SDW = phaseS;
   EDW = phaseE<<18;
   RDW0 = (u32)(RFstep * POW_REF); 
   FDW0 = (u32)(FFstep * POW_REF); 
   RSRR0 = (u16)(RTstep*0.4f);
   FSRR0 = (u16)(FTstep*0.4f);
   AD9959_Ch(Channel); 
   AD9959_WRrg(0x06,0x0003ff);
   if(DWELL)
     //AD9959_WRrg(0x03,0x80C300);    // dwell enable[23:22] 01 amp sweep 10 fre sweep 11 phase sweep
     AD9959_WRrg(0x03,0xc0c314);//auto clear accumulator
   else
   	 AD9959_WRrg(0x03,0xc04314);
     //AD9959_WRrg(0x03,0x804300);   //??CFR:????,????.DWELL DISABLED
   AD9959_WRrg(0x01,0xD00000); 
   AD9959_WRrg(0x05,SDW);
   AD9959_WRrg(0x0A,EDW);
   AD9959_WRrg(0x08,RDW0);
   AD9959_WRrg(0x09,FDW0);
   AD9959_WRrg(0x07,FSRR0*256+RSRR0);
}



