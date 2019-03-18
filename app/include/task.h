/*
 * task.h
 *
 *  Created on: 2019��1��31��
 *      Author: BBA
 */

#ifndef APP_INCLUDE_TASK_H_
#define APP_INCLUDE_TASK_H_

#include "driver/uart.h"
#include "osapi.h"
#include "gpio.h"
#include "eagle_soc.h"
#include "mem.h"	//�ڴ�������
#include "user_interface.h"
#include "espconn.h"		//����ӿ����
#include "pwm.h"
#include "MyFlash.h"

#define MAX_REPLY_PACKET_SIZE	21
#define PWM_HZ	50
#define PWM_PERIOD	1/PWM_HZ	//��λus
#define CHANNEL	1
#define CHANNEL_NUM	1

#define MESSAGE_PACKET_SIZE	100

enum MessageCtrFunction{
	HEARTBEAT=0,
	FIND_DEVICE,
	LIGHT,
	FAN,
	HUMITURE=4,
	WIFI_TRANSPARENT=5,		//͸��ģ��
	GET_WIFI_IP=253,	//��ȡ�����ϵ�IP
	WIFI_CONFIG=254,
	DEVICE_ERROS=255
};

struct MessagePacketStruct{
	uint16 sender;
	uint16 receiver;
	uint8 function_word;
	uint8 data[95];
};
union MessagePacketUnion{
	struct MessagePacketStruct messagePacket;
	uint8 p_buff[MESSAGE_PACKET_SIZE];	//Э��涨�������Ϊ100
};

extern uint16 deviceNumber;
bool DealWithMessagePacketState(struct espconn *espconn,uint8 *p_buffer,uint16 length);



#endif /* APP_INCLUDE_TASK_H_ */
