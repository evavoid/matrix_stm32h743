#include "iap.h"
/*************************************************************************/
/*****************************BANK1/BANK2*********************************/
/*************************************************************************/
//��ȡָ����ַ����(32λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(__IO uint32_t *)faddr; 
}

//��ȡĳ����ַ���ڵ�flash����,������BANK1����
//addr:flash��ַ
//����ֵ:0~11,��addr���ڵ�����
uint16_t STMFLASH_GetFlashSector(u32 addr)
{
	if     (addr<ADDR_FLASH_SECTOR_1_BANK1)return FLASH_BANK0_SEC0;
	else if(addr<ADDR_FLASH_SECTOR_2_BANK1)return FLASH_BANK0_SEC1;
	else if(addr<ADDR_FLASH_SECTOR_3_BANK1)return FLASH_BANK0_SEC2;
	else if(addr<ADDR_FLASH_SECTOR_4_BANK1)return FLASH_BANK0_SEC3;
	else if(addr<ADDR_FLASH_SECTOR_5_BANK1)return FLASH_BANK0_SEC4;
	else if(addr<ADDR_FLASH_SECTOR_6_BANK1)return FLASH_BANK0_SEC5;
	else if(addr<ADDR_FLASH_SECTOR_7_BANK1)return FLASH_BANK0_SEC6;
	
	else if(addr<ADDR_FLASH_SECTOR_0_BANK2)return FLASH_BANK1_SEC8;
	else if(addr<ADDR_FLASH_SECTOR_1_BANK2)return FLASH_BANK1_SEC9;
	else if(addr<ADDR_FLASH_SECTOR_2_BANK2)return FLASH_BANK1_SEC10;
	else if(addr<ADDR_FLASH_SECTOR_3_BANK2)return FLASH_BANK1_SEC11;
	else if(addr<ADDR_FLASH_SECTOR_4_BANK2)return FLASH_BANK1_SEC12;
	else if(addr<ADDR_FLASH_SECTOR_5_BANK2)return FLASH_BANK1_SEC13;
	else if(addr<ADDR_FLASH_SECTOR_6_BANK2)return FLASH_BANK1_SEC14;
	else if(addr<ADDR_FLASH_SECTOR_7_BANK2)return FLASH_BANK1_SEC15;
	return FLASH_BANK1_SEC15;
}

//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ر�ע��:��ΪSTM32H7������ʵ��̫��,û�취���ر�����������,���Ա�����
//         д��ַ�����0XFF,��ô���Ȳ������������Ҳ�������������.����
//         д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.����д֮ǰȷ��������
//         û����Ҫ����,��������������Ȳ�����,Ȼ����������д. 
//�ú�����OTP����Ҳ��Ч!��������дOTP��!
//OTP�����ַ��Χ:0X1FF0F000~0X1FF0F41F
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ4�ı���!!)
//pBuffer:����ָ��
//NumToWrite:��(32λ)��(����Ҫд���32λ���ݵĸ���.) 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
  FLASH_EraseInitTypeDef FlashEraseInit;
  HAL_StatusTypeDef FlashStatus=HAL_OK;
  u32 SectorError=0;
	u32 addrx=0;
	u32 endaddr=0;	
  if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//�Ƿ���ַ
    
 	HAL_FLASH_Unlock();             //����	
	addrx=WriteAddr;				//д�����ʼ��ַ
	endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
    
  if(addrx<0X1FF00000)
  {
    while(addrx<endaddr)		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
		{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
			{   
				FlashEraseInit.Banks=(STMFLASH_GetFlashSector(addrx)/8?FLASH_BANK_2:FLASH_BANK_1);						//����BANK1
        FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //�������ͣ��������� 
        FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx)%8;   //Ҫ����������
        FlashEraseInit.NbSectors=1;                             //һ��ֻ����һ������
        FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //��ѹ��Χ��VCC=2.7~3.6V֮��!!
        if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
        {
          break;//����������	
        }
        SCB_CleanInvalidateDCache();                            //�����Ч��D-Cache
			}else addrx+=4;
      FLASH_WaitForLastOperation(FLASH_WAITETIME,(STMFLASH_GetFlashSector(addrx)/8?FLASH_BANK_2:FLASH_BANK_1));    //�ȴ��ϴβ������
    }
  }
  FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME,(STMFLASH_GetFlashSector(addrx)/8?FLASH_BANK_2:FLASH_BANK_1));       //�ȴ��ϴβ������
	if(FlashStatus==HAL_OK)
	{
		while(WriteAddr<endaddr)//д����
		{
      if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD,WriteAddr,(uint64_t)pBuffer)!=HAL_OK)//д������
			{ 
				break;	//д���쳣
			}
			WriteAddr+=32;
			pBuffer+=8;
		} 
	}
	HAL_FLASH_Lock();           //����
} 

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToRead:��(32λ)��
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
}

