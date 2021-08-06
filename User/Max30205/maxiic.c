#include "maxiic.h"
#include "./dwt_delay/core_delay.h" 

#define delay_us(us)  CPU_TS_Tmr_Delay_US(us)
#define delay_ms(ms)  CPU_TS_Tmr_Delay_MS(ms)
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

//��ʼ��IIC
void MAXIIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC->APB2ENR|=1<<4;//��ʹ������IO PORTCʱ�� 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
 
	MAXIIC_SCL=1;
	MAXIIC_SDA=1;

}
//����IIC��ʼ�ź�
void MAXIIC_Start(void)
{
	MAXSDA_OUT();     //sda�����
	MAXIIC_SDA=1;	  	  
	MAXIIC_SCL=1;
	delay_us(4);
 	MAXIIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	MAXIIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void MAXIIC_Stop(void)
{
	MAXSDA_OUT();//sda�����
	MAXIIC_SCL=0;
	MAXIIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	MAXIIC_SCL=1; 
	MAXIIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 MAXIIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	MAXSDA_IN();      //SDA����Ϊ����  
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
	MAXIIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void MAXIIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	MAXSDA_OUT(); 	    
    MAXIIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        MAXIIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		MAXIIC_SCL=1;
		delay_us(2); 
		MAXIIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 MAXIIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	MAXSDA_IN();//SDA����Ϊ����
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
        MAXIIC_NAck();//����nACK
    else
        MAXIIC_Ack(); //����ACK   
    return receive;
}

