#ifndef __INNERADC_H
#define __INNERADC_H
#include "matrixos.h" 	

/***********ADC MODE********************************//*和下列头文件区域一致*/
#define SIGNALCHANNEL_ONCECHANGE_ADC 0
#define SIGNALCHANNEL_ONCECHANGE_ADC_DMA_TWOBUFFER 0
#define MULTCHANNEL_ONCECHANGE_ADC_DMA 1

#if SIGNALCHANNEL_ONCECHANGE_ADC
/*************************************signal channel************************************************************/
//PA5->ADC_CHANNEL_19

//单通道 单次转化 软件触发初始化
void adc1_1c1g_soft_none_init(void);//adcx_xc(通道数)xg(转化数)_(触发)_(特殊项)_init

/*adc1_signalchannel_once_value*///获得某个通道的单次转换值
u16 adc1_1c1g_soft_none_value(u32 ch);

/*adc1_signalchannel_once_average_value*///得到某个通道给定次数采样的平均值
u16 adc1_1c1g_soft_none_averagevalue(u32 ch,u8 times);

/**example:
float volt=(float)Get_Adc(ADC_CHANNEL_19)*(3.3/65536);
**/
#endif

#if SIGNALCHANNEL_ONCECHANGE_ADC_DMA_TWOBUFFER
/*************************************signal channel************************************************************/
//PA5->ADC_CHANNEL_19

//单通道 定时器触发转换
void adc1_1c1g_tim_dma_two_buffer_init(void);
/**example:
	adc1_1c1g_tim_dma_two_buffer_init();
	u32 temp;
	// The current DMA operation is post-semi-buffered, so we can manipulate the pre-semi-buffered
	if(s_DmaFlag == 1)
	{
		DISABLE_INT();
		s_DmaFlag = 0;
		values = (ADCxValues[0] + ADCxValues[1] + ADCxValues[2] + ADCxValues[3])/4;

		ENABLE_INT();
	}
	// The first half-buffer of the current DMA operation, so we can operate the second half-buffer
	else if(s_DmaFlag == 2)
	{
		DISABLE_INT();
		s_DmaFlag = 0;
		values = (ADCxValues[64] + ADCxValues[65] + ADCxValues[66] + ADCxValues[67])/4;
		ENABLE_INT();
	}
	float value=temp *3.3 / 65536;
**/
#endif

#if MULTCHANNEL_ONCECHANGE_ADC_DMA
/********************************multiple channel************************************************************/
void adc3_nc1g_soft_dma_init(void);
void adc3_nc1g_soft_dma_value(void);
#endif
#endif 


