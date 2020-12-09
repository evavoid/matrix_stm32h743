#ifndef __IAP_H
#define __IAP_H
#include "matrixos.h"
//////////////////////////////////////////// 	
//�����������ʹ�øÿ�
#define IAP_MASTER 0

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
#define FLASH_WAITETIME  50000          //FLASH�ȴ���ʱʱ��

/** @defgroup FLASH_Sectors FLASH Sectors
  * @{
  */
#define FLASH_BANK0_SEC0           ((uint32_t)0U) /*!< Sector Number 0   */
#define FLASH_BANK0_SEC1           ((uint32_t)1U) /*!< Sector Number 1   */
#define FLASH_BANK0_SEC2           ((uint32_t)2U) /*!< Sector Number 2   */
#define FLASH_BANK0_SEC3           ((uint32_t)3U) /*!< Sector Number 3   */
#define FLASH_BANK0_SEC4           ((uint32_t)4U) /*!< Sector Number 4   */
#define FLASH_BANK0_SEC5           ((uint32_t)5U) /*!< Sector Number 5   */
#define FLASH_BANK0_SEC6           ((uint32_t)6U) /*!< Sector Number 6   */
#define FLASH_BANK0_SEC7           ((uint32_t)7U) /*!< Sector Number 7   */
#define FLASH_BANK1_SEC8           ((uint32_t)8U) /*!< Sector Number 0   */
#define FLASH_BANK1_SEC9           ((uint32_t)9U) /*!< Sector Number 1   */
#define FLASH_BANK1_SEC10          ((uint32_t)10U) /*!< Sector Number 2   */
#define FLASH_BANK1_SEC11          ((uint32_t)11U) /*!< Sector Number 3   */
#define FLASH_BANK1_SEC12          ((uint32_t)12U) /*!< Sector Number 4   */
#define FLASH_BANK1_SEC13          ((uint32_t)13U) /*!< Sector Number 5   */
#define FLASH_BANK1_SEC14          ((uint32_t)14U) /*!< Sector Number 6   */
#define FLASH_BANK1_SEC15          ((uint32_t)15U) /*!< Sector Number 7   */

//STM32H7 FLASH ��������ʼ��ַ
//BANK1
#define ADDR_FLASH_SECTOR_0_BANK1     ((uint32_t)0x08000000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_1_BANK1     ((uint32_t)0x08020000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_2_BANK1     ((uint32_t)0x08040000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_3_BANK1     ((uint32_t)0x08060000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_4_BANK1     ((uint32_t)0x08080000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_5_BANK1     ((uint32_t)0x080A0000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6_BANK1     ((uint32_t)0x080C0000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7_BANK1     ((uint32_t)0x080E0000) //����0��ʼ��ַ, 128 Kbytes  

//BANK2
#define ADDR_FLASH_SECTOR_0_BANK2     ((uint32_t)0x08100000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_1_BANK2     ((uint32_t)0x08120000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_2_BANK2     ((uint32_t)0x08140000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_3_BANK2     ((uint32_t)0x08160000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_4_BANK2     ((uint32_t)0x08180000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_5_BANK2     ((uint32_t)0x081A0000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6_BANK2     ((uint32_t)0x081C0000) //����0��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7_BANK2     ((uint32_t)0x081E0000) //����0��ʼ��ַ, 128 Kbytes  

u32 STMFLASH_ReadWord(u32 faddr);		  	//������  
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
/*******************************IAP****************************************/
typedef  void (*iapfun)(void);				//����һ���������͵Ĳ��� 
void iap_load_app(u32 appxaddr);			//��ת��app��ִ��
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 applen);//��ָ����ַд��app
void Disable_global_irq(void);
/*****************************/

#if IAP_MASTER
#define _oldcount_ IAP_state->data[0]
#define _index_ IAP_state->data[1]
void iap_load_event(local_state_space *IAP_state);
////////////////////////////�����䲻�ɻ���
void iap_erase_event(local_state_space *IAP_state);
void iap_enter_event(local_state_space *IAP_state);
#endif

/*********************************************************************************************
SLAVE
��� Disable_global_irq();
���ʵ��ĵط��� void iap_load_app(u32 appxaddr);
��main������ͷ���� SCB->VTOR = FLASH_BASE|0x20000;
����ѡ��Target�еĳ�����ʼ��
����ѡ��User�ڵ�C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe  --bin -o  ..\OBJ\output.bin ..\OBJ\Template.axf
**********************************************************************************************/

#endif
