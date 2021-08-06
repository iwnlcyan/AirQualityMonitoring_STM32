#ifndef __MAX30205_H
#define __MAX30205_H
#include "stm32f10x.h"

#define MAX30205_ADDRESS  0X90    //8bit address converted to 7bit
//一些寄存器地址
#define MAX30205_TEMPERATURE 0X00
#define MAX30205_CONFIGURATION 0X01
#define MAX30205_THYST         0X02
#define MAX30205_TOS           0X03
#define IIC_WRITE 0   //数据方向 写入
#define IIC_READ   1   //数据方向 读取
//extern float temperature;
void shutdown(void);
void max_init (void);
void I2CwriteByte(u8 address,u8 subAddress,u8 data);
void I2Cwriteaddr(u8 address,u8 subAddress);
u8 I2CreadByte(u8 address,u8 subAddress);
void I2CreadBytes(u8 address,u8 subAddress,u8*dest,u8 count);
int16_t Read_max16(u8 address,u8 subAddress);
double GetTemperature(void);
#endif

