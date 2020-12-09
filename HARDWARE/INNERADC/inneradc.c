#include "inneradc.h"

/*********************************************************************************************************************************/
#if SIGNALCHANNEL_ONCECHANGE_ADC
ADC_HandleTypeDef ADC1_Handler;//ADC���

//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16,ch: ADC_channels 
void adc1_1c1g_soft_none_init(void)
{ 
    ADC1_Handler.Instance=ADC1;
    ADC1_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4��Ƶ��ADCCLK=PER_CK/4=64/4=16MHZ
    ADC1_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16λģʽ
    ADC1_Handler.Init.ScanConvMode=DISABLE;                    	//��ɨ��ģʽ
    ADC1_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//�ر�EOC�ж�
	  ADC1_Handler.Init.LowPowerAutoWait=DISABLE;					//�Զ��͹��Ĺر�				
    ADC1_Handler.Init.ContinuousConvMode=DISABLE;               //�ر�����ת��
    ADC1_Handler.Init.NbrOfConversion=1;                        //1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;            //��ֹ����������ģʽ
    ADC1_Handler.Init.NbrOfDiscConversion=0;                    //����������ͨ����Ϊ0
    ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //�������
    ADC1_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//ʹ���������
	  ADC1_Handler.Init.BoostMode=ENABLE;							//BOOTģʽ�ر�
	  ADC1_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			//���µ����ݵ�����ֱ�Ӹ��ǵ�������
	  ADC1_Handler.Init.OversamplingMode=DISABLE;					//�������ر�
	  ADC1_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //����ͨ�������ݽ���������DR�Ĵ�������
    HAL_ADC_Init(&ADC1_Handler);                                 //��ʼ�� 
	
	HAL_ADCEx_Calibration_Start(&ADC1_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADCУ׼
}

//hadc:ADC���,�˺����ᱻHAL_ADC_Init()����,ADC�ײ��������������ã�ʱ��ʹ��
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC12_CLK_ENABLE();           //ʹ��ADC1/2ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
	  __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP); //ADC����ʱ��ѡ��
	
    GPIO_Initure.Pin=GPIO_PIN_5;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //ģ��
    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

//����ֵ:ת�����,���ADCֵ,ch: ͨ��ֵ 0~16��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
u16 adc1_1c1g_soft_none_value(u32 ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel=ch;                                   //ͨ��
    ADC1_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1������
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//����ʱ��       
	  ADC1_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//���߲ɼ�          		
	  ADC1_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
	  ADC1_ChanConf.Offset=0;   
    HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);        //ͨ������

    HAL_ADC_Start(&ADC1_Handler);                               //����ADC
	
    HAL_ADC_PollForConversion(&ADC1_Handler,10);                //��ѯת��
	return (u16)HAL_ADC_GetValue(&ADC1_Handler);	            //�������һ��ADC1�������ת�����
}
//��ȡָ��ͨ����ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
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
//data[10];[��ʱ������,��������]
/*********************************************************************************************************************************/
#endif

#if SIGNALCHANNEL_ONCECHANGE_ADC_DMA_TWOBUFFER

/* ѡ��ADC��ʱ��Դ */
#define ADC_CLOCK_SOURCE_AHB     /* ѡ��AHBʱ��Դ */
//#define ADC_CLOCK_SOURCE_PLL     /* ѡ��PLLʱ��Դ */

/* ����Cache���API��������32�ֽڶ��� */
ALIGN_32BYTES(__attribute__((section (".RAM_D3"))) uint16_t ADCxValues[128]);
__IO u32 s_DmaFlag = 0;  /* 1��ʾ��DMA�봫������жϣ�2��ʾ����DMA��������ж� */
DMA_HandleTypeDef   DmaHandle = {0};	

