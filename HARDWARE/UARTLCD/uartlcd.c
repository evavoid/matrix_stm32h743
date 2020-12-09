#include "uartlcd.h"
//对于单片机PA2 TX PA3 RX
//PA2接转接板上TXD,PA3接转接板上RXD,9600
UART_HandleTypeDef USART2_Handler; 
/***********************************程序管理块***********************************/
data_management tx_manage={.state=0,.order_successful=0,.cnt_pin=0};
data_management rx_manage={.state=0,.order_successful=0,.cnt_pin=0};
/*************************************调试块*************************************/
u32 rx_num_of_serial_port=0;
u32 tx_num_of_serial_port=0;
/**************************************init**************************************/
void usartlcd_init(u32 bound)
{	
	/**********map********/
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOA_CLK_ENABLE();			
	__HAL_RCC_USART2_CLK_ENABLE();			
	GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3; 
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		
	GPIO_Initure.Pull=GPIO_PULLUP;			
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;		
	GPIO_Initure.Alternate=GPIO_AF7_USART2;	
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   
	/*********uart2*******/
	USART2_Handler.Instance=USART2;					   
	USART2_Handler.Init.BaudRate=bound;				   
	USART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;
	USART2_Handler.Init.StopBits=UART_STOPBITS_1;	
	USART2_Handler.Init.Parity=UART_PARITY_NONE;		
	USART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	
	USART2_Handler.Init.Mode=UART_MODE_TX_RX;			   
	HAL_UART_Init(&USART2_Handler);		
	/******NVIC*********/
	HAL_NVIC_EnableIRQ(USART2_IRQn);			
	HAL_NVIC_SetPriority(USART2_IRQn,3,3);		
	/******IQR**********/
	__HAL_UART_ENABLE_IT(&USART2_Handler,UART_IT_RXNE);	
}
/**********************************management**************************************/
/*********不可加st-link中断函数**********/
void USART2_IRQHandler(void)                	
{ 
	u8 Res;
	if((__HAL_UART_GET_FLAG(&USART2_Handler,UART_FLAG_RXNE)!=RESET))//接收到一个byte
	{
		HAL_UART_Receive(&USART2_Handler,&Res,1,1000); 
		if(!rx_manage.order_successful)//没有待处理的指令
		{
			rx_manage.USART_BUF[rx_manage.cnt_pin]=Res;
			rx_manage.cnt_pin++;
			
			switch(rx_manage.state)//状态机截取一整条指令，每一个字节的接收都会进入一次
			{
				case 0:{if(Res==0xEE) {rx_manage.state=1;rx_manage.cnt_pin=1;rx_manage.USART_BUF[0]=Res;}break;}
				case 1:{if(Res==0xFF) rx_manage.state=2;break;}
				case 2:{if(Res==0xFC) rx_manage.state=3;else rx_manage.state=1;break;}
				case 3:{if(Res==0xFF) rx_manage.state=4;else rx_manage.state=1;break;}
				case 4:{if(Res==0xFF) {rx_manage.state=0;rx_manage.order_successful=1;Instruction_rx();}else rx_manage.state=1;break;}
			}
		}else Instruction_rx();/**指令接收完毕**/
	}
	HAL_UART_IRQHandler(&USART2_Handler);	
}
/*********不可加st-link中断函数************/
void Instruction_rx(void)//according to a tree.指令级解析
{
	if(rx_manage.USART_BUF[1]==0xb1&&rx_manage.USART_BUF[2]==0x11)//按钮
	{
		instructions(0,1)();
	}
	rx_manage.cnt_pin=0;//计数量为0
	rx_manage.order_successful=0;//处理结束，缓冲区空闲
	rx_num_of_serial_port+=1;
}
void Instruction_tx(u32 code_group,u32 code_id)
{
	tx_manage.USART_BUF[0]=0xee;
	instructions(code_group,code_id)();//只处理其中一部分
	tx_manage.USART_BUF[tx_manage.cnt_pin]=0xff;
	tx_manage.USART_BUF[tx_manage.cnt_pin+1]=0xfc;
	tx_manage.USART_BUF[tx_manage.cnt_pin+2]=0xff;
	tx_manage.USART_BUF[tx_manage.cnt_pin+3]=0xff;
	HAL_UART_Transmit(&USART2_Handler,tx_manage.USART_BUF,tx_manage.cnt_pin+4,0xffff);
	tx_manage.cnt_pin=0;
	tx_num_of_serial_port+=1;
	//无successful标志说明是实时发送
	//计数位由参数集解释器负责
}
/*********指令集散中心**************/
callbake instructions(u32 code_group,u32 code_id)
{
	void (*rx[10])(void);
	void (*tx[10])(void);
	/*********0************/
	rx[0]=Parameter_key_rx;
	/*********1************/
	tx[0]=Parameter_wave_tx;
	tx[1]=Parameter_datasheet_data_tx;
	tx[2]=Parameter_datasheet_clear_tx;
	tx[3]=Parameter_labal_tx;
	
	switch(code_group)
	{
		case 0:return rx[code_id];break;
		case 1:return tx[code_id];break;
	}
	return Parameter_error;
}
/**********************************parameter**************************************/
void Parameter_key_rx(void)
{

}
void Parameter_datasheet_data_tx(void)
{
	#define DATASHEET_ROW    8
	#define DATASHEET_COLUMN 3
	
	u32 con_data_cnt;//该函数内当前发送数据数，每次调用该函数，发送不超过DATASHEET_COLUMN个数字
	float data[10];//取数据
	u32 output_data;//实际发送接口
	//printf("%x\r\n",USART_BUF[0]);
	//EE B1 52 00 00 00 01 32 3B 32 3B 32 3B FF FC FF FF 
	
	tx_manage.USART_BUF[1]=0xB1;
	tx_manage.USART_BUF[2]=0x52;
	tx_manage.USART_BUF[3]=0x00;
	tx_manage.USART_BUF[4]=0x00;
	tx_manage.USART_BUF[5]=0x00;
	tx_manage.USART_BUF[6]=0x01;
	tx_manage.cnt_pin=7;
	
	for(con_data_cnt=0;con_data_cnt<DATASHEET_COLUMN;con_data_cnt++)//发送一行数据
	{
		if(data[con_data_cnt])output_data=(u32)(data[con_data_cnt]);//取正数
		else
		{
			tx_manage.USART_BUF[tx_manage.cnt_pin++]=0x2d;
			output_data=(u32)fabs(data[con_data_cnt]);//取绝对值
		}
		u32 datalength=data_size_u32(output_data);
	
		for(u32 i=0;i<datalength;i++,tx_manage.cnt_pin++)//发送一行里一个格子内的数据//i数据控制器，对于单个数据来调用
		{
			u32 divide=pow(10,datalength-1-i);
			tx_manage.USART_BUF[tx_manage.cnt_pin]=output_data/divide+0x30;
			output_data-=(tx_manage.USART_BUF[tx_manage.cnt_pin]-0x30)*divide;
		}
		tx_manage.USART_BUF[tx_manage.cnt_pin]=0x3B;
		tx_manage.cnt_pin+=1;
	}
}

