/**
  ******************************************************************************
  * @file    gps_config.c
  * @author  fire
  * @version V1.0
  * @date    2016-07-xx
  * @brief   gps模块接口配置驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/

#include "./gps/gps_config.h"
#include "nmea.h"
#include "./usart/bsp_usart.h"


/* DMA接收缓冲  */
//uint8_t gps_rbuff[GPS_RBUFF_SIZE];
uint8_t gnss_rbuff[GPS_RBUFF_SIZE];
uint16_t gps_rblen = 0;

/* DMA传输结束标志 */
__IO uint8_t GPS_TransferEnd = 0, GPS_HalfTransferEnd = 0;

/**
  * @brief  GPS_Interrupt_Config 配置GPS使用的DMA中断 
  * @param  None.
  * @retval None.
  */
static void GPS_Interrupt_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	// DMA2 Channel Interrupt ENABLE
	NVIC_InitStructure.NVIC_IRQChannel = GPS_USART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

/*
 * 函数名：GPS_USART_Config
 * 描述  ：USART GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
static void GPS_USART_Config(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* config USART clock */
	RCC_APB2PeriphClockCmd(GPS_USART_RX_GPIO_CLK|GPS_USART_TX_GPIO_CLK, ENABLE);
	GPS_RCC_PeriphClockCmd_Fun(GPS_USART_CLK, ENABLE);

	/* USART GPIO config */
  /* Configure USART Tx  as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPS_USART_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPS_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	    
  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GPS_USART_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPS_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	  
	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = GPS_USART_BAUDRATE;                //GPS模块默认使用波特率：9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(GPS_USART, &USART_InitStructure); 

	GPS_Interrupt_Config();
	// 使能串口接收中断
	USART_ITConfig(GPS_USART, USART_IT_RXNE, ENABLE);

	USART_Cmd(GPS_USART, ENABLE);	
	
}

void Gnss_Rev_Analysis(void)
{
	static uint16_t Oldlen=0;
	
		if(Oldlen == gps_rblen &&  gps_rblen > 0)
		{
			
			NMEA_Analysis(&gnssx,gnss_rbuff);
			
			//printf("%s",gnss_rbuff);
			
			gps_rblen=0;
			gnss_rbuff[0] = 0;
		}
		Oldlen = gps_rblen;
}

/**
  * @brief  GPS_Config gps 初始化
  * @param  无
  * @retval 无
  */
void GPS_Config(void)
{
  GPS_USART_Config();
  //GPS_DMA_Config();  
  
}


/**
  * @brief  GPS_ProcessDMAIRQ GPS DMA中断服务函数
  * @param  None.
  * @retval None.
  */
void GPS_ProcessDMAIRQ(void)
{  
  if(DMA_GetITStatus(GPS_DMA_IT_HT) )         /* DMA 半传输完成 */
  {
    GPS_HalfTransferEnd = 1;                //设置半传输完成标志位
    DMA_ClearFlag(GPS_DMA_FLAG_HT);
  }
  else if(DMA_GetITStatus(GPS_DMA_IT_TC))     /* DMA 传输完成 */
  {
    GPS_TransferEnd = 1;                    //设置传输完成标志位
    DMA_ClearFlag(GPS_DMA_FLAG_TC);

   }
}



/**
  * @brief  trace 在解码时输出捕获的GPS语句
  * @param  str: 要输出的字符串，str_size:数据长度
  * @retval 无
  */
