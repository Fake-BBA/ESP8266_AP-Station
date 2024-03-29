/*
 * task.h
 *
 *  Created on: 2019年1月31日
 *      Author: BBA
 */

#ifndef APP_INCLUDE_TASK_H_
#define APP_INCLUDE_TASK_H_

#include "driver/uart.h"
#include "osapi.h"
#include "gpio.h"
#include "eagle_soc.h"
#include "mem.h"	//内存操作相关
#include "user_interface.h"
#include "espconn.h"		//网络接口相关
#include "pwm.h"
#include "MyFlash.h"

#define MAX_REPLY_PACKET_SIZE	21
#define PWM_HZ	50
#define PWM_PERIOD	1/PWM_HZ	//单位us
#define CHANNEL	1
#define CHANNEL_NUM	1

#define MESSAGE_PACKET_SIZE	100

enum MessageCtrFunction{
	HEARTBEAT=0,
	FIND_DEVICE,
	LIGHT,
	FAN,
	HUMITURE=4,
	WIFI_TRANSPARENT=5,		//透传模块
	GET_WIFI_IP=253,	//获取连接上的IP
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
	uint8 p_buff[MESSAGE_PACKET_SIZE];	//协议规定最长包数据为100
};

extern uint16 deviceNumber;
bool DealWithMessagePacketState(struct espconn *espconn,uint8 *p_buffer,uint16 length);



#endif /* APP_INCLUDE_TASK_H_ */
