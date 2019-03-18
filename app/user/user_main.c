/*
 * 此工程由BBA(401131019@qq.com编写，用以完成课程实践
 */
#include "user_main.h"

uint32 priv_param_start_sec;

static const partition_item_t at_partition_table[] = {
    { SYSTEM_PARTITION_BOOTLOADER, 						0x0, 												0x1000},	//boot占一个扇区
    { SYSTEM_PARTITION_OTA_1,   						0x1000, 											SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_OTA_2,   						SYSTEM_PARTITION_OTA_2_ADDR, 						SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_RF_CAL,  						SYSTEM_PARTITION_RF_CAL_ADDR, 						0x1000},
    { SYSTEM_PARTITION_PHY_DATA, 						SYSTEM_PARTITION_PHY_DATA_ADDR, 					0x1000},
    { SYSTEM_PARTITION_SYSTEM_PARAMETER, 				SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR, 			0x3000},
    { SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM,             SYSTEM_PARTITION_CUSTOMER_PRIV_PARAM_ADDR,          0x1000},
};

void ICACHE_FLASH_ATTR user_pre_init(void)
{
    if(!system_partition_table_regist(at_partition_table, sizeof(at_partition_table)/sizeof(at_partition_table[0]),SPI_FLASH_SIZE_MAP)) {
		os_printf("system_partition_table_regist fail\r\n");
		while(1);
	}
}


ETSTimer connect_timer;	//定时器事件结构体

struct station_config temp_station_config;	//设置路由器的信息
struct softap_config temp_AP_config;		//设置自身AP的信息

struct espconn station_Transparent_ptrespconn;	//STATION UDP 网络结构体，station 透明传输用
struct espconn AP_ptrespconn;	//AP UDP imformation struct，AP 设置信息用
struct espconn AP_Transparent_ptrespconn;	//AP UDP 透明传输用

remot_info *Mpremot=NULL;	//用于暂存UDP的连接信息
/*
 * UDP接收事件
 */
void ICACHE_FLASH_ATTR
udp_event_recv_BBA_SET(void *arg, char *pusrdata, unsigned short length)
{
	DealWithMessagePacketState(arg,pusrdata,length);

	if (espconn_get_connection_info(&AP_ptrespconn, &Mpremot, 0) != 0)
			os_printf("get_connection_info fail\n");
	else{
		os_memcpy(AP_ptrespconn.proto.udp->remote_ip, Mpremot->remote_ip, 4);
		AP_ptrespconn.proto.udp->remote_port = Mpremot->remote_port;
	}

    return;
}

void ICACHE_FLASH_ATTR
udp_event_recv_BBA_Station_TRANSPARENT(void *arg, char *pusrdata, unsigned short length)
{
	uart0_tx_buffer(pusrdata,length);

	if (espconn_get_connection_info(&station_Transparent_ptrespconn, &Mpremot, 0) != 0)
			os_printf("get_connection_info fail\n");
	else{
		os_memcpy(station_Transparent_ptrespconn.proto.udp->remote_ip, Mpremot->remote_ip, 4);
		station_Transparent_ptrespconn.proto.udp->remote_port = Mpremot->remote_port;
	}
	
	
    return;
}

void ICACHE_FLASH_ATTR
udp_event_recv_BBA_AP_TRANSPARENT(void *arg, char *pusrdata, unsigned short length)
{
	uart0_tx_buffer(pusrdata,length);

	if (espconn_get_connection_info(&AP_Transparent_ptrespconn, &Mpremot, 0) != 0)
			os_printf("get_connection_info fail\n");
	else{
		os_memcpy(AP_Transparent_ptrespconn.proto.udp->remote_ip, Mpremot->remote_ip, 4);
		AP_Transparent_ptrespconn.proto.udp->remote_port = Mpremot->remote_port;
	}

    return;
}


void ICACHE_FLASH_ATTR udp_init(struct espconn* p_ptrespconn,uint8 IP[],int local_port,void (*recv)(void *arg, char *pusrdata, unsigned short length))
{
	//初始化UDP
	p_ptrespconn->type = ESPCONN_UDP;
	p_ptrespconn->proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
	memcpy(p_ptrespconn->proto.udp->local_ip,IP,4);
	p_ptrespconn->proto.udp->local_port = local_port;
	espconn_regist_recvcb(p_ptrespconn, recv);
	espconn_create(p_ptrespconn);
	os_printf("UDP init done\r\n");
}

