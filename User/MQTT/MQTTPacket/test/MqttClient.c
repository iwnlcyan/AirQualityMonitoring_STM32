/**
  ******************************************************************************
  * @author  yang feng wu 
  * @version V1.0.0
  * @date    2019/12/15
  * @brief   
  ******************************************************************************
	
  ******************************************************************************
  */

#define MQTTCLIENT_C_//���û�ж���

#include "MqttClient.h"
#include "string.h"


mqtt_connect_info_t connect_info;


#define MQTT_MAX_FIXED_HEADER_SIZE 3

uint16_t mqtt_message_id = 0;

enum mqtt_connect_flag
{
  MQTT_CONNECT_FLAG_USERNAME = 1 << 7,
  MQTT_CONNECT_FLAG_PASSWORD = 1 << 6,
  MQTT_CONNECT_FLAG_WILL_RETAIN = 1 << 5,
  MQTT_CONNECT_FLAG_WILL = 1 << 2,
  MQTT_CONNECT_FLAG_CLEAN_SESSION = 1 << 1
};
//__attribute((__packed__))
struct  mqtt_connect_variable_header
{
  uint8_t lengthMsb;
  uint8_t lengthLsb;
  uint8_t magic[4];
  uint8_t version;
  uint8_t flags;
  uint8_t keepaliveMsb;
  uint8_t keepaliveLsb;
};


int mqtt_get_type(uint8_t* buffer) { return (buffer[0] & 0xf0) >> 4; }
int mqtt_get_connect_ret_code(uint8_t* buffer) { return (buffer[3]); }
int mqtt_get_qos(uint8_t* buffer) { return (buffer[0] & 0x06) >> 1; }


int append_string(int *length, uint8_t* buffer,int buffer_length, char* string, int len)
{
  if((*length) + len + 2 > buffer_length)//���� ClientID �� ��¼ ClientID����(��λ) �Ժ󳬳�������
    return -1;

  buffer[(*length)++] = len >> 8;
  buffer[(*length)++] = len & 0xff;
  c_memcpy(buffer + (*length), string, len);
  (*length) += len;
  return len + 2;
}



uint16_t append_message_id(int *length, uint8_t* buffer,int buffer_length, uint16_t message_id)
{
  // If message_id is zero then we should assign one, otherwise
  // we'll use the one supplied by the caller
  while(message_id == 0)
    message_id = ++mqtt_message_id;

  if((*length) + 2 > buffer_length)
    return 0;

  buffer[(*length)++] = message_id >> 8;
  buffer[(*length)++] = message_id & 0xff;
	
  return message_id;
}


int fini_message(uint8_t **data_ptr,int	length,uint8_t* buffer, int type, int dup, int qos, int retain)
{
  int remaining_length = length - MQTT_MAX_FIXED_HEADER_SIZE;
	
  if(remaining_length > 127)
  {
    buffer[0] = ((type & 0x0f) << 4) | ((dup & 1) << 3) | ((qos & 3) << 1) | (retain & 1);
    buffer[1] = 0x80 | (remaining_length % 128);
    buffer[2] = remaining_length / 128;
    length = remaining_length + 3;
    *data_ptr = buffer;
  }
  else
  {
    buffer[1] = ((type & 0x0f) << 4) | ((dup & 1) << 3) | ((qos & 3) << 1) | (retain & 1);
    buffer[2] = remaining_length;
    length = remaining_length + 2;
    *data_ptr = buffer + 1;
  }

  return length;
}



uint16_t mqtt_get_id(uint8_t* buffer, uint16_t length)
{
  if(length < 1)
    return 0;
	
  switch(mqtt_get_type(buffer))
  {
    case MQTT_MSG_TYPE_PUBLISH:
    {
      int i;
      int topiclen;

      for(i = 1; i < length; ++i)
      {
        if((buffer[i] & 0x80) == 0)
        {
          ++i;
          break;
        }
      }

      if(i + 2 >= length)
        return 0;
      topiclen = buffer[i++] << 8;
      topiclen |= buffer[i++];

      if(i + topiclen >= length)
        return 0;
      i += topiclen;

      if(mqtt_get_qos(buffer) > 0)
      {
        if(i + 2 >= length)
          return 0;
        //i += 2;
      } else {
    	  return 0;
      }
      return (buffer[i] << 8) | buffer[i + 1];
    }
    case MQTT_MSG_TYPE_PUBACK:
    case MQTT_MSG_TYPE_PUBREC:
    case MQTT_MSG_TYPE_PUBREL:
    case MQTT_MSG_TYPE_PUBCOMP:
    case MQTT_MSG_TYPE_SUBACK:
    case MQTT_MSG_TYPE_UNSUBACK:
    case MQTT_MSG_TYPE_SUBSCRIBE:
    {
      // This requires the remaining length to be encoded in 1 byte,
      // which it should be.
      if(length >= 4 && (buffer[1] & 0x80) == 0)
        return (buffer[2] << 8) | buffer[3];
      else
        return 0;
    }

    default:
      return 0;
  }
}






