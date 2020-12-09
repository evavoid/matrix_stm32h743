#ifndef __FPGASPI_H
#define __FPGASPI_H
#include "matrixos.h"

extern SPI_HandleTypeDef SPI2_Handler_FPGA;  //SPI句柄
extern DMA_HandleTypeDef  SPI2RxDMA_Handler;      //DMA句柄
extern DMA_HandleTypeDef  SPI2TxDMA_Handler;      //DMA句柄
/*******************************************base init******************************************************/
void fpga_32_init(void);
void DMA_ConfigRX(DMA_Stream_TypeDef *DMA_Streamx);
void DMA_ConfigTX(DMA_Stream_TypeDef *DMA_Streamx);
/********************************************user interface************************************************/
//总的初始化函数，包含了DMA_ConfigTX，DMA_ConfigRX，fpga_32_init，若不开dma则只初始化最后一个即可 
void fpga_spi_stm32_init(void);
void FPGA_ReadWriteByte(u8* TxData,u8 *Rxdata,u16 num);//非dma下使用
void FPGA_ReadWriteByte_DMA(u8* TxData,u8 *Rxdata,u16 num);//dma模式下使用
/*****************************************example**********************************************************/	
	/*
		fpga_spi_stm32_init();
		u8 outdata[16]={123,29,75,55,97,01,75,55,123,29,75,55,97,29,75,55};
		u8 indata[16]={0};
		//Cache_Enable();                 //打开L1-Cache
		//开启三个中断
		FPGA_ReadWriteByte_DMA(outdata,indata,4);
	*/
	/*
		fpga_32_init();
		u8 outdata[3]={123,29,75};
		u8 indata[3]={0};
		FPGA_ReadWriteByte(outdata,indata,3);
	*/
#endif

