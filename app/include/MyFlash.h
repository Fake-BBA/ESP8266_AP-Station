#ifndef MY_FLASH_H__
#define MY_FLASH_H__

#include "driver/uart.h"
#include "osapi.h"
#include "gpio.h"
#include "spi_flash.h"
#include "eagle_soc.h"
#include "mem.h"	//�ڴ�������
#include "user_interface.h"	//


#define BBA_SECTORS	250			//�Ե�250������Ϊ���ݱ����������ӵ�0��������ʼ����
#define BBA_PARTITION_ADDR BBA_SECTORS*4096	//��ʼ��ַ
#define BBA_BBA_PARTITION_SIZE_BYTES	sizeof(struct BBA_FlashData) //����������4�����ڴ��ȡ����ᵼ�²�������

//����4�ֽڶ���
struct BBA_FlashData{
    
    uint8 wifi_ssid[32];
    uint32 ssidLen;
    uint8 wifi_password[64];
    uint32 passwordLen;

    uint8 functionState;  //��һ�α���Ĺ���
    uint8 flag_init;        //�˰��Ƿ��ѱ���ʼ��
    uint16 deviceNumber;    //�豸��
    
    uint8 data[4];          //�����ֶ�Ӧ�����ݣ����ǵ����Ӧ����
};

uint32 ICACHE_FLASH_ATTR ReadMyFlashData(struct BBA_FlashData *p);
uint32 ICACHE_FLASH_ATTR WriteMyFlashData(struct BBA_FlashData *p,uint32 len);

#endif