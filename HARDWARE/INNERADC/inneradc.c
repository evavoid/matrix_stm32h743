#include "inneradc.h"

/*********************************************************************************************************************************/
#if SIGNALCHANNEL_ONCECHANGE_ADC
ADC_HandleTypeDef ADC1_Handler;//ADC句柄

//通道值 0~16取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16,ch: ADC_channels 
void adc1_1c1g_soft_none_init(void)
{ 
    ADC1_Handler.Instance=ADC1;
    ADC1_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4分频，ADCCLK=PER_CK/4=64/4=16MHZ
    ADC1_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16位模式
    ADC1_Handler.Init.ScanConvMode=DISABLE;                    	//非扫描模式
    ADC1_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//关闭EOC中断
	  ADC1_Handler.Init.LowPowerAutoWait=DISABLE;					//自动低功耗关闭				
    ADC1_Handler.Init.ContinuousConvMode=DISABLE;               //关闭连续转换
    ADC1_Handler.Init.NbrOfConversion=1;                        //1个转换在规则序列中 也就是只转换规则序列1 
    ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;            //禁止不连续采样模式
    ADC1_Handler.Init.NbrOfDiscConversion=0;                    //不连续采样通道数为0
    ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //软件触发
    ADC1_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//使用软件触发
	  ADC1_Handler.Init.BoostMode=ENABLE;							//BOOT模式关闭
	  ADC1_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			//有新的数据的死后直接覆盖掉旧数据
	  ADC1_Handler.Init.OversamplingMode=DISABLE;					//过采样关闭
	  ADC1_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //规则通道的数据仅仅保存在DR寄存器里面
    HAL_ADC_Init(&ADC1_Handler);                                 //初始化 
	
	HAL_ADCEx_Calibration_Start(&ADC1_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADC校准
}

//hadc:ADC句柄,此函数会被HAL_ADC_Init()调用,ADC底层驱动，引脚配置，时钟使能
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC12_CLK_ENABLE();           //使能ADC1/2时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
	  __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP); //ADC外设时钟选择
	
    GPIO_Initure.Pin=GPIO_PIN_5;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //模拟
    GPIO_Initure.Pull=GPIO_NOPULL;          //不带上下拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

