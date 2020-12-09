#if ESPUSART3




#include "atkesp8266.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////tim
//基本定时器7中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM7_Int_Init(u16 arr,u16 psc)
{
		TIM7_Handler.Instance=TIM7;                          //通用定时器3
    TIM7_Handler.Init.Prescaler=psc;                     //分频系数
    TIM7_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM7_Handler.Init.Period=arr;                        //自动装载值
    TIM7_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM7_Handler);
		/**msp**/
		__HAL_RCC_TIM7_CLK_ENABLE();            //使能TIM7时钟
		HAL_NVIC_SetPriority(TIM7_IRQn,0,1);    //设置中断优先级，抢占优先级0，子优先级1
		HAL_NVIC_EnableIRQ(TIM7_IRQn);          //开启ITM7中断 
    /**irq**/
    HAL_TIM_Base_Start_IT(&TIM7_Handler); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE									 
}	

void TIM7_IRQHandler(void)
{ 	    		    
		USART3_RX_STA|=1<<15;	//标记接收完成
		__HAL_TIM_CLEAR_FLAG(&TIM7_Handler,TIM_EventSource_Update );       //清除TIM7更新中断标志  
		TIM7->CR1&=~(1<<0);     			//关闭定时器7     											 
} 
//////////////////////////////////////////////////////////////////////////////////////////////////////usart
/************************
	base function
************************/ 
/***********handle********************/  
UART_HandleTypeDef UART3_Handler; //UART句柄
TIM_HandleTypeDef TIM7_Handler;      //定时器句柄 
/**********data space*****************/
//串口发送缓存区 	
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//发送缓冲,最大USART3_MAX_SEND_LEN字节  	  
//串口接收缓存区 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u16 USART3_RX_STA=0; 
/**********function******************/	
//初始化IO 串口3//bound:波特率
void usart3_init(u32 bound)
{	
	/**msp**/
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();			//使能GPIOB时钟
	__HAL_RCC_USART3_CLK_ENABLE();			//使能USART3时钟
	GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;			//PB10
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;		//高速
	GPIO_Initure.Alternate=GPIO_AF7_USART3;	//复用为USART3
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//初始化PB10
	//UART 初始化设置
	UART3_Handler.Instance=USART3;					    //USART3
	UART3_Handler.Init.BaudRate=bound;				    //波特率
	UART3_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART3_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART3_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART3_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART3_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART3_Handler);					    //HAL_UART_Init()会使能UART1
	/******NVIC*********/
	HAL_NVIC_EnableIRQ(USART3_IRQn);			
	HAL_NVIC_SetPriority(USART3_IRQn,2,3);		
	/******IQR**********/
	__HAL_UART_ENABLE_IT(&UART3_Handler,UART_IT_RXNE);	
	
	TIM7_Int_Init(1000-1,9000-1);		//100ms中断
	USART3_RX_STA=0;		//清零
	TIM7->CR1&=~(1<<0);        //关闭定时器7
}

//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
		while((USART3->ISR&0X40)==0);			//循环发送,直到发送完毕   
		USART3->TDR=USART3_TX_BUF[j];  
	} 
}