/**
* @brief   �������MQTTָ��
* @param   info     MQTT��Ϣ
* @param   data_ptr ����������׵�ַ
* @param   buffer   �����������
* @param   buffer_length ���鳤��
* @retval  ���ݳ��� 
* @warning None
* @example 
**/
int mqtt_msg_connect(mqtt_connect_info_t* info,uint8_t **data_ptr,uint8_t* buffer,int buffer_length)
{
	int length;
  struct mqtt_connect_variable_header* variable_header;
	
	mqtt_message_id = 0;
	
	length = MQTT_MAX_FIXED_HEADER_SIZE;//ͷ.��������1λ,���ݸ���2λ(�������127����Ҫ��λ)
	
  if(length + sizeof(*variable_header) > buffer_length)//���鲻���洢��
    return 0;
	
  variable_header = (void*)(buffer + length);//������ָ�����ṹ������ı���
  length += sizeof(*variable_header);//�洢�� ��������,�������ݸ���,�汾�Ÿ���,�汾��,��
	
  variable_header->lengthMsb = 0;//�汾���Ƹ�����λ
  variable_header->lengthLsb = 4;//�汾���Ƹ�����λ
  c_memcpy(variable_header->magic, "MQTT", 4);//�汾����MQTT
  variable_header->version = 4;//�汾��
  variable_header->flags = 0;//������
  variable_header->keepaliveMsb = info->keepalive >> 8;//������ʱ��
  variable_header->keepaliveLsb = info->keepalive & 0xff;//������ʱ��

  if(info->clean_session)//���������Ϣ
    variable_header->flags |= MQTT_CONNECT_FLAG_CLEAN_SESSION;

  if(info->client_id != NULL && info->client_id[0] != '\0')//client_id
  {
    if(append_string(&length,buffer,buffer_length, info->client_id, c_strlen(info->client_id)) < 0)//����
      return -1;//���鲻����ѽ...
  }
  else
    return -2;//û������client_id

  if(info->will_topic != NULL && info->will_topic[0] != '\0')//����
  {
    if(append_string(&length,buffer,buffer_length , info->will_topic, c_strlen(info->will_topic)) < 0)//����������
      return -3;

    if(append_string(&length,buffer,buffer_length , info->will_message, c_strlen(info->will_message)) < 0)//��������Ϣ
      return -4;

    variable_header->flags |= MQTT_CONNECT_FLAG_WILL;//��Ҫ����
    if(info->will_retain)//�����ǹ���Ҫ����������
      variable_header->flags |= MQTT_CONNECT_FLAG_WILL_RETAIN;//��������
    variable_header->flags |= (info->will_qos & 3) << 3;//������Ϣ�ȼ�
  }

  if(info->username != NULL && info->username[0] != '\0')//username
  {
    if(append_string(&length,buffer,buffer_length, info->username, c_strlen(info->username)) < 0)//�����û���
      return -5;

    variable_header->flags |= MQTT_CONNECT_FLAG_USERNAME;//���û���
  }
	
  if(info->password != NULL && info->password[0] != '\0')//password
  {
    if(append_string(&length,buffer,buffer_length, info->password, c_strlen(info->password)) < 0)
      return -6;

    variable_header->flags |= MQTT_CONNECT_FLAG_PASSWORD;//������
  }

  return fini_message(data_ptr,length, buffer, MQTT_MSG_TYPE_CONNECT, 0, 0, 0);//�����������MQTT��ָ��
}


/**
* @brief  �ж��Ƿ�������MQTT
* @param  ���������ص�����
* @param  
* @retval 0 ���ӳɹ�
* @example 
**/
char MqttConnectMqttAck(uint8_t *buff)
{
	if(mqtt_get_type(buff) == MQTT_MSG_TYPE_CONNACK)
	{
		return mqtt_get_connect_ret_code(buff);
	}
	return -1;
}



