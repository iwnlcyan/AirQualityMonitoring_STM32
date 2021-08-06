/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "transport.h"
#include "MQTTPacket.h"
#include "MQTTConnect.h"
#include "usart.h"
//#include "AgreeMent.h"
#include "MqttClient.h"
#include "aes_soft.h"
#include "gps.h" 
#include "AgreeMent.h"
#include "calender.h"

#if !defined(SOCKET_ERROR)
	/** error in socket operation */
	#define SOCKET_ERROR -1
#endif

#if defined(WIN32)
/* default on Windows is 64 - increase to make Linux and Windows the same */
#define FD_SETSIZE 1024
#include <winsock2.h>
#include <ws2tcpip.h>
#define MAXHOSTNAMELEN 256
#define EAGAIN WSAEWOULDBLOCK
#define EINTR WSAEINTR
#define EINVAL WSAEINVAL
#define EINPROGRESS WSAEINPROGRESS
#define EWOULDBLOCK WSAEWOULDBLOCK
#define ENOTCONN WSAENOTCONN
#define ECONNRESET WSAECONNRESET
#define ioctl ioctlsocket
#define socklen_t int
#else
#define INVALID_SOCKET SOCKET_ERROR
//#include <sys/socket.h>
//#include <sys/param.h>
//#include <sys/time.h>
//#include <netinet/in.h>
//#include <netinet/tcp.h>
//#include <arpa/inet.h>
//#include <netdb.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <errno.h>
//#include <fcntl.h>
//#include <string.h>
//#include <stdlib.h>
#endif

#if defined(WIN32)
#include <Iphlpapi.h>
#else
//#include <sys/ioctl.h>
//#include <net/if.h>
#endif

/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/

option_msg optmsg;

char MQTTkeepAlive = 120;//心跳包时间

static int mysock = INVALID_SOCKET;

int Revlen = 0;

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	int rc = 0;
	//rc = write(sock, buf, buflen);
	return rc;
}


