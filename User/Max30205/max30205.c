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
//д��ָ����ַһ���ֽ�����
void I2CwriteByte(u8 address,u8 subAddress,u8 data)  //
{
     //IIC_Start();                                       //������ʼ�ź�
       I2Cwriteaddr(address|IIC_WRITE,subAddress);  //��������ַд�������ź� Ȼ��Ĵ�����ַ
       MAXIIC_Send_Byte(data);                               //�����ݽ���д��ȥ
       MAXIIC_Wait_Ack();                                    //�ȴ���Ӧ
       MAXIIC_Stop();                             //����ֹͣ�ź�
}
//I2C��һ���ֽڲ���
u8 I2CreadByte(u8 address,u8 subAddress)
{   
       u8 data;
    // IIC_Start();                           //��ʼ
       I2Cwriteaddr(address|IIC_WRITE, subAddress); //��������ַ����д������ д��Ĵ�����ַ
			MAXIIC_Start();                                           //��ʼ
       MAXIIC_Send_Byte(address|IIC_READ);                //���Ͷ�ȡ����
     
       MAXIIC_Wait_Ack();                                    //�ȴ���Ӧ
       data=MAXIIC_Read_Byte(0);
       MAXIIC_Stop();                             //����ֹͣ�ź�
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


void I2Cwriteaddr(u8 address,u8 subAddress)//д��ַ�ͼĴ�������
{
     MAXIIC_Start();                         //������ʼ����
       MAXIIC_Send_Byte(address);              //д��������ַ
       MAXIIC_Wait_Ack();                      //�ȴ���Ӧ
       MAXIIC_Send_Byte(subAddress);           //д��Ĵ�����ַ
       MAXIIC_Wait_Ack();                       //�ȴ���Ӧ
       //IIC_Stop();                             //����ֹͣ�ź�
}
int16_t Read_max16(u8 address,u8 subAddress)
{
            int16_t data;
            u8 instant;
        // IIC_Start();                           //��ʼ
            I2Cwriteaddr(address|IIC_WRITE, subAddress); //��������ַ����д������ д��Ĵ�����ַ
            MAXIIC_Start();                                           //��ʼ
            MAXIIC_Send_Byte(address|IIC_READ);                //���Ͷ�ȡ����
         
            MAXIIC_Wait_Ack();                                    //�ȴ���Ӧ
            data=MAXIIC_Read_Byte(1);//��������Ӧ���ź�
            //IIC_Wait_Ack();
            instant = MAXIIC_Read_Byte(0);//�������ͷ�Ӧ���ź�
            MAXIIC_Stop();                             //����ֹͣ�ź�
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