//*********************************************************************************************************
//*	�� �� ��: TIM1_Config
//*	����˵��: ����TIM1�����ڴ���ADC����ǰ���õ�100KHz����Ƶ��
//*	��    ��: ��									  
//*	�� �� ֵ: ��
//*********************************************************************************************************
static void TIM1_Config(void)
{
	TIM_HandleTypeDef  htim ={0};
	TIM_OC_InitTypeDef sConfig = {0};


	/* ʹ��ʱ�� */  
	__HAL_RCC_TIM1_CLK_ENABLE();
      
    /*-----------------------------------------------------------------------
		bsp.c �ļ��� void SystemClock_Config(void) ������ʱ�ӵ���������: 

        System Clock source       = PLL (HSE)
        SYSCLK(Hz)                = 400000000 (CPU Clock)
        HCLK(Hz)                  = 200000000 (AXI and AHBs Clock)
        AHB Prescaler             = 2
        D1 APB3 Prescaler         = 2 (APB3 Clock  100MHz)
        D2 APB1 Prescaler         = 2 (APB1 Clock  100MHz)
        D2 APB2 Prescaler         = 2 (APB2 Clock  100MHz)
        D3 APB4 Prescaler         = 2 (APB4 Clock  100MHz)

        ��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = APB1 x 2 = 200MHz;
        ��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = APB2 x 2 = 200MHz;
        APB4�����TIMxCLKû�з�Ƶ�����Ծ���100MHz;

        APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14��LPTIM1
        APB2 ��ʱ���� TIM1, TIM8 , TIM15, TIM16��TIM17

        APB4 ��ʱ���� LPTIM2��LPTIM3��LPTIM4��LPTIM5

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

    /* ռ�ձ�50% */
    sConfig.Pulse = 1000;  
    HAL_TIM_OC_ConfigChannel(&htim, &sConfig, TIM_CHANNEL_1);

		/* ����OC1 */
    HAL_TIM_OC_Start(&htim, TIM_CHANNEL_1);
}


