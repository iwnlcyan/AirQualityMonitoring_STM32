#ifndef __MYIIC_H
#define __MYIIC_H
#include "stm32f10x.h"
#include "bsp_led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//IIC 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/10 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

   	   		   
//IO方向设置
#define MAXSDA_IN()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
#define MAXSDA_OUT() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}

//IO操作函数	 
#define MAXIIC_SCL    PCout(12) //SCL
#define MAXIIC_SDA    PCout(11) //SDA	 
#define MAXREAD_SDA   PCin(11)  //输入SDA 

//IIC所有操作函数
void MAXIIC_Init(void);                //初始化IIC的IO口				 
void MAXIIC_Start(void);				//发送IIC开始信号
void MAXIIC_Stop(void);	  			//发送IIC停止信号
void MAXIIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 MAXIIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 MAXIIC_Wait_Ack(void); 				//IIC等待ACK信号
void MAXIIC_Ack(void);					//IIC发送ACK信号
void MAXIIC_NAck(void);				//IIC不发送ACK信号

void MAXIIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 MAXIIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















