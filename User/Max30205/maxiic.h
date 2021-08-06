#ifndef __MYIIC_H
#define __MYIIC_H
#include "stm32f10x.h"
#include "bsp_led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/10 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

   	   		   
//IO��������
#define MAXSDA_IN()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
#define MAXSDA_OUT() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}

//IO��������	 
#define MAXIIC_SCL    PCout(12) //SCL
#define MAXIIC_SDA    PCout(11) //SDA	 
#define MAXREAD_SDA   PCin(11)  //����SDA 

//IIC���в�������
void MAXIIC_Init(void);                //��ʼ��IIC��IO��				 
void MAXIIC_Start(void);				//����IIC��ʼ�ź�
void MAXIIC_Stop(void);	  			//����IICֹͣ�ź�
void MAXIIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 MAXIIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 MAXIIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void MAXIIC_Ack(void);					//IIC����ACK�ź�
void MAXIIC_NAck(void);				//IIC������ACK�ź�

void MAXIIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 MAXIIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















