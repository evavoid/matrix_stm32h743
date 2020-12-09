#include "ad8302.h"
ADC_HandleTypeDef ADCD_Handler;//ADC句柄
ADC_HandleTypeDef ADCC_Handler;//ADC句柄
ADC_ChannelConfTypeDef ADCC_ChanConf;
ADC_ChannelConfTypeDef ADCD_ChanConf;

/*************************************************************************/
//ADC1 ADC_CHANNEL_19 PA5 ADC_C1
//ADC1 ADC_CHANNEL_18 PA4 ADC_C2
//ADC2 ADC_CHANNEL_4  PC4 ADC_D1
//ADC2 ADC_CHANNEL_8  PC5 ADC_D2
void AD8302_adc_init(void)
{
		ADCD_Handler.Instance=ADC2;
    ADCD_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4分频，ADCCLK=PER_CK/4=64/4=16MHZ
    ADCD_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16位模式
    ADCD_Handler.Init.ScanConvMode=DISABLE;                    	//非扫描模式
    ADCD_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//关闭EOC中断
	  ADCD_Handler.Init.LowPowerAutoWait=DISABLE;					//自动低功耗关闭				
    ADCD_Handler.Init.ContinuousConvMode=DISABLE;               //关闭连续转换
    ADCD_Handler.Init.NbrOfConversion=1;                        //1个转换在规则序列中 也就是只转换规则序列1 
    ADCD_Handler.Init.DiscontinuousConvMode=DISABLE;            //禁止不连续采样模式
    ADCD_Handler.Init.NbrOfDiscConversion=0;                    //不连续采样通道数为0
    ADCD_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //软件触发
    ADCD_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//使用软件触发
	  ADCD_Handler.Init.BoostMode=ENABLE;							//BOOT模式关闭
	  ADCD_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			//有新的数据的死后直接覆盖掉旧数据
	  ADCD_Handler.Init.OversamplingMode=DISABLE;					//过采样关闭
	  ADCD_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //规则通道的数据仅仅保存在DR寄存器里面
    HAL_ADC_Init(&ADCD_Handler);                                 //初始化 
	
		GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC12_CLK_ENABLE();           //使能ADC1/2时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOA时钟
	  __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP); //ADC外设时钟选择
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_4;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //模拟
    GPIO_Initure.Pull=GPIO_NOPULL;          //不带上下拉
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
		
		HAL_ADCEx_Calibration_Start(&ADCD_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADC校准
	
	  /******************************/
	
		ADCC_Handler.Instance=ADC1;
    ADCC_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4分频，ADCCLK=PER_CK/4=64/4=16MHZ
    ADCC_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16位模式
    ADCC_Handler.Init.ScanConvMode=DISABLE;                    	//非扫描模式
    ADCC_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//关闭EOC中断
	  ADCC_Handler.Init.LowPowerAutoWait=DISABLE;					//自动低功耗关闭				
    ADCC_Handler.Init.ContinuousConvMode=DISABLE;               //关闭连续转换
    ADCC_Handler.Init.NbrOfConversion=1;                        //1个转换在规则序列中 也就是只转换规则序列1 
    ADCC_Handler.Init.DiscontinuousConvMode=DISABLE;            //禁止不连续采样模式
    ADCC_Handler.Init.NbrOfDiscConversion=0;                    //不连续采样通道数为0
    ADCC_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //软件触发
    ADCC_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//使用软件触发
	  ADCC_Handler.Init.BoostMode=ENABLE;							//BOOT模式关闭
	  ADCC_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			//有新的数据的死后直接覆盖掉旧数据
	  ADCC_Handler.Init.OversamplingMode=DISABLE;					//过采样关闭
	  ADCC_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //规则通道的数据仅仅保存在DR寄存器里面
		HAL_ADC_Init(&ADCC_Handler);      
		
    __HAL_RCC_ADC12_CLK_ENABLE();           //使能ADC1/2时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
	  __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP); //ADC外设时钟选择
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_4;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //模拟
    GPIO_Initure.Pull=GPIO_NOPULL;          //不带上下拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);                      //初始化 

		HAL_ADCEx_Calibration_Start(&ADCC_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADC校准
		/************************/
		ADCC_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1个序列
		ADCC_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//采样时间       
		ADCC_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//单边采集          		
		ADCC_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
		ADCC_ChanConf.Offset=0;   
		/************************/
		ADCD_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1个序列
		ADCD_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//采样时间       
		ADCD_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//单边采集          		
		ADCD_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
		ADCD_ChanConf.Offset=0;   
}
//ADC1 ADC_CHANNEL_19 PA5 ADC_C1
//ADC1 ADC_CHANNEL_18 PA4 ADC_C2
//exp:get_ADC_C(1);get_ADC_C(2);
float get_ADC_C(u8 type)
{
  if(type==1)ADCC_ChanConf.Channel=ADC_CHANNEL_19;   //通道
	else if(type==2)ADCC_ChanConf.Channel=ADC_CHANNEL_18; 
	
	HAL_ADC_ConfigChannel(&ADCC_Handler,&ADCC_ChanConf);        //通道配置
	HAL_ADC_Start(&ADCC_Handler);                               //开启ADC
	HAL_ADC_PollForConversion(&ADCC_Handler,10);                //轮询转换
	
	return (float)HAL_ADC_GetValue(&ADCC_Handler)*1100/65536/10-30;           //返回最近一次ADC1规则组的转换结果
}
//ADC2 ADC_CHANNEL_4  PC4 ADC_D1
//ADC2 ADC_CHANNEL_8  PC5 ADC_D2
//exp:get_ADC_D(1);get_ADC_D(2);
float get_ADC_D(u8 type)//特别注意返回时候的double
{
  if(type==1)ADCD_ChanConf.Channel=ADC_CHANNEL_4;   //通道
	else if(type==2)ADCD_ChanConf.Channel=ADC_CHANNEL_8; 
	
	HAL_ADC_ConfigChannel(&ADCD_Handler,&ADCD_ChanConf);        //通道配置
	HAL_ADC_Start(&ADCD_Handler);                               //开启ADC
	HAL_ADC_PollForConversion(&ADCD_Handler,10);                //轮询转换
		
	return 180-(double)HAL_ADC_GetValue(&ADCD_Handler)*5.0354/1000;          //返回最近一次ADC1规则组的转换结果
}

