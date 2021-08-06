
#include "stm32f10x.h"
#include "./systick/bsp_SysTick.h"
#include "./usart/bsp_usart.h"
#include "./dht11/bsp_dht11.h"
#include "bsp_esp8266.h"
#include "bsp_led.h"
#include "SGP30.h"
#include "MQTTPacket.h"
#include "MQTTConnect.h"
#include "string.h"

typedef struct
{
	float   humi;		  //Temperature
	float   temp;	 	  //Humidity
	float sgp30; 			//HCHO
	float co2; 			//CO2
	int PM2_5VAL;
	int PM1VAL;
	int PM10VAL;
} Dev_Data_TypeDef;

Dev_Data_TypeDef dev_data;

void Toggle_Led(void)
{
	static uint8_t Cnt = 0;
	
	Cnt ++;
	
	if(Cnt >= 10)
	{
		macLED1_TOGGLE ();
		Cnt = 0;
	}
}

typedef struct  
{										    
  char ClientID[100];
  char MQTTUserName[300];
  char MQTTPassWord[300];
}option_msg; 

option_msg optmsg;

unsigned short MQTT_ConnectPacket(unsigned char* Conbuf,char *client_id,char *username,char *password)
{
	unsigned char Oribuff[128] = {0};
	unsigned short Orilen = 0;
	unsigned char *MainString;
	unsigned short enclen = 0;
	char opt_buff[70]={0};
	
	
	sprintf((char*)optmsg.ClientID,"%s",client_id);
	sprintf((char*)optmsg.MQTTUserName,"%s",username);
	sprintf((char*)optmsg.MQTTPassWord,"%s",password);
	
	MQTTSerialize_opt(&options_data);
		
	options_data.clientID.cstring = optmsg.ClientID;
	options_data.keepAliveInterval = 120;
	options_data.cleansession = 1;
		
	options_data.username.cstring = optmsg.MQTTUserName;
	options_data.password.cstring = optmsg.MQTTPassWord;
	
	Orilen = MQTTSerialize_connect(Conbuf, 600, &options_data);
	
	enclen = Orilen;
	
	return enclen;
}

#define CLINET_ID   "714133969" //clientID
#define PROD_ID     "423564"             //ProductID
#define SN          "DHT11"      //Lablename


unsigned char GetDataFixedHead(unsigned char MesType,unsigned char DupFlag,unsigned char QosLevel,unsigned char Retain)
{
	unsigned char dat = 0;
	dat = (MesType & 0x0f) << 4;
	dat |= (DupFlag & 0x01) << 3;
	dat |= (QosLevel & 0x03) << 1;
	dat |= (Retain & 0x01);
	return dat;
}

#define		MQTT_TypePUBLISH					3//Publishing

unsigned int GetDataONENETPUBLISH(unsigned char *buff,unsigned char onenetty,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg)//Get data
{
	unsigned int i,len=0,lennum=0;
	buff[0] = GetDataFixedHead(MQTT_TypePUBLISH,dup,qos,retain);
	len = strlen(topic);
	buff[2] = len>>8;
	buff[3] = len;
	for(i = 0;i<len;i++)
	{
		buff[4+i] = topic[i];
	}

  lennum = len;
	len = strlen(msg);

  buff[4+lennum] = onenetty;
  buff[4+lennum+1] = len>>8;
  buff[4+lennum+2] = len;

	for(i = 0;i<len;i++)
	{
		buff[4+i+lennum+3] = msg[i];
	}
	lennum += len;
	buff[1] = lennum + 2+3;

  return buff[1]+2;
}

unsigned short MQTT_PublishDataPacket(unsigned char* Conbuf,unsigned char* topic)
{
	//unsigned char Oribuff[200] = {0};
	unsigned char MQTTWillMessage[400] = {0};
	unsigned short Msglen = 0;
	char opt_buff[70]={0};
	unsigned short Orilen = 0;
	unsigned short enclen = 0;
	static unsigned int tag = 0;
	
	
	MQTTString topicString = MQTTString_initializer;
	
	sprintf((char*)opt_buff,"%s",topic);
	
	Msglen = sprintf((char *)MQTTWillMessage,"{\"temperature\": %.2f,\"humidity\": %.1f,\"co2\": %.1f,\"pm10\": %d,\"pm25\": %d,\"hcho\": %.1f,\"tag\": %d}",\
																								dev_data.temp,dev_data.humi,dev_data.co2,dev_data.PM10VAL,dev_data.PM2_5VAL,dev_data.sgp30,tag);
  
	tag = tag + 1;
	topicString.cstring = opt_buff;
	
	enclen = MQTTSerialize_publish(Conbuf, 400, 0, 1, 0, 0, topicString, (unsigned char*)MQTTWillMessage, Msglen);
	
	return enclen;
}

void Mqtt_Send(unsigned char *mqtt_msg,unsigned short len)
{
	unsigned short i;
	
	for(i = 0;i < len; i++)
	{
		while(USART_GetFlagStatus(macESP8266_USARTx,USART_FLAG_TC)==RESET){}      //waiting for completion
			USART_SendData( macESP8266_USARTx ,mqtt_msg[i]); //Forwarding to ESP82636
	}
	
}

