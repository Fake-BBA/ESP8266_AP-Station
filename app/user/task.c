/*
 * task.c
 *
 *  Created on: 2019��1��31��
 *      Author: BBA
 */

#include "task.h"
extern struct BBA_FlashData flashData;

bool DealWithMessagePacketState(struct espconn *espconn,uint8 *p_buffer,uint16 length)
{
	enum MessageCtrFunction messageCtrFunction;
	static union MessagePacketUnion messagePacketUnion;
	os_memcpy(messagePacketUnion.p_buff, p_buffer, MESSAGE_PACKET_SIZE);

	messageCtrFunction=messagePacketUnion.messagePacket.function_word;
	flashData.functionState=messageCtrFunction;

	os_printf("�����ߣ�%d\r\n",messagePacketUnion.messagePacket.sender);
	os_printf("�����ߣ�%d\r\n",messagePacketUnion.messagePacket.receiver);

	extern struct espconn AP_ptrespconn;

	struct ip_info stationIP;

	switch(messageCtrFunction)
	{
	case WIFI_CONFIG:
		flashData.deviceNumber=messagePacketUnion.messagePacket.receiver;
		flashData.ssidLen=messagePacketUnion.messagePacket.data[0];
		memcpy(flashData.wifi_ssid,messagePacketUnion.messagePacket.data+1,flashData.ssidLen);
		
		flashData.passwordLen=messagePacketUnion.messagePacket.data[flashData.ssidLen+1];
		memcpy(flashData.wifi_password,messagePacketUnion.messagePacket.data+flashData.ssidLen+2,flashData.passwordLen);

		//���������ַ���������
		flashData.wifi_ssid[flashData.ssidLen]='\0';
		flashData.ssidLen++;
		flashData.wifi_password[flashData.passwordLen]='\0';
		flashData.passwordLen++;

		flashData.flag_init=1;
		os_printf("device number:%d\r\n",flashData.deviceNumber);
		os_printf("ssidLen:%d\r\n",flashData.ssidLen);
        os_printf("wifi_ssid:%s\r\n",flashData.wifi_ssid);
		os_printf("passWordLen:%d\r\n",flashData.passwordLen);
        os_printf("wifi_password:%s\r\n",flashData.wifi_password);
        os_printf("function World:%d\r\n",flashData.functionState);
        os_printf("function data:%d\r\n",flashData.data);
        os_printf("flag:%d\r\n",flashData.flag_init);  

		messagePacketUnion.messagePacket.receiver=messagePacketUnion.messagePacket.sender;	//���ý�����
		messagePacketUnion.messagePacket.sender=flashData.deviceNumber;	//������Ϊ���豸��
		messagePacketUnion.messagePacket.function_word=WIFI_CONFIG;	//	//WiFi�������

		espconn_send(&AP_ptrespconn,messagePacketUnion.p_buff,5);	//��ʾwifi�������
		WriteMyFlashData(&flashData,sizeof(flashData));
		system_restart();
		break;
	case GET_WIFI_IP:
		messagePacketUnion.messagePacket.function_word=GET_WIFI_IP;
		//#define STATION_IF 0x00
		//#define SOFTAP_IF 0x01
		messagePacketUnion.messagePacket.receiver=messagePacketUnion.messagePacket.sender;	//���ý�����
		messagePacketUnion.messagePacket.sender=flashData.deviceNumber;	//������Ϊ���豸��

		if(wifi_get_ip_info(STATION_IF,&stationIP))
		{
			os_printf("WIFI was Connected!\r\n");
			memcpy(&messagePacketUnion.messagePacket.data[0],&(stationIP.ip.addr),4);
			espconn_send(&AP_ptrespconn,messagePacketUnion.p_buff,9);	//����IP��
		}
		else
		{
			os_printf("WIFI Connect Fail\r\n");	//������Ӳ��ϣ������
			memset(messagePacketUnion.messagePacket.data,0x00,4);
			espconn_send(&AP_ptrespconn,messagePacketUnion.p_buff,9);	//����IP��
		}

		break;
	default:
		messagePacketUnion.messagePacket.receiver=messagePacketUnion.messagePacket.sender;	//���ý�����
		messagePacketUnion.messagePacket.sender=flashData.deviceNumber;	//������Ϊ���豸��
		espconn_send(&AP_ptrespconn,messagePacketUnion.p_buff,5);	//����IP��
		break;
	}
}