//接收完成标志只在两种情况下打开，一种是接收到的字符数目超过了限制，则认为接收完毕，
//另一种是在接收到第一个字符后就打开了一个定时器，定时器计录一个时间，我们认为，单次传输无论如何都会在这个时间内完成
//也就是说在这个时间内，无论受到多少数据，我们都要在定时结束后认为此轮接收完毕。
//于是自然而然的，需要在外界检测这个标志位
void USART3_IRQHandler(void)
{
	u8 res;	      
	if(__HAL_UART_GET_FLAG(&UART3_Handler,UART_FLAG_RXNE)!=RESET)//接收到数据
	{	 
//		HAL_UART_Receive(&UART3_Handler,&res,1,1000);
		res=USART3->RDR; 			 
		if((USART3_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据，处理完毕即变为0才可继续接收
		{ 
			if(USART3_RX_STA<USART3_MAX_RECV_LEN)	//还可以接收数据
			{
//				__HAL_TIM_SetCounter(&TIM7_Handler,0);	
				TIM7->CNT=0;         				//计数器清空	
				if(USART3_RX_STA==0) 				//使能定时器7的中断 
				{
//					__HAL_RCC_TIM7_CLK_ENABLE();            //使能TIM7时钟
					TIM7->CR1|=1<<0;     			//使能定时器7
				}
				USART3_RX_BUF[USART3_RX_STA++]=res;	//记录接收到的值	 
			}else 
			{
				USART3_RX_STA|=1<<15;				//强制标记接收完成
			} 
		}
	}  
	HAL_UART_IRQHandler(&UART3_Handler);	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////esp8266 
//用户配置区

//连接端口号:8086,可自行修改为其他端口.
const u8* portnum="8086";		 

//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
const u8* wifista_ssid="iPhone";			//路由器SSID号
const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifista_password="77777777"; 	//连接密码

//WIFI AP模式,模块对外的无线参数,可自行修改.
const u8* wifiap_ssid="ATK-ESP8266";			//对外SSID号
const u8* wifiap_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifiap_password="12345678"; 		//连接密码 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//4个网络模式
const u8 *ATK_ESP8266_CWMODE_TBL[3]={"STA模式 ","AP模式 ","AP&STA模式 "};	//ATK-ESP8266,3种网络模式,默认为路由器(ROUTER)模式 
//4种工作模式
const u8 *ATK_ESP8266_WORKMODE_TBL[3]={"TCP服务器","TCP客户端"," UDP 模式"};	//ATK-ESP8266,4种工作模式
//5种加密方式
const u8 *ATK_ESP8266_ECN_TBL[5]={"OPEN","WEP","WPA_PSK","WPA2_PSK","WPA_WAP2_PSK"};
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
u8 led0sta=1;
//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void atk_8266_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		printf("%s",USART3_RX_BUF);	//发送到串口
		if(mode)USART3_RX_STA=0;
	} 
}
//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* atk_8266_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))
				{
					printf("ack:%s\r\n",(u8*)ack);
					break;//得到有效数据 
				}
					USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//向ATK-ESP8266发送指定数据
//data:发送的数据(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)luojian
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s",data);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))break;//得到有效数据 
				USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}
//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
u8 atk_8266_quit_trans(void)
{
	while((USART3->ISR&0X40)==0);	//等待发送空
	USART3->TDR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->ISR&0X40)==0);	//等待发送空
	USART3->TDR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->ISR&0X40)==0);	//等待发送空
	USART3->TDR='+';      
	delay_ms(500);					//等待500ms
	return atk_8266_send_cmd("AT","OK",20);//退出透传判断.
}
//获取ATK-ESP8266模块的AP+STA连接状态
//返回值:0，未连接;1,连接成功
u8 atk_8266_apsta_check(void)
{
	if(atk_8266_quit_trans())return 0;			//退出透传 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//发送AT+CIPSTATUS指令,查询连接状态
	if(atk_8266_check_cmd("+CIPSTATUS:0")&&
		 atk_8266_check_cmd("+CIPSTATUS:1")&&
		 atk_8266_check_cmd("+CIPSTATUS:2")&&
		 atk_8266_check_cmd("+CIPSTATUS:4"))
		return 0;
	else return 1;
}
//获取ATK-ESP8266模块的连接状态
//返回值:0,未连接;1,连接成功.
u8 atk_8266_consta_check(void)
{
	u8 *p;
	u8 res;
	if(atk_8266_quit_trans())return 0;			//退出透传 
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	//发送AT+CIPSTATUS指令,查询连接状态
	p=atk_8266_check_cmd("+CIPSTATUS:"); 
	res=*p;									//得到连接状态	
	return res;
}

