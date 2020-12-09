#include "ad9957_eval.h"


/**
  @brief Pin initiation ³õÊ¼»¯¶Ë¿Ú
	
**/
void Init_AD9957_EVAL(void)
{
	/*µ×²ã³õÊ¼»¯*/
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();	

	GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;		
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		
	GPIO_Initure.Pull=GPIO_PULLUP;         			
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	
	HAL_GPIO_Init(GPIOE,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_5;
	GPIO_Initure.Mode=GPIO_MODE_INPUT;  		
	GPIO_Initure.Pull=GPIO_PULLUP;         			
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	
	HAL_GPIO_Init(GPIOE,&GPIO_Initure);

	GPIO_Initure.Pin=GPIO_PIN_5;
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		
	GPIO_Initure.Pull=GPIO_PULLUP;         			
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	/*¼Ä´æÆ÷ÅäÖÃ*/
	unsigned int CFR1, CFR2, CFR3 ,CFR4;
	unsigned int inpbuff[4]={0x00,0x00,0x00,0x00};
	
	//À´×ÔÆÀ¹ÀÈí¼þ£¬ºóÃæÄÇÐ©ÊäÈë²ÎÊý¶¼¿ÉÒÔÖ±½ÓÉè³É0
	//»¹Ã»ÓÐÐ´»Ø¶Á¼Ä´æÆ÷µÄº¯Êý£¬Î´·â×°¡£
	CFR1 = 0x01400000;//CFR1 = SINE_OUTPUT|EXT_PDWN_CTRL|SDIO_IP|SINGLE_TONE;
	CFR2 = 0x00400820;//CFR2 = SYNC_CLK_EN|OFFSET_BINARY|PDCLK_ENABLE;
	CFR3 = 0x1D3F4129;//CFR3 = LOW_OUT_CURRENT|REFCLK_INP_DIV_BYPASS|PLL_ENABLE|REFCLK_INP_DIV_RST;
	CFR4 = 0x00007F7F;//
	
	ControlFunctionReg1(WRITE,CFR1,inpbuff);
	delay_us(10);
	ControlFunctionReg2(WRITE,CFR2,0x00,inpbuff);
	delay_us(10);
	ControlFunctionReg3(WRITE,0x00,0x00,0x00,CFR3,inpbuff);
	delay_us(10);
	AuxDacControlReg(WRITE,CFR4,inpbuff);
	delay_us(10);
}

// porting from ADI ExampleCode

/**
	@brief: void IO_Update(void)
		--------- IO UPDATE------------
**/
void IO_Update(void)
{
	AD9957_IO_UPDATE(0);
	delay_us(20);
	AD9957_IO_UPDATE(1);
	delay_us(20);
}

void AD9957_WriteOneByte(u8 Data)
{
	u8 i;
	AD9957_CS(1);
	AD9957_CS(0);
		
	AD9957_SCLK(0);	//¿ÕÏÐÊ±µÍ
	
	delay_us(1);//¶à¸ö·¢ËÍµÄÊ±ºòÑÓ³ÙÒ»ÏÂ
	for(i = 0; i<8;i++)
	{
		if(Data & 0x80)
		{
			AD9957_SDIO(1);
		}
		else
		{
			AD9957_SDIO(0);
		}
		AD9957_SCLK(1);
		delay_us(1);
		Data<<=1;
		AD9957_SCLK(0);
		delay_us(1);
	}
}

void AD9957_WriteReg(unsigned int Data, unsigned int NumBytes)
{
	u8 i;
	u32 Data_to_write[4] = {0x00,0x00,0x00,0x00};
	Data_to_write[0] = Data & 0xFF000000;
	Data_to_write[0] >>= 24;
	Data_to_write[1] = Data & 0x00FF0000;
	Data_to_write[1] >>= 16;
	Data_to_write[2] = Data & 0x0000FF00;
	Data_to_write[2] >>= 8;
	Data_to_write[3] = Data & 0x000000FF;
	
	if(NumBytes == 1)
	{
		AD9957_WriteOneByte(Data_to_write[3]);
	}
	else
	{
		for(i=0;i<NumBytes;i++)
		{
			AD9957_WriteOneByte(Data_to_write[i]);
		}
	}
}
uint8_t AD9957_ReadOneByte(void)
{
	uint32_t i;
	uint8_t Data=0;

	AD9957_CS(0);
	
	AD9957_SCLK(0);
		delay_us(1);
	
		for(i=0;i<8;i++)
	{

	  AD9957_SCLK(1);
		delay_us(1);
		
		AD9957_SCLK(0);
		delay_us(1);	
		
		if(AD9957_SDO_READ)
			Data |= 0x00000001;
		else
			Data &= ~0x00000001;
		if(i<7)
			Data<<=1;
		
	}
	
	AD9957_CS(1);
		return Data;
}
void AD9957_ReadReg(unsigned int ReadBuff[], unsigned int NumBytes)
{
	uint32_t i=0;
	uint8_t buff;
	
	for(i=0;i<NumBytes;i++)
	{
		*ReadBuff<<=8;
		buff=AD9957_ReadOneByte();
		*ReadBuff|=buff;
	}
	*ReadBuff>>=1;
	
//	for(i=0;i<NumBytes;i++)
//	{
//		AD9957_ReadOneByte(buff);
//		*ReadBuff|=*buff;
//	}
	
	
}

/**
	@brief: void ControlFunctionReg1(unsigned char mode, unsigned int Data, unsigned int InputBuff[])
		-------------------------- Control Function Register 1-------------------------------------
		@param mode: ( WRITE,READ )
								WRITE: Write to the register	
								READ: Read from register
		@param Data: (RAM_ENABLE|RAM_PLAYBACK_IQPATH|MANUAL_OSK_CONTROL|INVERSE_SINC_ENABLE|CLEAR_CCI|SINE_OUTPUT|ATCLR_PHSE_ACC|
										CLR_PHSE_ACC|ARR_IO_UP|OSK_ENABLE|AUTO_OSK|DIG_POW_DOWN|DAC_POW_DOWN|REFCLK_POW_DWN|AUX_DAC_POW_DWN|
										EXT_PDWN_CTRL|AUTO_PDWN_ENABLE|SDIO_IP|LSB_FIRST )
								
								RAM_ENABLE: Enable RAM, (disable - default in program)
								RAM_PLAYBACK_IQPATH: RAM playback data, (0x00 - default in program)
								MANUAL_OSK_CONTROL: Manual OSK Enable, (disable - default in program)
								INVERSE_SINC_ENABLE: Inverse SINC filter, (disable - default in program)
								CLEAR_CCI: CCI, (disable - default in program)
								SINE_OP: Sine output enable, (cosine output - default in program)
								ATCLR_PHSE_ACC: Autoclear phase accumulator, (normal operation - default in program)
								CLR_PHSE_ACC: Clear phase accumulator, (normal operation - default in program)
								ARR_IO_UP: Load ARR at IO update, (disable - default in program)
								OSK_EN: OSK enable, (disable - default in program)
								AUTO_OSK: Auto OSK, (disable - default in program
								DIG_POW_DOWN: Digital power down enable, (disable - default in program)
								DAC_POW_DOWN: DAC power down enable, (disable - default in program)
								REFCLK_POW_DWN: REFCLK power down, (disable - default in program)
								AUX_DAC_POW_DWN: Auxillary DAC power down, (disable - default in program)
								EXT_PDWN_CTRL: External power down control - assertion of the EXT_PWR_DWN pin affects fast recovery power-down,
																											(assertion of the EXT_PWR_DWN pin affects power-down -- default in program)
								AUTO_PDWN_ENABLE: Auto power down, (disable - default in program)
								SDIO_IP: SDIO to function as input only, (SDIO bidirectional - default in program)
								LSB_MODE: LSB first mode, (MSB mode - default in program)		

								Operating mode: ( QUADRATURE_MOD|SINGLE_TONE|INTERPOLATING_DAC )

		@param InputBuff[] = array containing data that is read from AD9957
**/
void ControlFunctionReg1(unsigned char mode, unsigned int Data, unsigned int InputBuff[])
{
	unsigned int Temp = 0x00000000;
	AD9957_CS(1);
//	AD9957_CS(0);
//	delay_us(1);
	if(mode == 0)
	{
		Data = Temp|Data;
		AD9957_WriteReg(0x00,1);
		AD9957_WriteReg(Data,4);
		IO_Update();
	}
	else
	{
		AD9957_WriteReg(0x80,1);
		AD9957_ReadReg(InputBuff,4);
		IO_Update();
	}
	delay_us(1);
	AD9957_CS(1);
	delay_us(2);
}


/**
	@brief: void ControlFunctionReg2(unsigned char mode, unsigned int Data, unsigned int IOUpdateRate, unsigned int InputBuff[])
		-------------------------- Control Function Register 2-------------------------------------
		@param mode: ( WRITE,READ )
								WRITE: Write to the register	
								READ: Read from register
		@param Data: ( BLACKFIN_MODE_EN|BLACKFIN_BIT_ORDER|BLACKFIN_EARLY_FRAME_EN|PROF_REG_ASF_SOURCE|INT_IO_UPDATE|SYNC_CLK_EN|EFFEC_FTW|
									 PDCLK_RATE|OFFSET_BINARY|PDCLK_ENABLE|PDCLK_INVERT|TX_INVERT|Q_FIRST_EN|DATA_ASSEM_HLD|SYNC_TIM_DIS )
								
									BLACKFIN_MODE_EN: Blackfin interface mode, (disable - default in program)
									BLACKFIN_BIT_ORDER: Blackfin bit order, (dual serial port (BFI) configured for MSB first operation (default)).
									BLACKFIN_EARLY_FRAME_EN: Blackfin Early Frame Sync, (the dual serial port (BFI) configured to be compatible with Blackfin late frame sync operation (default))
									PROF_REG_ASF_SOURCE: Enable Profile Registers as ASF Source, (amplitude scale factor bypassed (unity gain))
									INT_IO_UPDATE: Internal I/O Update Active, (serial I/O programming is synchronized with external assertion of the I/O_UPDATE pin,
																	which is configured as an input pin (default))
									SYNC_CLK_EN: SYNC_CLK Enable, (the SYNC_CLK pin is disabled; static Logic 0 output)
									EFFEC_FTW: Read Effective FTW, (a serial I/O port read operation of the FTW register reports the contents of the FTW register (default))
									PDCLK_RATE: PDCLK Rate Control, (PDCLK operates at the input data rate (default))
									OFFSET_BINARY: Data Format, (the data-words applied to Pin D<17:0> are expected to be coded as twos complement (default))
									PDCLK_ENABLE: PDCLK Enable, (the PDCLK pin is disabled and forced to a static Logic 0 state; the internal clock signal 
																continues to operate and provide timing to the data assembler)
									PDCLK_INVERT: PDCLK Invert, (normal PDCLK polarity; Q-data associated with Logic 1, I-data with Logic 0 (default))
									TX_INVERT: TxEnable Invert, (normal TxENABLE polarity; Logic 0 is standby, Logic 1 is transmit (default))
									Q_FIRST_EN: Q-First Data Pairing, (an I/Q data pair is delivered as I-data first, followed by Q-data (default))
									DATA_ASSEM_HLD: Data Assembler Hold Last Value, (when the TxENABLE pin is false, the data assembler ignores the input data and 
																	internally forces zeros on the baseband signal path (default))
									SYNC_TIM_DIS: Sync Timing Validation Disable
		@param IOUpdateRate: I/O Update Rate Control, (0x0 to 0x03)

		@param InputBuff[] = array containing data that is read from AD9957
**/
void ControlFunctionReg2(unsigned char mode, unsigned int Data, unsigned int IOUpdateRate, unsigned int InputBuff[])
{
	unsigned int var = 0x00000000;
	var = IOUpdateRate;
	var <<= 14;
	Data |= Data|var;
	AD9957_CS(1);
	if(mode == 0)
	{
		AD9957_WriteReg(0x01,1);
		AD9957_WriteReg(Data,4);
		IO_Update();
	}
	else
	{
		AD9957_WriteReg(0x81,1);
		AD9957_ReadReg(InputBuff,4);
		IO_Update();
	}
	delay_us(1);
	AD9957_CS(1);
	delay_us(2);
}


/**
	@brief: void ControlFunctionReg3(unsigned char mode, unsigned int VCOSel, unsigned int CurrSett, unsigned int NDivide, unsigned int Data, unsigned int InputBuff[])
		-------------------------- Control Function Register 3-------------------------------------
		@param mode: ( WRITE,READ )
								WRITE: Write to the register	
								READ: Read from register
		@param VCOSel: VCO SEL, (0x00 to 0x07)
		@param CurrSett: Icp settings, (0x00 to 0x07)
		@param NDivide: N divide value, (0x0C to 0x7F)
		@param Data: ( REFCLK_INP_DIV_BYPASS|REFCLK_INP_DIV_RST|PLL_ENABLE )
								
									REFCLK_INP_DIV_BYPASS: REFCLK Input Divider Bypass, (input divider is selected (default))
									REFCLK_INP_DIV_RST: REFCLK Input Divider ResetB, (input divider is reset)
									PLL_ENABLE: PLL Enable, (REFCLK PLL bypassed (default))

								DRV0: ( LOW_OUT_CURRENT|MEDIUM_OUT_CURRENT|HIGH_OUT_CURRENT )

		@param InputBuff[] = array containing data that is read from AD9957
**/
void ControlFunctionReg3(unsigned char mode, unsigned int VCOSel, unsigned int CurrSett, unsigned int NDivide, unsigned int Data, unsigned int InputBuff[])
{
	unsigned int Temp = 0x00000000, var = 0x00000000;
	var = VCOSel;
	var <<= 24;
	Temp |= var;
	var = 0x00;
	var = CurrSett;
	var <<= 19;
	Temp |= var;
	var = 0x00;
	var = NDivide;
	var <<= 1;
	Temp |= var|Data;
	AD9957_CS(1);
//	Delay1(5);
	if(mode == 0)
	{
		AD9957_WriteReg(0x02,1);
		AD9957_WriteReg(Temp,4);
		IO_Update();
	}
	else
	{
//		AD9957_WriteReg(0x82,1);
//		ReadReg(InputBuff,4);
//		//IO_Update();
	}
	delay_us(5);
	AD9957_CS(1);
	delay_us(2);
}


/**
	@brief: void AuxDacControlReg(unsigned char mode, unsigned int FSC, unsigned int InputBuff[])
		-------------------------- Auxillary DAC control register ------------------------------------
		@param mode: ( WRITE,READ )
								WRITE: Write to the register	
								READ: Read from register
		@param FSC: 0x00 to 0xFF
		@param InputBuff[] = array containing data that is read from AD9957
**/
void AuxDacControlReg(unsigned char mode, unsigned int FSC, unsigned int InputBuff[])
{
	unsigned int Temp = 0x00000000, Data;
	if(mode == 0)
	{
		Data = Temp|FSC;
		AD9957_WriteReg(0x03,1);
		AD9957_WriteReg(Data,4);
		IO_Update();
	}
	else
	{
//		AD9957_WriteReg(0x83,1);
//		ReadReg(InputBuff,4);
//		IO_Update();
	}
}


/**
	@brief: void IOUpdateRateReg(unsigned char mode, unsigned int Data, unsigned int InputBuff[])
		-------------------------- IO Update rate control register ------------------------------------
		@param mode: ( WRITE,READ )
								WRITE: Write to the register	
								READ: Read from register
		@param Data: 0x00000000 to 0xFFFFFFFF
		@param InputBuff[] = array containing data that is read from AD9957
**/
void IOUpdateRateReg(unsigned char mode, unsigned int Data, unsigned int InputBuff[])
{
	unsigned int Temp = 0x00000000;
	if(mode == 0)
	{
		Data = Temp|Data;
		AD9957_WriteReg(0x04,1);
		AD9957_WriteReg(Data,4);
		IO_Update();
	}
	else
	{
//		AD9957_WriteReg(0x84,1);
//		ReadReg(InputBuff,4);
//		IO_Update();
	}
}


/**
	@brief: void RAMSegmentReg(unsigned char mode, unsigned char SegmentReg, unsigned int StepRate, unsigned int EndAddr, unsigned int StartAddr, unsigned char RAMPlayback, unsigned int InputBuff1[], unsigned int InputBuff2[])
		-------------------------- RAM segment register ------------------------------------
		@param mode: ( WRITE,READ )
								WRITE: Write to the register	
								READ: Read from register
		@param SegmentReg: ( RAM_SEGMENT_REG_0, RAM_SEGMENT_REG_1 )
		@param StepRate: 0x0000 to 0xFFFF
		@param EndAddr: 0x000 to 0x3FF
		@param StartAddr: 0x000 to 0x3FFF
		@param RAMPlayback: RAM Playback Mode, (0x0 to 0x7)
		@param InputBuff1[] = array containing data that is read from AD9957
		@param InputBuff2[] = array containing data that is read from AD9957
**/
void RAMSegmentReg(unsigned char mode, unsigned char SegmentReg, unsigned int StepRate, unsigned int EndAddr, unsigned int StartAddr, unsigned char RAMPlayback, unsigned int InputBuff1[], unsigned int InputBuff2[])
{
	unsigned int Temp = 0x00000000, var1 = 0x00000000, var2 = 0x00000000, addr;
	addr = 0x80|SegmentReg;
	var1 = StepRate;
	var1 <<= 16;
	var2 = EndAddr;
	var2 <<= 22;
	Temp |= var2;
	var2 = 0x00000000;
	var2 = StartAddr;
	var2 <<= 6;
	Temp |= var2|RAMPlayback;
	if(mode == 0)
	{ 
		AD9957_WriteReg(SegmentReg,1);
		AD9957_WriteReg(var1,2);
		AD9957_WriteReg(Temp,4);
		IO_Update();
	}
	else
	{
//		AD9957_WriteReg(addr,1);
//		ReadReg(InputBuff1,2);
//		ReadReg(InputBuff2,4);
//		IO_Update();
	}
}


/**
	@brief: void AmpScaleReg(unsigned char mode, unsigned int RampRate, unsigned int ScaleFactor, unsigned int StepSize, unsigned int InputBuff[])
		-------------------------- Amplitude scale register ------------------------------------
		@param mode: ( WRITE,READ )
								WRITE: Write to the register	
								READ: Read from register
		@param RampRate: Amplitude Ramp Rate, (0x0000 to 0xFFFF)
		@param ScaleFactor: Amplitude Scale Factor, (0x0000 to 0x3FFF)
		@param StepSize: Amplitude Step Size, (0x0 to 0x3)
		@param InputBuff[] = array containing data that is read from AD9957
**/
void AmpScaleReg(unsigned char mode, unsigned int RampRate, unsigned int ScaleFactor, unsigned int StepSize, unsigned int InputBuff[])
{
	unsigned int Temp = 0x00000000, var = 0x00000000;
	var = RampRate;
	var <<= 16;
	Temp |= var;
	var = 0x000;
	var = ScaleFactor;
	var <<= 2;
	Temp |= var|StepSize;
	if(mode == 0)
	{ 
		AD9957_WriteReg(0x09,1);
		AD9957_WriteReg(Temp,4);
		IO_Update();
	}
	else
	{
//		AD9957_WriteReg(0x89,1);
//		ReadReg(InputBuff,4);
//		IO_Update();
	}
}


/**
	@brief: void MultiChipSyncReg(unsigned char mode, unsigned char SyncValDelay, unsigned char SyncStatePreset, unsigned char SyncGenDelay, unsigned char SyncRecDelay, unsigned int Data, unsigned int InputBuff[])
		-------------------------- Multichip Sync register ------------------------------------
		@param mode: ( WRITE,READ )
								WRITE: Write to the register	
								READ: Read from register
		@param SyncValDelay: Sync Validation Delay, (0x00 to 0x0F)
		@param SyncStatePreset: Sync State Preset Value, (0x00 to 0x3F)
		@param SyncGenDelay: Sync Generator Delay, (0x00 to 0x1F)
		@param SyncRecDelay: Sync Receiver Delay, (0x00 to 0x1F)
		@param Data: ( SYNC_REC_ENABLE|SYNC_GEN_ENABLE|SYNC_GEN_POL )
									
									SYNC_REC_ENABLE: Sync Receiver Enable, (synchronization clock receiver disabled (default))
									SYNC_GEN_ENABLE: Sync Generator Enable, (synchronization clock generator disabled (default))
									SYNC_GEN_POL: Sync Generator Polarity, (synchronization clock generator coincident with the rising edge of the system clock (default))

		@param InputBuff[] = array containing data that is read from AD9957
**/
void MultiChipSyncReg(unsigned char mode, unsigned char SyncValDelay, unsigned char SyncStatePreset, unsigned char SyncGenDelay, unsigned char SyncRecDelay, unsigned int Data, unsigned int InputBuff[])
{
	unsigned int Temp = 0x000000, var = 0x000000;
	var = SyncValDelay;
	var <<= 28;
	Temp |= var;
	var = 0x00;
	var = SyncStatePreset;
	var <<= 18;
	Temp |= var;
	var = 0x000;
	var = SyncGenDelay;
	var <<= 11;
	Temp |= var;
	var = 0x000;
	var = SyncRecDelay;
	var <<= 3;
	Temp |= var|Data;
	if(mode == 0)
	{ 
		AD9957_WriteReg(0x0A,1);
		AD9957_WriteReg(Temp,4);
		IO_Update();
	}
	else
	{
//		AD9957_WriteReg(0x8A,1);
//		ReadReg(InputBuff,4);
//		IO_Update();
	}
}


/**
	@brief: void ProfSingleToneReg(unsigned char mode, unsigned char ProfNum, unsigned int ASF, unsigned int POW, unsigned int FTW, unsigned int InputBuff1[], unsigned int InputBuff2[])
		-------------------------- Profile single tone register ------------------------------------
		@param mode: ( WRITE,READ )
								WRITE: Write to the register	
								READ: Read from register
		@param ProfNum: ( PROFILE_0,PROFILE_1,PROFILE_2,PROFILE_3,PROFILE_4,PROFILE_5,PROFILE_6,PROFILE_7 )
		@param ASF: Amplitude Scale Factor, (0x0000 to 0x3FFF)
		@param POW: Phase Offset Word, (0x0000 to 0xFFFF)
		@param FTW: Frequency Tuning Word, (0x00000000 to 0xFFFFFFFF)
		@param InputBuff1[] = array containing data that is read from AD9957
		@param InputBuff2[] = array containing data that is read from AD9957
**/
void ProfSingleToneReg(unsigned char mode, unsigned char ProfNum, unsigned int ASF, unsigned int POW, unsigned int FTW, unsigned int InputBuff1[], unsigned int InputBuff2[])
{
	unsigned int Temp1 = 0x0000000, var1, var2;
	var2 = ProfNum|0x80;
	var1 = ASF;
	var1 <<= 16;
	Temp1 |= var1|POW;
	AD9957_CS(1);
//	Delay1(1);
	if(mode == 0)
	{ 
		AD9957_WriteReg(ProfNum,1);
		AD9957_WriteReg(Temp1,4);
		//IO_Update();
		//AD9957_WriteReg(ProfNum,1);
		AD9957_WriteReg(FTW,4);
		IO_Update();
	}
	else
	{
//		AD9957_WriteReg(var2,1);
//		ReadReg(InputBuff1,4);
//		//IO_Update();
//		//AD9957_WriteReg(var2,1);
//		ReadReg(InputBuff2,4);
//		IO_Update();
	}
	delay_us(1);
	AD9957_CS(1);
	delay_us(2);
}


/**
	@brief: void ProfQDUCReg(unsigned char mode, unsigned char ProfNum, unsigned char InterpolRate, unsigned int OSF, unsigned int POW, unsigned int FTW, unsigned char settings, unsigned int InputBuff1[], unsigned int InputBuff2[])
		-------------------------- Profile QDUC register ------------------------------------
		@param mode: ( WRITE,READ )
								WRITE: Write to the register	
								READ: Read from register
		@param ProfNum: ( PROFILE_0,PROFILE_1,PROFILE_2,PROFILE_3,PROFILE_4,PROFILE_5,PROFILE_6,PROFILE_7 )
		@param InterpolRate: CC Interpolation Rate, (0x00 to 0x3F)
		@param OSF: Output Scale Factor, (0x00 to 0xFF)
		@param POW: Phase Offset Word, (0x0000 to 0xFFFF)
		@param FTW: Frequency Tuning Word, (0x00000000 to 0xFFFFFFFF)
		@param settings: ( SPECTRAL_INVERT|INVERSE_CCI_BYPASS )
											
											SPECTRAL_INVERT: Spectral Invert, (the modulator output takes the form: I(t) × cos(ct) – Q(t) × sin(ct))
											INVERSE_CCI_BYPASS: Inverse CCI Bypass, (the inverse CCI filter is enabled)

		@param InputBuff1[] = array containing data that is read from AD9957
		@param InputBuff2[] = array containing data that is read from AD9957
**/
void ProfQDUCReg(unsigned char mode, unsigned char ProfNum, unsigned char InterpolRate, unsigned int OSF, unsigned int POW, unsigned int FTW, unsigned char settings, unsigned int InputBuff1[], unsigned int InputBuff2[])
{
	unsigned int temp = 0x000000, var1, var2;
	var2 = ProfNum|0x80;
	var1 = InterpolRate;
	var1 <<= 26;
	temp |= var1;
	var1 = 0x0000;
	var1 = OSF;
	var1 <<= 16;
	temp |= var1|POW|settings;
	if(mode == 0)
	{ 
		AD9957_WriteReg(ProfNum,1);
		AD9957_WriteReg(temp,4);
		AD9957_WriteReg(FTW,4);
		IO_Update();
	}
	else
	{
//		AD9957_WriteReg(var2,1);
//		ReadReg(InputBuff1,4);
//		ReadReg(InputBuff2,4);
//		IO_Update();
	}
}

/**
	@brief: void RAMReg(unsigned char mode, unsigned int Data, unsigned int InputBuff[])
		-------------------------- RAM register ------------------------------------
		@param mode: ( WRITE,READ )
								WRITE: Write to the register	
								READ: Read from register
		@param Data: 0x00000000 to 0xFFFFFFFF
		@param InputBuff[] = array containing data that is read from AD9957
**/
void RAMReg(unsigned char mode, unsigned int Data, unsigned int InputBuff[])
{
	unsigned int temp = 0x00000000;
	temp = temp|Data;
	if(mode == 0)
	{ 
		AD9957_WriteReg(0x16,1);
		AD9957_WriteReg(temp,4);
		IO_Update();
	}
	else
	{
//		AD9957_WriteReg(0x96,1);
//		ReadReg(InputBuff,4);
//		IO_Update();
	}
}

/**
	@brief: void GPIOReg(unsigned char mode, unsigned int Data, unsigned int InputBuff[])
		-------------------------- GPIO Configuration Register ------------------------------------
		@param mode: ( WRITE,READ )
								WRITE: Write to the register	
								READ: Read from register
		@param Data: 0x0000 to 0xFFFF
		@param InputBuff[] = array containing data that is read from AD9957
**/
void GPIOReg(unsigned char mode, unsigned int Data, unsigned int InputBuff[])
{
	unsigned int temp = 0x00000000;
	temp = temp|Data;
	temp <<= 16;
	if(mode == 0)
	{ 
		AD9957_WriteReg(0x18,1);
		AD9957_WriteReg(temp,2);
		IO_Update();
	}
	else
	{
//		AD9957_WriteReg(0x98,1);
//		ReadReg(InputBuff,2);
//		IO_Update();
	}
}


/**
	@brief: void GPIODataReg(unsigned char mode, unsigned int Data, unsigned int InputBuff[])
		-------------------------- GPIO Data Register ------------------------------------
		@param mode: ( WRITE,READ )
								WRITE: Write to the register	
								READ: Read from register
		@param Data: 0x0000 to 0xFFFF
		@param InputBuff[] = array containing data that is read from AD9957
**/
void GPIODataReg(unsigned char mode, unsigned int Data, unsigned int InputBuff[])
{
	unsigned int temp = 0x00000000;
	temp = temp|Data;
	if(mode == 0)
	{ 
		AD9957_WriteReg(0x19,1);
		AD9957_WriteReg(temp,2);
		IO_Update();
	}
	else
	{
//		AD9957_WriteReg(0x99,1);
//		ReadReg(InputBuff,2);
//		IO_Update();
	}
}
	