/**
* @brief   ��������
* @param   topic   ���ĵ�����
* @param   qos     ��Ϣ�ȼ�
* @param   data_ptr ����������׵�ַ
* @param   buffer   �����������
* @param   buffer_length ���鳤��
* @retval  ���ݳ��� 
* @warning None
* @example 
**/
int mqtt_msg_subscribe_topic(char* topic, int qos,uint8_t **data_ptr,uint8_t* buffer,int buffer_length)
{
	int length;
	length = MQTT_MAX_FIXED_HEADER_SIZE;
	
	if(topic == NULL || topic[0] == '\0')
		return -1;
	
	if((mqtt_message_id = append_message_id(&length, buffer, buffer_length, 0)) == 0)
		return -2;
	
	if(append_string(&length, buffer, buffer_length, topic, c_strlen(topic)) < 0)
		return -3;
	
	if(length + 1 > buffer_length)
    return -4;
  buffer[length++] = qos;
	
	return fini_message(data_ptr,length, buffer, MQTT_MSG_TYPE_SUBSCRIBE, 0, 1, 0);
}



/**
* @brief  �ж��Ƿ�ɹ�����
* @param  buffer  ���������ص�����
* @param  length  ���������ص����ݳ���
* @retval 0:�ɹ�  1:ʧ��
* @example 
**/
char MqttSubscribeAck(uint8_t* buffer, uint16_t length)
{
	if(mqtt_get_type(buffer) == MQTT_MSG_TYPE_SUBACK)
	{
		if(mqtt_get_id(buffer,length) == mqtt_message_id)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 1;
	}
}


/**
* @brief   ������Ϣ
* @param   topic    ����
* @param   data     ��Ϣ
* @param   data_length ��Ϣ����
* @param   qos      ��Ϣ�ȼ�
* @param   retain   �Ƿ���Ҫ������Ϣ
* @param   data_ptr ����������׵�ַ
* @param   buffer   �����������
* @param   buffer_length ���鳤��
* @retval  ���ݳ��� 
* @warning None
* @example 
**/
int mqtt_msg_publish(char* topic, char* date, int data_length, int qos, int retain, uint8_t **data_ptr,uint8_t* buffer,int buffer_length)
{
	int length;
	length = MQTT_MAX_FIXED_HEADER_SIZE;

  if(topic == NULL || topic[0] == '\0')
    return -1;

  if(append_string(&length, buffer, buffer_length, topic, strlen(topic)) < 0)
    return -2;

  if(qos > 0)
  {
    if((mqtt_message_id = append_message_id(&length, buffer, buffer_length,  0)) == 0)
      return -3;
  }
  else
    mqtt_message_id = 0;

  if(length + data_length > buffer_length)
    return -4;
  memcpy(buffer + length, date, data_length);
  length += data_length;

  return fini_message(data_ptr,length, buffer, MQTT_MSG_TYPE_PUBLISH, 0, qos, retain);
}



//char*  mqtt_get_publish_topic(uint8_t* buffer, uint16_t* length)
//{
//  int i;
//  int totlen = 0;
//  int topiclen;

//  for(i = 1; i < *length; ++i)
//  {
//    totlen += (buffer[i] & 0x7f) << (7 * (i -1));
//    if((buffer[i] & 0x80) == 0)
//    {
//      ++i;
//      break;
//    }
//  }
//  totlen += i;

//  if(i + 2 >= *length)
//    return NULL;
//  topiclen = buffer[i++] << 8;
//  topiclen |= buffer[i++];

//  if(i + topiclen > *length)
//    return NULL;

//  *length = topiclen;
//  return (const char*)(buffer + i);
//}

//char* mqtt_get_publish_data(uint8_t* buffer, uint16_t* length)
//{
//  int i;
//  int totlen = 0;
//  int topiclen;
//  int blength = *length;
//  *length = 0;

//  for(i = 1; i < blength; ++i)
//  {
//    totlen += (buffer[i] & 0x7f) << (7 * (i - 1));
//    if((buffer[i] & 0x80) == 0)
//    {
//      ++i;
//      break;
//    }
//  }
//  totlen += i;

//  if(i + 2 >= blength)
//    return NULL;
//  topiclen = buffer[i++] << 8;
//  topiclen |= buffer[i++];

