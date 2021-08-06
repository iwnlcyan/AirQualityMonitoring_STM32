#ifndef MQTTCLIENT_H_
#define MQTTCLIENT_H_


#ifndef MQTTCLIENT_C_//���û�ж���
#define MQTTCLIENT_Cx_ extern
#else
#define MQTTCLIENT_Cx_
#endif

#include "string.h"
#include "stm32f4xx_hal.h"
/*!< Unsigned integer types  */
//typedef unsigned char     uint8_t;
//typedef unsigned short    uint16_t;
//typedef unsigned long     uint32_t;

#define c_memcpy memcpy
#define c_memset memset
#define c_strlen strlen

//typedef unsigned char uint8_t;
//typedef unsigned int  uint16_t;


enum mqtt_message_type
{
  MQTT_MSG_TYPE_CONNECT = 1,
  MQTT_MSG_TYPE_CONNACK = 2,
  MQTT_MSG_TYPE_PUBLISH = 3,
  MQTT_MSG_TYPE_PUBACK = 4,
  MQTT_MSG_TYPE_PUBREC = 5,
  MQTT_MSG_TYPE_PUBREL = 6,
  MQTT_MSG_TYPE_PUBCOMP = 7,
  MQTT_MSG_TYPE_SUBSCRIBE = 8,
  MQTT_MSG_TYPE_SUBACK = 9,
  MQTT_MSG_TYPE_UNSUBSCRIBE = 10,
  MQTT_MSG_TYPE_UNSUBACK = 11,
  MQTT_MSG_TYPE_PINGREQ = 12,
  MQTT_MSG_TYPE_PINGRESP = 13,
  MQTT_MSG_TYPE_DISCONNECT = 14
};

enum mqtt_connack_return_code
{
    MQTT_CONN_FAIL_SERVER_NOT_FOUND = -5,
    MQTT_CONN_FAIL_NOT_A_CONNACK_MSG = -4,
    MQTT_CONN_FAIL_DNS = -3,
    MQTT_CONN_FAIL_TIMEOUT_RECEIVING = -2,
    MQTT_CONN_FAIL_TIMEOUT_SENDING = -1,
    MQTT_CONNACK_ACCEPTED = 0,
    MQTT_CONNACK_REFUSED_PROTOCOL_VER = 1,
    MQTT_CONNACK_REFUSED_ID_REJECTED = 2,
    MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE = 3,
    MQTT_CONNACK_REFUSED_BAD_USER_OR_PASS = 4,
    MQTT_CONNACK_REFUSED_NOT_AUTHORIZED = 5
};



//����MQTTָ�� 
typedef struct mqtt_connect_info
{
  char* client_id;
  char* username;
  char* password;
  char* will_topic;
  char* will_message;
  int keepalive;
  int will_qos;
  int will_retain;
  int clean_session;
	
} mqtt_connect_info_t;

MQTTCLIENT_Cx_ mqtt_connect_info_t connect_info;


int mqtt_get_type(uint8_t* buffer);
int mqtt_get_connect_ret_code(uint8_t* buffer);
int mqtt_get_qos(uint8_t* buffer);

int mqtt_msg_connect(mqtt_connect_info_t* info,uint8_t **data_ptr,uint8_t* buffer,int buffer_length);
char MqttConnectMqttAck(uint8_t *buff);
int mqtt_msg_subscribe_topic(char* topic, int qos,uint8_t **data_ptr,uint8_t* buffer,int buffer_length);
char MqttSubscribeAck(uint8_t* buffer, uint16_t length);
int mqtt_msg_publish(char* topic,char* date, int data_length, int qos, int retain, uint8_t **data_ptr,uint8_t* buffer,int buffer_length);


#define		MQTT_TypeCONNECT					1//��������  
#define		MQTT_TypeCONNACK					2//����Ӧ��  
#define		MQTT_TypePUBLISH					3//������Ϣ  
#define		MQTT_TypePUBACK						4//����Ӧ��  
#define		MQTT_TypePUBREC						5//�����ѽ��գ���֤����1  
#define		MQTT_TypePUBREL						6//�����ͷţ���֤����2  
#define		MQTT_TypePUBCOMP					7//������ɣ���֤����3  
#define		MQTT_TypeSUBSCRIBE					8//��������  
#define		MQTT_TypeSUBACK						9//����Ӧ��  
#define		MQTT_TypeUNSUBSCRIBE					10//ȡ������  
#define		MQTT_TypeUNSUBACK					11//ȡ������Ӧ��  
#define		MQTT_TypePINGREQ					12//ping����  
#define		MQTT_TypePINGRESP					13//ping��Ӧ  
#define		MQTT_TypeDISCONNECT 					14//�Ͽ�����  
 
#define		MQTT_StaCleanSession					1	//����Ự 
#define		MQTT_StaWillFlag					0	//������־
#define		MQTT_StaWillQoS						0	//����QoS���ӱ�־�ĵ�4�͵�3λ��
#define		MQTT_StaWillRetain					0	//��������
#define		MQTT_StaUserNameFlag					1	//�û�����־ User Name Flag
#define		MQTT_StaPasswordFlag					1	//�����־ Password Flag
#define		MQTT_KeepAlive								60
#define		MQTT_ClientIdentifier                                   "Tan1"	//�ͻ��˱�ʶ�� Client Identifier
#define		MQTT_WillTopic			                        ""				//�������� Will Topic
#define		MQTT_WillMessage		                        ""				//������Ϣ Will Message
#define		MQTT_UserName			                        "admin"			//�û��� User Name
#define		MQTT_Password			                        "password"	//���� Password
 
unsigned char GetDataFixedHead(unsigned char MesType,unsigned char DupFlag,unsigned char QosLevel,unsigned char Retain);
unsigned int GetDataPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg);//��ȡ������Ϣ�����ݰ�		 	
void GetDataSUBSCRIBE(unsigned char *buff,const char *dat,unsigned int Num,unsigned char RequestedQoS);//������������ݰ� Num:������� RequestedQoS:��������Ҫ��0,1��2
void GetDataDisConnet(unsigned char *buff);//��ȡ�Ͽ����ӵ����ݰ�
void GetDataConnet(unsigned char *buff);//��ȡ���ӵ����ݰ���ȷ���ӷ���20 02 00 00
void GetDataPINGREQ(unsigned char *buff);//������������ݰ��ɹ�����d0 0
unsigned int GetDataONENETPUBLISH(unsigned char *buff,unsigned char onenetty,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg);//��ȡ������Ϣ�����ݰ�

#endif



