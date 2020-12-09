#include "fpga_spi.h"
/*
�����ţ������뿴cube
H743�˽ӿڶ��壺SPI2
SPI_CE   PB12
SPI_CLK  PB13
SPI_MISO PB14
*/
/************************************handle&irq*************************************************************/
SPI_HandleTypeDef  SPI2_Handler_FPGA;      //SPI2���
DMA_HandleTypeDef  SPI2RxDMA_Handler;      //DMA���
DMA_HandleTypeDef  SPI2TxDMA_Handler;      //DMA���
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
  SPI2_Handler_FPGA.Init.Mode=SPI_MODE_MASTER;          //����SPI����ģʽ������Ϊ��ģʽ
  SPI2_Handler_FPGA.Init.Direction=SPI_DIRECTION_2LINES;//����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ
	
  SPI2_Handler_FPGA.Init.DataSize=SPI_DATASIZE_8BIT;    //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	
  SPI2_Handler_FPGA.Init.CLKPolarity=SPI_POLARITY_LOW; //����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ
  SPI2_Handler_FPGA.Init.CLKPhase=SPI_PHASE_1EDGE;      //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
  SPI2_Handler_FPGA.Init.NSS=SPI_NSS_HARD_OUTPUT;              //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
  SPI2_Handler_FPGA.Init.NSSPMode=SPI_NSS_PULSE_ENABLE;//NSS�ź�����ʧ��
  SPI2_Handler_FPGA.Init.MasterKeepIOState=SPI_MASTER_KEEP_IO_STATE_ENABLE;  //SPI��ģʽIO״̬����ʹ��
  SPI2_Handler_FPGA.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_16;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
  SPI2_Handler_FPGA.Init.FirstBit=SPI_FIRSTBIT_MSB;     //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
  SPI2_Handler_FPGA.Init.TIMode=SPI_TIMODE_DISABLE;     //�ر�TIģʽ
  SPI2_Handler_FPGA.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//�ر�Ӳ��CRCУ��
  SPI2_Handler_FPGA.Init.CRCPolynomial=7;               //CRCֵ����Ķ���ʽ
  HAL_SPI_Init(&SPI2_Handler_FPGA);
	/*gpio init***********************************************************/
	GPIO_InitTypeDef GPIO_Initure;
  RCC_PeriphCLKInitTypeDef SPI2ClkInit;
	
  __HAL_RCC_GPIOB_CLK_ENABLE();                   //ʹ��GPIOFʱ��
  __HAL_RCC_SPI2_CLK_ENABLE();                    //ʹ��SPI2ʱ��
    
	//����SPI2��ʱ��Դ 
	SPI2ClkInit.PeriphClockSelection=RCC_PERIPHCLK_SPI2;	    //����SPI2ʱ��Դ
	SPI2ClkInit.Spi123ClockSelection=RCC_SPI123CLKSOURCE_PLL;	//SPI2ʱ��Դʹ��PLL1Q
	HAL_RCCEx_PeriphCLKConfig(&SPI2ClkInit);
	
  //SCK MOSI MISO->PB13,14,15
  GPIO_Initure.Pin=GPIO_PIN_13|GPIO_PIN_15|GPIO_PIN_14;//�ӵ㣬��Ҫ�������ݾͱ�����ô����
  GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //�����������
  GPIO_Initure.Pull=GPIO_PULLUP;                  //����////
  GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;   //����    
  GPIO_Initure.Alternate=GPIO_AF5_SPI2;           //����ΪSPI2
  HAL_GPIO_Init(GPIOB,&GPIO_Initure);             //��ʼ��
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
    __HAL_RCC_DMA1_CLK_ENABLE();                                       //DMA1ʱ��ʹ��   
    
    //Rx DMA����
    SPI2RxDMA_Handler.Instance=DMA_Streamx;                            //������ѡ��
		SPI2RxDMA_Handler.Init.Request=DMA_REQUEST_SPI2_RX;				         //SPI2 to DMA
    SPI2RxDMA_Handler.Init.Direction=DMA_PERIPH_TO_MEMORY;             //�洢��������
    SPI2RxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //����DIS����ģʽ???
    SPI2RxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //�洢������ģʽ
    SPI2RxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;    //�������ݳ���:16λ
    SPI2RxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_WORD;       //�洢�����ݳ���:16λ
    SPI2RxDMA_Handler.Init.Mode=DMA_NORMAL;                            //��������ģʽ
    SPI2RxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //HIGH���ȼ�
    SPI2RxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;      	
    SPI2RxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
    SPI2RxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //�洢��ͻ�����δ���
    SPI2RxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //����ͻ�����δ���
	
		__HAL_LINKDMA(&SPI2_Handler_FPGA,hdmarx,SPI2RxDMA_Handler);        //��DMA��USART1��ϵ����(����DMA)
    
    HAL_DMA_DeInit(&SPI2RxDMA_Handler);   
    HAL_DMA_Init(&SPI2RxDMA_Handler);
	
		
		HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
}

void DMA_ConfigTX(DMA_Stream_TypeDef *DMA_Streamx)
{ 
    __HAL_RCC_DMA1_CLK_ENABLE();//DMA1ʱ��ʹ��  
    
    //Tx DMA����
    SPI2TxDMA_Handler.Instance=DMA_Streamx;                            //������ѡ��
		SPI2TxDMA_Handler.Init.Request=DMA_REQUEST_SPI2_TX;				         //SPI2 to DMA
    SPI2TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;             //�洢��������
    SPI2TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //����DIS����ģʽ???
    SPI2TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //�洢������ģʽ
    SPI2TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;    //�������ݳ���:16λ
    SPI2TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_WORD;       //�洢�����ݳ���:16λ
    SPI2TxDMA_Handler.Init.Mode=DMA_NORMAL;                            //��������ģʽ
    SPI2TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //HIGH���ȼ�
    SPI2TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;             	
    SPI2TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;     
    SPI2TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //�洢��ͻ�����δ���
    SPI2TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //����ͻ�����δ���
    
    HAL_DMA_DeInit(&SPI2TxDMA_Handler);   
    HAL_DMA_Init(&SPI2TxDMA_Handler);
		
		__HAL_LINKDMA(&SPI2_Handler_FPGA,hdmatx,SPI2TxDMA_Handler);        //��DMA��USART1��ϵ����(����DMA)
		
	  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
} 
/********************************************user interface************************************************/
//�ܵĳ�ʼ������
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

