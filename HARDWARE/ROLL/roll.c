#include "roll.h"

ADC_HandleTypeDef ADC2_Handler;//ADC���
//PC4 PC5 ADC2INP4 ADC2INP8 x y
//ʹ�õ�ʱ������Msp����

//��ʼ��ADC
//ch: ADC_channels 
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
void roll_Init(void)
{ 
    ADC2_Handler.Instance=ADC2;
    ADC2_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4��Ƶ��ADCCLK=PER_CK/4=64/4=16MHZ
    ADC2_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16λģʽ
    ADC2_Handler.Init.ScanConvMode=DISABLE;                    	//��ɨ��ģʽ
    ADC2_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//�ر�EOC�ж�
	  ADC2_Handler.Init.LowPowerAutoWait=DISABLE;					//�Զ��͹��Ĺر�				
    ADC2_Handler.Init.ContinuousConvMode=DISABLE;               //�ر�����ת��
    ADC2_Handler.Init.NbrOfConversion=1;                        //1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADC2_Handler.Init.DiscontinuousConvMode=DISABLE;            //��ֹ����������ģʽ
    ADC2_Handler.Init.NbrOfDiscConversion=0;                    //����������ͨ����Ϊ0
    ADC2_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //�������
    ADC2_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//ʹ���������
	  ADC2_Handler.Init.BoostMode=ENABLE;							//BOOTģʽ�ر�
	  ADC2_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			//���µ����ݵ�����ֱ�Ӹ��ǵ�������
	  ADC2_Handler.Init.OversamplingMode=DISABLE;					//�������ر�
	  ADC2_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //����ͨ�������ݽ���������DR�Ĵ�������
    HAL_ADC_Init(&ADC2_Handler);                                 //��ʼ�� 
		HAL_ADCEx_Calibration_Start(&ADC2_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADCУ׼
}
/*
//ADC�ײ��������������ã�ʱ��ʹ��
//�˺����ᱻHAL_ADC_Init()����
//hadc:ADC���
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC12_CLK_ENABLE();           //ʹ��ADC1/2ʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOAʱ��
	  __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP); //ADC����ʱ��ѡ��
	
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_4;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //ģ��
    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
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
    
	ADC2_ChanConf.Channel=ADC_CHANNEL_8;                                   //ͨ��
	ADC2_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1������
	ADC2_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//����ʱ��       
	ADC2_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//���߲ɼ�          		
	ADC2_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
	ADC2_ChanConf.Offset=0;   
	HAL_ADC_ConfigChannel(&ADC2_Handler,&ADC2_ChanConf);        //ͨ������

	HAL_ADC_Start(&ADC2_Handler);                               //����ADC
	
	HAL_ADC_PollForConversion(&ADC2_Handler,10);                //��ѯת��
	return (float)HAL_ADC_GetValue(&ADC2_Handler)*(3.3/65536);           //�������һ��ADC1�������ת�����
}
float get_x_channel(void)
{
	ADC_ChannelConfTypeDef ADC2_ChanConf;
    
  ADC2_ChanConf.Channel=ADC_CHANNEL_4;                                   //ͨ��
  ADC2_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1������
  ADC2_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//����ʱ��       
	ADC2_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//���߲ɼ�          		
	ADC2_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
  ADC2_ChanConf.Offset=0;   
  HAL_ADC_ConfigChannel(&ADC2_Handler,&ADC2_ChanConf);        //ͨ������

  HAL_ADC_Start(&ADC2_Handler);                               //����ADC
	
  HAL_ADC_PollForConversion(&ADC2_Handler,10);                //��ѯת��
	return (float)HAL_ADC_GetValue(&ADC2_Handler)*(3.3/65536);            //�������һ��ADC1�������ת�����
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

