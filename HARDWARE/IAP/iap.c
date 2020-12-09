#include "iap.h"
/*************************************************************************/
/*****************************BANK1/BANK2*********************************/
/*************************************************************************/
//读取指定地址的字(32位数据) 
//faddr:读地址 
//返回值:对应数据.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(__IO uint32_t *)faddr; 
}

//获取某个地址所在的flash扇区,仅用于BANK1！！
//addr:flash地址
//返回值:0~11,即addr所在的扇区
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

//从指定地址开始写入指定长度的数据
//特别注意:因为STM32H7的扇区实在太大,没办法本地保存扇区数据,所以本函数
//         写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
//         写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
//         没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写. 
//该函数对OTP区域也有效!可以用来写OTP区!
//OTP区域地址范围:0X1FF0F000~0X1FF0F41F
//WriteAddr:起始地址(此地址必须为4的倍数!!)
//pBuffer:数据指针
//NumToWrite:字(32位)数(就是要写入的32位数据的个数.) 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
  FLASH_EraseInitTypeDef FlashEraseInit;
  HAL_StatusTypeDef FlashStatus=HAL_OK;
  u32 SectorError=0;
	u32 addrx=0;
	u32 endaddr=0;	
  if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//非法地址
    
 	HAL_FLASH_Unlock();             //解锁	
	addrx=WriteAddr;				//写入的起始地址
	endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址
    
  if(addrx<0X1FF00000)
  {
    while(addrx<endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
			{   
				FlashEraseInit.Banks=(STMFLASH_GetFlashSector(addrx)/8?FLASH_BANK_2:FLASH_BANK_1);						//操作BANK1
        FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //擦除类型，扇区擦除 
        FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx)%8;   //要擦除的扇区
        FlashEraseInit.NbSectors=1;                             //一次只擦除一个扇区
        FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //电压范围，VCC=2.7~3.6V之间!!
        if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
        {
          break;//发生错误了	
        }
        SCB_CleanInvalidateDCache();                            //清除无效的D-Cache
			}else addrx+=4;
      FLASH_WaitForLastOperation(FLASH_WAITETIME,(STMFLASH_GetFlashSector(addrx)/8?FLASH_BANK_2:FLASH_BANK_1));    //等待上次操作完成
    }
  }
  FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME,(STMFLASH_GetFlashSector(addrx)/8?FLASH_BANK_2:FLASH_BANK_1));       //等待上次操作完成
	if(FlashStatus==HAL_OK)
	{
		while(WriteAddr<endaddr)//写数据
		{
      if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD,WriteAddr,(uint64_t)pBuffer)!=HAL_OK)//写入数据
			{ 
				break;	//写入异常
			}
			WriteAddr+=32;
			pBuffer+=8;
		} 
	}
	HAL_FLASH_Lock();           //上锁
} 

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(32位)数
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//读取4个字节.
		ReadAddr+=4;//偏移4个字节.	
	}
}

iapfun jump2app; 
u32 iapbuf[512]; 	//2K字节缓存
/*先把appbuf(内含程序)中的数据压缩到iapbuf,然后把iapbuf中的数据写入flash*/
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
//appxaddr:输入地址跳转到指定app.
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
		IAP_state->ready_state=run_state0;//等待发送来的程序
	}
	else if(IAP_state->ready_state==run_state0)
	{
		if(USART_RX_CNT)
		{
			if(_oldcount_==USART_RX_CNT)//新周期内,没有收到任何数据,认为本次数据接收完成.
			{
				USART_RX_CNT=0;
				printf("用户程序接收完成!\r\n");
				printf("代码长度:%dBytes\r\n",_oldcount_);
				IAP_state->ready_state=run_state1;//进入判断该程序可否加载执行的状态
			}else _oldcount_=USART_RX_CNT;			
		}
	}
	else if(IAP_state->ready_state==run_state1)
	{
		if(((*(vu32*)(0x24001000+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
		{
			printf("可以更新固件...\r\n");
			__app_load_en__=0;//进去之前先清理一次假信号
			IAP_state->ready_state=run_state2;
		}
		else 
		{
			printf("非FLASH应用程序!\r\n");
			_oldcount_=0;
			IAP_state->ready_state=run_state0;
		}	
	}
	else if(IAP_state->ready_state==run_state2)//成功接收程序并准备进入
	{	
		if(__app_load_en__)
		{
			__app_load_en__=0;
			__exist_table__[__app_select__]=1;//填充
			
			for(_index_=0;_index_<8;_index_++)
			{
				if(__exist_table__[_index_]==1)
					existdata_buf_out[_index_]=1;
				else
					existdata_buf_out[_index_]=0;
			}
			SD_WriteDisk(existdata_buf_out,0,1);
			
			
			iap_write_appbin(__address_table__[__app_select__],USART_RX_BUF,_oldcount_);//更新FLASH代码   
			printf("固件更新完成!\r\n");
			IAP_state->ready_state=run_state0;
		}
	}
}

void iap_enter_event(local_state_space *IAP_state)
{
	if(IAP_state->ready_state==prepare_state)
	{
		IAP_state->ready_state=run_state0;//等待发送来的程序
	}
	else if(IAP_state->ready_state==run_state0)
	{
		if(__app_enter__)
		{
			__app_enter__=0;
			if(__exist_table__[__app_select__])
			{
				printf("进入APP");			
				HAL_TIM_Base_DeInit(&TIM3_Handler);
				HAL_TIM_Base_DeInit(&TIM4_Handler);
				HAL_UART_DeInit(&UART1_Handler);
				
				iap_load_app(__address_table__[__app_select__]);
			}
			else printf("此Sector不存在程序");
		}
	}
}

void iap_erase_event(local_state_space *IAP_state)
{
	if(IAP_state->ready_state==prepare_state)
	{
		IAP_state->ready_state=run_state0;//等待发送来的程序
	}
	else if(IAP_state->ready_state==run_state0)
	{
		if(__app_erase__)
		{
			__app_erase__=0;
			if(__exist_table__[__app_select__])
			{
				if(__app_select__==0)printf("不可擦除Bootloader");
				else 
				{
					__exist_table__[__app_select__]=0;
					printf("擦除完毕");
				}
			}
			else printf("不可擦除空白区域");
		}
	}
}

#endif