int transport_getdata(unsigned char* buf, int count)
{
	int rc=0;
		
	rc=Revlen;
	
	if(rc >= count)
		rc = count;
	//rc = recv(mysock, buf, count, 0);
	//printf("received %d bytes count %d\n", rc, (int)count);
	return rc;
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	int sock = *((int *)sck); 	/* sck: pointer to whatever the system may use to identify the transport */
	/* this call will return after the timeout set on initialization if no bytes;
	   in your system you will use whatever you use to get whichever outstanding
	   bytes your socket equivalent has ready to be extracted right now, if any,
	   or return immediately */
	int rc=0;
		
	//rc = recv(sock, buf, count, 0);
	
	if (rc == -1) {
		/* check error conditions from your system here, and return -1 */
		return 0;
	}
	return rc;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
int transport_open(char* addr, int port)
{
int* sock = &mysock;
	
	return mysock;
}

int transport_close(int sock)
{
int rc;

//	rc = shutdown(sock, SHUT_WR);
//	rc = recv(sock, NULL, (size_t)0, 0);
//	rc = close(sock);

	return rc;
}

unsigned short MQTT_ConnectPacket(unsigned char* Conbuf)
{
	unsigned char Oribuff[128] = {0};
	unsigned short Orilen = 0;
	unsigned char *MainString;
	unsigned short enclen = 0;
	char opt_buff[70]={0};
	
	MQTTSerialize_opt(&options_data);
		
	options_data.clientID.cstring = optmsg.ClientID;
	options_data.keepAliveInterval = MQTTkeepAlive;
	options_data.cleansession = 1;
		
	options_data.username.cstring = optmsg.MQTTUserName;
	options_data.password.cstring = optmsg.MQTTPassWord;
	
	Orilen = MQTTSerialize_connect(Conbuf, 200, &options_data);
	
	enclen = Orilen;
	
	return enclen;
}

unsigned char MQTT_ConnectAnalysis(unsigned char* conbuf,unsigned short conlen)
{
	if(MqttConnectMqttAck(conbuf)>=0)
	{
		return 1;
	}
	return 0;
}

unsigned short MQTT_SubsPacket(unsigned char* Conbuf)
{
	unsigned char Oribuff[128] = {0};
	MQTTString topicString = MQTTString_initializer;
	int req_qos = 0;
	unsigned short enclen = 0;
	char opt_buff[70]={0};
	
	sprintf(opt_buff,"%s%s%s","gwwz/",optmsg.ClientID,"/control");
	
	topicString.cstring = opt_buff;
	
  enclen = MQTTSerialize_subscribe(Conbuf, 200, 0, 1, 1, &topicString, &req_qos);
	
	return enclen;
}

unsigned char MQTT_SubsAnalysis(unsigned char* conbuf,unsigned short conlen)
{
	unsigned char sessionPresent, connack_rc;
	unsigned char Revbuf[128] = {0};
	
	if(MqttSubscribeAck(conbuf,Revlen)==0)
	{
		return 1;
	}			
	
	return 0;
}

unsigned short MQTT_PubrstPacket(unsigned char* Conbuf)
{
	unsigned char Oribuff[200] = {0};
	char MQTTWillMessage[200] = {0};
	unsigned short Msglen = 0;
	char opt_buff[70]={0};
	unsigned short Orilen = 0;
	unsigned short enclen = 0;
	MQTTString topicString = MQTTString_initializer;
	

	sprintf((char*)opt_buff,"%s%s%s","njltd/",optmsg.ClientID,"/status");
	Msglen = sprintf((char *)Oribuff,"restarted");
	
	topicString.cstring = opt_buff;
	
	mbedtls_aes_Dataencrypt(Oribuff, (unsigned char*)MQTTWillMessage, Msglen, &Orilen);
	
	enclen = MQTTSerialize_publish(Conbuf, 200, 0, 1, 0, 0, topicString, (unsigned char*)MQTTWillMessage, Orilen);
	
	return enclen;
}

unsigned short MQTT_PublishstaPacket(unsigned char* Conbuf)
{
	unsigned char Oribuff[200] = {0};
	char MQTTWillMessage[200] = {0};
	unsigned short Msglen = 0;
	char opt_buff[70]={0};
	unsigned short Orilen = 0;
	unsigned short enclen = 0;
	MQTTString topicString = MQTTString_initializer;
	

	sprintf((char*)opt_buff,"%s%s%s","njltd/",optmsg.ClientID,"/status");
	Msglen = sprintf((char *)Oribuff,"online");
	
	topicString.cstring = opt_buff;
	
	mbedtls_aes_Dataencrypt(Oribuff, (unsigned char*)MQTTWillMessage, Msglen, &Orilen);
	
	enclen = MQTTSerialize_publish(Conbuf, 200, 0, 1, 0, 0, topicString, (unsigned char*)MQTTWillMessage, Orilen);
	
	return enclen;
}

uint16_t HexToASCII(uint8_t *Dstbuf,uint8_t *data_hex,uint16_t hexlen)
{
	uint16_t Dstlen = 0;
	uint16_t i = 0;
	
	for(i = 0; i < hexlen; i++)
		Dstlen += sprintf((char*)(Dstbuf+Dstlen),"%02X",data_hex[i]);
	
	return Dstlen;
}

#if 1
unsigned short MQTT_PublishDataPacket(unsigned char* Conbuf)
{
	unsigned char Oribuff[200] = {0};
	unsigned char MQTTWillMessage[200] = {0};
	unsigned short Msglen = 0;
	char opt_buff[70]={0};
	char Timestamp[20]={0};
	unsigned short Orilen = 0;
	unsigned short enclen = 0;
	unsigned char gpsflag = 0;
	int batvalue = 0;
	DateTime time;
	MQTTString topicString = MQTTString_initializer;
	
	batvalue = (int)(((Agrmsg.Voltage[Agrmsg.voltageCnt-1] - 10.0)/2.0+0.005)*100);
	
	if(batvalue >= 100)
		batvalue = 100;
	else if(batvalue <= 0)
		batvalue = 0;
	
	if(gpsx.ewhemi=='E'||gpsx.ewhemi=='W')
	{
		gpsflag = 1;
	}
	
	time = SecToTime(MainTimerCount+28800, UTC);
	
	sprintf((char*)Timestamp,"%04d%02d%02d%02d%02d%02d",time.year,time.month,time.day,time.hour,time.minute,time.second);
	
	sprintf((char*)opt_buff,"%s%s%s","njltd/",optmsg.ClientID,"/data");
	
  Msglen = sprintf((char *)Oribuff,"[{\"1\":{\"1\":\"%d\",\"2\":\"%d\",\"3\":\"%.2f\",\"4\":\"%.7lf\",\"5\":\"%.7lf\",\"15\":\"%d\",\"19\":\"0\"},\"id\":\"%s\",\"timestamp\":\"%s\"}]",
																			Agrmsg.Csq,batvalue,gpsx.speed,gpsx.longitude,gpsx.latitude,gpsflag,optmsg.ClientID,Timestamp);
	
	topicString.cstring = opt_buff;
	
	Uart7Send(Oribuff,Msglen);
	
	mbedtls_aes_Dataencrypt(Oribuff, (unsigned char*)MQTTWillMessage, Msglen, &Orilen);
	
	Uart7Send(MQTTWillMessage,Orilen);
	
	enclen = MQTTSerialize_publish(Conbuf, 200, 0, 1, 0, 0, topicString, (unsigned char*)MQTTWillMessage, Orilen);
	
	return enclen;
}
#else
unsigned short MQTT_PublishDataPacket(unsigned char* Conbuf)
{
	unsigned char Oribuff[200] = {0};
	unsigned char MQTTWillMessage[200] = {0};
	unsigned short Msglen = 0;
	char opt_buff[70]={0};
	char Timestamp[20]={0};
	unsigned short Orilen = 0;
	unsigned short enclen = 0;
	unsigned char gpsflag = 0;
	DateTime time;
	MQTTString topicString = MQTTString_initializer;
	
	if(gpsx.ewhemi=='E'||gpsx.ewhemi=='W')
	{
		gpsflag = 1;
	}
	
	time = SecToTime(MainTimerCount+28800, UTC);
	
	sprintf((char*)Timestamp,"%04d%02d%02d%02d%02d%02d",time.year,time.month,time.day,time.hour,time.minute,time.second);
	
	sprintf((char*)opt_buff,"%s%s%s","njltd/",optmsg.ClientID,"/data");
	
  Msglen = sprintf((char *)Oribuff,"[{\"1\":{\"1\":\"%d\",\"2\":\"83\",\"3\":\"%.2f\",\"4\":\"%.7lf\",\"5\":\"%.7lf\",\"15\":\"%d\",\"19\":\"0\"},\"id\":\"%s\",\"timestamp\":\"%s\"}]",
																			Agrmsg.Csq,gpsx.speed,gpsx.longitude,gpsx.latitude,gpsflag,optmsg.ClientID,Timestamp);
	
	topicString.cstring = opt_buff;
	
	Uart5Send(Oribuff,Msglen);
	
//	mbedtls_aes_Dataencrypt(Oribuff, (unsigned char*)MQTTWillMessage, Msglen, &Orilen);
//	
//	Uart5Send(MQTTWillMessage,Orilen);
	
	enclen = MQTTSerialize_publish(Conbuf, 200, 0, 1, 0, 0, topicString, (unsigned char*)Oribuff, Msglen);
	
	return enclen;
}
#endif

unsigned char MQTT_PublishAnalysis(unsigned char* conbuf,unsigned short conlen)
{
	
	if(mqtt_get_type(conbuf) == MQTT_MSG_TYPE_PUBACK)
	{
		return 1;
	}
	
	return 0;
}

unsigned char MQTT_ControlAnalysis(unsigned char* conbuf,unsigned short conlen)
{
	char opt_buff[70]={0};
	uint8_t Oribuf[96] = {0};
	uint16_t Orilen = 0;
	uint16_t i = 0;
	uint16_t enclen = 0;
	
	sprintf((char*)opt_buff,"%s%s%s","gwwz/",optmsg.ClientID,"/control");
	
	if(mqtt_get_type(conbuf) == MQTT_MSG_TYPE_PUBLISH)//获取到MQTT消息
	{
		for(i = 0; i < conlen; i++)
		{
			if(conbuf[i]=='g'&&conbuf[i+1]=='w'&&conbuf[i+2]=='w'&&conbuf[i+3]=='z')
			if(strstr((char*)&conbuf[i], opt_buff)!=NULL)
			{
				enclen = conlen - (i+29);
				
				mbedtls_aes_Datadecrypt(Oribuf,&conbuf[i+29],enclen,&Orilen);
				
				if(strstr((char*)Oribuf, "restart")!=NULL)
				{
				
				}
				
				return 1;
			}
		}
	}
	
	return 0;
}

unsigned short MQTT_PublishOfflinePacket(unsigned char* Conbuf)
{
	unsigned char Oribuff[200] = {0};
	char MQTTWillMessage[200] = {0};
	unsigned short Msglen = 0;
	char opt_buff[70]={0};
	unsigned short Orilen = 0;
	unsigned short enclen = 0;
	MQTTString topicString = MQTTString_initializer;
	

	sprintf((char*)opt_buff,"%s%s%s","njltd/",optmsg.ClientID,"/status");
	Msglen = sprintf((char *)Oribuff,"offline");
	
	topicString.cstring = opt_buff;
	
	mbedtls_aes_Dataencrypt(Oribuff, (unsigned char*)MQTTWillMessage, Msglen, &Orilen);
	
	enclen = MQTTSerialize_publish(Conbuf, 200, 0, 1, 0, 0, topicString, (unsigned char*)MQTTWillMessage, Orilen);
	
	return enclen;
}

void Csq_Check(void)
{
	Ec20CmdCSQ((uint8_t*)"AT+CSQ\r\n","OK",3);
	
	if(Agrmsg.Csq >= 0)
		Agrmsg.Csq = 0;
}

