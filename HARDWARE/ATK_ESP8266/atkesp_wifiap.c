#if ESPUSART3


#include "atkesp8266.h"

u8 ap_string[64];
void atk8266_ap_init(u8 mode,u8 ipbuf[16])//UDP0 TCPS1 TCPC2
//192.168.1.2
//如果是UDP或TCP客户端，则inpuf需填入ip号码，如果是TCP服务端则不管即可
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
	/*****AP主体配置******/
	printf("开始配置AP\r\n");
	atk_8266_send_cmd("AT+CWMODE=2","OK",100);//设置为AP模式
	atk_8266_send_cmd("AT+RST","OK",100);//重启
	delay_ms(1000);	delay_ms(1000);	delay_ms(1000); delay_ms(1000);//重启等待
	sprintf((char*)ap_string,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password);
	atk_8266_send_cmd(ap_string,"OK",1000);//反复配置AP参数
	/*****AP细节配置******/
	if(mode==0)//UDP
	{
		printf("目标机ip:%s,port:%s\r\n",ipbuf,(u8*)portnum);
		printf("尝试配置UDP...\r\n");
		atk_8266_send_cmd("AT+CIPMUX=0","OK",100);  //单链接模式
		sprintf((char*)ap_string,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标UDP服务器
		while(atk_8266_send_cmd(ap_string,"OK",500));//反复连接目标机
		printf("UDP配置成功并连接至目标机\r\n");
	}
	else if(mode==1)//TCP Client
	{
		printf("目标机ip:%s,port:%s\r\n",ipbuf,(u8*)portnum);
		printf("尝试配置TCP_Client\r\n");
		atk_8266_send_cmd("AT+CIPMUX=0","OK",100);  //单链接模式
		sprintf((char*)ap_string,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标UDP服务器
		while(atk_8266_send_cmd(ap_string,"OK",200));//反复连接目标机	
		atk_8266_send_cmd("AT+CIPMODE=1","OK",200);//传输模式为：透传		
		printf("TCP_Client配置成功并连接至目标机\r\n");
	}
	else if(mode==2)//TCP Server
	{
		printf("尝试配置TCP_Server\r\n");
		atk_8266_send_cmd("AT+CIPMUX=1","OK",20);//1：多连接
		sprintf((char*)ap_string,"AT+CIPSERVER=1,%s",(u8*)portnum);
		atk_8266_send_cmd(ap_string,"OK",20);//配置TCP主机
		printf("TCP_Server配置成功\r\n");
		atk_8266_get_wanip(ipbuf);//服务器模式,获取IP
		sprintf((char*)ap_string,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum);printf("%s\r\n",ap_string);
		/****等待连接******/
		printf("请用设备连接WIFI热点:%s,%s,%s",(u8*)wifiap_ssid,(u8*)wifiap_encryption,(u8*)wifiap_password);
	}
}

void atk8266_ap_data_transform(u8 mode,u8 stop,u32 byte_size,u8* data)
{
	u8 tem_string[32];
	if(stop)
	{
		atk_8266_quit_trans();	//退出透传
		atk_8266_send_cmd("AT+CIPMODE=0","OK",20);   //关闭透传模式
		return;
	}
	if(mode==0)//UDP
	{
		sprintf((char*)tem_string,"AT+CIPSEND=%d",byte_size);
		atk_8266_send_cmd(tem_string,"OK",200);  //发送指定长度的数据的预备指令
		delay_ms(200);
		atk_8266_send_data(data,"OK",100);  //发送指定长度的数据
	}
	else if(mode==1)//TCP Client
	{
		atk_8266_quit_trans();
		atk_8266_send_cmd("AT+CIPSEND","OK",20);       //开始透传
		u3_printf("%s",data);
	}
	else if(mode==2)//TCP Server
	{
		sprintf((char*)tem_string,"AT+CIPSEND=0,%d",byte_size);
		atk_8266_send_cmd(tem_string,"OK",200);  //发送指定长度的数据的预备指令
		delay_ms(200);
		atk_8266_send_data(data,"OK",100);  //发送指定长度的数据
	}
}
void atk8266_ap_check_connect(void)
{
	if(atk_8266_consta_check()=='+')printf("连接成功");  //连接状态
	else printf("连接失败"); 	  	
}
//atk_8266_at_response即为一个标准的接收模板
/*
if(USART3_RX_STA&0X8000)		//接收到一次数据了
{ 
	rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
	USART3_RX_BUF[rlen]=0;		//添加结束符 
	printf("%s",USART3_RX_BUF);	//发送到串口   
		
	USART3_RX_STA=0;
}  
*/

#endif