//*********************************************************************************************************
//*	�� �� ��: bsp_InitADC
//*	����˵��: ��ʼ��ADC
//*	��    ��: ��
//*	�� �� ֵ: ��
//*********************************************************************************************************
void adc1_1c1g_tim_dma_two_buffer_init(void)
{
  ADC_HandleTypeDef        AdcHandle = {0};
	ADC_ChannelConfTypeDef   sConfig = {0};
	GPIO_InitTypeDef         GPIO_InitStruct;

	
  /* ## - 1 - ����ADC������ʱ�� ####################################### */
#if defined (ADC_CLOCK_SOURCE_PLL)
	/* ����PLL2ʱ��Ϊ��72MHz�������Ƶ����ADC���ʱ��36MHz */
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
  
  /* ʹ��AHBʱ�ӵĻ����������ã�Ĭ��ѡ��*/
  
#endif

	/* ## - 2 - ����ADC����ʹ�õ����� ####################################### */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* ## - 3 - ����ADC����ʹ�õ�ʱ�� ####################################### */
	__HAL_RCC_DMA1_CLK_ENABLE();
	DmaHandle.Instance                 = DMA1_Stream2;            /* ʹ�õ�DMA1 Stream2 */
	DmaHandle.Init.Request             = DMA_REQUEST_ADC1;  	  /* �������Ͳ���DMA_REQUEST_ADC1 */  
	DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;    /* ���䷽���Ǵ����赽�洢�� */  
	DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;        /* �����ַ������ֹ */ 
	DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;         /* �洢����ַ����ʹ�� */  
	DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;  /* �������ݴ���λ��ѡ����֣���16bit */     
	DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;  /* �洢�����ݴ���λ��ѡ����֣���16bit */    
	DmaHandle.Init.Mode                = DMA_CIRCULAR;            /* ѭ��ģʽ */   
	DmaHandle.Init.Priority            = DMA_PRIORITY_LOW;        /* ���ȼ��� */  
	DmaHandle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;    /* ��ֹFIFO*/
	DmaHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL; /* ��ֹFIFO��λ�������ã��������÷�ֵ */
	DmaHandle.Init.MemBurst            = DMA_MBURST_SINGLE;       /* ��ֹFIFO��λ�������ã����ڴ洢��ͻ�� */
	DmaHandle.Init.PeriphBurst         = DMA_PBURST_SINGLE;       /* ��ֹFIFO��λ�������ã���������ͻ�� */
 
  /* ��ʼ��DMA */
  HAL_DMA_Init(&DmaHandle);
    
  /* ����DMA1 Stream1���ж� */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
    
  /* ����ADC�����DMA��� */
	__HAL_LINKDMA(&AdcHandle, DMA_Handle, DmaHandle);

	/* ## - 4 - ����ADC ########################################################### */
	__HAL_RCC_ADC12_CLK_ENABLE();
	AdcHandle.Instance = ADC1;
	
#if defined (ADC_CLOCK_SOURCE_PLL)
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV2;          /* ����PLL�첽ʱ�ӣ�2��Ƶ����72MHz/2 = 36MHz */
#elif defined (ADC_CLOCK_SOURCE_AHB)
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;      /* ����AHBͬ��ʱ�ӣ�4��Ƶ����200MHz/4 = 50MHz */
#endif
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_16B;            /* 16λ�ֱ��� */
	AdcHandle.Init.ScanConvMode          = ADC_SCAN_DISABLE;              /* ��ֹɨ�裬��Ϊ������һ��ͨ�� */
	AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;           /* EOCת��������־ */
	AdcHandle.Init.LowPowerAutoWait      = DISABLE;                       /* ��ֹ�͹����Զ��ӳ����� */
	AdcHandle.Init.ContinuousConvMode    = DISABLE;                       /* ��ֹ�Զ�ת�������õĶ�ʱ������ת�� */
	AdcHandle.Init.NbrOfConversion       = 1;                             /* ʹ����1��ת��ͨ�� */
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* ��ֹ������ģʽ */
	AdcHandle.Init.NbrOfDiscConversion   = 1;                             /* ��ֹ������ģʽ�󣬴˲������ԣ���λ���������ò�����������ͨ���� */
	AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIG_T1_CC1;            /* ��ʱ��1��CC1���� */
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;    /* �����ش��� */
	AdcHandle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* DMAѭ��ģʽ����ADCת�������� */
	AdcHandle.Init.BoostMode                = ENABLE;                          /* ADCʱ�ӳ���20MHz�Ļ���ʹ��boost */
	AdcHandle.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* ADCת������Ļ�������ADC�����ݼĴ��� */
	AdcHandle.Init.OversamplingMode         = DISABLE;                         /* ��ֹ������ */

  /* ��ʼ��ADC */
	HAL_ADC_Init(&AdcHandle);
	/* У׼ADC������ƫ��У׼ */
	HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
  
	/* ����ADCͨ��  */
	sConfig.Channel      = ADC_CHANNEL_19;              /* ����ʹ�õ�ADCͨ�� */
	sConfig.Rank         = ADC_REGULAR_RANK_1;          /* ����������ĵ�1�� */
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;     /* �������� */
	sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* �������� */
	sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* ��ƫ�� */ 
	sConfig.Offset = 0;                                 /* ��ƫ�Ƶ�����£��˲������� */

	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
  
	/* ## - 5 - ����ADC�Ķ�ʱ������ ####################################### */
	TIM1_Config();
  
	/* ## - 6 - ����ADC��DMA��ʽ���� ####################################### */
	HAL_ADC_Start_DMA(&AdcHandle, (uint32_t *)ADCxValues, 128);
}