void ad8302_event(local_state_space *AD8302_state)
{
	if(AD8302_state->ready_state==prepare_state)
	{	
		/*                            --->C1幅值差
			I(90 degree)\              /
									 ---->(AD8302)X---->D1相角差的绝对值
			O(00 degree)X              
									 ---->(AD8302)X---->C2幅值差
			Q(00 degree)/              \
		                              --->D2相角差的绝对值
		*/
	}
	else if(AD8302_state->ready_state==run_state0)//单一扫描模式
	{
		//设置频率点  AD9959_IOQ_Output(1,2,3,频率值,1000);
		//幅值偏移(O路信号)=(get_ADC_C(1)-get_ADC_C(2))/2+当前频率的幅值误差修正-6;
		/*
			d1_temp=get_ADC_D(1);d2_temp=get_ADC_D(2);
			if(fabs(d1_temp+d2_temp-90)>20)
			{
				s1=(d2_temp>d1_temp)?-1:1;
				s2=(d2_temp<d1_temp)?-1:1;
			}
			else
			{
				s1=1;
				s2=1;
			}
			相位偏移(O路信号)=(d1_temp*s1-d2_temp*s2-90)/2+当前频率的相位误差修正;
		*/
	}
	else if(AD8302_state->ready_state==run_state1)//归一化状态
	{
		//设置频率点  AD9959_IOQ_Output(1,2,3,频率值,1000);
		//当前频率的幅值误差修正=6-(get_ADC_C(1)-get_ADC_C(2))/2;
		/*
			d1_temp=get_ADC_D(1);d2_temp=get_ADC_D(2);
			if(fabs(d1_temp+d2_temp-90)>20)
			{
				s1=(d2_temp>d1_temp)?-1:1;
				s2=(d2_temp<d1_temp)?-1:1;
			}
			else
			{
				s1=1;
				s2=1;
			}
			当前频率的相位误差修正=(90+d2_temp*s2-d1_temp*s1)/2;
		*/
	}
}

