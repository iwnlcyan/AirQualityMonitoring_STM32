#include "maxiic.h"
#include "./dwt_delay/core_delay.h" 

#define delay_us(us)  CPU_TS_Tmr_Delay_US(us)
#define delay_ms(ms)  CPU_TS_Tmr_Delay_MS(ms)
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

//初始化IIC
void MAXIIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC->APB2ENR|=1<<4;//先使能外设IO PORTC时钟 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
 
	MAXIIC_SCL=1;
	MAXIIC_SDA=1;

}
//产生IIC起始信号
void MAXIIC_Start(void)
{
	MAXSDA_OUT();     //sda线输出
	MAXIIC_SDA=1;	  	  
	MAXIIC_SCL=1;
	delay_us(4);
 	MAXIIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	MAXIIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void MAXIIC_Stop(void)
{
	MAXSDA_OUT();//sda线输出
	MAXIIC_SCL=0;
	MAXIIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	MAXIIC_SCL=1; 
	MAXIIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 MAXIIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	MAXSDA_IN();      //SDA设置为输入  
	MAXIIC_SDA=1;delay_us(1);	   
	MAXIIC_SCL=1;delay_us(1);	 
	while(MAXREAD_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			MAXIIC_Stop();
			return 1;
		}
	}
	MAXIIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void MAXIIC_Ack(void)
{
	MAXIIC_SCL=0;
	MAXSDA_OUT();
	MAXIIC_SDA=0;
	delay_us(2);
	MAXIIC_SCL=1;
	delay_us(2);
	MAXIIC_SCL=0;
}
//不产生ACK应答		    
void MAXIIC_NAck(void)
{
	MAXIIC_SCL=0;
	MAXSDA_OUT();
	MAXIIC_SDA=1;
	delay_us(2);
	MAXIIC_SCL=1;
	delay_us(2);
	MAXIIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void MAXIIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	MAXSDA_OUT(); 	    
    MAXIIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        MAXIIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		MAXIIC_SCL=1;
		delay_us(2); 
		MAXIIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 MAXIIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	MAXSDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        MAXIIC_SCL=0; 
        delay_us(2);
		MAXIIC_SCL=1;
        receive<<=1;
        if(MAXREAD_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        MAXIIC_NAck();//发送nACK
    else
        MAXIIC_Ack(); //发送ACK   
    return receive;
}

