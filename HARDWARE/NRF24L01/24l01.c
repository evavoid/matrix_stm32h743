#include "24l01.h"

u8 tmp_buf[200];

const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
SPI_HandleTypeDef SPI2_Handler;  //SPI2���

void NRF24L01_SPI2_Init(void)
{
  SPI2_Handler.Instance=SPI2;                      //SP2
  SPI2_Handler.Init.Mode=SPI_MODE_MASTER;          //����SPI����ģʽ������Ϊ��ģʽ
  SPI2_Handler.Init.Direction=SPI_DIRECTION_2LINES;//����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ
  SPI2_Handler.Init.DataSize=SPI_DATASIZE_8BIT;    //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
  SPI2_Handler.Init.CLKPolarity=SPI_POLARITY_LOW; //����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ
  SPI2_Handler.Init.CLKPhase=SPI_PHASE_1EDGE;      //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
  SPI2_Handler.Init.NSS=SPI_NSS_SOFT;              //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
  SPI2_Handler.Init.NSSPMode=SPI_NSS_PULSE_DISABLE;//NSS�ź�����ʧ��
  SPI2_Handler.Init.MasterKeepIOState=SPI_MASTER_KEEP_IO_STATE_ENABLE;  //SPI��ģʽIO״̬����ʹ��
  SPI2_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
  SPI2_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;     //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
  SPI2_Handler.Init.TIMode=SPI_TIMODE_DISABLE;     //�ر�TIģʽ
  SPI2_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//�ر�Ӳ��CRCУ��
  SPI2_Handler.Init.CRCPolynomial=7;               //CRCֵ����Ķ���ʽ
  HAL_SPI_Init(&SPI2_Handler);
    
  __HAL_SPI_ENABLE(&SPI2_Handler);                 //ʹ��SPI2
  SPI2_ReadWriteByte(0Xff);                        //��������
}

//    __HAL_SPI_ENABLE(&SPI2_Handler);                //ʹ��SPI2
// __HAL_SPI_DISABLE(&SPI2_Handler);               //�ȹر�SPI2
//HAL_SPI_TransmitReceive(&SPI2_Handler,&TxData,&Rxdata,1, 1000);    


//SPI2�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_SPI_Init()����
//hspi:SPI���
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef GPIO_Initure;
  RCC_PeriphCLKInitTypeDef SPI2ClkInit;
	
  __HAL_RCC_GPIOB_CLK_ENABLE();                   //ʹ��GPIOFʱ��
  __HAL_RCC_SPI2_CLK_ENABLE();                    //ʹ��SPI2ʱ��
    
	//����SPI2��ʱ��Դ 
	SPI2ClkInit.PeriphClockSelection=RCC_PERIPHCLK_SPI2;	    //����SPI2ʱ��Դ
	SPI2ClkInit.Spi123ClockSelection=RCC_SPI123CLKSOURCE_PLL;	//SPI2ʱ��Դʹ��PLL1Q
	HAL_RCCEx_PeriphCLKConfig(&SPI2ClkInit);
	
    //PB13,14,15
  GPIO_Initure.Pin=GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //�����������
  GPIO_Initure.Pull=GPIO_PULLUP;                  //����
  GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;   //����    
  GPIO_Initure.Alternate=GPIO_AF5_SPI2;           //����ΪSPI2
  HAL_GPIO_Init(GPIOB,&GPIO_Initure);             //��ʼ��
}
//SPI�ٶ����ú���
//SPI�ٶ�=PLL1Q/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_256
//PLL1Qʱ��һ��Ϊ200Mhz��
void SPI2_SetSpeed(u32 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
  __HAL_SPI_DISABLE(&SPI2_Handler);            //�ر�SPI
  SPI2_Handler.Instance->CFG1&=~(0X7<<28);     //λ30-28���㣬�������ò�����
  SPI2_Handler.Instance->CFG1|=SPI_BaudRatePrescaler;//����SPI�ٶ�
  __HAL_SPI_ENABLE(&SPI2_Handler);             //ʹ��SPI  
}

