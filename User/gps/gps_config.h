#ifndef __GPS_CONFIG_H
#define	__GPS_CONFIG_H

#include "stm32f10x.h"
#include "nmea.h"

extern nmeaINFO info;          //GPS解码后得到的信息
extern double deg_lat;//转换成[degree].[degree]格式的纬度
extern double deg_lon;//转换成[degree].[degree]格式的经度
/* 配置 */

//#define __GPS_DEBUG     //定义这个宏，在nmea解码时会输出调试信息

//#define __GPS_LOG_FILE    //定义这个宏，对SD卡上的gpslog.txt文件进行解码；不定义的话使用串口接收GPS信息解码


/* GPS接口配置 使用不同的串口时，要修改对应的接口 */

#define GPS_DR_Base            (USART2_BASE+0x04)		  // 串口的数据寄存器地址


#define GPS_DATA_ADDR             GPS_DR_Base        //GPS使用的串口的数据寄存器地址
#define GPS_RBUFF_SIZE            1200                   //串口接收缓冲区大小
#define HALF_GPS_RBUFF_SIZE       (GPS_RBUFF_SIZE/2)    //串口接收缓冲区一半  

extern uint8_t gnss_rbuff[GPS_RBUFF_SIZE];
extern uint16_t gps_rblen;
/* 外设 */
/*******************************************************/
//GPS串口波特率
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
#define GPS_DMA_IRQn                 DMA1_Channel6_IRQn         //GPS中断源

/* 外设标志 */
#define GPS_DMA_FLAG_TC              DMA1_FLAG_TC6
#define GPS_DMA_FLAG_TE              DMA1_FLAG_TE6
#define GPS_DMA_FLAG_HT              DMA1_FLAG_HT6
#define GPS_DMA_FLAG_GL              DMA1_FLAG_GL6
#define GPS_DMA_IT_HT                DMA1_IT_HT6
#define GPS_DMA_IT_TC                DMA1_IT_TC6


/* 中断函数 */
#define GPS_DMA_IRQHANDLER           DMA1_Channel6_IRQHandler   //GPS使用的DMA中断服务函数

__packed typedef struct  
{										    
 	volatile uint16_t year;	//年份
	volatile uint16_t month;	//月份
	volatile uint16_t date;	//日期
	volatile uint16_t hour; 	//小时
	volatile uint16_t min; 	//分钟
	volatile uint16_t sec; 	//秒钟
}nmea_utc;   	

__packed typedef struct  
{	
	nmea_utc utc;			//UTC时间
 	uint8_t gnssvnum;	//可见卫星数
	uint8_t ewhemi;		//东经/西经,E:东经;W:西经
	uint8_t nshemi;		//北纬/南纬,N:北纬;S:南纬	
	uint8_t gpssta;		//GPS状态:1,单点定位;2,伪距差分定位;4,RTK固定解，差分定位;5,RTK浮点解，7，用户设定位置				  
	uint8_t fixmode;	//定位类型:1,没有定位;2,2D定位;3,3D定位
	float pdop;				//位置精度因子 0~500,对应实际值0~50.0
	float hdop;				//水平精度因子 0~500,对应实际值0~50.0
	float vdop;				//垂直精度因子 0~500,对应实际值0~50.0 
	double  lat;      //纬度
	double  lon;      //经度
	uint8_t posstate;        //定位有效/无效
	float altitude;			 	//海拔高度,单位:0.1m	
	float geoidal;			 		//高程异常值
	float speed;					//地面速率	 
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
