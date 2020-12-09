#include "ad8302.h"
ADC_HandleTypeDef ADCD_Handler;//ADC���
ADC_HandleTypeDef ADCC_Handler;//ADC���
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
    ADCD_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4��Ƶ��ADCCLK=PER_CK/4=64/4=16MHZ
    ADCD_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16λģʽ
    ADCD_Handler.Init.ScanConvMode=DISABLE;                    	//��ɨ��ģʽ
    ADCD_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//�ر�EOC�ж�
	  ADCD_Handler.Init.LowPowerAutoWait=DISABLE;					//�Զ��͹��Ĺر�				
    ADCD_Handler.Init.ContinuousConvMode=DISABLE;               //�ر�����ת��
    ADCD_Handler.Init.NbrOfConversion=1;                        //1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADCD_Handler.Init.DiscontinuousConvMode=DISABLE;            //��ֹ����������ģʽ
    ADCD_Handler.Init.NbrOfDiscConversion=0;                    //����������ͨ����Ϊ0
    ADCD_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //�������
    ADCD_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//ʹ���������
	  ADCD_Handler.Init.BoostMode=ENABLE;							//BOOTģʽ�ر�
	  ADCD_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			//���µ����ݵ�����ֱ�Ӹ��ǵ�������
	  ADCD_Handler.Init.OversamplingMode=DISABLE;					//�������ر�
	  ADCD_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //����ͨ�������ݽ���������DR�Ĵ�������
    HAL_ADC_Init(&ADCD_Handler);                                 //��ʼ�� 
	
		GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC12_CLK_ENABLE();           //ʹ��ADC1/2ʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOAʱ��
	  __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP); //ADC����ʱ��ѡ��
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_4;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //ģ��
    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
		
		HAL_ADCEx_Calibration_Start(&ADCD_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADCУ׼
	
	  /******************************/
	
		ADCC_Handler.Instance=ADC1;
    ADCC_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4��Ƶ��ADCCLK=PER_CK/4=64/4=16MHZ
    ADCC_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16λģʽ
    ADCC_Handler.Init.ScanConvMode=DISABLE;                    	//��ɨ��ģʽ
    ADCC_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//�ر�EOC�ж�
	  ADCC_Handler.Init.LowPowerAutoWait=DISABLE;					//�Զ��͹��Ĺر�				
    ADCC_Handler.Init.ContinuousConvMode=DISABLE;               //�ر�����ת��
    ADCC_Handler.Init.NbrOfConversion=1;                        //1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADCC_Handler.Init.DiscontinuousConvMode=DISABLE;            //��ֹ����������ģʽ
    ADCC_Handler.Init.NbrOfDiscConversion=0;                    //����������ͨ����Ϊ0
    ADCC_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //�������
    ADCC_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//ʹ���������
	  ADCC_Handler.Init.BoostMode=ENABLE;							//BOOTģʽ�ر�
	  ADCC_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			//���µ����ݵ�����ֱ�Ӹ��ǵ�������
	  ADCC_Handler.Init.OversamplingMode=DISABLE;					//�������ر�
	  ADCC_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //����ͨ�������ݽ���������DR�Ĵ�������
		HAL_ADC_Init(&ADCC_Handler);      
		
    __HAL_RCC_ADC12_CLK_ENABLE();           //ʹ��ADC1/2ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
	  __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP); //ADC����ʱ��ѡ��
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_4;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //ģ��
    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);                      //��ʼ�� 

		HAL_ADCEx_Calibration_Start(&ADCC_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADCУ׼
		/************************/
		ADCC_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1������
		ADCC_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//����ʱ��       
		ADCC_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//���߲ɼ�          		
		ADCC_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
		ADCC_ChanConf.Offset=0;   
		/************************/
		ADCD_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1������
		ADCD_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//����ʱ��       
		ADCD_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//���߲ɼ�          		
		ADCD_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
		ADCD_ChanConf.Offset=0;   
}
//ADC1 ADC_CHANNEL_19 PA5 ADC_C1
//ADC1 ADC_CHANNEL_18 PA4 ADC_C2
//exp:get_ADC_C(1);get_ADC_C(2);
float get_ADC_C(u8 type)
{
  if(type==1)ADCC_ChanConf.Channel=ADC_CHANNEL_19;   //ͨ��
	else if(type==2)ADCC_ChanConf.Channel=ADC_CHANNEL_18; 
	
	HAL_ADC_ConfigChannel(&ADCC_Handler,&ADCC_ChanConf);        //ͨ������
	HAL_ADC_Start(&ADCC_Handler);                               //����ADC
	HAL_ADC_PollForConversion(&ADCC_Handler,10);                //��ѯת��
	
	return (float)HAL_ADC_GetValue(&ADCC_Handler)*1100/65536/10-30;           //�������һ��ADC1�������ת�����
}
//ADC2 ADC_CHANNEL_4  PC4 ADC_D1
//ADC2 ADC_CHANNEL_8  PC5 ADC_D2
//exp:get_ADC_D(1);get_ADC_D(2);
float get_ADC_D(u8 type)//�ر�ע�ⷵ��ʱ���double
{
  if(type==1)ADCD_ChanConf.Channel=ADC_CHANNEL_4;   //ͨ��
	else if(type==2)ADCD_ChanConf.Channel=ADC_CHANNEL_8; 
	
	HAL_ADC_ConfigChannel(&ADCD_Handler,&ADCD_ChanConf);        //ͨ������
	HAL_ADC_Start(&ADCD_Handler);                               //����ADC
	HAL_ADC_PollForConversion(&ADCD_Handler,10);                //��ѯת��
		
	return 180-(double)HAL_ADC_GetValue(&ADCD_Handler)*5.0354/1000;          //�������һ��ADC1�������ת�����
}

void ad8302_event(local_state_space *AD8302_state)
{
	if(AD8302_state->ready_state==prepare_state)
	{	
		/*                            --->C1��ֵ��
			I(90 degree)\              /
									 ---->(AD8302)X---->D1��ǲ�ľ���ֵ
			O(00 degree)X              
									 ---->(AD8302)X---->C2��ֵ��
			Q(00 degree)/              \
		                              --->D2��ǲ�ľ���ֵ
		*/
	}
	else if(AD8302_state->ready_state==run_state0)//��һɨ��ģʽ
	{
		//����Ƶ�ʵ�  AD9959_IOQ_Output(1,2,3,Ƶ��ֵ,1000);
		//��ֵƫ��(O·�ź�)=(get_ADC_C(1)-get_ADC_C(2))/2+��ǰƵ�ʵķ�ֵ�������-6;
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
			��λƫ��(O·�ź�)=(d1_temp*s1-d2_temp*s2-90)/2+��ǰƵ�ʵ���λ�������;
		*/
	}
	else if(AD8302_state->ready_state==run_state1)//��һ��״̬
	{
		//����Ƶ�ʵ�  AD9959_IOQ_Output(1,2,3,Ƶ��ֵ,1000);
		//��ǰƵ�ʵķ�ֵ�������=6-(get_ADC_C(1)-get_ADC_C(2))/2;
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
			��ǰƵ�ʵ���λ�������=(90+d2_temp*s2-d1_temp*s1)/2;
		*/
	}
}