//获取Client ip地址
//ipbuf:ip地址输出缓存区
void atk_8266_get_wanip(u8* ipbuf)
{
	u8 *p,*p1;
		if(atk_8266_send_cmd("AT+CIFSR","OK",50))//获取WAN IP地址失败
		{
			ipbuf[0]=0;
			return;
		}		
		p=atk_8266_check_cmd("\"");
		p1=(u8*)strstr((const char*)(p+1),"\"");
		*p1=0;
		sprintf((char*)ipbuf,"%s",p+1);	
}
/*
//获取AP+STA ip地址并在指定位置显示
//ipbuf:ip地址输出缓存区
void atk_8266_get_ip(u8 x,u8 y)
{
		u8 *p;
		u8 *p1;
		u8 *p2;
		u8 *ipbuf;
		u8 *buf;
		p=mymalloc(SRAMIN,32);							//申请32字节内存
		p1=mymalloc(SRAMIN,32);							//申请32字节内存
		p2=mymalloc(SRAMIN,32);							//申请32字节内存
	  ipbuf=mymalloc(SRAMIN,32);							//申请32字节内存
		buf=mymalloc(SRAMIN,32);							//申请32字节内存
		if(atk_8266_send_cmd("AT+CIFSR","OK",50))//获取WAN IP地址失败
		{ 
			*ipbuf=0;
		}
		else
		{		
			p=atk_8266_check_cmd("APIP,\"");
			p1=(u8*)strstr((const char*)(p+6),"\"");
			p2=p1;
			*p1=0;
			ipbuf=p+6;
			sprintf((char*)buf,"AP IP:%s 端口:%s",ipbuf,(u8*)portnum);
			Show_Str(x,y,200,12,buf,12,0);				//显示AP模式的IP地址和端口
			p=(u8*)strstr((const char*)(p2+1),"STAIP,\"");
			p1=(u8*)strstr((const char*)(p+7),"\"");
			*p1=0;
			ipbuf=p+7;
			sprintf((char*)buf,"STA IP:%s 端口:%s",ipbuf,(u8*)portnum);
			Show_Str(x,y+15,200,12,buf,12,0);				//显示STA模式的IP地址和端口
			myfree(SRAMIN,p);		//释放内存
			myfree(SRAMIN,p1);		//释放内存
			myfree(SRAMIN,p2);		//释放内存
			myfree(SRAMIN,ipbuf);		//释放内存
			myfree(SRAMIN,buf);		//释放内存
		}
}
//ATK-ESP8266模块信息显示
//x,y:显示信息的起始坐标.
//wanip:0,全部更新显示;1,仅更新wanip.
void atk_8266_msg_show(u16 x,u16 y,u8 wanip)
{
	u8 *p,*p1,*p2;
	p=mymalloc(SRAMIN,32);							//申请32字节内存
	p1=mymalloc(SRAMIN,32);							//申请32字节内存
	p2=mymalloc(SRAMIN,32);							//申请32字节内存
	POINT_COLOR=BLUE;
	atk_8266_send_cmd("AT+CWMODE=2","OK",20);
	atk_8266_send_cmd("AT+RST","OK",20);
	delay_ms(1000);//延时2s等待模块重启
	delay_ms(1000);//
	delay_ms(1000);
	delay_ms(1000);
	sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password);    //配置模块AP模式无线参数
	atk_8266_send_cmd(p,"OK",1000);
	if(wanip==0)//全更新
	{
		atk_8266_send_cmd("AT+GMR","OK",20);		//获取固件版本号
		p=atk_8266_check_cmd("SDK version:");
		p1=(u8*)strstr((const char*)(p+12),"(");
		*p1=0;
		Show_Str(x,y,240,16,"固件版本:",16,0);
		Show_Str(x+72,y,240,16,p+12,16,0);
		atk_8266_send_cmd("AT+CWMODE?","+CWMODE:",20);	//获取网络模式
		p=atk_8266_check_cmd(":");
		Show_Str(x,y+16,240,16,"网络模式:",16,0);Show_Str(x+72,y+16,240,16,(u8*)ATK_ESP8266_CWMODE_TBL[*(p+1)-'1'],16,0);
  	atk_8266_send_cmd("AT+CWSAP?","+CWSAP:",20);	//获取wifi配置
		p=atk_8266_check_cmd("\"");
		p1=(u8*)strstr((const char*)(p+1),"\"");
		p2=p1;
		*p1=0;
		Show_Str(x,y+32,240,16,"SSID号:",16,0);Show_Str(x+56,y+32,240,16,p+1,16,0);
		p=(u8*)strstr((const char*)(p2+1),"\"");
		p1=(u8*)strstr((const char*)(p+1),"\"");
		p2=p1;
		*p1=0;		
		Show_Str(x,y+48,240,16,"密码:",16,0);Show_Str(x+40,y+48,240,16,p+1,16,0);
		p=(u8*)strstr((const char*)(p2+1),",");
		p1=(u8*)strstr((const char*)(p+1),",");
		*p1=0;
		Show_Str(x,y+64,240,16,"通道号:",16,0);Show_Str(x+56,y+64,240,16,p+1,16,0);
		Show_Str(x,y+80,240,16,"加密方式:",16,0);Show_Str(x+72,y+80,240,16,(u8*)ATK_ESP8266_ECN_TBL[*(p1+1)-'0'],16,0);
	}
	myfree(SRAMIN,p);		//释放内存 
	myfree(SRAMIN,p1);		//释放内存 
	myfree(SRAMIN,p2);		//释放内存 
}
//ATK-ESP8266模块WIFI配置参数显示(仅WIFI STA/WIFI AP模式测试时使用)
//x,y:显示信息的起始坐标.
//rmd:提示信息
//ssid,encryption,password:无线网络的SSID,加密方式,密码
void atk_8266_wificonf_show(u16 x,u16 y,u8* rmd,u8* ssid,u8* encryption,u8* password)
{ 
	POINT_COLOR=RED;
	Show_Str(x,y,240,12,rmd,12,0);//显示提示信息 
	Show_Str(x,y+20,240,12,"SSID:",12,0);
	Show_Str(x,y+36,240,12,"加密方式:",12,0);
	Show_Str(x,y+52,240,12,"密码:",12,0); 
	POINT_COLOR=BLUE;
	Show_Str(x+30,y+20,240,12,ssid,12,0);
	Show_Str(x+54,y+36,240,12,encryption,12,0);
	Show_Str(x+30,y+52,240,12,password,12,0); 	  
}
//工作模式选择
//返回值:
//0,TCP服务器
//1,TCP客户端
//2,UDP模式
u8 atk_8266_netpro_sel(u16 x,u16 y,u8* name) 
{
	u8 key=0,t=0,*p;
	u8 netpro=0;
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	p=mymalloc(SRAMIN,50);//申请50个字节的内存
	sprintf((char*)p,"%s 工作模式选择",name);
	Show_Str_Mid(0,y,p,16,240); 				    	 
	Show_Str(x,y+30,200,16,"KEY0:下一个",16,0); 				    	 
	Show_Str(x,y+50,200,16,"KEY1:上一个",16,0);				    	 
	Show_Str(x,y+70,200,16,"WK_UP:确定",16,0); 
	Show_Str(x,y+100,200,16,"请选择:",16,0); 
	POINT_COLOR=BLUE;
 	Show_Str(x+16,y+120,200,16,"TCP服务器",16,0); 				    	 
	Show_Str(x+16,y+140,200,16,"TCP客户端",16,0);				    	 
	Show_Str(x+16,y+160,200,16,"UDP模式",16,0); 
	POINT_COLOR=RED;
	Show_Str(x,y+120,200,16,"→",16,0); 
	while(1)
	{
		key=KEY_Scan(0); 
		if(key)
		{
			if(key==WKUP_PRES)break;       //WK_UP按下
			Show_Str(x,y+120+netpro*20,200,16,"  ",16,0);//清空之前的显示
			if(key==KEY0_PRES)//KEY0按下
			{
				if(netpro<2)netpro++;
				else netpro=0;
			}else if(key==KEY1_PRES)//KEY1按下
			{
				if(netpro>0)netpro--;
				else netpro=2; 
			}
			Show_Str(x,y+120+netpro*20,200,16,"→",16,0);//指向新条目
				
		} 
		delay_ms(10);
		atk_8266_at_response(1);
		if((t++)==20){t=0;LED0_Toggle;}//LED闪烁
	} 
	myfree(SRAMIN,p);
	return netpro;//返回网络模式选择值 
} 

//STA模式下的AP的TCP、UDP工作模式配置
u8 atk_8266_mode_cofig(u8 netpro)
{
	//u8 netpro;
	u8 ipbuf[16]; 	//IP缓存
	u8 *p;
	u8 key;
	p=mymalloc(SRAMIN,32);//申请32个字节的内存
PRESTA:		
	netpro|=(atk_8266_netpro_sel(50,30,(u8*)ATK_ESP8266_CWMODE_TBL[1]))<<4;	//网络模式选择
	if(netpro&0X20)
	{
		LCD_Clear(WHITE);
		if(atk_8266_ip_set("WIFI-AP远端UDP IP设置","UDP模式",(u8*)portnum,ipbuf))goto PRESTA;	//IP输入
		if(netpro&0X03)sprintf((char*)p,"AT+CIPSTART=1,\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标UDP服务器,及ID号，STA模式下为0
		else sprintf((char*)p,"AT+CIPSTART=0,\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标UDP服务器,及ID号，STA模式下为0
		delay_ms(200);
		LCD_Clear(WHITE);
		atk_8266_send_cmd(p,"OK",200);
	}
	else if(netpro&0X10)     //AP TCP Client    透传模式测试
	{
		LCD_Clear(WHITE);
		POINT_COLOR=RED;
		Show_Str_Mid(0,30,"ATK-ESP WIFI-STA 测试",16,240); 
		Show_Str(30,50,200,16,"正在配置ATK-ESP模块,请稍等...",12,0);
		if(atk_8266_ip_set("WIFI-AP 远端IP设置","TCP Client",(u8*)portnum,ipbuf))goto PRESTA;	//IP输入
		if(netpro&0X03)sprintf((char*)p,"AT+CIPSTART=1,\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器,及ID号，STA模式为client时，为1
		else sprintf((char*)p,"AT+CIPSTART=0,\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器,及ID号，STA模式为server时，为0
		while(atk_8266_send_cmd(p,"OK",200))
			{
				LCD_Clear(WHITE);
				POINT_COLOR=RED;
				Show_Str_Mid(0,40,"WK_UP:返回重选",16,240);
				Show_Str(30,80,200,12,"ATK-ESP 连接TCP SERVER失败",12,0); //连接失败	 
				key=KEY_Scan(0);
				if(key==WKUP_PRES)goto PRESTA;
			}				
	}
	else;   //服务器模式不用配置
	myfree(SRAMIN,p);
	return netpro;
}




//IP设置
//title:ip设置标题
//mode:工作模式
//port:端口号
//ip:ip缓存区(返回IP给上层函数)
//返回值:0,确认连接;1,取消连接.
u8 atk_8266_ip_set(u8* title,u8* mode,u8* port,u8* ip) 
{
	u8 res=0;
 	u8 key;
	u8 timex=0;  
	u8 iplen=0;			//IP长度 
	LCD_Clear(WHITE);  
	POINT_COLOR=RED; 
	Show_Str_Mid(0,30,title,16,240);		//显示标题	 	 	
	Show_Str(30,90,200,16,"工作模式:",16,0);	//工作模式显示
	Show_Str(30,110,200,16,"IP地址:",16,0);	//IP地址可以键盘设置
	Show_Str(30,130,200,16,"端口:",16,0);	//端口号
	kbd_fn_tbl[0]="连接";
	kbd_fn_tbl[1]="返回"; 
	atk_8266_load_keyboard(0,180);			//显示键盘 
	POINT_COLOR=BLUE;
 	Show_Str(30+72,90,200,16,mode,16,0);	//显示工作模式	
 	Show_Str(30+40,130,200,16,port,16,0);	//显示端口 	
	ip[0]=0; 		
	while(1)
	{ 
		key=atk_8266_get_keynum(0,180);
		if(key)
		{   
			if(key<12)
			{
				if(iplen<15)
				{ 
					ip[iplen++]=kbd_tbl[key-1][0];
				}
			}else
			{
				if(key==13)if(iplen)iplen--;	//删除  
				if(key==14&&iplen)break; 		//确认连接 
				if(key==15){res=1;break;}		//取消连接
			} 
			ip[iplen]=0; 
			LCD_Fill(30+56,110,239,110+16,WHITE);
			Show_Str(30+56,110,200,16,ip,16,0);			//显示IP地址 	
		} 
		timex++;
		if(timex==20)
		{
			timex=0;
			LED0_Toggle;
		}
		delay_ms(10);
		atk_8266_at_response(1);//WIFI模块发过来的数据,及时上传给电脑
	} 
	return res;
}
//测试界面主UI
void atk_8266_mtest_ui(u16 x,u16 y)
{ 
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str_Mid(0,y,"ATK_ESP8266 WIFI模块测试",16,240); 
	Show_Str(x,y+25,200,16,"请选择:",16,0); 				    	 
	Show_Str(x,y+45,200,16,"KEY0:WIFI STA+AP",16,0); 				    	 
	Show_Str(x,y+65,200,16,"KEY1:WIFI STA",16,0);				    	 
	Show_Str(x,y+85,200,16,"WK_UP:WIFI AP",16,0); 
 	atk_8266_msg_show(x,y+125,0);
}
//ATK-ESP8266模块测试主函数
void atk_8266_test(void)
{
//	u16 rlen=0;
	u8 key;
	u8 timex;
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-ESP8266 WIFI模块测试",16,240); 
	while(atk_8266_send_cmd("AT","OK",20))//检查WIFI模块是否在线
	{
		atk_8266_quit_trans();//退出透传
		atk_8266_send_cmd("AT+CIPMODE=0","OK",200);  //关闭透传模式	
		Show_Str(40,55,200,16,"未检测到模块!!!",16,0);
		delay_ms(800);
		LCD_Fill(40,55,200,55+16,WHITE);
		Show_Str(40,55,200,16,"尝试连接模块...",16,0); 
	} 
		while(atk_8266_send_cmd("ATE0","OK",20));//关闭回显
		atk_8266_mtest_ui(32,30);
	while(1)
	{
		delay_ms(10); 
		atk_8266_at_response(1);//检查ATK-ESP8266模块发送过来的数据,及时上传给电脑
		key=KEY_Scan(0); 
		if(key)
		{
			LCD_Clear(WHITE);
			POINT_COLOR=RED;
			switch(key)
			{
				case 1://KEY0
					Show_Str_Mid(0,30,"ATK-ESP WIFI-AP+STA 测试",16,240);
					Show_Str_Mid(0,50,"正在配置ATK-ESP8266模块，请稍等...",12,240);
					atk_8266_apsta_test();	//串口以太网测试
					break;
				case 2://KEY1
					Show_Str_Mid(0,30,"ATK-ESP WIFI-STA 测试",16,240);
					Show_Str_Mid(0,50,"正在配置ATK-ESP8266模块，请稍等...",12,240);
					atk_8266_wifista_test();//WIFI STA测试
					break;
				case 4://WK_UP
					atk_8266_wifiap_test();	//WIFI AP测试
					break;
			}
			atk_8266_mtest_ui(32,30);
			timex=0;
		} 	 
		if((timex%20)==0)LED0_Toggle;//200ms闪烁 
		timex++;	 
	} 
}
*/


































#endif