void Parameter_datasheet_clear_tx(void)
{
	//EE B1 58 00 00 00 01 00 00 FF FC FF FF 
	tx_manage.USART_BUF[1]=0xB1;
	tx_manage.USART_BUF[2]=0x58;
	tx_manage.USART_BUF[3]=0x00;
	tx_manage.USART_BUF[4]=0x00;
	tx_manage.USART_BUF[5]=0x00;
	tx_manage.USART_BUF[6]=0x01;
	tx_manage.USART_BUF[7]=0x00;
	tx_manage.USART_BUF[8]=0x00;
	tx_manage.cnt_pin=9;
}
void Parameter_wave_tx(void)
{
	//EE B1 32 SCREEN_ID CONTROL_ID CHANNEL DATA_LEN DATA FF FC FF FF
	/**
		数据参数：
		data_xrange0，代表数据的横坐标左范围，定为0不变即可
		data_xrange1，代表数据的横坐标右范围，定为数据长度即可------可改
		data_yrange0，代表数据的纵坐标下范围，按照实际设定----------可改
	  data_yrange1，代表数据的纵坐标上范围，按照实际设定----------可改
		data_data   , 代表数据--------------------------------------可改
	**/
	/**
		屏幕参数：
		act_hp，代表屏幕的横向像素点数，从0->act_hp---只改一次，适配屏幕，不能超过200
		act_lp，代表屏幕的纵向像素点数，从0->act_lp
	**/
	float data_xrange0=0,data_xrange1=14,data_yrange0=0,data_yrange1=60;
	float uartlcd_wave_data[15]={12,41,42,23,12,43,13,34,54,12,33,56,7,32,38};
	float act_hp=50;
	float act_lp=255;
	/**
		计算参数
	**/
	float data_hp=data_xrange1-data_xrange0;//横向范围
	float data_lp=data_yrange1-data_yrange0;//纵向范围

	float lp_scale=(float)act_lp/(float)data_lp;//纵向放缩比例
	float hp_scale=(float)data_hp/(float)act_hp;//横向放缩比例
	tx_manage.USART_BUF[1]=0xee;
	tx_manage.USART_BUF[1]=0xb1;
	tx_manage.USART_BUF[2]=0x35;
	tx_manage.USART_BUF[3]=0x00;
	tx_manage.USART_BUF[4]=0x00;
	tx_manage.USART_BUF[5]=0x00;
	tx_manage.USART_BUF[6]=0x02;
	tx_manage.USART_BUF[7]=0x00;
	
	tx_manage.USART_BUF[8]=(u8)((u32)act_hp/256);
	tx_manage.USART_BUF[9]=(u8)((u32)act_hp%256);
	
	for(tx_manage.cnt_pin=10;tx_manage.cnt_pin<10+act_hp;tx_manage.cnt_pin++)
	{
		tx_manage.USART_BUF[tx_manage.cnt_pin]=(u8)
		((uartlcd_wave_data[(u32)((tx_manage.cnt_pin-10)*hp_scale)]-data_yrange0)*lp_scale);
	}
	/*
	for(data_num=0;data_num<tx_manage.cnt_pin;data_num++)
		printf("%02x ",tx_manage.USART_BUF[data_num]);
	*/
}
void Parameter_labal_tx(void)
{
	float data=global_data.data[8];//提取装载的数
	u32 output_data;
	//EE B1 10 00 00 00 1E 32 38 33 FF FC FF FF 
	tx_manage.USART_BUF[1]=0xB1;
	tx_manage.USART_BUF[2]=0x10;
	tx_manage.USART_BUF[3]=0x00;
	tx_manage.USART_BUF[4]=0x00;
	tx_manage.USART_BUF[5]=0x00;
	tx_manage.USART_BUF[6]=0x25;//标签id
	tx_manage.cnt_pin=7;
	
	if(data)output_data=(u32)(data);
	else
	{
		tx_manage.USART_BUF[tx_manage.cnt_pin++]=0x2d;
		output_data=(u32)fabs(data);
	}
	u32 datalength=data_size_u32(output_data);
	
	for(u32 i=0;i<datalength;i++)//?????????????
	{
		u32 divide=pow(10,datalength-1-i);
		tx_manage.USART_BUF[tx_manage.cnt_pin]=data/divide+0x30;
		data-=(tx_manage.USART_BUF[tx_manage.cnt_pin]-0x30)*divide;
		tx_manage.cnt_pin+=1;
	}
}
void Parameter_error(void)
{
	while(1)printf("error");
}
u32 data_size_u32(u32 data)
{
	u32 cnt=0;
	u32 inner_data=data;
	while(inner_data)
	{
		inner_data/=10;
		cnt+=1;
	}
	if(data)return cnt;
	else return 1;	
}
