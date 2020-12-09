#include "tjc_usartlcd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//基于STM32H743IIT6、正点原子底层的陶晶驰K0串口屏驱动函数
//by 王天可								  
////////////////////////////////////////////////////////////////////////////////// 	
#if TJC_LCD
//PA2 TX, PA3 RX
UART_HandleTypeDef USART2_Handler;  //USART2句柄
u8 hmi_recieve[1];
u8 hmi_recieve_flag=0;//接收标志位，0为默认，1指令无效，2指令成功，3触摸事件，4触摸坐标，5透传就绪，6透传完成，7字符串返回，8数值返回
u8 hmi_finish_flag=3;//结束标志位，等于3时一个数据传输结束
u8 hmi_recieve_bit=0;//内部参数，别瞎几把修改调用
u8 hmi_ready_flag=0;//发送准备标志，0为准备，1为占用
u8 hmi_write_waiting=0;
u8 hmi_recieve_data[50], hmi_write_buf[4];//接收数据储存
u8 hmi_recieve[1];
u8 finish_bit[3]={0xff,0xff,0xff};

//初始化IO 串口2
//bound:波特率
void usart2_init(u32 bound)//默认921600，原装9600
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
	__HAL_RCC_USART2_CLK_ENABLE();			//使能USART2时钟
	
	GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3; //PA2,3
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;		//高速
	GPIO_Initure.Alternate=GPIO_AF7_USART2;	//复用为USART2
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA2,3
	
	HAL_NVIC_EnableIRQ(USART2_IRQn);				//使能USART1中断通道
	HAL_NVIC_SetPriority(USART2_IRQn,0,0);			//抢占优先级3，子优先级3
    
    //USART 初始化设置
	USART2_Handler.Instance=USART2;					    //USART2
	USART2_Handler.Init.BaudRate=bound;				    //波特率
	USART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;	//字长为8位数据格式
	USART2_Handler.Init.StopBits=UART_STOPBITS_1;		//一个停止位
	USART2_Handler.Init.Parity=UART_PARITY_NONE;		//无奇偶校验位
	USART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//无硬件流控
	USART2_Handler.Init.Mode=UART_MODE_TX_RX;			    //收发模式
	HAL_UART_Init(&USART2_Handler);					    //HAL_UART_Init()会使能USART2
  HAL_UART_Receive_IT(&USART2_Handler, &hmi_recieve[0], 1);
	
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
}

//显示单个字符 参数：名称（省略T），字符
u8 hmi_show_txt(u8 name, u8 number)
{
	u8 out[30];
	sprintf(out,"t%d.txt=\"%c\"",name, number);
	
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, out, strlen(out), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	hmi_ready_flag=0;
}

//显示字符串 参数：名称（省略T），字符数组指针
u8 hmi_show_string(u8 name, char* mystring)
{
	u8 out[30];
	sprintf(out,"t%d.txt=\"%s\"",name, mystring);
	
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, out, strlen(out), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	hmi_ready_flag=0;
}

//在文本类型控件中显示数字（此函数可能有bug） 参数：名称（省略T），字符，模式（0为整数，1为浮点数）
u8 hmi_show_number(u8 name, double number, u8 mode)
{
	u8 out[30];
	u32 temp=0;
	switch(mode){
		case 0: temp=(u32)number; sprintf(out,"t%d.txt=\"%d\"",name, temp);break;
		case 1: sprintf(out,"t%d.txt=\"%5.2f\"",name, number);break;
	}
	
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, out, strlen(out), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	
}

//显示整数 参数：名称（省略n），数字
void hmi_val(u8 name, int32_t number)
{
	u8 out[30];
	
	sprintf(out,"n%d.val=%d",name, number);
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, out, strlen(out), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	hmi_ready_flag=0;
}

//显示虚拟浮点数  参数：名称（省略x），数字，小数位位数
void hmi_xval(u8 name, float number, double point)
{
	u8 out[30];
	int32_t show_number = (int32_t)(number*pow(10, point)+0.5);
	sprintf(out,"x%d.val=%d",name, show_number);
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, out, strlen(out), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	hmi_ready_flag=0;
}

u8 get_hmi_recieve_flag(void)
{
	switch(hmi_recieve[0])
	{
		case 0x00: return 1;
		case 0x01: return 2;
		case 0x65: return 3;
		case 0x67: return 4;
		case 0xfe: return 5;
		case 0xfd: return 6;
		case 0x70: return 7;
		case 0x71: return 8;
		default: return 0;
	}
		
}

