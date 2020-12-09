#include "fpga_spi.h"
/*
不可信，具体请看cube
H743端接口定义：SPI2
SPI_CE   PB12
SPI_CLK  PB13
SPI_MISO PB14
*/
/************************************handle&irq*************************************************************/
SPI_HandleTypeDef  SPI2_Handler_FPGA;      //SPI2句柄
DMA_HandleTypeDef  SPI2RxDMA_Handler;      //DMA句柄
DMA_HandleTypeDef  SPI2TxDMA_Handler;      //DMA句柄
void DMA1_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&SPI2RxDMA_Handler);
}
void DMA1_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&SPI2TxDMA_Handler);
}
void SPI2_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&SPI2_Handler_FPGA);
}
/************************************base init*************************************************************/
void fpga_32_init(void)
{
	/*spi init************************************************************/
	SPI2_Handler_FPGA.Instance=SPI2;                      //SP2
  SPI2_Handler_FPGA.Init.Mode=SPI_MODE_MASTER;          //设置SPI工作模式，设置为主模式
  SPI2_Handler_FPGA.Init.Direction=SPI_DIRECTION_2LINES;//设置SPI单向或者双向的数据模式:SPI设置为双线模式
	
  SPI2_Handler_FPGA.Init.DataSize=SPI_DATASIZE_8BIT;    //设置SPI的数据大小:SPI发送接收8位帧结构
	
  SPI2_Handler_FPGA.Init.CLKPolarity=SPI_POLARITY_LOW; //串行同步时钟的空闲状态为低电平
  SPI2_Handler_FPGA.Init.CLKPhase=SPI_PHASE_1EDGE;      //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
  SPI2_Handler_FPGA.Init.NSS=SPI_NSS_HARD_OUTPUT;              //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
  SPI2_Handler_FPGA.Init.NSSPMode=SPI_NSS_PULSE_ENABLE;//NSS信号脉冲失能
  SPI2_Handler_FPGA.Init.MasterKeepIOState=SPI_MASTER_KEEP_IO_STATE_ENABLE;  //SPI主模式IO状态保持使能
  SPI2_Handler_FPGA.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_16;//定义波特率预分频的值:波特率预分频值为256
  SPI2_Handler_FPGA.Init.FirstBit=SPI_FIRSTBIT_MSB;     //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
  SPI2_Handler_FPGA.Init.TIMode=SPI_TIMODE_DISABLE;     //关闭TI模式
  SPI2_Handler_FPGA.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//关闭硬件CRC校验
  SPI2_Handler_FPGA.Init.CRCPolynomial=7;               //CRC值计算的多项式
  HAL_SPI_Init(&SPI2_Handler_FPGA);
	/*gpio init***********************************************************/
	GPIO_InitTypeDef GPIO_Initure;
  RCC_PeriphCLKInitTypeDef SPI2ClkInit;
	
  __HAL_RCC_GPIOB_CLK_ENABLE();                   //使能GPIOF时钟
  __HAL_RCC_SPI2_CLK_ENABLE();                    //使能SPI2时钟
    
	//设置SPI2的时钟源 
	SPI2ClkInit.PeriphClockSelection=RCC_PERIPHCLK_SPI2;	    //设置SPI2时钟源
	SPI2ClkInit.Spi123ClockSelection=RCC_SPI123CLKSOURCE_PLL;	//SPI2时钟源使用PLL1Q
	HAL_RCCEx_PeriphCLKConfig(&SPI2ClkInit);
	
  //SCK MOSI MISO->PB13,14,15
  GPIO_Initure.Pin=GPIO_PIN_13|GPIO_PIN_15|GPIO_PIN_14;//坑点，想要读出数据就必须这么设置
  GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
  GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉////
  GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;   //快速    
  GPIO_Initure.Alternate=GPIO_AF5_SPI2;           //复用为SPI2
  HAL_GPIO_Init(GPIOB,&GPIO_Initure);             //初始化
	//CE PB12
	GPIO_Initure.Pin=GPIO_PIN_12;
	GPIO_Initure.Pull=GPIO_PULLUP;
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	/*nvic irq***********************************************************/
	HAL_NVIC_SetPriority(SPI2_IRQn, 0, 2);
  HAL_NVIC_EnableIRQ(SPI2_IRQn);
}  

