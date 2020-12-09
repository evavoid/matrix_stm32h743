#include "ads1274.h"
#include "fpga_spi.h"
/*
利用硬件spi高速采样，一定要包含spi驱动文件并初始化
引脚定义：PH13 SYNC; PH14 SDRY; PB13 SCLK; PB14 DOUT1
*/
void ADS1274_Init(void)
{
	/*PH13->SYNC,PH14->DRDY*/
	/*PH12->SCLK,PH15->DOUT*///当四通道时不需要他们
   GPIO_InitTypeDef GPIO_Initure;
   __HAL_RCC_GPIOH_CLK_ENABLE();       
	
	/******in_io**********/
	 #if SPI_4_CHANNEL
	 GPIO_Initure.Pin=GPIO_PIN_14;
	 #else
	 GPIO_Initure.Pin=GPIO_PIN_14|GPIO_PIN_15;
	 #endif      
   GPIO_Initure.Mode=GPIO_MODE_INPUT;    
   GPIO_Initure.Pull=GPIO_PULLDOWN;      
   GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;    
   HAL_GPIO_Init(GPIOH,&GPIO_Initure);
	
   /******out_io********/
	 #if SPI_4_CHANNEL
	 GPIO_Initure.Pin=GPIO_PIN_13;
	 #else
	 GPIO_Initure.Pin=GPIO_PIN_13|GPIO_PIN_12;
	 #endif
   GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		
   GPIO_Initure.Pull=GPIO_PULLUP;         			
   GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	
   HAL_GPIO_Init(GPIOH,&GPIO_Initure);     	

	 ADS1274_SYNC_L;
	 delay_ms(1);
	 ADS1274_SYNC_H;
}

//读取AD值
u32 ADS1274ReadData(void)
{
	int i;
	u32 RxData=0;
	while (ADS1274_DRDY){};
	
	for(i=0;i<24;i++)
	{
		RxData = RxData<<1;
		ADS1274_SCLK_H;
		if(ADS1274_DOUT)
		{
				RxData++;
		}
		ADS1274_SCLK_L;;
	}  
	return RxData;
}

void ADS1274ReadData_SPI(u8 *data_in)
{
	while(ADS1274_DRDY){};
		HAL_SPI_Receive(&SPI2_Handler_FPGA,data_in,3,200); 
		HAL_SPI_Receive(&SPI2_Handler_FPGA,data_in+3,3,200); 
		HAL_SPI_Receive(&SPI2_Handler_FPGA,data_in+6,3,200); 	
		HAL_SPI_Receive(&SPI2_Handler_FPGA,data_in+9,3,200); 

	//u32 data_out=((u32)data_in[2]<<16)+((u32)data_in[1]<<8)+((u32)data_in[0]);
//	printf("%x,%x,%x\r\n",data_in[0],data_in[1],data_in[2]);
//	printf("%x,%x,%x\r\n",data_in[3],data_in[4],data_in[5]);
//	printf("%x,%x,%x\r\n",data_in[6],data_in[7],data_in[8]);
//	printf("%x,%x,%x\r\n",data_in[9],data_in[10],data_in[11]);
}

/*ADS1274电压转换函数*/
/*ADS1274的编码方式为二级制补码,但实测与手册上提供的转换公式不对应，这是因为我们使用的AD板差分输入画反了
 * 采样正的直流电压时按原编码方式得出负电压的结果，故我们的转换做出相应的改变*///这也太扯淡了-王天可
/*编码范围800000h~FFFFFh*/
u32 ADS1274_VolConv(u32 data)
{
    u32 VolConv=0;
    VolConv=2500000-(data-8388608)*0.2980232594;//2500000/8388607
    if((int)VolConv==2500000)
        VolConv=0;
    return VolConv;
}

double ADS1274_Data2Vol(u32 data)
{
    double volt;
    if(data==0x7FFFFF)
        volt=-2500000;
    else if(data<0x7FFFFF)
        volt=data*LSB_N;
    else if(data>0x800000)
        volt=2494540-(data-0x800000)*LSB_P;
    else if(data==0x800000)
        volt=2494540;
    return volt;
}

void ADS1274_DATAconv(u8 *data_in, double *data_out)
{
	u32 data_conv[4];
	/**************************/
	data_conv[0]=((u32)data_in[2])|((u32)data_in[1]<<8)|((u32)data_in[0]<<16);
	data_conv[1]=((u32)data_in[5])|((u32)data_in[4]<<8)|((u32)data_in[3]<<16);
	data_conv[2]=((u32)data_in[8])|((u32)data_in[7]<<8)|((u32)data_in[6]<<16);
	data_conv[3]=((u32)data_in[11])|((u32)data_in[10]<<8)|((u32)data_in[9]<<16);
	/**************************/
	for(u8 i=0; i<4; i++)
	{
		if(data_conv[i]==0x7FFFFF)
				data_out[i]=-2500000;
		else if(data_conv[i]<0x7FFFFF)
				data_out[i]=data_conv[i]*LSB_N;
		else if(data_conv[i]>0x800000)
				data_out[i]=2494540-(data_conv[i]-0x800000)*LSB_P;
		else if(data_conv[i]==0x800000)
				data_out[i]=2494540;
	}
}
u8 spi_test[98304];
void ADS1274_getlist_value(double *data_out,u32 blocksize)
{
	for(u32 i=0; i<blocksize; i++)
	{
			while(ADS1274_DRDY){};
				HAL_SPI_Receive(&SPI2_Handler_FPGA,spi_test+i*12,3,200); 
				HAL_SPI_Receive(&SPI2_Handler_FPGA,spi_test+i*12+3,3,200); 
				HAL_SPI_Receive(&SPI2_Handler_FPGA,spi_test+i*12+6,3,200); 	
				HAL_SPI_Receive(&SPI2_Handler_FPGA,spi_test+i*12+9,3,200);   
	}
	for(u32 i=0; i<blocksize; i++)ADS1274_DATAconv(&spi_test[12*i], &data_out[4*i]);
}
