#include "roll.h"

ADC_HandleTypeDef ADC2_Handler;//ADC句柄
//PC4 PC5 ADC2INP4 ADC2INP8 x y
//使用的时候必须把Msp加上

//初始化ADC
//ch: ADC_channels 
//通道值 0~16取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
void roll_Init(void)
{ 
    ADC2_Handler.Instance=ADC2;
    ADC2_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4分频，ADCCLK=PER_CK/4=64/4=16MHZ
    ADC2_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16位模式
    ADC2_Handler.Init.ScanConvMode=DISABLE;                    	//非扫描模式
    ADC2_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//关闭EOC中断
	  ADC2_Handler.Init.LowPowerAutoWait=DISABLE;					//自动低功耗关闭				
    ADC2_Handler.Init.ContinuousConvMode=DISABLE;               //关闭连续转换
    ADC2_Handler.Init.NbrOfConversion=1;                        //1个转换在规则序列中 也就是只转换规则序列1 
    ADC2_Handler.Init.DiscontinuousConvMode=DISABLE;            //禁止不连续采样模式
    ADC2_Handler.Init.NbrOfDiscConversion=0;                    //不连续采样通道数为0
    ADC2_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //软件触发
    ADC2_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//使用软件触发
	  ADC2_Handler.Init.BoostMode=ENABLE;							//BOOT模式关闭
	  ADC2_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			//有新的数据的死后直接覆盖掉旧数据
	  ADC2_Handler.Init.OversamplingMode=DISABLE;					//过采样关闭
	  ADC2_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //规则通道的数据仅仅保存在DR寄存器里面
    HAL_ADC_Init(&ADC2_Handler);                                 //初始化 
		HAL_ADCEx_Calibration_Start(&ADC2_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADC校准
}
/*
//ADC底层驱动，引脚配置，时钟使能
//此函数会被HAL_ADC_Init()调用
//hadc:ADC句柄
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC12_CLK_ENABLE();           //使能ADC1/2时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOA时钟
	  __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP); //ADC外设时钟选择
	
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_4;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //模拟
    GPIO_Initure.Pull=GPIO_NOPULL;          //不带上下拉
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
}
*/
u32 roll_judge(float adc)
{
	float width=(float)(MAX_VOLTAGE+MIN_VOLTAGE)/ADVANCE_SEL;
	u32 index;
	
	if(adc>(float)MAX_VOLTAGE||adc<(float)MIN_VOLTAGE)
		ERROR_HANDLER("ROLL_vlotage_error",1);
	for(index=0;index<ADVANCE_SEL;index++)
	{
		if(adc<((float)MIN_VOLTAGE+width+width*index))
			return index;
	}
	return (ADVANCE_SEL+1)/2;
}
float get_y_channel(void)
{
	ADC_ChannelConfTypeDef ADC2_ChanConf;
    
	ADC2_ChanConf.Channel=ADC_CHANNEL_8;                                   //通道
	ADC2_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1个序列
	ADC2_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//采样时间       
	ADC2_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//单边采集          		
	ADC2_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
	ADC2_ChanConf.Offset=0;   
	HAL_ADC_ConfigChannel(&ADC2_Handler,&ADC2_ChanConf);        //通道配置

	HAL_ADC_Start(&ADC2_Handler);                               //开启ADC
	
	HAL_ADC_PollForConversion(&ADC2_Handler,10);                //轮询转换
	return (float)HAL_ADC_GetValue(&ADC2_Handler)*(3.3/65536);           //返回最近一次ADC1规则组的转换结果
}
float get_x_channel(void)
{
	ADC_ChannelConfTypeDef ADC2_ChanConf;
    
  ADC2_ChanConf.Channel=ADC_CHANNEL_4;                                   //通道
  ADC2_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1个序列
  ADC2_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//采样时间       
	ADC2_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//单边采集          		
	ADC2_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
  ADC2_ChanConf.Offset=0;   
  HAL_ADC_ConfigChannel(&ADC2_Handler,&ADC2_ChanConf);        //通道配置

  HAL_ADC_Start(&ADC2_Handler);                               //开启ADC
	
  HAL_ADC_PollForConversion(&ADC2_Handler,10);                //轮询转换
	return (float)HAL_ADC_GetValue(&ADC2_Handler)*(3.3/65536);            //返回最近一次ADC1规则组的转换结果
}
void roll_event(local_state_space *	ROLL_state)
{
	if(ROLL_state->ready_state==prepare_state)
	{
		roll_Init();
		ROLL_state->ready_state=run_state0;
		return;
	}
	else if(ROLL_state->ready_state==run_state0)
	{
		printf("x:%d\r\n",roll_judge(get_x_channel()));
		delay_ms(200);
		return;
	}
}

