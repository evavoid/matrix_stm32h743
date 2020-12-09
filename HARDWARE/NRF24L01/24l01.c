#include "24l01.h"

u8 tmp_buf[200];

const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
SPI_HandleTypeDef SPI2_Handler;  //SPI2句柄

void NRF24L01_SPI2_Init(void)
{
  SPI2_Handler.Instance=SPI2;                      //SP2
  SPI2_Handler.Init.Mode=SPI_MODE_MASTER;          //设置SPI工作模式，设置为主模式
  SPI2_Handler.Init.Direction=SPI_DIRECTION_2LINES;//设置SPI单向或者双向的数据模式:SPI设置为双线模式
  SPI2_Handler.Init.DataSize=SPI_DATASIZE_8BIT;    //设置SPI的数据大小:SPI发送接收8位帧结构
  SPI2_Handler.Init.CLKPolarity=SPI_POLARITY_LOW; //串行同步时钟的空闲状态为低电平
  SPI2_Handler.Init.CLKPhase=SPI_PHASE_1EDGE;      //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
  SPI2_Handler.Init.NSS=SPI_NSS_SOFT;              //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
  SPI2_Handler.Init.NSSPMode=SPI_NSS_PULSE_DISABLE;//NSS信号脉冲失能
  SPI2_Handler.Init.MasterKeepIOState=SPI_MASTER_KEEP_IO_STATE_ENABLE;  //SPI主模式IO状态保持使能
  SPI2_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//定义波特率预分频的值:波特率预分频值为256
  SPI2_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;     //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
  SPI2_Handler.Init.TIMode=SPI_TIMODE_DISABLE;     //关闭TI模式
  SPI2_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//关闭硬件CRC校验
  SPI2_Handler.Init.CRCPolynomial=7;               //CRC值计算的多项式
  HAL_SPI_Init(&SPI2_Handler);
    
  __HAL_SPI_ENABLE(&SPI2_Handler);                 //使能SPI2
  SPI2_ReadWriteByte(0Xff);                        //启动传输
}

//    __HAL_SPI_ENABLE(&SPI2_Handler);                //使能SPI2
// __HAL_SPI_DISABLE(&SPI2_Handler);               //先关闭SPI2
//HAL_SPI_TransmitReceive(&SPI2_Handler,&TxData,&Rxdata,1, 1000);    


//SPI2底层驱动，时钟使能，引脚配置
//此函数会被HAL_SPI_Init()调用
//hspi:SPI句柄
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef GPIO_Initure;
  RCC_PeriphCLKInitTypeDef SPI2ClkInit;
	
  __HAL_RCC_GPIOB_CLK_ENABLE();                   //使能GPIOF时钟
  __HAL_RCC_SPI2_CLK_ENABLE();                    //使能SPI2时钟
    
	//设置SPI2的时钟源 
	SPI2ClkInit.PeriphClockSelection=RCC_PERIPHCLK_SPI2;	    //设置SPI2时钟源
	SPI2ClkInit.Spi123ClockSelection=RCC_SPI123CLKSOURCE_PLL;	//SPI2时钟源使用PLL1Q
	HAL_RCCEx_PeriphCLKConfig(&SPI2ClkInit);
	
    //PB13,14,15
  GPIO_Initure.Pin=GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
  GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
  GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;   //快速    
  GPIO_Initure.Alternate=GPIO_AF5_SPI2;           //复用为SPI2
  HAL_GPIO_Init(GPIOB,&GPIO_Initure);             //初始化
}
//SPI速度设置函数
//SPI速度=PLL1Q/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_256
//PLL1Q时钟一般为200Mhz：
void SPI2_SetSpeed(u32 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
  __HAL_SPI_DISABLE(&SPI2_Handler);            //关闭SPI
  SPI2_Handler.Instance->CFG1&=~(0X7<<28);     //位30-28清零，用来设置波特率
  SPI2_Handler.Instance->CFG1|=SPI_BaudRatePrescaler;//设置SPI速度
  __HAL_SPI_ENABLE(&SPI2_Handler);             //使能SPI  
}