//  if(i + topiclen >= blength)
//    return NULL;

//  i += topiclen;

//  if(mqtt_get_qos(buffer) > 0)
//  {
//    if(i + 2 >= blength)
//      return NULL;
//    i += 2;
//  }

//  if(totlen < i)
//    return NULL;

//  if(totlen <= blength)
//    *length = totlen - i;
//  else
//    *length = blength - i;
//  return (const char*)(buffer + i);
//}


unsigned char GetDataFixedHead(unsigned char MesType,unsigned char DupFlag,unsigned char QosLevel,unsigned char Retain)
{
	unsigned char dat = 0;
	dat = (MesType & 0x0f) << 4;
	dat |= (DupFlag & 0x01) << 3;
	dat |= (QosLevel & 0x03) << 1;
	dat |= (Retain & 0x01);
	return dat;
}
void GetDataConnet(unsigned char *buff)//��ȡ���ӵ����ݰ���ȷ���ӷ���20 02 00 00
{
	unsigned int i,len,lennum = 0;
	unsigned char *msg;
	buff[0] = GetDataFixedHead(MQTT_TypeCONNECT,0,0,0);
	buff[2] = 0x00;
	buff[3] = 0x04;
	buff[4] = 'M';
	buff[5] = 'Q';
	buff[6] = 'T';
	buff[7] = 'T';
	buff[8] = 0x04;//Э�鼶�� Protocol Level
	buff[9] = 0 | (MQTT_StaCleanSession << 1) | (MQTT_StaWillFlag << 1)
							| (MQTT_StaWillQoS << 3) | (MQTT_StaWillRetain << 5) 
							|	(MQTT_StaPasswordFlag << 6) |(MQTT_StaUserNameFlag << 7);//���ӱ�־
	buff[10] = MQTT_KeepAlive >> 8;
	buff[11] = MQTT_KeepAlive;
	len = strlen(MQTT_ClientIdentifier);
	buff[12] = len >> 8;
	buff[13] = len;
	msg = MQTT_ClientIdentifier;
	for(i = 0;i<len;i++)
	{
		buff[14+i] =  msg[i];
	}
	lennum += len;
	if(MQTT_StaWillFlag)
	{
		len = strlen(MQTT_WillTopic);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = MQTT_WillTopic;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
		len = strlen(MQTT_WillMessage);
		buff[12] = len >> 8;
		buff[13] = len;
		lennum += 2;
		msg = MQTT_WillMessage;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
	}
	if(MQTT_StaUserNameFlag)
	{
		len = strlen(MQTT_UserName);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = MQTT_UserName;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
		
	}
	if(MQTT_StaPasswordFlag)
	{
		len = strlen(MQTT_Password);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = MQTT_Password;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
	}
	buff[1] = 13 + lennum - 1;
}
void GetDataDisConnet(unsigned char *buff)//��ȡ�Ͽ����ӵ����ݰ�
{
	buff[0] = 0xe0;
	buff[1] = 0;
}
void GetDataPINGREQ(unsigned char *buff)//������������ݰ��ɹ�����d0 00
{
	buff[0] = 0xc0;
	buff[1] = 0;
}
/*
	�ɹ�����90 0x 00 Num RequestedQoS
*/
void GetDataSUBSCRIBE(unsigned char *buff,const char *dat,unsigned int Num,unsigned char RequestedQoS)//������������ݰ� Num:������� RequestedQoS:��������Ҫ��0,1��2
{
	unsigned int i,len = 0,lennum = 0; 
	buff[0] = 0x82;
	len = strlen(dat);
	buff[2] = Num>>8;
	buff[3] = Num;
	buff[4] = len>>8;
	buff[5] = len;
	for(i = 0;i<len;i++)
	{
		buff[6+i] = dat[i];
	}
	lennum = len;
	buff[6 + lennum ] = RequestedQoS;
	buff[1] = lennum + 5;
}
unsigned int GetDataPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg)//��ȡ������Ϣ�����ݰ�
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
	for(i = 0;i<len;i++)
	{
		buff[4+i+lennum] = msg[i];
	}
	lennum += len;
	buff[1] = lennum + 2;

  return lennum+4;
}

unsigned int GetDataONENETPUBLISH(unsigned char *buff,unsigned char onenetty,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg)//��ȡ������Ϣ�����ݰ�
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