//*********************************************************************************************************
//*	�� �� ��: DMA1_Stream1_IRQHandler
//*	����˵��: DMA1 Stream1�жϷ������
//*	��    ��: ��
//*	�� �� ֵ: ��
//**********************************************************************************************************
void DMA1_Stream2_IRQHandler(void)
{
	/* ��������ж� */
	if((DMA1->LISR & DMA_FLAG_TCIF1_5) != RESET)
	{
				/*
		   1��ʹ�ô˺���Ҫ�ر�ע�⣬��1��������ַҪ32�ֽڶ��룬��2������Ҫ��32�ֽڵ���������
		   2�����봫������жϣ���ǰDMA����ʹ�û�������ǰ�벿�֣��û����Բ�����벿�֡�
		*/
		SCB_InvalidateDCache_by_Addr((uint32_t *)(&ADCxValues[64]), 128);
		
		s_DmaFlag = 2;
		
		/* �����־ */
		DMA1->LIFCR = DMA_FLAG_TCIF1_5;
	}
	/* �봫������ж� */    
	if((DMA1->LISR & DMA_FLAG_HTIF1_5) != RESET)
	{
		/*
		   1��ʹ�ô˺���Ҫ�ر�ע�⣬��1��������ַҪ32�ֽڶ��룬��2������Ҫ��32�ֽڵ���������
		   2������봫������жϣ���ǰDMA����ʹ�û������ĺ�벿�֣��û����Բ���ǰ�벿�֡�
		*/
		SCB_InvalidateDCache_by_Addr((uint32_t *)(&ADCxValues[0]), 128);
		s_DmaFlag = 1;
        
		/* �����־ */
		DMA1->LIFCR = DMA_FLAG_HTIF1_5;
	}

	/* ��������ж� */
	if((DMA1->LISR & DMA_FLAG_TEIF1_5) != RESET)
	{
		/* �����־ */
		DMA1->LIFCR = DMA_FLAG_TEIF1_5;
	}

	/* ֱ��ģʽ�����ж� */
	if((DMA1->LISR & DMA_FLAG_DMEIF1_5) != RESET)
	{
		/* �����־ */
		DMA1->LIFCR = DMA_FLAG_DMEIF1_5;
	}
}
#endif
#if MULTCHANNEL_ONCECHANGE_ADC_DMA
/* ѡ��ADC��ʱ��Դ */
#define ADC_CLOCK_SOURCE_AHB     /* ѡ��AHBʱ��Դ */
//#define ADC_CLOCK_SOURCE_PLL     /* ѡ��PLLʱ��Դ */

/* ����Cache���API��������32�ֽڶ��� */
ALIGN_32BYTES(__attribute__((section (".RAM_D3"))) uint16_t ADCxValues[4]);

DMA_HandleTypeDef   DMA_Handle = {0};