//返回值:转换结果,获得ADC值,ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
u16 adc1_1c1g_soft_none_value(u32 ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel=ch;                                   //通道
    ADC1_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1个序列
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//采样时间       
	  ADC1_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//单边采集          		
	  ADC1_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
	  ADC1_ChanConf.Offset=0;   
    HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);        //通道配置

    HAL_ADC_Start(&ADC1_Handler);                               //开启ADC
	
    HAL_ADC_PollForConversion(&ADC1_Handler,10);                //轮询转换
	return (u16)HAL_ADC_GetValue(&ADC1_Handler);	            //返回最近一次ADC1规则组的转换结果
}
//获取指定通道的转换值，取times次,然后平均 
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u16 adc1_1c1g_soft_none_averagevalue(u32 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=adc1_signalchannel_once_value(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 
//data[10];[超时检测变量,计数变量]
/*********************************************************************************************************************************/
#endif

#if SIGNALCHANNEL_ONCECHANGE_ADC_DMA_TWOBUFFER

/* 选择ADC的时钟源 */
#define ADC_CLOCK_SOURCE_AHB     /* 选择AHB时钟源 */
//#define ADC_CLOCK_SOURCE_PLL     /* 选择PLL时钟源 */

/* 方便Cache类的API操作，做32字节对齐 */
ALIGN_32BYTES(__attribute__((section (".RAM_D3"))) uint16_t ADCxValues[128]);
__IO u32 s_DmaFlag = 0;  /* 1表示进DMA半传输完成中断，2表示进入DMA传输完成中断 */
DMA_HandleTypeDef   DmaHandle = {0};	

//*********************************************************************************************************
//*	函 数 名: TIM1_Config
//*	功能说明: 配置TIM1，用于触发ADC，当前配置的100KHz触发频率
//*	形    参: 无									  
//*	返 回 值: 无
//*********************************************************************************************************
static void TIM1_Config(void)
{
	TIM_HandleTypeDef  htim ={0};
	TIM_OC_InitTypeDef sConfig = {0};


	/* 使能时钟 */  
	__HAL_RCC_TIM1_CLK_ENABLE();
      
    /*-----------------------------------------------------------------------
		bsp.c 文件中 void SystemClock_Config(void) 函数对时钟的配置如下: 

        System Clock source       = PLL (HSE)
        SYSCLK(Hz)                = 400000000 (CPU Clock)
        HCLK(Hz)                  = 200000000 (AXI and AHBs Clock)
        AHB Prescaler             = 2
        D1 APB3 Prescaler         = 2 (APB3 Clock  100MHz)
        D2 APB1 Prescaler         = 2 (APB1 Clock  100MHz)
        D2 APB2 Prescaler         = 2 (APB2 Clock  100MHz)
        D3 APB4 Prescaler         = 2 (APB4 Clock  100MHz)

        因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = APB1 x 2 = 200MHz;
        因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = APB2 x 2 = 200MHz;
        APB4上面的TIMxCLK没有分频，所以就是100MHz;

        APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14，LPTIM1
        APB2 定时器有 TIM1, TIM8 , TIM15, TIM16，TIM17

        APB4 定时器有 LPTIM2，LPTIM3，LPTIM4，LPTIM5

    TIM12CLK = 200MHz/(Period + 1) / (Prescaler + 1) = 200MHz / 2000 / 1 = 100KHz
	----------------------------------------------------------------------- */  
    HAL_TIM_Base_DeInit(&htim);
    
    htim.Instance = TIM1;
		htim.Init.Period            = 1999;
		htim.Init.Prescaler         = 0;
		htim.Init.ClockDivision     = 0;
		htim.Init.CounterMode       = TIM_COUNTERMODE_UP;
		htim.Init.RepetitionCounter = 0;
		HAL_TIM_Base_Init(&htim);
    
    sConfig.OCMode     = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = TIM_OCPOLARITY_LOW;

    /* 占空比50% */
    sConfig.Pulse = 1000;  
    HAL_TIM_OC_ConfigChannel(&htim, &sConfig, TIM_CHANNEL_1);

		/* 启动OC1 */
    HAL_TIM_OC_Start(&htim, TIM_CHANNEL_1);
}


//*********************************************************************************************************
//*	函 数 名: bsp_InitADC
//*	功能说明: 初始化ADC
//*	形    参: 无
//*	返 回 值: 无
//*********************************************************************************************************
void adc1_1c1g_tim_dma_two_buffer_init(void)
{
  ADC_HandleTypeDef        AdcHandle = {0};
	ADC_ChannelConfTypeDef   sConfig = {0};
	GPIO_InitTypeDef         GPIO_InitStruct;

	
  /* ## - 1 - 配置ADC采样的时钟 ####################################### */
#if defined (ADC_CLOCK_SOURCE_PLL)
	/* 配置PLL2时钟为的72MHz，方便分频产生ADC最高时钟36MHz */
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInitStruct.PLL2.PLL2M = 25;
	PeriphClkInitStruct.PLL2.PLL2N = 504;
	PeriphClkInitStruct.PLL2.PLL2P = 7;
	PeriphClkInitStruct.PLL2.PLL2Q = 7;
	PeriphClkInitStruct.PLL2.PLL2R = 7;
	PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
	PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
	PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
	PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);  
	}
#elif defined (ADC_CLOCK_SOURCE_AHB)
  
  /* 使用AHB时钟的话，无需配置，默认选择*/
  
#endif

	/* ## - 2 - 配置ADC采样使用的引脚 ####################################### */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* ## - 3 - 配置ADC采样使用的时钟 ####################################### */
	__HAL_RCC_DMA1_CLK_ENABLE();
	DmaHandle.Instance                 = DMA1_Stream2;            /* 使用的DMA1 Stream2 */
	DmaHandle.Init.Request             = DMA_REQUEST_ADC1;  	  /* 请求类型采用DMA_REQUEST_ADC1 */  
	DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;    /* 传输方向是从外设到存储器 */  
	DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;        /* 外设地址自增禁止 */ 
	DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;         /* 存储器地址自增使能 */  
	DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;  /* 外设数据传输位宽选择半字，即16bit */     
	DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;  /* 存储器数据传输位宽选择半字，即16bit */    
	DmaHandle.Init.Mode                = DMA_CIRCULAR;            /* 循环模式 */   
	DmaHandle.Init.Priority            = DMA_PRIORITY_LOW;        /* 优先级低 */  
	DmaHandle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;    /* 禁止FIFO*/
	DmaHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL; /* 禁止FIFO此位不起作用，用于设置阀值 */
	DmaHandle.Init.MemBurst            = DMA_MBURST_SINGLE;       /* 禁止FIFO此位不起作用，用于存储器突发 */
	DmaHandle.Init.PeriphBurst         = DMA_PBURST_SINGLE;       /* 禁止FIFO此位不起作用，用于外设突发 */
 
  /* 初始化DMA */
  HAL_DMA_Init(&DmaHandle);
    
  /* 开启DMA1 Stream1的中断 */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
    
  /* 关联ADC句柄和DMA句柄 */
	__HAL_LINKDMA(&AdcHandle, DMA_Handle, DmaHandle);

	/* ## - 4 - 配置ADC ########################################################### */
	__HAL_RCC_ADC12_CLK_ENABLE();
	AdcHandle.Instance = ADC1;
	
#if defined (ADC_CLOCK_SOURCE_PLL)
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV2;          /* 采用PLL异步时钟，2分频，即72MHz/2 = 36MHz */
#elif defined (ADC_CLOCK_SOURCE_AHB)
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;      /* 采用AHB同步时钟，4分频，即200MHz/4 = 50MHz */
#endif
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_16B;            /* 16位分辨率 */
	AdcHandle.Init.ScanConvMode          = ADC_SCAN_DISABLE;              /* 禁止扫描，因为仅开了一个通道 */
	AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;           /* EOC转换结束标志 */
	AdcHandle.Init.LowPowerAutoWait      = DISABLE;                       /* 禁止低功耗自动延迟特性 */
	AdcHandle.Init.ContinuousConvMode    = DISABLE;                       /* 禁止自动转换，采用的定时器触发转换 */
	AdcHandle.Init.NbrOfConversion       = 1;                             /* 使用了1个转换通道 */
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* 禁止不连续模式 */
	AdcHandle.Init.NbrOfDiscConversion   = 1;                             /* 禁止不连续模式后，此参数忽略，此位是用来配置不连续子组中通道数 */
	AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIG_T1_CC1;            /* 定时器1的CC1触发 */
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;    /* 上升沿触发 */
	AdcHandle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* DMA循环模式接收ADC转换的数据 */
	AdcHandle.Init.BoostMode                = ENABLE;                          /* ADC时钟超过20MHz的话，使能boost */
	AdcHandle.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* ADC转换溢出的话，覆盖ADC的数据寄存器 */
	AdcHandle.Init.OversamplingMode         = DISABLE;                         /* 禁止过采样 */

  /* 初始化ADC */
	HAL_ADC_Init(&AdcHandle);
	/* 校准ADC，采用偏移校准 */
	HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
  
	/* 配置ADC通道  */
	sConfig.Channel      = ADC_CHANNEL_19;              /* 配置使用的ADC通道 */
	sConfig.Rank         = ADC_REGULAR_RANK_1;          /* 采样序列里的第1个 */
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;     /* 采样周期 */
	sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* 单端输入 */
	sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* 无偏移 */ 
	sConfig.Offset = 0;                                 /* 无偏移的情况下，此参数忽略 */

	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
  
	/* ## - 5 - 配置ADC的定时器触发 ####################################### */
	TIM1_Config();
  
	/* ## - 6 - 启动ADC的DMA方式传输 ####################################### */
	HAL_ADC_Start_DMA(&AdcHandle, (uint32_t *)ADCxValues, 128);
}