iapfun jump2app; 
u32 iapbuf[512]; 	//2K�ֽڻ���
/*�Ȱ�appbuf(�ں�����)�е�����ѹ����iapbuf,Ȼ���iapbuf�е�����д��flash*/
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)
{
	u32 t;
	u16 i=0;
	u32 temp;
	u32 fwaddr=appxaddr;//???????
	u8 *dfu=appbuf;
	for(t=0;t<appsize;t+=4)
	{						   
		temp=(u32)dfu[3]<<24;   
		temp|=(u32)dfu[2]<<16;    
		temp|=(u32)dfu[1]<<8;
		temp|=(u32)dfu[0];	  
		dfu+=4;//??4???
		iapbuf[i++]=temp;	    
		if(i==512)
		{
			i=0; 
			STMFLASH_Write(fwaddr,iapbuf,512);
			fwaddr+=2048;//??2048  512*4=2048
		}
	} 
	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//?????????????.  
}

//????????
//appxaddr:�����ַ��ת��ָ��app.
void iap_load_app(u32 appxaddr)
{ 
	if(((*(vu32*)appxaddr)&0x2FF00000)==0x24000000)	//??????????.
	{ 
		Disable_global_irq();
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//????????????????(????)		
		MSR_MSP(*(vu32*)appxaddr);					//???APP????(??????????????????)
		jump2app();									//???APP.
	}
}		 

void Disable_global_irq(void)
{
	INTX_DISABLE();
}

#if IAP_MASTER
u8 existdata_buf_out[512];
void iap_load_event(local_state_space *IAP_state)
{
	if(IAP_state->ready_state==prepare_state)
	{
		_oldcount_=0;//oldcount
		IAP_state->ready_state=run_state0;//�ȴ��������ĳ���
	}
	else if(IAP_state->ready_state==run_state0)
	{
		if(USART_RX_CNT)
		{
			if(_oldcount_==USART_RX_CNT)//��������,û���յ��κ�����,��Ϊ�������ݽ������.
			{
				USART_RX_CNT=0;
				printf("�û�����������!\r\n");
				printf("���볤��:%dBytes\r\n",_oldcount_);
				IAP_state->ready_state=run_state1;//�����жϸó���ɷ����ִ�е�״̬
			}else _oldcount_=USART_RX_CNT;			
		}
	}
	else if(IAP_state->ready_state==run_state1)
	{
		if(((*(vu32*)(0x24001000+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
		{
			printf("���Ը��¹̼�...\r\n");
			__app_load_en__=0;//��ȥ֮ǰ������һ�μ��ź�
			IAP_state->ready_state=run_state2;
		}
		else 
		{
			printf("��FLASHӦ�ó���!\r\n");
			_oldcount_=0;
			IAP_state->ready_state=run_state0;
		}	
	}
	else if(IAP_state->ready_state==run_state2)//�ɹ����ճ���׼������
	{	
		if(__app_load_en__)
		{
			__app_load_en__=0;
			__exist_table__[__app_select__]=1;//���
			
			for(_index_=0;_index_<8;_index_++)
			{
				if(__exist_table__[_index_]==1)
					existdata_buf_out[_index_]=1;
				else
					existdata_buf_out[_index_]=0;
			}
			SD_WriteDisk(existdata_buf_out,0,1);
			
			
			iap_write_appbin(__address_table__[__app_select__],USART_RX_BUF,_oldcount_);//����FLASH����   
			printf("�̼��������!\r\n");
			IAP_state->ready_state=run_state0;
		}
	}
}

void iap_enter_event(local_state_space *IAP_state)
{
	if(IAP_state->ready_state==prepare_state)
	{
		IAP_state->ready_state=run_state0;//�ȴ��������ĳ���
	}
	else if(IAP_state->ready_state==run_state0)
	{
		if(__app_enter__)
		{
			__app_enter__=0;
			if(__exist_table__[__app_select__])
			{
				printf("����APP");			
				HAL_TIM_Base_DeInit(&TIM3_Handler);
				HAL_TIM_Base_DeInit(&TIM4_Handler);
				HAL_UART_DeInit(&UART1_Handler);
				
				iap_load_app(__address_table__[__app_select__]);
			}
			else printf("��Sector�����ڳ���");
		}
	}
}

void iap_erase_event(local_state_space *IAP_state)
{
	if(IAP_state->ready_state==prepare_state)
	{
		IAP_state->ready_state=run_state0;//�ȴ��������ĳ���
	}
	else if(IAP_state->ready_state==run_state0)
	{
		if(__app_erase__)
		{
			__app_erase__=0;
			if(__exist_table__[__app_select__])
			{
				if(__app_select__==0)printf("���ɲ���Bootloader");
				else 
				{
					__exist_table__[__app_select__]=0;
					printf("�������");
				}
			}
			else printf("���ɲ����հ�����");
		}
	}
}

#endif