void DMA_ConfigRX(DMA_Stream_TypeDef *DMA_Streamx)
{ 
    __HAL_RCC_DMA1_CLK_ENABLE();                                       //DMA1时钟使能   
    
    //Rx DMA配置
    SPI2RxDMA_Handler.Instance=DMA_Streamx;                            //数据流选择
		SPI2RxDMA_Handler.Init.Request=DMA_REQUEST_SPI2_RX;				         //SPI2 to DMA
    SPI2RxDMA_Handler.Init.Direction=DMA_PERIPH_TO_MEMORY;             //存储器到外设
    SPI2RxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //外设DIS增量模式???
    SPI2RxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //存储器增量模式
    SPI2RxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;    //外设数据长度:16位
    SPI2RxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_WORD;       //存储器数据长度:16位
    SPI2RxDMA_Handler.Init.Mode=DMA_NORMAL;                            //外设流控模式
    SPI2RxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //HIGH优先级
    SPI2RxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;      	
    SPI2RxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
    SPI2RxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //存储器突发单次传输
    SPI2RxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //外设突发单次传输
	
		__HAL_LINKDMA(&SPI2_Handler_FPGA,hdmarx,SPI2RxDMA_Handler);        //将DMA与USART1联系起来(发送DMA)
    
    HAL_DMA_DeInit(&SPI2RxDMA_Handler);   
    HAL_DMA_Init(&SPI2RxDMA_Handler);
	
		
		HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
}

void DMA_ConfigTX(DMA_Stream_TypeDef *DMA_Streamx)
{ 
    __HAL_RCC_DMA1_CLK_ENABLE();//DMA1时钟使能  
    
    //Tx DMA配置
    SPI2TxDMA_Handler.Instance=DMA_Streamx;                            //数据流选择
		SPI2TxDMA_Handler.Init.Request=DMA_REQUEST_SPI2_TX;				         //SPI2 to DMA
    SPI2TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;             //存储器到外设
    SPI2TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //外设DIS增量模式???
    SPI2TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //存储器增量模式
    SPI2TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;    //外设数据长度:16位
    SPI2TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_WORD;       //存储器数据长度:16位
    SPI2TxDMA_Handler.Init.Mode=DMA_NORMAL;                            //外设流控模式
    SPI2TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //HIGH优先级
    SPI2TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;             	
    SPI2TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;     
    SPI2TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //存储器突发单次传输
    SPI2TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //外设突发单次传输
    
    HAL_DMA_DeInit(&SPI2TxDMA_Handler);   
    HAL_DMA_Init(&SPI2TxDMA_Handler);
		
		__HAL_LINKDMA(&SPI2_Handler_FPGA,hdmatx,SPI2TxDMA_Handler);        //将DMA与USART1联系起来(发送DMA)
		
	  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
} 
/********************************************user interface************************************************/
//总的初始化函数
void fpga_spi_stm32_init(void)
{
	fpga_32_init();
	DMA_ConfigRX(DMA1_Stream0);
	DMA_ConfigTX(DMA1_Stream1);
}

void FPGA_ReadWriteByte(u8* TxData,u8 *Rxdata,u16 num)
{
  HAL_SPI_TransmitReceive(&SPI2_Handler_FPGA,TxData,Rxdata,num,200);     
}
void FPGA_ReadWriteByte_DMA(u8* TxData,u8 *Rxdata,u16 num)
{
  HAL_SPI_TransmitReceive_DMA(&SPI2_Handler_FPGA,TxData,Rxdata,num);
}