//SPI2 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{
  u8 Rxdata;
  HAL_SPI_TransmitReceive(&SPI2_Handler,&TxData,&Rxdata,1, 1000); //1���ֽڣ�1000�ӳ�      
 	return Rxdata;          		    //�����յ�������		
}
//��ʼ��24L01��IO��
void NRF24L01_Init(void)
{
  GPIO_InitTypeDef GPIO_Initure;
  __HAL_RCC_GPIOG_CLK_ENABLE();			//����GPIOGʱ��
  __HAL_RCC_GPIOI_CLK_ENABLE();			//����GPIOIʱ��
    
  GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_12; //PG10,12
  GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
  GPIO_Initure.Pull=GPIO_PULLUP;          //����
  GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //����
  HAL_GPIO_Init(GPIOG,&GPIO_Initure);     //��ʼ��
     
  GPIO_Initure.Pin=GPIO_PIN_11;           //PI11
  GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
  HAL_GPIO_Init(GPIOI,&GPIO_Initure);     //��ʼ��
    
       		                //��ʼ��SPI2  
	NRF24L01_SPI2_Init();                    //���NRF���ص��޸�SPI������
	NRF24L01_CE(0); 			            //ʹ��24L01
	NRF24L01_CSN(1);			            //SPIƬѡȡ��	 		 	 
}
//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_32); //spi�ٶ�Ϊ6.25Mhz��24L01�����SPIʱ��Ϊ10Mhz��   	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//��Ŀ���ַ������д���������
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//���24L01����	
	return 0;		 //��⵽24L01
}	 	 
//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;	
  NRF24L01_CSN(0);                //ʹ��SPI����
  status =SPI2_ReadWriteByte(reg);//���ͼĴ����� 
  SPI2_ReadWriteByte(value);      //д��Ĵ�����ֵ
  NRF24L01_CSN(1);                //��ֹSPI����	   
  return(status);       			//����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
 	NRF24L01_CSN(0);            //ʹ��SPI����		
  SPI2_ReadWriteByte(reg);    //���ͼĴ�����
  reg_val=SPI2_ReadWriteByte(0XFF);//��ȡ�Ĵ�������
  NRF24L01_CSN(1);            //��ֹSPI����		    
 	return(reg_val);            //����״ֵ̬
}	
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,u8_ctr;	       
  NRF24L01_CSN(0);            //ʹ��SPI����
  status=SPI2_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//��������
  	NRF24L01_CSN(1);            //�ر�SPI����
  return status;              //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr;	    
 	NRF24L01_CSN(0);            //ʹ��SPI����
  status = SPI2_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //д������	 
  	NRF24L01_CSN(1);            //�ر�SPI����
  return status;              //���ض�����״ֵ̬
}				   
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
 	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_16); //spi�ٶ�Ϊ6.25Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	NRF24L01_CE(0);
	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
 	NRF24L01_CE(1);                         //��������	   
	while(NRF24L01_IRQ!=0);                 //�ȴ��������
	sta=NRF24L01_Read_Reg(STATUS);          //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&MAX_TX)                          //�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);  //���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(sta&TX_OK)                           //�������
	{
		return TX_OK;
	}
	return 0xff;//����ԭ����ʧ��
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		    							   
	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_16); //spi�ٶ�Ϊ6.25Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	sta=NRF24L01_Read_Reg(STATUS);          //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)//���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);  //���RX FIFO�Ĵ��� 
		return 0; 
	}	   
	return 1;//û�յ��κ�����
}					    
//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE(0);	  
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
	  
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);       //ʹ��ͨ��0���Զ�Ӧ��    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);   //ʹ��ͨ��0�Ľ��յ�ַ  	 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	        //����RFͨ��Ƶ��		  
  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	    
 	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);    //����TX�������,0db����,2Mbps,���������濪��   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);     //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
  NRF24L01_CE(1); //CEΪ��,�������ģʽ 
}						 
//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE(0);	    
  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //����RFͨ��Ϊ40
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF24L01_CE(1);//CEΪ��,10us����������
}

