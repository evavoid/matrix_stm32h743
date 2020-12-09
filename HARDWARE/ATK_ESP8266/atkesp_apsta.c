#if ESPUSART3



#include "atkesp8266.h"

u8 apsta_string[64];
void atk8266_apsta_init(u8 apmode,u8 stamode,u8 ipbuf[16])//UDP0 TCPS1 TCPC2
{
	/*****������******/
	usart3_init(115200);delay_us(10);//��ʼ������3������Ϊ115200
	while(atk_8266_send_cmd("AT","OK",20))//���ģ���Ƿ����
	{
		atk_8266_quit_trans();//�ر�͸��
		atk_8266_send_cmd("AT+CIPMODE=0","OK",200); //�ر�͸��ģʽ
		printf("δ��⵽ģ��!\r\n");
		delay_ms(800);
		printf("��������ģ��...\r\n");
	}printf("ģ�����ӳɹ�...\r\n");
	while(atk_8266_send_cmd("ATE0","OK",20));printf("�ɹ��رջ���\r\n");//�����رջ���ֱ����OK����
	/*****STA+AP��������******/
	printf("��ʼ����STA+AP\r\n");
	atk_8266_send_cmd("AT+CWMODE=3","OK",100);//����ΪAP+STAģʽ
	atk_8266_send_cmd("AT+RST","OK",100);//����
	delay_ms(1000);	delay_ms(1000);	delay_ms(1000); delay_ms(1000);//�����ȴ�
	
	sprintf((char*)apsta_string,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password);
	atk_8266_send_cmd(apsta_string,"OK",1000);//��������AP����
	//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	sprintf((char*)apsta_string,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//�������߲���:ssid,����
	atk_8266_send_cmd(apsta_string,"WIFI GOT IP",300);					//����Ŀ��·����,���һ��IP
	/*******ͬ������*******/
	while(atk_8266_send_cmd("AT+CIFSR","STAIP",20));   //����Ƿ���STA IP
	while(atk_8266_send_cmd("AT+CIFSR","APIP",20));   //����Ƿ���AP IP
	
	atk_8266_send_cmd("AT+CIPMUX=1","OK",50);   //0�������ӣ�1��������
	delay_ms(500);
	sprintf((char*)apsta_string,"AT+CIPSERVER=1,%s",(u8*)portnum);
	atk_8266_send_cmd(apsta_string,"OK",50);     //����Serverģʽ���˿ں�Ϊ8086
	delay_ms(500);
	atk_8266_send_cmd("AT+CIPSTO=1200","OK",50);     //���÷�������ʱʱ��
	/*****STA+APϸ������******/
	/**AP**/	
	if(stamode==0)//UDP
	{
		printf("Ŀ���ip:%s,port:%s\r\n",ipbuf,(u8*)portnum);
		printf("��������UDP...\r\n");
		sprintf((char*)apsta_string,"AT+CIPSTART=0,\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��UDP������
		while(atk_8266_send_cmd(apsta_string,"OK",500));//��������Ŀ���
		printf("UDP���óɹ���������Ŀ���\r\n");
	}
	else if(stamode==1)//TCP Client
	{
		printf("Ŀ���ip:%s,port:%s\r\n",ipbuf,(u8*)portnum);
		printf("��������TCP_Client\r\n");
		sprintf((char*)apsta_string,"AT+CIPSTART=0,\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��UDP������
		while(atk_8266_send_cmd(apsta_string,"OK",500));//��������Ŀ���
		printf("TCP_Client���óɹ���������Ŀ���\r\n");
	}
	else if(stamode==2)//TCP Server
	{
		printf("��������TCP_Server\r\n");
		printf("TCP_Server���óɹ�\r\n");
		atk_8266_get_wanip(ipbuf);//������ģʽ,��ȡIP
		sprintf((char*)apsta_string,"IP��ַ:%s �˿�:%s",ipbuf,(u8*)portnum);printf("%s\r\n",apsta_string);
		/****�ȴ�����******/
		printf("�����豸����WIFI�ȵ�:%s,%s,%s",(u8*)wifiap_ssid,(u8*)wifiap_encryption,(u8*)wifiap_password);
	}
}
/*



















//ATK-ESP8266 AP+STAģʽ����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//    ����,�������
extern u8 led0sta;
u8 atk_8266_apsta_test(void)
{

	USART3_RX_STA=0;
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)			//WK_UP �˳�����		 
		{ 
			res=0;	
			break;												 
		}
		else if(key==KEY0_PRES)	//KEY0 ��ID0�������� 
		{
			  sprintf((char*)p,"ATK-8266ģ��ID0������%02d\r\n",t/10);//��������
				Show_Str(15+54,125,200,12,p,12,0);
				atk_8266_send_cmd("AT+CIPSEND=0,25","OK",200);  //����ָ�����ȵ�����
				delay_ms(200);
				atk_8266_send_data(p,"OK",100);  //����ָ�����ȵ�����
				timex=100;
		}
		else if(key==KEY1_PRES)	//KEY1 ��ID1�������� 
		{
			  sprintf((char*)p,"ATK-8266ģ��ID1������%02d\r\n",t/10);//��������
				Show_Str(15+54,125,200,12,p,12,0);
				atk_8266_send_cmd("AT+CIPSEND=1,25","OK",200);  //����ָ�����ȵ�����
				delay_ms(200);
				atk_8266_send_data(p,"OK",100);  //����ָ�����ȵ�����
				timex=100;
		}
			
	 if(timex)timex--;
 	 if(timex==1)LCD_Fill(30+54,125,239,122,WHITE);
	 t++;
	 delay_ms(10);
	 if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
		USART3_RX_BUF[rlen]=0;		//��ӽ����� 
		printf("%s",USART3_RX_BUF);	//���͵�����   
		sprintf((char*)p,"�յ�%d�ֽ�,��������",rlen);//���յ����ֽ��� 
		LCD_Fill(15+54,140,239,152,WHITE);
		POINT_COLOR=BRED;
		Show_Str(15+54,140,156,12,p,12,0); 			//��ʾ���յ������ݳ���
		POINT_COLOR=BLUE;
		LCD_Fill(15,155,239,319,WHITE);
		Show_Str(15,155,180,190,USART3_RX_BUF,12,0);//��ʾ���յ�������  
		USART3_RX_STA=0;
		if(constate!='+')t=1000;		//״̬Ϊ��δ����,������������״̬
		else t=0;                   //״̬Ϊ�Ѿ�������,10����ټ��
	}  
	if(t==1000)//����10����û���յ��κ�����,��������ǲ��ǻ�����.
	{
		LCD_Fill(15+54,125,239,138,WHITE);
//		LCD_Fill(15+54,95,120,110,WHITE);
		constate=atk_8266_consta_check();//�õ�����״̬
		if(constate=='+')Show_Str(15+54,95,200,12,"���ӳɹ�",12,0);  //����״̬
		else Show_Str(15+54,95,200,12,"����ʧ��",12,0); 	 
		t=0;
	}
	if((t%20)==0)LED0_Toggle;
	atk_8266_at_response(1);
}
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	return res;		
} 














*/






#endif