u8 hmi_read_data(void)
{
	switch(hmi_recieve_flag)
	{
		case 0: return 0xff;
		case 1: return 1;
		case 2: return 2;
		case 3: {
		  hmi_recieve_data[hmi_recieve_bit]=hmi_recieve[0];
			hmi_recieve_bit++;
			return 3;}
		case 4: {
			hmi_recieve_data[hmi_recieve_bit]=hmi_recieve[0];
			hmi_recieve_bit++;
			return 4;}
		case 5: return 5;
		case 6: return 6;
		case 7: {
			hmi_recieve_data[hmi_recieve_bit]=hmi_recieve[0];
			hmi_recieve_bit++;
			return 7;}
		case 8: {
			hmi_recieve_data[hmi_recieve_bit]=hmi_recieve[0];
			hmi_recieve_bit++;
			return 8;}
		default: return 0xff;
	}
			
}
//单片机接收到一个完整数据包之后的处理函数，针对各种数据的处理请在此函数内完成。否则自行修改中断函数
//数据类型已记录在变量hmi_recieve_flag中
u8 hmi_write_data(void)
{
//	u8 data=0, mode=0;
//	if(hmi_recieve_flag==3)
//	{
//		if(hmi_recieve_data[1]<24)
//			data = hmi_recieve_data[1]-14;
//		else
//			switch(hmi_recieve_data[1])
//			{
//				case 24: data = 0;  mode=0; printf("%d\n",data); break;
//				case 25: data = '<'; mode=1; printf("%c\n",data) ;break;
//				case 26: data = '>'; mode=1; printf("%c\n",data); break;
//				case 27: data = 'A'; mode=1; printf("%c\n",data); break;
//				case 28: data = 'B'; mode=1; printf("%c\n",data); break;
//				case 29: data = 'C'; mode=1; printf("%c\n",data); break;
//				case 30: data = 'D'; mode=1; printf("%c\n",data); break;
//			}
//			
//		if(hmi_ready_flag==0&&mode==1)
//			hmi_show_txt(HMI_ID_keyword,data);
//			else 
//				if(hmi_ready_flag==0&&mode==0)
//					hmi_show_number(HMI_ID_keyword,data,0);
//				else
//				{
//					hmi_write_waiting=1;
//					hmi_write_buf[0]=data;
//					hmi_write_buf[1]=mode;
//				}
//	}
}

//无视此函数
u8 hmi_write_supplement(void)
{
	if(hmi_write_waiting==1&&hmi_write_buf[1]==0)
		hmi_show_number(HMI_ID_keyword,hmi_write_buf[0],0);
	if(hmi_write_waiting==1&&hmi_write_buf[1]==1)
		hmi_show_txt(HMI_ID_keyword,hmi_write_buf[0]);
	hmi_write_waiting=0;
}
	
//数据透传 ，成功返回0，失败返回0xfe(指令失败)0xff(传输失败)	，参数依次为控件id、通道号、数据量、发送的数组指针
u8 serial_passthrough(u8 id, u8 ch, int lenth, u8* obj)
{
	int i=0, nmd_timeout=12000000;
	u8 nmd_transmit[20]={0};
	sprintf(nmd_transmit, "addt %d,%d,%d",id,ch,lenth);
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, nmd_transmit, strlen(nmd_transmit), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	
	for(i=0; i<nmd_timeout; i++)
		if(hmi_recieve_flag==5&&hmi_finish_flag==3)
			break;
	if(i>=nmd_timeout)
		return 0xfe;
	else
		HAL_UART_Transmit(&USART2_Handler, obj, lenth, 0xffff);
	
	for(i=0; i<nmd_timeout; i++)
		if(hmi_recieve_flag==6&&hmi_finish_flag==3)
			break;
		hmi_ready_flag=0;
	if(i>=nmd_timeout)
		return 0xff;
	else
		return 0;

}

//在曲线控件中打一个数据点 参数：曲线控件ID，通道，数据
u8 hmi_add_data(u8 id, u8 ch, int number)
{
	u8 out[20]={0};
	sprintf(out, "add %d,%d,%d",id,ch,number);
	hmi_ready_flag=1;
	HAL_UART_Transmit(&USART2_Handler, out, strlen(out), 0xffff);
	HAL_UART_Transmit(&USART2_Handler, finish_bit, 3, 0xffff);
	hmi_ready_flag=0;
}

//中断处理函数
void USART2_IRQHandler(void)                	
{
	HAL_UART_IRQHandler(&USART2_Handler);	//调用HAL库中断处理公用函数
	if(hmi_recieve[0]==0xff)
	{
		hmi_finish_flag++;
		if(hmi_finish_flag==3)
			hmi_write_data();
	}
	else 
	{
		if(hmi_recieve_flag!=0&&hmi_finish_flag!=3)
			hmi_finish_flag=0;
		if(hmi_finish_flag==3)
		{
			hmi_recieve_flag=get_hmi_recieve_flag();
			hmi_finish_flag=0;
			hmi_recieve_bit=0;
		}
		else
			hmi_read_data();
	}
		
//	printf("%x ",hmi_recieve[0]);
	//else
	//	printf("error\n");
	HAL_UART_Receive_IT(&USART2_Handler, &hmi_recieve[0], 1);
	
} 
#endif