void user_init(void)
{
	system_set_os_print (0);	//0关闭系统打印
	system_init_done_cb(system_init_done);	//注册系统初始化完成回调函数
}

#define LIGHT_PIN 2
struct BBA_FlashData flashData;

void ICACHE_FLASH_ATTR system_init_done()
{
	os_printf("System init done\r\n");
	
	uart_init(1000000, BIT_RATE_115200);	//串口初始化

	wifi_set_opmode(0x03);		//设置WiFi工作模式为AP+Station并保存到Flash

	AP_UDP_Init();	//用于在任何时刻都可对ESP8266进行设置

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U,FUNC_GPIO2);	//选择引脚功能
	GPIO_OUTPUT_SET(LIGHT_PIN, 0);	//灯亮，表示未连接上AP
	ReadMyFlashData(&flashData);

	struct station_config stationConf;

	if(flashData.flag_init==1)
	{
		os_memcpy(&stationConf.ssid, flashData.wifi_ssid, flashData.ssidLen); //0s_memcpy 内存拷贝函数 32表示拷贝前32个字符
		os_memcpy(&stationConf.password, flashData.wifi_password, flashData.passwordLen);
		wifi_station_set_config_current(&stationConf);
		if(wifi_station_connect())	//如果wifi连接成功
		{
			
			os_printf("wifi connecting!");
			os_timer_setfn(&connect_timer,Wifi_conned,NULL);	//设置软件定时器和回调函数，NULL回调函数不带参数
			os_timer_arm(&connect_timer,1000,1); //每隔2s 去检测是否真的连接上，NULL代表不重复
	
		}
		else
		{
			os_printf("Connect Fail!\r\n");
		}
	}
	else
	{
		os_printf("wifi havn't set\r\n");
	}
}

void ICACHE_FLASH_ATTR AP_UDP_Init()
{
	wifi_softap_get_config(&temp_AP_config);	//查询AP接口保存在Flash中的配置
	if(temp_AP_config.ssid[0]!='B')
	{
		os_memset(temp_AP_config.ssid, 0, 32);
		os_memset(temp_AP_config.password, 0, 64);
		os_memcpy(temp_AP_config.ssid, "BBA's_AP", 8);
		os_memcpy(temp_AP_config.password, "123123123", 9);
		temp_AP_config.authmode = AUTH_WPA_WPA2_PSK;
		temp_AP_config.ssid_len = 8;// or its actual length
		temp_AP_config.max_connection = 4; // how many stations can connect to ESP8266 softAP at most.
		wifi_softap_set_config(&temp_AP_config);
		os_printf("AP Set Sucess!");
	}
	//初始化AP_UDP
	uint8 ip[4]={192,168,4,1};
	udp_init(&AP_ptrespconn,ip,1026,&udp_event_recv_BBA_SET);		//设置端口
	udp_init(&AP_Transparent_ptrespconn,ip,1025,&udp_event_recv_BBA_AP_TRANSPARENT);	//透传端口
}


void ICACHE_FLASH_ATTR Wifi_conned(void *arg)
{
	uint8 status;
	struct ip_info stationIP;
	static bool bool_connected=1;
	uint8 ip[4];

	status = wifi_station_get_connect_status();
	if(status == STATION_GOT_IP)
	{
		//os_printf("WIFI was Connected!\r\n");
		GPIO_OUTPUT_SET(LIGHT_PIN, 1);		//如果连接上，则灯灭
		
		if(bool_connected)
		{
			bool_connected=0;
			wifi_get_ip_info(STATION_IF,&stationIP);
			memcpy(ip,&stationIP.ip,4);
			udp_init(&station_Transparent_ptrespconn,ip,1025,&udp_event_recv_BBA_Station_TRANSPARENT);	//初始化UDP
		}
	}
	else
	{
		GPIO_OUTPUT_SET(LIGHT_PIN, 0);
		//os_printf("WIFI Connect Fail\r\n");	//如果连接不上，则灯亮
	}
}