u8 NRF24L01_Rxmail(u8 *rxbuf,u32 packetnum)//������
{
	u8 * tem_buf=&rxbuf[0];
	static u32 now_packet=0;
	static u32 run_time=0;
	
	run_time++;
	if(run_time>1000)
	{
		printf("RXError:packet %d\r\n",now_packet);
		return 1;//��ʱ
	}
	
	if(NRF24L01_RxPacket(tem_buf+RX_PLOAD_WIDTH*now_packet)==0)//һ�����յ���Ϣ,����ʾ����.
	{
		printf("RX:packet %d ok\r\n",now_packet);
		now_packet++;
		run_time=0;
	}	
	
	if(now_packet==packetnum)
	{
		now_packet=0;
		run_time=0;
		printf("RX:%d bytes,%d packets ok\r\n",packetnum*RX_PLOAD_WIDTH,packetnum);
		return 0;
	}
	else
		return 2;//��δ�������
}//���ն����������,blocksizeΪ�ܹ������ֽ�
u8 NRF24L01_Txmail(u8 *txbuf,u32 packetnum)
{
	u8 * tem_buf=&txbuf[0];
	static u32 now_packet=0;
	static u32 run_time=0;
	
	run_time++;
	if(run_time>100)
	{
		printf("TXError:packet %d\r\n",now_packet);
		return 1;//��ʱ
	}
	
	if(NRF24L01_TxPacket(tem_buf+TX_PLOAD_WIDTH*now_packet)==TX_OK)//һ�����յ���Ϣ,����ʾ����.
	{
		printf("TX:packet %d ok\r\n",now_packet);
		now_packet++;
		run_time=0;
	}	
	
	if(now_packet==packetnum)
	{
		now_packet=0;
		run_time=0;
		printf("TX:%d bytes,%d packets ok\r\n",packetnum*TX_PLOAD_WIDTH,packetnum);
		return 0;
	}
	else
		return 2;//��δ�������
}
/////////////////////////////////////////////////////////////////////////////////////
void nrf24l01_rx_event(local_state_space *NRFRX_state)
{
	u8 i;
	if(NRFRX_state->ready_state==prepare_state)
	{
		NRF24L01_Init();
		while(NRF24L01_Check())///////////////////////////		  
		{	
			ERROR_HANDLER("nrf24l01_rx init error",0);
		}
		NRF24L01_RX_Mode();
		printf("nrf24l01_rx init ok\r\n");
		NRFRX_state->ready_state=run_state0;
	}
	else if(NRFRX_state->ready_state==run_state0)
	{
		if(NRF24L01_Rxmail(tmp_buf,1)==0)//һ�����յ���Ϣ,����ʾ����.
		{
			for(i=0;i<1*RX_PLOAD_WIDTH;i++)
			{
				printf("%d",tmp_buf[i]);
			}
			printf("%d\r\n\r\n",tmp_buf[i]);
		}	
	}
}
void nrf24l01_tx_event(local_state_space *NRFTX_state)
{
	if(NRFTX_state->ready_state==prepare_state)
	{
		NRF24L01_Init();
		while(NRF24L01_Check())///////////////////////////		  
		{	
			ERROR_HANDLER("nrf24l01_tx init error",0);
		}
		NRF24L01_TX_Mode();
		NRFTX_state->ready_state=run_state0;
	}
	else if(NRFTX_state->ready_state==run_state0)
	{
		sprintf((char*)tmp_buf,"It's good!%d\r\n",23);
		if(NRF24L01_Txmail(tmp_buf,1)==TX_OK)//һ�����յ���Ϣ,����ʾ����.
		{
			printf("all ok\r\n");
		}
	}
}

