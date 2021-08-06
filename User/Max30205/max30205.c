#include "max30205.h"
#include "maxiic.h"
//float temperature=0;
void max_init (void)
{
   MAXIIC_Init();
   I2CwriteByte(MAX30205_ADDRESS, MAX30205_CONFIGURATION, 0x00);
     //I2CwriteByte(MAX30205_ADDRESS, MAX30205_THYST ,        0x00);
  // I2CwriteByte(MAX30205_ADDRESS, MAX30205_TOS, 0x00);
}
//写入指定地址一个字节数据
void I2CwriteByte(u8 address,u8 subAddress,u8 data)  //
{
     //IIC_Start();                                       //产生开始信号
       I2Cwriteaddr(address|IIC_WRITE,subAddress);  //对器件地址写入命令信号 然后寄存器地址
       MAXIIC_Send_Byte(data);                               //对数据进行写进去
       MAXIIC_Wait_Ack();                                    //等待回应
       MAXIIC_Stop();                             //产生停止信号
}
//I2C读一个字节操作
u8 I2CreadByte(u8 address,u8 subAddress)
{   
       u8 data;
    // IIC_Start();                           //开始
       I2Cwriteaddr(address|IIC_WRITE, subAddress); //对器件地址发送写入命令 写入寄存器地址
			MAXIIC_Start();                                           //开始
       MAXIIC_Send_Byte(address|IIC_READ);                //发送读取命令
     
       MAXIIC_Wait_Ack();                                    //等待回应
       data=MAXIIC_Read_Byte(0);
       MAXIIC_Stop();                             //产生停止信号
       return data;      
}

void I2CreadBytes(u8 address,u8 subAddress,u8*dest,u8 count)
{
    int i;
      for(i=0;i<count;i++)
        {
            dest[ i]=I2CreadByte(address,subAddress);
            subAddress+=1;
        }

}


void I2Cwriteaddr(u8 address,u8 subAddress)//写地址和寄存器操作
{
     MAXIIC_Start();                         //产生开始条件
       MAXIIC_Send_Byte(address);              //写入器件地址
       MAXIIC_Wait_Ack();                      //等待回应
       MAXIIC_Send_Byte(subAddress);           //写入寄存器地址
       MAXIIC_Wait_Ack();                       //等待回应
       //IIC_Stop();                             //产生停止信号
}
int16_t Read_max16(u8 address,u8 subAddress)
{
            int16_t data;
            u8 instant;
        // IIC_Start();                           //开始
            I2Cwriteaddr(address|IIC_WRITE, subAddress); //对器件地址发送写入命令 写入寄存器地址
            MAXIIC_Start();                                           //开始
            MAXIIC_Send_Byte(address|IIC_READ);                //发送读取命令
         
            MAXIIC_Wait_Ack();                                    //等待回应
            data=MAXIIC_Read_Byte(1);//主机发送应答信号
            //IIC_Wait_Ack();
            instant = MAXIIC_Read_Byte(0);//主机发送非应答信号
            MAXIIC_Stop();                             //产生停止信号
            data = (data<<8)|instant;
            return data;   
}   

double GetTemperature(void)
{
   double temperature= 0.0;
   int16_t temp;
   temp = Read_max16(MAX30205_ADDRESS,MAX30205_TEMPERATURE);
   temperature = temp*0.00390625;
   return  temperature;
}
