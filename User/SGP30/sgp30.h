#ifndef __HX711_H
#define __HX711_H
#include "stm32f10x.h"

//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

#define  SGP30_SCL   PBout(0)
#define  SGP30_SDA   PBout(1)


// USART GPIO ���ź궨��
#define  SGP30_SCL_GPIO_CLK        RCC_APB2Periph_GPIOB
#define  SGP30_SCL_GPIO_PORT       GPIOB
#define  SGP30_SCL_GPIO_PIN        GPIO_Pin_0

#define  SGP30_SDA_GPIO_SDA        RCC_APB2Periph_GPIOB
#define  SGP30_SDA_GPIO_PORT       GPIOB
#define  SGP30_SDA_GPIO_PIN        GPIO_Pin_1

#define  SGP30_SDA_READ()           GPIO_ReadInputDataBit(SGP30_SDA_GPIO_PORT, SGP30_SDA_GPIO_PIN)

#define SGP30_read  0xb1  //SGP30�Ķ���ַ
#define SGP30_write 0xb0  //SGP30��д��ַ


void SGP30_IIC_Start(void);				//����IIC��ʼ�ź�
void SGP30_IIC_Stop(void);	  			//����IICֹͣ�ź�
void SGP30_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u16 SGP30_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 SGP30_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void SGP30_IIC_Ack(void);					//IIC����ACK�ź�
void SGP30_IIC_NAck(void);				//IIC������ACK�ź�
void SGP30_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 SGP30_IIC_Read_One_Byte(u8 daddr,u8 addr);	
void SGP30_Init(void);				  
void SGP30_Write(u8 a, u8 b);
u32 SGP30_Read(void);



#endif