//*********************************************************************************************************
//*	函 数 名: DMA1_Stream1_IRQHandler
//*	功能说明: DMA1 Stream1中断服务程序
//*	形    参: 无
//*	返 回 值: 无
//**********************************************************************************************************
void DMA1_Stream2_IRQHandler(void)
{
	/* 传输完成中断 */
	if((DMA1->LISR & DMA_FLAG_TCIF1_5) != RESET)
	{
				/*
		   1、使用此函数要特别注意，第1个参数地址要32字节对齐，第2个参数要是32字节的整数倍。
		   2、进入传输完成中断，当前DMA正在使用缓冲区的前半部分，用户可以操作后半部分。
		*/
		SCB_InvalidateDCache_by_Addr((uint32_t *)(&ADCxValues[64]), 128);
		
		s_DmaFlag = 2;
		
		/* 清除标志 */
		DMA1->LIFCR = DMA_FLAG_TCIF1_5;
	}
	/* 半传输完成中断 */    
	if((DMA1->LISR & DMA_FLAG_HTIF1_5) != RESET)
	{
		/*
		   1、使用此函数要特别注意，第1个参数地址要32字节对齐，第2个参数要是32字节的整数倍。
		   2、进入半传输完成中断，当前DMA正在使用缓冲区的后半部分，用户可以操作前半部分。
		*/
		SCB_InvalidateDCache_by_Addr((uint32_t *)(&ADCxValues[0]), 128);
		s_DmaFlag = 1;
        
		/* 清除标志 */
		DMA1->LIFCR = DMA_FLAG_HTIF1_5;
	}

	/* 传输错误中断 */
	if((DMA1->LISR & DMA_FLAG_TEIF1_5) != RESET)
	{
		/* 清除标志 */
		DMA1->LIFCR = DMA_FLAG_TEIF1_5;
	}

	/* 直接模式错误中断 */
	if((DMA1->LISR & DMA_FLAG_DMEIF1_5) != RESET)
	{
		/* 清除标志 */
		DMA1->LIFCR = DMA_FLAG_DMEIF1_5;
	}
}
#endif
#if MULTCHANNEL_ONCECHANGE_ADC_DMA
/* 选择ADC的时钟源 */
#define ADC_CLOCK_SOURCE_AHB     /* 选择AHB时钟源 */
//#define ADC_CLOCK_SOURCE_PLL     /* 选择PLL时钟源 */

