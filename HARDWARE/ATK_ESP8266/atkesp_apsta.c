#if ESPUSART3



#include "atkesp8266.h"

u8 apsta_string[64];
void atk8266_apsta_init(u8 apmode,u8 stamode,u8 ipbuf[16])//UDP0 TCPS1 TCPC2
{
	/*****主配置******/
	usart3_init(115200);delay_us(10);//初始化串口3波特率为115200
	while(atk_8266_send_cmd("AT","OK",20))//检测模块是否存在
	{
		atk_8266_quit_trans();//关闭透传
		atk_8266_send_cmd("AT+CIPMODE=0","OK",200); //关闭透传模式
		printf("未检测到模块!\r\n");
		delay_ms(800);
		printf("尝试连接模块...\r\n");
	}printf("模块连接成功...\r\n");
	while(atk_8266_send_cmd("ATE0","OK",20));printf("成功关闭回显\r\n");//反复关闭回显直到有OK出现
	/*****STA+AP主体配置******/
	printf("开始配置STA+AP\r\n");
	atk_8266_send_cmd("AT+CWMODE=3","OK",100);//设置为AP+STA模式
	atk_8266_send_cmd("AT+RST","OK",100);//重启
	delay_ms(1000);	delay_ms(1000);	delay_ms(1000); delay_ms(1000);//重启等待
	
	sprintf((char*)apsta_string,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password);
	atk_8266_send_cmd(apsta_string,"OK",1000);//反复配置AP参数
	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	sprintf((char*)apsta_string,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
	atk_8266_send_cmd(apsta_string,"WIFI GOT IP",300);					//连接目标路由器,并且获得IP
	/*******同步配置*******/
	while(atk_8266_send_cmd("AT+CIFSR","STAIP",20));   //检测是否获得STA IP
	while(atk_8266_send_cmd("AT+CIFSR","APIP",20));   //检测是否获得AP IP
	
	atk_8266_send_cmd("AT+CIPMUX=1","OK",50);   //0：单连接，1：多连接
	delay_ms(500);
	sprintf((char*)apsta_string,"AT+CIPSERVER=1,%s",(u8*)portnum);
	atk_8266_send_cmd(apsta_string,"OK",50);     //开启Server模式，端口号为8086
	delay_ms(500);
	atk_8266_send_cmd("AT+CIPSTO=1200","OK",50);     //设置服务器超时时间
	/*****STA+AP细节配置******/
	/**AP**/	
	if(stamode==0)//UDP
	{
		printf("目标机ip:%s,port:%s\r\n",ipbuf,(u8*)portnum);
		printf("尝试配置UDP...\r\n");
		sprintf((char*)apsta_string,"AT+CIPSTART=0,\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标UDP服务器
		while(atk_8266_send_cmd(apsta_string,"OK",500));//反复连接目标机
		printf("UDP配置成功并连接至目标机\r\n");
	}
	else if(stamode==1)//TCP Client
	{
		printf("目标机ip:%s,port:%s\r\n",ipbuf,(u8*)portnum);
		printf("尝试配置TCP_Client\r\n");
		sprintf((char*)apsta_string,"AT+CIPSTART=0,\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标UDP服务器
		while(atk_8266_send_cmd(apsta_string,"OK",500));//反复连接目标机
		printf("TCP_Client配置成功并连接至目标机\r\n");
	}
	else if(stamode==2)//TCP Server
	{
		printf("尝试配置TCP_Server\r\n");
		printf("TCP_Server配置成功\r\n");
		atk_8266_get_wanip(ipbuf);//服务器模式,获取IP
		sprintf((char*)apsta_string,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum);printf("%s\r\n",apsta_string);
		/****等待连接******/
		printf("请用设备连接WIFI热点:%s,%s,%s",(u8*)wifiap_ssid,(u8*)wifiap_encryption,(u8*)wifiap_password);
	}
}
/*



















//ATK-ESP8266 AP+STA模式测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码
extern u8 led0sta;
u8 atk_8266_apsta_test(void)
{

	USART3_RX_STA=0;
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)			//WK_UP 退出测试		 
		{ 
			res=0;	
			break;												 
		}
		else if(key==KEY0_PRES)	//KEY0 向ID0发送数据 
		{
			  sprintf((char*)p,"ATK-8266模块ID0发数据%02d\r\n",t/10);//测试数据
				Show_Str(15+54,125,200,12,p,12,0);
				atk_8266_send_cmd("AT+CIPSEND=0,25","OK",200);  //发送指定长度的数据
				delay_ms(200);
				atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
				timex=100;
		}
		else if(key==KEY1_PRES)	//KEY1 向ID1发送数据 
		{
			  sprintf((char*)p,"ATK-8266模块ID1发数据%02d\r\n",t/10);//测试数据
				Show_Str(15+54,125,200,12,p,12,0);
				atk_8266_send_cmd("AT+CIPSEND=1,25","OK",200);  //发送指定长度的数据
				delay_ms(200);
				atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
				timex=100;
		}
			
	 if(timex)timex--;
 	 if(timex==1)LCD_Fill(30+54,125,239,122,WHITE);
	 t++;
	 delay_ms(10);
	 if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
		USART3_RX_BUF[rlen]=0;		//添加结束符 
		printf("%s",USART3_RX_BUF);	//发送到串口   
		sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数 
		LCD_Fill(15+54,140,239,152,WHITE);
		POINT_COLOR=BRED;
		Show_Str(15+54,140,156,12,p,12,0); 			//显示接收到的数据长度
		POINT_COLOR=BLUE;
		LCD_Fill(15,155,239,319,WHITE);
		Show_Str(15,155,180,190,USART3_RX_BUF,12,0);//显示接收到的数据  
		USART3_RX_STA=0;
		if(constate!='+')t=1000;		//状态为还未连接,立即更新连接状态
		else t=0;                   //状态为已经连接了,10秒后再检查
	}  
	if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
	{
		LCD_Fill(15+54,125,239,138,WHITE);
//		LCD_Fill(15+54,95,120,110,WHITE);
		constate=atk_8266_consta_check();//得到连接状态
		if(constate=='+')Show_Str(15+54,95,200,12,"连接成功",12,0);  //连接状态
		else Show_Str(15+54,95,200,12,"连接失败",12,0); 	 
		t=0;
	}
	if((t%20)==0)LED0_Toggle;
	atk_8266_at_response(1);
}
	myfree(SRAMIN,p);		//释放内存 
	return res;		
} 














*/






#endif





