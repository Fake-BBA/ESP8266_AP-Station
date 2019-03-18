当Station连接上AP时，蓝灯会灭，否则常亮
这个程序是属于第二种

第一种：两个ESP8266,一个作AP放出wifi，另一个作station连接wifi，AP UDP的目标地址恒为192.168.4.1 port：1025，然后两个WiFi都以波特率为1000000的串口提供透传，第一次station必须先发一个数据包给AP，AP才可确定通讯IP，AP最大可以连入4个wifi设备
	
第二种：一个ESP8266,既使用AP，也使用station，AP默认放出热点为BBA's_AP,密码123123123，用作设置目标路由器WiFi和密码，新增连接AP也可以透传，一旦设置后，便自动保存到Flash，下一次自动连接，连接上后可以通过向AP发送指令获取station获取到的IP地址。ESP8266的设置IP为192.168.4.1 port：1026 透传IP：AP:192.68 port：1026或者Station：通过指令获取(192.168.31.80) port:1025，Station、AP透传端口为1025，AP设置端口为1026