void trace(const char *str, int str_size)
{
  #ifdef __GPS_DEBUG    //在gps_config.h文件配置这个宏，是否输出调试信息
    uint16_t i;
    printf("\r\nTrace: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
  
    printf("\n");
  #endif
}

/**
  * @brief  error 在解码出错时输出提示消息
  * @param  str: 要输出的字符串，str_size:数据长度
  * @retval 无
  */
void error(const char *str, int str_size)
{
    #ifdef __GPS_DEBUG   //在gps_config.h文件配置这个宏，是否输出调试信息

    uint16_t i;
    printf("\r\nError: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
    printf("\n");
    #endif
}

/**
  * @brief  error 在解码出错时输出提示消息
  * @param  str: 要输出的字符串，str_size:数据长度
  * @retval 无
  */
void gps_info(const char *str, int str_size)
{

    uint16_t i;
    printf("\r\nInfo: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
    printf("\n");
}



/******************************************************************************************************** 
**     函数名称:            bit        IsLeapYear(uint8_t    iYear) 
**    功能描述:            判断闰年(仅针对于2000以后的年份) 
**    入口参数：            iYear    两位年数 
**    出口参数:            uint8_t        1:为闰年    0:为平年 
********************************************************************************************************/ 
static uint8_t IsLeapYear(uint8_t iYear) 
{ 
    uint16_t    Year; 
    Year    =    2000+iYear; 
    if((Year&3)==0) 
    { 
        return ((Year%400==0) || (Year%100!=0)); 
    } 
     return 0; 
} 

/******************************************************************************************************** 
**     函数名称:            void    GMTconvert(uint8_t *DT,uint8_t GMT,uint8_t AREA) 
**    功能描述:            格林尼治时间换算世界各时区时间 
**    入口参数：            *DT:    表示日期时间的数组 格式 YY,MM,DD,HH,MM,SS 
**                        GMT:    时区数 
**                        AREA:    1(+)东区 W0(-)西区 
********************************************************************************************************/ 
void    GMTconvert(nmeaTIME *SourceTime, nmeaTIME *ConvertTime, uint8_t GMT,uint8_t AREA) 
{ 
    uint32_t    YY,MM,DD,hh,mm,ss;        //年月日时分秒暂存变量 
     
    if(GMT==0)    return;                //如果处于0时区直接返回 
    if(GMT>12)    return;                //时区最大为12 超过则返回         

    YY    =    SourceTime->year;                //获取年 
    MM    =    SourceTime->mon;                 //获取月 
    DD    =    SourceTime->day;                 //获取日 
    hh    =    SourceTime->hour;                //获取时 
    mm    =    SourceTime->min;                 //获取分 
    ss    =    SourceTime->sec;                 //获取秒 

    if(AREA)                        //东(+)时区处理 
    { 
        if(hh+GMT<24)    hh    +=    GMT;//如果与格林尼治时间处于同一天则仅加小时即可 
        else                        //如果已经晚于格林尼治时间1天则进行日期处理 
        { 
            hh    =    hh+GMT-24;        //先得出时间 
            if(MM==1 || MM==3 || MM==5 || MM==7 || MM==8 || MM==10)    //大月份(12月单独处理) 
            { 
                if(DD<31)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==4 || MM==6 || MM==9 || MM==11)                //小月份2月单独处理) 
            { 
                if(DD<30)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==2)    //处理2月份 
            { 
                if((DD==29) || (DD==28 && IsLeapYear(YY)==0))        //本来是闰年且是2月29日 或者不是闰年且是2月28日 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
                else    DD++; 
            } 
            else if(MM==12)    //处理12月份 
            { 
                if(DD<31)    DD++; 
                else        //跨年最后一天 
                {               
                    DD    =    1; 
                    MM    =    1; 
                    YY    ++; 
                } 
            } 
        } 
    } 
    else 
    {     
        if(hh>=GMT)    hh    -=    GMT;    //如果与格林尼治时间处于同一天则仅减小时即可 
        else                        //如果已经早于格林尼治时间1天则进行日期处理 
        { 
            hh    =    hh+24-GMT;        //先得出时间 
            if(MM==2 || MM==4 || MM==6 || MM==8 || MM==9 || MM==11)    //上月是大月份(1月单独处理) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    31; 
                    MM    --; 
                } 
            } 
            else if(MM==5 || MM==7 || MM==10 || MM==12)                //上月是小月份2月单独处理) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    30; 
                    MM    --; 
                } 
            } 
            else if(MM==3)    //处理上个月是2月份 
            { 
                if((DD==1) && IsLeapYear(YY)==0)                    //不是闰年 
                { 
                    DD    =    28; 
                    MM    --; 
                } 
                else    DD--; 
            } 
            else if(MM==1)    //处理1月份 
            { 
                if(DD>1)    DD--; 
                else        //新年第一天 
                {               
                    DD    =    31; 
                    MM    =    12; 
                    YY    --; 
                } 
            } 
        } 
    }         

    ConvertTime->year   =    YY;                //更新年 
    ConvertTime->mon    =    MM;                //更新月 
    ConvertTime->day    =    DD;                //更新日 
    ConvertTime->hour   =    hh;                //更新时 
    ConvertTime->min    =    mm;                //更新分 
    ConvertTime->sec    =    ss;                //更新秒 
}  






/*********************************************************end of file**************************************************/