//*********************************************************************************************************
//*	�� �� ��: bsp_InitADC
//*	����˵��: ��ʼ��ADC������DMA��ʽ���ж�ͨ���������ɼ���PC0, Vbat/4, VrefInt���¶�
//*	��    ��: ��
//*	�� �� ֵ: ��
//*********************************************************************************************************
void adc3_nc1g_soft_dma_init(void)
{
	ADC_HandleTypeDef   AdcHandle = {0};
	ADC_ChannelConfTypeDef   sConfig = {0};
	GPIO_InitTypeDef          GPIO_InitStruct;

  /* ## - 1 - ����ADC������ʱ�� ####################################### */
#if defined (ADC_CLOCK_SOURCE_PLL)
	/* ����PLL2ʱ��Ϊ��72MHz�������Ƶ����ADC���ʱ��36MHz */
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
  /* ʹ��AHBʱ�ӵĻ����������ã�Ĭ��ѡ��*/ 
#endif

	/* ## - 2 - ����ADC����ʹ�õ�IO   ####################################### */
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
	/* ## - 3 - ����ADC����ʹ�õ�ʱ�� ####################################### */
	__HAL_RCC_DMA1_CLK_ENABLE();
	DMA_Handle.Instance                 = DMA1_Stream1;            /* ʹ�õ�DMA1 Stream1 */
	DMA_Handle.Init.Request             = DMA_REQUEST_ADC3;  	   /* �������Ͳ���DMA_REQUEST_ADC3 */  
	DMA_Handle.Init.Direction           = DMA_PERIPH_TO_MEMORY;    /* ���䷽���ǴӴ洢�������� */  
	DMA_Handle.Init.PeriphInc           = DMA_PINC_DISABLE;        /* �����ַ������ֹ */ 
	DMA_Handle.Init.MemInc              = DMA_MINC_ENABLE;         /* �洢����ַ����ʹ�� */  
	DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;   /* �������ݴ���λ��ѡ����֣���16bit */     
	DMA_Handle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;   /* �洢�����ݴ���λ��ѡ����֣���16bit */    
	DMA_Handle.Init.Mode                = DMA_CIRCULAR;            /* ѭ��ģʽ */   
	DMA_Handle.Init.Priority            = DMA_PRIORITY_LOW;        /* ���ȼ��� */  
	DMA_Handle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;    /* ��ֹFIFO*/
	DMA_Handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL; /* ��ֹFIFO��λ�������ã��������÷�ֵ */
	DMA_Handle.Init.MemBurst            = DMA_MBURST_SINGLE;       /* ��ֹFIFO��λ�������ã����ڴ洢��ͻ�� */
	DMA_Handle.Init.PeriphBurst         = DMA_PBURST_SINGLE;       /* ��ֹFIFO��λ�������ã���������ͻ�� */

	/* ��ʼ��DMA */
	HAL_DMA_Init(&DMA_Handle);

	/* ����ADC�����DMA��� */
	__HAL_LINKDMA(&AdcHandle, DMA_Handle, DMA_Handle);
	
	/* ## - 4 - ����ADC ########################################################### */
	__HAL_RCC_ADC3_CLK_ENABLE();
	AdcHandle.Instance = ADC3;

#if defined (ADC_CLOCK_SOURCE_PLL)
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV8;          /* ����PLL�첽ʱ�ӣ�8��Ƶ����72MHz/8 = 36MHz */
#elif defined (ADC_CLOCK_SOURCE_AHB)
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;      /* ����AHBͬ��ʱ�ӣ�4��Ƶ����200MHz/4 = 50MHz */
#endif
	
	AdcHandle.Init.Resolution            = ADC_RESOLUTION_16B;        /* 16λ�ֱ��� */
	AdcHandle.Init.ScanConvMode          = ADC_SCAN_ENABLE;           /* ��ֹɨ�裬��Ϊ������һ��ͨ�� */
	AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;       /* EOCת��������־ */
	AdcHandle.Init.LowPowerAutoWait      = DISABLE;                   /* ��ֹ�͹����Զ��ӳ����� */
	AdcHandle.Init.ContinuousConvMode    = ENABLE;                    /* ��ֹ�Զ�ת�������õ�������� */
	AdcHandle.Init.NbrOfConversion       = 4;                         /* ʹ����4��ת��ͨ�� */
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;                   /* ��ֹ������ģʽ */
	AdcHandle.Init.NbrOfDiscConversion   = 1;                         /* ��ֹ������ģʽ�󣬴˲������ԣ���λ���������ò�����������ͨ���� */
	AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;        /* ����������� */
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;    /* ������������Ļ�����λ���� */
	AdcHandle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* DMAѭ��ģʽ����ADCת�������� */
	AdcHandle.Init.BoostMode             = DISABLE;                            /* ADCʱ�ӵ���20MHz�Ļ������Խ�ֹboost */
	AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;     	   /* ADCת������Ļ�������ADC�����ݼĴ��� */
	AdcHandle.Init.OversamplingMode      = DISABLE;                            /* ��ֹ������ */
	
/******oversampling setting********/
//	AdcHandle.Init.OversamplingMode         		  = ENABLE;                        /* ʹ�ܹ����� */
//	AdcHandle.Init.Oversampling.Ratio                 = 15;                            /* 15+1�������� */
//	AdcHandle.Init.Oversampling.RightBitShift         = ADC_RIGHTBITSHIFT_4;           /* ��������4bit��������16 */
//	AdcHandle.Init.Oversampling.TriggeredMode         = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;   /* �������������ÿ��ͨ�������й�����ת�� */
//	AdcHandle.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE; /* �������ע��ת��������������ʱֹͣ��������ע
//                                                                                               ��������ɺ������ע�����й����лᱣ��������������*/    
    /* ��ʼ��ADC */
	HAL_ADC_Init(&AdcHandle);
  
	/* У׼ADC������ƫ��У׼ */
	HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

	/* ����ADCͨ��������1������PC0���� */
	/*
		����PLL2ʱ�ӵĻ���ADCCLK = 72MHz / 8 = 9MHz
	    ADC�����ٶȣ���ת��ʱ�� = ����ʱ�� + ��αƽ�ʱ��
	                            = 810.5 + 8.5(16bit)
	                            = 820��ADCʱ������
	    ��ôת���ٶȾ���9MHz / 820 = 10975Hz
	*/
	sConfig.Channel      = ADC_CHANNEL_10;              /* ����ʹ�õ�ADCͨ�� */
	sConfig.Rank         = ADC_REGULAR_RANK_1;          /* ����������ĵ�1�� */
	sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;  /* �������� */
	sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* �������� */
	sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* ��ƫ�� */ 
	sConfig.Offset = 0;                                 /* ��ƫ�Ƶ�����£��˲������� */
	sConfig.OffsetRightShift       = DISABLE;           /* ��ֹ���� */
	sConfig.OffsetSignedSaturation = DISABLE;           /* ��ֹ�з��ű��� */
	
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
	
	/* ����ADCͨ��������2������Vbat/4 */
	sConfig.Channel      = ADC_CHANNEL_VBAT_DIV4;       /* ����ʹ�õ�ADCͨ�� */
	sConfig.Rank         = ADC_REGULAR_RANK_2;          /* ����������ĵ�1�� */
	sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;  /* �������� */
	sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* �������� */
	sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* ��ƫ�� */ 
	sConfig.Offset = 0;                                 /* ��ƫ�Ƶ�����£��˲������� */
	sConfig.OffsetRightShift       = DISABLE;           /* ��ֹ���� */
	sConfig.OffsetSignedSaturation = DISABLE;           /* ��ֹ�з��ű��� */
	
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
  
	/* ����ADCͨ��������3������VrefInt */
	sConfig.Channel      = ADC_CHANNEL_VREFINT;         /* ����ʹ�õ�ADCͨ�� */
	sConfig.Rank         = ADC_REGULAR_RANK_3;          /* ����������ĵ�1�� */
	sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;  /* �������� */
	sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* �������� */
	sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* ��ƫ�� */ 
	sConfig.Offset = 0;                                 /* ��ƫ�Ƶ�����£��˲������� */
	sConfig.OffsetRightShift       = DISABLE;           /* ��ֹ���� */
	sConfig.OffsetSignedSaturation = DISABLE;           /* ��ֹ�з��ű��� */
	
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);

	/* ����ADCͨ��������4�������¶� */
	sConfig.Channel      = ADC_CHANNEL_TEMPSENSOR;      /* ����ʹ�õ�ADCͨ�� */
	sConfig.Rank         = ADC_REGULAR_RANK_4;          /* ����������ĵ�1�� */
	sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;  /* �������� */
	sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* �������� */
	sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* ��ƫ�� */ 
	sConfig.Offset = 0;                                 /* ��ƫ�Ƶ�����£��˲������� */
	sConfig.OffsetRightShift       = DISABLE;           /* ��ֹ���� */
	sConfig.OffsetSignedSaturation = DISABLE;           /* ��ֹ�з��ű��� */
	
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
  

	/* ## - 6 - ����ADC��DMA��ʽ���� ####################################### */
	HAL_ADC_Start_DMA(&AdcHandle, (uint32_t *)ADCxValues, 4);
}