//Building MQTT disconnection package
u8 mqtt_disconnect_message(u8 *mqtt_message)
{ 
	mqtt_message[0] = 0XE0;	//0xE0 //Message type and tag DISCONNECT
	mqtt_message[1] = 0;	//Length of the remaining(Exclude head)
	
	return 2;
}

extern unsigned char mqtt_sta;

static void MQTT_CallBack(void)
{
	unsigned short mqtt_data_len;
	unsigned char mqtt_msg[600];
	static unsigned char Cnt = 0;
	static uint8_t tmCnt = 0;
	
	tmCnt ++;
	
	if(ucTcpClosedFlag)
		mqtt_sta = 0;
	
	if(tmCnt >= 40)
	{
		tmCnt = 0;
		if(ucTcpClosedFlag)
			mqtt_sta = 0;
		else
		{
			if(mqtt_sta == 0)
			{
				mqtt_data_len = MQTT_ConnectPacket(mqtt_msg,CLINET_ID,PROD_ID,SN);
				Mqtt_Send(mqtt_msg,mqtt_data_len);
		
				Cnt ++;
		
				if(Cnt >= 120)
				{
					mqtt_data_len = mqtt_disconnect_message(mqtt_msg);
					Mqtt_Send(mqtt_msg,mqtt_data_len);
					Cnt = 0;
				}
			}
			else
			{
				mqtt_data_len = MQTT_PublishDataPacket(mqtt_msg,"Air");
				Mqtt_Send(mqtt_msg,mqtt_data_len);
			}
		}
	}
}


void Send_Data2Server(void)
{
	static uint8_t Cnt = 0;
	uint8_t i = 0;
	uint8_t Buff[128] = {0};
	uint8_t Bufflen = 0;
	Cnt ++;
	
	if(Cnt >= 20 && !ucTcpClosedFlag )
	{
		
		//Bufflen = sprintf((char*)Buff,"$DATA,%.2f,%.2f,%.7lf,%.7lf,%.2f,%.2f,%.2f*FF\r\n",dev_data.temp,dev_data.humi,dev_data.lat,dev_data.lon,dev_data.speed,dev_data.Distance,dev_data.Mq131);
		
		for(i = 0;i < Bufflen; i++)
		{
			while(USART_GetFlagStatus(macESP8266_USARTx,USART_FLAG_TC)==RESET){}      //waiting for completion
				USART_SendData( macESP8266_USARTx ,Buff[i]); //Forwarding to ESP82636
		}
		
		Cnt = 0;
	}
}

void Read_Dht11(void)
{
	static uint8_t Cnt = 0;
	
	Cnt ++;
	
	if(Cnt >= 40)
	{
		if( DHT11_Read_TempAndHumidity ( & DHT11_Data ) == SUCCESS)
		{
				//printf("Humidity:%d.%d £¥RH , Temperature:%d.%d¡æ \r\n",\
				//DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
				
				dev_data.humi = DHT11_Data.humi_int;
				
				dev_data.temp = DHT11_Data.temp_int + (float)DHT11_Data.temp_deci/100 ;
		}			
		Cnt = 0;
	}
}

void Read_SPG30(void)
{
	static uint8_t Cnt = 0;
	u32 CO2Data,TVOCData;//Define variable for CO2 and TVOC
	u32 sgp30_dat;
	Cnt ++;
	
	if(Cnt >= 40)
	{
		SGP30_Write(0x20,0x08);
		sgp30_dat = SGP30_Read();//read SGP30
		CO2Data = (sgp30_dat & 0xffff0000) >> 16;//read CO2
		TVOCData = sgp30_dat & 0x0000ffff;       //read TVOC
		dev_data.sgp30 = TVOCData;
		dev_data.co2 = CO2Data;
		//printf("CO2:%dppm\r\nTVOC:%dppd\r\n",CO2Data,TVOCData);
		Cnt = 0;
	}
}

void Read_PM(uint8_t *valueBuffer,int len)
{
	int checksum = 0;
	int i;
	for(i = 0 ; i < 30 ; i++)
	{
		checksum += valueBuffer[i];	
	}
	
	if (valueBuffer[0] == 0x42 && valueBuffer[1] == 0x4d && checksum == valueBuffer[30]*256+valueBuffer[31])
	{
		dev_data.PM1VAL = valueBuffer[10]*256+valueBuffer[11];
		dev_data.PM2_5VAL = valueBuffer[12]*256+valueBuffer[13];
		dev_data.PM10VAL = valueBuffer[14]*256+valueBuffer[15];
				
		valueBuffer[0] = 0;
		valueBuffer[1] = 0;
	}
}



int main(void)
{

	/* Initialize the system clock*/
	SysTick_Init();
	
	USART_Config();//Initialize serial port
	ESP8266_Init ();
	ESP8266_StaTcpClient_startUnvarnishTest();
	LED_Init ();
	DHT11_Init();
	SGP30_Init();   //Initialize SGP30
	while(1)
	{	
		ESP8266_StaTcpClient_UnvarnishTest();
		Toggle_Led();
		Read_Dht11();
		Read_SPG30();
		Delay_ms(50);
		MQTT_CallBack();
	}
}
/*********************************************END OF FILE**********************/
