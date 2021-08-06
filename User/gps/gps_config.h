#ifndef __GPS_CONFIG_H
#define	__GPS_CONFIG_H

#include "stm32f10x.h"
#include "nmea.h"

extern nmeaINFO info;          //GPS�����õ�����Ϣ
extern double deg_lat;//ת����[degree].[degree]��ʽ��γ��
extern double deg_lon;//ת����[degree].[degree]��ʽ�ľ���
/* ���� */

//#define __GPS_DEBUG     //��������꣬��nmea����ʱ�����������Ϣ

//#define __GPS_LOG_FILE    //��������꣬��SD���ϵ�gpslog.txt�ļ����н��룻������Ļ�ʹ�ô��ڽ���GPS��Ϣ����


/* GPS�ӿ����� ʹ�ò�ͬ�Ĵ���ʱ��Ҫ�޸Ķ�Ӧ�Ľӿ� */

#define GPS_DR_Base            (USART2_BASE+0x04)		  // ���ڵ����ݼĴ�����ַ


#define GPS_DATA_ADDR             GPS_DR_Base        //GPSʹ�õĴ��ڵ����ݼĴ�����ַ
#define GPS_RBUFF_SIZE            1200                   //���ڽ��ջ�������С
#define HALF_GPS_RBUFF_SIZE       (GPS_RBUFF_SIZE/2)    //���ڽ��ջ�����һ��  

extern uint8_t gnss_rbuff[GPS_RBUFF_SIZE];
extern uint16_t gps_rblen;
/* ���� */
/*******************************************************/
//GPS���ڲ�����
#define GPS_USART_BAUDRATE                    9600

#define GPS_USART                             USART2
#define GPS_USART_CLK                         RCC_APB1Periph_USART2
#define GPS_RCC_PeriphClockCmd_Fun          RCC_APB1PeriphClockCmd

#define GPS_USART_RX_GPIO_PORT                GPIOA
#define GPS_USART_RX_GPIO_CLK                 RCC_APB2Periph_GPIOA
#define GPS_USART_RX_PIN                      GPIO_Pin_3


#define GPS_USART_TX_GPIO_PORT                GPIOA
#define GPS_USART_TX_GPIO_CLK                 RCC_APB2Periph_GPIOA
#define GPS_USART_TX_PIN                      GPIO_Pin_2

#define  GPS_USART_IRQ                				USART2_IRQn
#define  GPS_USART_IRQHandler         				USART2_IRQHandler

/************************************************************/
#define GPS_DMA                      DMA1
#define GPS_DMA_CLK                  RCC_AHBPeriph_DMA1
#define GPS_DMA_CHANNEL              DMA1_Channel6
#define GPS_DMA_IRQn                 DMA1_Channel6_IRQn         //GPS�ж�Դ

/* �����־ */
#define GPS_DMA_FLAG_TC              DMA1_FLAG_TC6
#define GPS_DMA_FLAG_TE              DMA1_FLAG_TE6
#define GPS_DMA_FLAG_HT              DMA1_FLAG_HT6
#define GPS_DMA_FLAG_GL              DMA1_FLAG_GL6
#define GPS_DMA_IT_HT                DMA1_IT_HT6
#define GPS_DMA_IT_TC                DMA1_IT_TC6


/* �жϺ��� */
#define GPS_DMA_IRQHANDLER           DMA1_Channel6_IRQHandler   //GPSʹ�õ�DMA�жϷ�����

__packed typedef struct  
{										    
 	volatile uint16_t year;	//���
	volatile uint16_t month;	//�·�
	volatile uint16_t date;	//����
	volatile uint16_t hour; 	//Сʱ
	volatile uint16_t min; 	//����
	volatile uint16_t sec; 	//����
}nmea_utc;   	

__packed typedef struct  
{	
	nmea_utc utc;			//UTCʱ��
 	uint8_t gnssvnum;	//�ɼ�������
	uint8_t ewhemi;		//����/����,E:����;W:����
	uint8_t nshemi;		//��γ/��γ,N:��γ;S:��γ	
	uint8_t gpssta;		//GPS״̬:1,���㶨λ;2,α���ֶ�λ;4,RTK�̶��⣬��ֶ�λ;5,RTK����⣬7���û��趨λ��				  
	uint8_t fixmode;	//��λ����:1,û�ж�λ;2,2D��λ;3,3D��λ
	float pdop;				//λ�þ������� 0~500,��Ӧʵ��ֵ0~50.0
	float hdop;				//ˮƽ�������� 0~500,��Ӧʵ��ֵ0~50.0
	float vdop;				//��ֱ�������� 0~500,��Ӧʵ��ֵ0~50.0 
	double  lat;      //γ��
	double  lon;      //����
	uint8_t posstate;        //��λ��Ч/��Ч
	float altitude;			 	//���θ߶�,��λ:0.1m	
	float geoidal;			 		//�߳��쳣ֵ
	float speed;					//��������	 
	uint8_t posmode;
	uint8_t insmode;
	uint8_t upflag;
}nmea_msg; 

extern nmea_msg gnssx;

unsigned char NMEA_Comma_Pos(unsigned char *buf,unsigned char cx);
unsigned int NMEA_Pow(unsigned char m,unsigned char n);
double NMEA_Str2num(unsigned char *buf,unsigned char *dx);

void NMEA_Analysis(nmea_msg *gnssx,u8 *buf);


extern uint8_t gps_rbuff[GPS_RBUFF_SIZE];
extern __IO uint8_t GPS_TransferEnd ;
extern __IO uint8_t GPS_HalfTransferEnd;


void GPS_ProcessDMAIRQ(void);
void GPS_Config(void);

void trace(const char *str, int str_size);
void error(const char *str, int str_size);
void gps_info(const char *str, int str_size);
void GMTconvert(nmeaTIME *SourceTime, nmeaTIME *ConvertTime, uint8_t GMT,uint8_t AREA) ;

int nmea_decode_test(void);

void Gnss_Rev_Analysis(void);

#endif 