//SPI2 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{
  u8 Rxdata;
  HAL_SPI_TransmitReceive(&SPI2_Handler,&TxData,&Rxdata,1, 1000); //1个字节，1000延迟      
 	return Rxdata;          		    //返回收到的数据		
}
//初始化24L01的IO口
void NRF24L01_Init(void)
{
  GPIO_InitTypeDef GPIO_Initure;
  __HAL_RCC_GPIOG_CLK_ENABLE();			//开启GPIOG时钟
  __HAL_RCC_GPIOI_CLK_ENABLE();			//开启GPIOI时钟
    
  GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_12; //PG10,12
  GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
  GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
  GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //高速
  HAL_GPIO_Init(GPIOG,&GPIO_Initure);     //初始化
     
  GPIO_Initure.Pin=GPIO_PIN_11;           //PI11
  GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
  HAL_GPIO_Init(GPIOI,&GPIO_Initure);     //初始化
    
       		                //初始化SPI2  
	NRF24L01_SPI2_Init();                    //针对NRF的特点修改SPI的设置
	NRF24L01_CE(0); 			            //使能24L01
	NRF24L01_CSN(1);			            //SPI片选取消	 		 	 
}
//检测24L01是否存在
//返回值:0，成功;1，失败	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_32); //spi速度为6.25Mhz（24L01的最大SPI时钟为10Mhz）   	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//往目标地址里增量写入五个数据
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//检测24L01错误	
	return 0;		 //检测到24L01
}	 	 
//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;	
  NRF24L01_CSN(0);                //使能SPI传输
  status =SPI2_ReadWriteByte(reg);//发送寄存器号 
  SPI2_ReadWriteByte(value);      //写入寄存器的值
  NRF24L01_CSN(1);                //禁止SPI传输	   
  return(status);       			//返回状态值
}
//读取SPI寄存器值
//reg:要读的寄存器
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
 	NRF24L01_CSN(0);            //使能SPI传输		
  SPI2_ReadWriteByte(reg);    //发送寄存器号
  reg_val=SPI2_ReadWriteByte(0XFF);//读取寄存器内容
  NRF24L01_CSN(1);            //禁止SPI传输		    
 	return(reg_val);            //返回状态值
}	
//在指定位置读出指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,u8_ctr;	       
  NRF24L01_CSN(0);            //使能SPI传输
  status=SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//读出数据
  	NRF24L01_CSN(1);            //关闭SPI传输
  return status;              //返回读到的状态值
}
//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr;	    
 	NRF24L01_CSN(0);            //使能SPI传输
  status = SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //写入数据	 
  	NRF24L01_CSN(1);            //关闭SPI传输
  return status;              //返回读到的状态值
}				   
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
 	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_16); //spi速度为6.25Mhz（24L01的最大SPI时钟为10Mhz）   
	NRF24L01_CE(0);
	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
 	NRF24L01_CE(1);                         //启动发送	   
	while(NRF24L01_IRQ!=0);                 //等待发送完成
	sta=NRF24L01_Read_Reg(STATUS);          //读取状态寄存器的值	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&MAX_TX)                          //达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);  //清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(sta&TX_OK)                           //发送完成
	{
		return TX_OK;
	}
	return 0xff;//其他原因发送失败
}
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		    							   
	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_16); //spi速度为6.25Mhz（24L01的最大SPI时钟为10Mhz）   
	sta=NRF24L01_Read_Reg(STATUS);          //读取状态寄存器的值    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&RX_OK)//接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);  //清除RX FIFO寄存器 
		return 0; 
	}	   
	return 1;//没收到任何数据
}					    
//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE(0);	  
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	  
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);       //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);   //使能通道0的接收地址  	 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	        //设置RF通信频率		  
  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
 	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);    //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);     //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
  NRF24L01_CE(1); //CE为高,进入接收模式 
}						 
//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE(0);	    
  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  

  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //设置RF通道为40
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF24L01_CE(1);//CE为高,10us后启动发送
}

u8 NRF24L01_Rxmail(u8 *rxbuf,u32 packetnum)//顶格发送
{
	u8 * tem_buf=&rxbuf[0];
	static u32 now_packet=0;
	static u32 run_time=0;
	
	run_time++;
	if(run_time>1000)
	{
		printf("RXError:packet %d\r\n",now_packet);
		return 1;//超时
	}
	
	if(NRF24L01_RxPacket(tem_buf+RX_PLOAD_WIDTH*now_packet)==0)//一旦接收到信息,则显示出来.
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
		return 2;//还未接收完毕
}//接收多个包的数据,blocksize为总共多少字节
u8 NRF24L01_Txmail(u8 *txbuf,u32 packetnum)
{
	u8 * tem_buf=&txbuf[0];
	static u32 now_packet=0;
	static u32 run_time=0;
	
	run_time++;
	if(run_time>100)
	{
		printf("TXError:packet %d\r\n",now_packet);
		return 1;//超时
	}
	
	if(NRF24L01_TxPacket(tem_buf+TX_PLOAD_WIDTH*now_packet)==TX_OK)//一旦接收到信息,则显示出来.
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
		return 2;//还未接收完毕
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
		if(NRF24L01_Rxmail(tmp_buf,1)==0)//一旦接收到信息,则显示出来.
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
		if(NRF24L01_Txmail(tmp_buf,1)==TX_OK)//一旦接收到信息,则显示出来.
		{
			printf("all ok\r\n");
		}
	}
}