/* 方便Cache类的API操作，做32字节对齐 */
ALIGN_32BYTES(__attribute__((section (".RAM_D3"))) uint16_t ADCxValues[4]);

DMA_HandleTypeDef   DMA_Handle = {0};

//*********************************************************************************************************
//*	函 数 名: bsp_InitADC
//*	功能说明: 初始化ADC，采用DMA方式进行多通道采样，采集了PC0, Vbat/4, VrefInt和温度
//*	形    参: 无
//*	返 回 值: 无
//*********************************************************************************************************
void adc3_nc1g_soft_dma_init(void)
{
	ADC_HandleTypeDef   AdcHandle = {0};
	ADC_ChannelConfTypeDef   sConfig = {0};
	GPIO_InitTypeDef          GPIO_InitStruct;

  /* ## - 1 - 配置ADC采样的时钟 ####################################### */
#if defined (ADC_CLOCK_SOURCE_PLL)
	/* 配置PLL2时钟为的72MHz，方便分频产生ADC最高时钟36MHz */
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInitStruct.PLL2.PLL2M = 25;
	PeriphClkInitStruct.PLL2.PLL2N = 504;
	PeriphClkInitStruct.PLL2.PLL2P = 7;
	PeriphClkInitStruct.PLL2.PLL2Q = 7;
	PeriphClkInitStruct.PLL2.PLL2R = 7;
	PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
	PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
	PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
	PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

#elif defined (ADC_CLOCK_SOURCE_AHB)
  /* 使用AHB时钟的话，无需配置，默认选择*/ 
#endif

	/* ## - 2 - 配置ADC采样使用的IO   ####################################### */
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
	/* ## - 3 - 配置ADC采样使用的时钟 ####################################### */
	__HAL_RCC_DMA1_CLK_ENABLE();
	DMA_Handle.Instance                 = DMA1_Stream1;            /* 使用的DMA1 Stream1 */
	DMA_Handle.Init.Request             = DMA_REQUEST_ADC3;  	   /* 请求类型采用DMA_REQUEST_ADC3 */  
	DMA_Handle.Init.Direction           = DMA_PERIPH_TO_MEMORY;    /* 传输方向是从存储器到外设 */  
	DMA_Handle.Init.PeriphInc           = DMA_PINC_DISABLE;        /* 外设地址自增禁止 */ 
	DMA_Handle.Init.MemInc              = DMA_MINC_ENABLE;         /* 存储器地址自增使能 */  
	DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;   /* 外设数据传输位宽选择半字，即16bit */     
	DMA_Handle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;   /* 存储器数据传输位宽选择半字，即16bit */    
	DMA_Handle.Init.Mode                = DMA_CIRCULAR;            /* 循环模式 */   
	DMA_Handle.Init.Priority            = DMA_PRIORITY_LOW;        /* 优先级低 */  
	DMA_Handle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;    /* 禁止FIFO*/
	DMA_Handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL; /* 禁止FIFO此位不起作用，用于设置阀值 */
	DMA_Handle.Init.MemBurst            = DMA_MBURST_SINGLE;       /* 禁止FIFO此位不起作用，用于存储器突发 */
	DMA_Handle.Init.PeriphBurst         = DMA_PBURST_SINGLE;       /* 禁止FIFO此位不起作用，用于外设突发 */

	/* 初始化DMA */
	HAL_DMA_Init(&DMA_Handle);

	/* 关联ADC句柄和DMA句柄 */
	__HAL_LINKDMA(&AdcHandle, DMA_Handle, DMA_Handle);
	
	/* ## - 4 - 配置ADC ########################################################### */
	__HAL_RCC_ADC3_CLK_ENABLE();
	AdcHandle.Instance = ADC3;

#if defined (ADC_CLOCK_SOURCE_PLL)
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV8;          /* 采用PLL异步时钟，8分频，即72MHz/8 = 36MHz */
#elif defined (ADC_CLOCK_SOURCE_AHB)
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;      /* 采用AHB同步时钟，4分频，即200MHz/4 = 50MHz */
#endif
	
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_16B;        /* 16位分辨率 */
	AdcHandle.Init.ScanConvMode          = ADC_SCAN_ENABLE;           /* 禁止扫描，因为仅开了一个通道 */
	AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;       /* EOC转换结束标志 */
	AdcHandle.Init.LowPowerAutoWait      = DISABLE;                   /* 禁止低功耗自动延迟特性 */
	AdcHandle.Init.ContinuousConvMode    = ENABLE;                    /* 禁止自动转换，采用的软件触发 */
	AdcHandle.Init.NbrOfConversion       = 4;                         /* 使用了4个转换通道 */
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;                   /* 禁止不连续模式 */
	AdcHandle.Init.NbrOfDiscConversion   = 1;                         /* 禁止不连续模式后，此参数忽略，此位是用来配置不连续子组中通道数 */
	AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;        /* 采用软件触发 */
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;    /* 采用软件触发的话，此位忽略 */
	AdcHandle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* DMA循环模式接收ADC转换的数据 */
	AdcHandle.Init.BoostMode             = DISABLE;                            /* ADC时钟低于20MHz的话，可以禁止boost */
	AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;     	   /* ADC转换溢出的话，覆盖ADC的数据寄存器 */
	AdcHandle.Init.OversamplingMode      = DISABLE;                            /* 禁止过采样 */
	
/******oversampling setting********/
//	AdcHandle.Init.OversamplingMode         		  = ENABLE;                        /* 使能过采样 */
//	AdcHandle.Init.Oversampling.Ratio                 = 15;                            /* 15+1倍过采样 */
//	AdcHandle.Init.Oversampling.RightBitShift         = ADC_RIGHTBITSHIFT_4;           /* 数据右移4bit，即除以16 */
//	AdcHandle.Init.Oversampling.TriggeredMode         = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;   /* 触发后连续完成每个通道的所有过采样转换 */
//	AdcHandle.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE; /* 如果触发注入转换，过采样会暂时停止，并会在注
//                                                                                               入序列完成后继续，注入序列过程中会保留过采样缓冲区*/    
    /* 初始化ADC */
	HAL_ADC_Init(&AdcHandle);
  
	/* 校准ADC，采用偏移校准 */
	HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

	/* 配置ADC通道，序列1，采样PC0引脚 */
	/*
		采用PLL2时钟的话，ADCCLK = 72MHz / 8 = 9MHz
	    ADC采样速度，即转换时间 = 采样时间 + 逐次逼近时间
	                            = 810.5 + 8.5(16bit)
	                            = 820个ADC时钟周期
	    那么转换速度就是9MHz / 820 = 10975Hz
	*/
	sConfig.Channel      = ADC_CHANNEL_10;              /* 配置使用的ADC通道 */
	sConfig.Rank         = ADC_REGULAR_RANK_1;          /* 采样序列里的第1个 */
	sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;  /* 采样周期 */
	sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* 单端输入 */
	sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* 无偏移 */ 
	sConfig.Offset = 0;                                 /* 无偏移的情况下，此参数忽略 */
	sConfig.OffsetRightShift       = DISABLE;           /* 禁止右移 */
	sConfig.OffsetSignedSaturation = DISABLE;           /* 禁止有符号饱和 */
	
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
	
	/* 配置ADC通道，序列2，采样Vbat/4 */
	sConfig.Channel      = ADC_CHANNEL_VBAT_DIV4;       /* 配置使用的ADC通道 */
	sConfig.Rank         = ADC_REGULAR_RANK_2;          /* 采样序列里的第1个 */
	sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;  /* 采样周期 */
	sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* 单端输入 */
	sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* 无偏移 */ 
	sConfig.Offset = 0;                                 /* 无偏移的情况下，此参数忽略 */
	sConfig.OffsetRightShift       = DISABLE;           /* 禁止右移 */
	sConfig.OffsetSignedSaturation = DISABLE;           /* 禁止有符号饱和 */
	
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
  
	/* 配置ADC通道，序列3，采样VrefInt */
	sConfig.Channel      = ADC_CHANNEL_VREFINT;         /* 配置使用的ADC通道 */
	sConfig.Rank         = ADC_REGULAR_RANK_3;          /* 采样序列里的第1个 */
	sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;  /* 采样周期 */
	sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* 单端输入 */
	sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* 无偏移 */ 
	sConfig.Offset = 0;                                 /* 无偏移的情况下，此参数忽略 */
	sConfig.OffsetRightShift       = DISABLE;           /* 禁止右移 */
	sConfig.OffsetSignedSaturation = DISABLE;           /* 禁止有符号饱和 */
	
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);

	/* 配置ADC通道，序列4，采样温度 */
	sConfig.Channel      = ADC_CHANNEL_TEMPSENSOR;      /* 配置使用的ADC通道 */
	sConfig.Rank         = ADC_REGULAR_RANK_4;          /* 采样序列里的第1个 */
	sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;  /* 采样周期 */
	sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* 单端输入 */
	sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* 无偏移 */ 
	sConfig.Offset = 0;                                 /* 无偏移的情况下，此参数忽略 */
	sConfig.OffsetRightShift       = DISABLE;           /* 禁止右移 */
	sConfig.OffsetSignedSaturation = DISABLE;           /* 禁止有符号饱和 */
	
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
  

	/* ## - 6 - 启动ADC的DMA方式传输 ####################################### */
	HAL_ADC_Start_DMA(&AdcHandle, (uint32_t *)ADCxValues, 4);
}