//*********************************************************************************************************
//*	�� �� ��: bsp_GetAdcValues
//*	����˵��: ��ȡADC�����ݲ���ӡ
//*	��    ��: ��
//*	�� �� ֵ: ��
//*********************************************************************************************************
void adc3_nc1g_soft_dma_value(void)
{
    float AdcValues[5];
    uint16_t TS_CAL1;
    uint16_t TS_CAL2;
    
    /*
       ʹ�ô˺���Ҫ�ر�ע�⣬��1��������ַҪ32�ֽڶ��룬��2������Ҫ��32�ֽڵ�������
    */
    SCB_InvalidateDCache_by_Addr((uint32_t *)ADCxValues,  sizeof(ADCxValues));
    AdcValues[0] = ADCxValues[0] * 3.3 / 65536;
    AdcValues[1] = ADCxValues[1] * 3.3 / 65536; 
    AdcValues[2] = ADCxValues[2] * 3.3 / 65536;     
 
    /* ���ݲο��ֲ���Ĺ�ʽ�����¶�ֵ */
    TS_CAL1 = *(__IO uint16_t *)(0x1FF1E820);
    TS_CAL2 = *(__IO uint16_t *)(0x1FF1E840);
    
    AdcValues[3] = (110.0 - 30.0) * (ADCxValues[3] - TS_CAL1)/ (TS_CAL2 - TS_CAL1) + 30;  
    
    printf("PC0 = %5.3fV, Vbat/4 = %5.3fV, VrefInt = %5.3fV�� TempSensor = %5.3f��\r\n", 
            AdcValues[0],  AdcValues[1], AdcValues[2], AdcValues[3]);

}



#endif

