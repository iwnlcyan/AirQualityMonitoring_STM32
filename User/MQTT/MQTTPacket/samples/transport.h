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
#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__
 
typedef struct  
{										    
  unsigned int send_delay;
  char ClientID[20];
  char MQTTUserName[20];
  char MQTTPassWord[20];
  unsigned char server_ip[18];
  unsigned char server_port[6];
}option_msg;  

extern option_msg optmsg;

extern int Revlen;

int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen);
int transport_getdata(unsigned char* buf, int count);
int transport_getdatanb(void *sck, unsigned char* buf, int count);
int transport_open(char* host, int port);
int transport_close(int sock);

unsigned short MQTT_ConnectPacket(unsigned char* Conbuf);
unsigned char MQTT_ConnectAnalysis(unsigned char* conbuf,unsigned short conlen);

unsigned short MQTT_SubsPacket(unsigned char* Conbuf);
unsigned char MQTT_SubsAnalysis(unsigned char* conbuf,unsigned short conlen);

unsigned short MQTT_PublishstaPacket(unsigned char* Conbuf);
unsigned short MQTT_PubrstPacket(unsigned char* Conbuf);
unsigned short MQTT_PublishDataPacket(unsigned char* Conbuf);
unsigned char MQTT_PublishAnalysis(unsigned char* conbuf,unsigned short conlen);
unsigned short MQTT_PublishOfflinePacket(unsigned char* Conbuf);

unsigned char MQTT_ControlAnalysis(unsigned char* conbuf,unsigned short conlen);

void Csq_Check(void);

#endif