//*********************************************************************************************************
//*	函 数 名: bsp_GetAdcValues
//*	功能说明: 获取ADC的数据并打印
//*	形    参: 无
//*	返 回 值: 无
//*********************************************************************************************************
void adc3_nc1g_soft_dma_value(void)
{
    float AdcValues[5];
    uint16_t TS_CAL1;
    uint16_t TS_CAL2;
    
    /*
       使用此函数要特别注意，第1个参数地址要32字节对齐，第2个参数要是32字节的整数倍
    */
    SCB_InvalidateDCache_by_Addr((uint32_t *)ADCxValues,  sizeof(ADCxValues));
    AdcValues[0] = ADCxValues[0] * 3.3 / 65536;
    AdcValues[1] = ADCxValues[1] * 3.3 / 65536; 
    AdcValues[2] = ADCxValues[2] * 3.3 / 65536;     
 
    /* 根据参考手册给的公式计算温度值 */
    TS_CAL1 = *(__IO uint16_t *)(0x1FF1E820);
    TS_CAL2 = *(__IO uint16_t *)(0x1FF1E840);
    
    AdcValues[3] = (110.0 - 30.0) * (ADCxValues[3] - TS_CAL1)/ (TS_CAL2 - TS_CAL1) + 30;  
    
    printf("PC0 = %5.3fV, Vbat/4 = %5.3fV, VrefInt = %5.3fV， TempSensor = %5.3f℃\r\n", 
            AdcValues[0],  AdcValues[1], AdcValues[2], AdcValues[3]);

}



#endif

