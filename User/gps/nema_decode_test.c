/**
  ******************************************************************************
  * @file    nmea_decode_test.c
  * @author  fire
  * @version V1.0
  * @date    2016-07-xx
  * @brief   测试NEMA解码库
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火F103开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./gps/gps_config.h"
#include "nmea.h"
#include <string.h>
#include <math.h>

nmeaINFO info;          //GPS解码后得到的信息
double deg_lat = 0;//转换成[degree].[degree]格式的纬度
double deg_lon = 0;//转换成[degree].[degree]格式的经度	
	

nmea_msg gnssx;

unsigned char NMEA_Comma_Pos(unsigned char *buf,unsigned char cx)
{
    unsigned char *p=buf;
    while(cx)
    {
        if(*buf=='*'||*buf>'z')return 0XFF;//遇到'*'或者非法字符,则不存在第cx个逗号
        if(*buf==',')cx--;
        buf++;
    }
    return buf-p;
}

unsigned int NMEA_Pow(unsigned char m,unsigned char n)
{
    unsigned int result=1;
    while(n--)result*=m;
    return result;
}

double NMEA_Str2num(unsigned char *buf,unsigned char *dx)
{
    unsigned char *p=buf;
    long long ires=0,fres=0;
    int ilen=0,flen=0,i;
    unsigned char mask=0;
    unsigned char k_flag=0;
    double res;
    while(1)
    {
        if(*p=='-'){mask|=0X02;p++;}
        if(*p==','||(*p=='*')||(*p==';')||(*p=='\r')||(*p=='\n'))break;
        if(*p=='.'){mask|=0X01;p++;}
        else if(*p>'9'||(*p<'0'))
        {

            if(*p!='-')
            {
                k_flag++;
                p++;
            }
            continue;
        }
        if(mask&0X01)flen++;
        else ilen++;
        p++;
    }
    if(mask&0X02)buf++;
    for(i=0;i<k_flag;i++)
        buf++;
    for(i=0;i<ilen;i++)
    {
        ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
    }
    if(flen>12)flen=12;
    *dx=flen;
    for(i=0;i<flen;i++)
    {
        fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
    }
    res=(double)fres*pow(10,-flen);
    res=(double)ires+res;

    if(mask&0X02)res=-res;
    return res;
}

void NMEA_RMC_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	static uint8_t *p1,dx;			 
	static uint8_t posx;     
	uint32_t temp;
	double rs;
	uint32_t llh;
	
	p1=(uint8_t*)strstr((const char *)buf,"RMC,");
	if( p1 == NULL)
	{
		return;
	}
	
  posx=NMEA_Comma_Pos(p1,1);								//得到UTC时间
	if(posx!=0XFF)
	{
		rs = NMEA_Str2num(p1+posx,&dx);	 	//得到UTC时间,去掉ms
		gpsx->utc.hour= ((int)rs)/10000;
		gpsx->utc.min= (((int)rs)/100)%100;
		gpsx->utc.sec= ((int)rs)%100;	 
	}
	posx=NMEA_Comma_Pos(p1,2);							
	if(posx!=0XFF)gpsx->posstate=*(p1+posx);		//得到定位状态
	
	
	posx=NMEA_Comma_Pos(p1,3);								//得到纬度
	if(posx!=0XFF)
	{
		rs=NMEA_Str2num(p1+posx,&dx);
		
		llh = rs/100.0;	//得到°
		rs=(rs-llh*100.0)/60.0;				//得到'		 
		gpsx->lat = rs + llh;//转换为°			
	}
	posx=NMEA_Comma_Pos(p1,4);								//南纬还是北纬 
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);
	
 	posx=NMEA_Comma_Pos(p1,5);								//得到经度
	if(posx!=0XFF)
	{												  
		rs=NMEA_Str2num(p1+posx,&dx);

		llh = rs/100.0;	//得到°
		rs=(rs-llh*100.0)/60.0;				//得到'		 
		gpsx->lon = rs + llh;//转换为°	
	}
	posx=NMEA_Comma_Pos(p1,6);								//东经还是西经
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);	

	posx=NMEA_Comma_Pos(p1,7);					//得到速度，KM/H
	if(posx!=0XFF)
	{	
		rs=NMEA_Str2num(p1+posx,&dx);
		gpsx->speed=(rs*1.852)/3.6;
	}

	posx=NMEA_Comma_Pos(p1,9);								//得到UTC日期
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//得到UTC日期
		gpsx->utc.date=temp/10000;
		gpsx->utc.month=(temp/100)%100;
		gpsx->utc.year=2000+temp%100;	  
	} 
	
	posx=NMEA_Comma_Pos(p1,12);								
	if(posx!=0XFF)gpsx->posmode=*(p1+posx);		
}

void NMEA_GGA_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	static uint8_t *p1,dx;			 
	static uint8_t posx;     
	double rs;
	
	p1=(uint8_t*)strstr((const char *)buf,"GGA,");
	if( p1 == NULL)
	{
		return;
	}
	
	posx=NMEA_Comma_Pos(p1,6);								//得到GPS状态
	if(posx!=0XFF) 
	{
		gpsx->gpssta=*(p1+posx);
	}
	
	posx=NMEA_Comma_Pos(p1,7);                 //使用中的卫星数
	if(posx!=0XFF)
		gpsx->gnssvnum=NMEA_Str2num(p1+posx,&dx);
	
	posx=NMEA_Comma_Pos(p1,8);                 //HDOP
	if(posx!=0XFF)
		gpsx->hdop=NMEA_Str2num(p1+posx,&dx);
	
	posx=NMEA_Comma_Pos(p1,9);								//得到高度
	if(posx!=0XFF)
	{
		rs=NMEA_Str2num(p1+posx,&dx);	
		gpsx->altitude=rs;
	}
	posx=NMEA_Comma_Pos(p1,11);								//得到高程异常值
	if(posx!=0XFF)
	{
		rs=NMEA_Str2num(p1+posx,&dx);	
		gpsx->geoidal=rs;
	}
}

void NMEA_ATT_Analysis(nmea_msg *gpsx,uint8_t *buf)
{
	static uint8_t *p1,dx;			 
	static uint8_t posx;     
	double rs;
	
	p1=(uint8_t*)strstr((const char *)buf,"ATT,");
	if( p1 == NULL)
	{
		return;
	}
	
	posx=NMEA_Comma_Pos(p1,11);								//得到INS STA
	if(posx!=0XFF)
	{
		rs=NMEA_Str2num(p1+posx,&dx);	
		gpsx->insmode=rs;
	}
}

void NMEA_Analysis(nmea_msg *gnssx,u8 *buf)
{
	NMEA_RMC_Analysis(gnssx,buf);
	NMEA_GGA_Analysis(gnssx,buf);
//	NMEA_ATT_Analysis(gnssx,buf);
}
	
	
#ifdef __GPS_LOG_FILE             //对SD卡上的gpslog.txt文件进行解码；（需要在sd卡上存放gpslog.txt文件）

FATFS fs;
FIL log_file;
FRESULT res; 
UINT br, bw;            					/* File R/W count */

/**
  * @brief  nmea_decode_test 解码GPS文件信息
  * @param  无
  * @retval 无
  */
void nmea_decode_test(void)
{
		double deg_lat;//转换成[degree].[degree]格式的纬度
		double deg_lon;//转换成[degree].[degree]格式的经度

    nmeaINFO info;          //GPS解码后得到的信息
    nmeaPARSER parser;      //解码时使用的数据结构  
    
    nmeaTIME beiJingTime;    //北京时间 

    char buff[2048];
  
    	/* 注册盘符 */
    res = f_mount(&fs,"0:",1);	
  
    if(res != FR_OK)
    {
      printf("\r\n！！SD卡挂载文件系统失败。(%d)，请给开发板接入SD卡\r\n",res);
      while(1);
    }

    /* 打开记录有GPS信息的文件 */

		res = f_open(&log_file,"0:gpslog.txt", FA_OPEN_EXISTING|FA_READ);

    if(!(res == FR_OK))
    {
        printf("\r\n打开gpslog.txt文件失败，请检查SD卡的根目录是否存放了gpslog.txt文件!\r\n");
        return ;      
    }
    

    /* 设置用于输出调试信息的函数 */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;
    nmea_property()->info_func = &gps_info;
		
    /* 初始化GPS数据结构 */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

    while(!f_eof(&log_file))
    {
      
        f_read(&log_file, &buff[0], 100, &br);

        /* 进行nmea格式解码 */
        nmea_parse(&parser, &buff[0], br, &info);
      
        /* 对解码后的时间进行转换，转换成北京时间 */
        GMTconvert(&info.utc,&beiJingTime,8,1);
        
        /* 输出解码得到的信息 */
				printf("\r\n时间%d-%02d-%02d,%d:%d:%d\r\n", beiJingTime.year+1900, beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
        
				//info.lat lon中的格式为[degree][min].[sec/60]，使用以下函数转换成[degree].[degree]格式
				deg_lat = nmea_ndeg2degree(info.lat);
				deg_lon = nmea_ndeg2degree(info.lon);
			
				printf("\r\n纬度：%f,经度%f\r\n",deg_lat,deg_lon);
        printf("\r\n海拔高度：%f 米 ", info.elv);
        printf("\r\n速度：%f km/h ", info.speed);
        printf("\r\n航向：%f 度", info.direction);
				
				printf("\r\n正在使用的GPS卫星：%d,可见GPS卫星：%d",info.satinfo.inuse,info.satinfo.inview);

				printf("\r\n正在使用的北斗卫星：%d,可见北斗卫星：%d",info.BDsatinfo.inuse,info.BDsatinfo.inview);
				printf("\r\nPDOP：%f,HDOP：%f，VDOP：%f",info.PDOP,info.HDOP,info.VDOP);

	
	}

    f_lseek(&log_file, f_size(&log_file));

    /* 释放GPS数据结构 */
    nmea_parser_destroy(&parser);
  
    /* 关闭文件 */
    f_close(&log_file);
    
   
}
#else       //对GPS模块传回的信息进行解码

/**
  * @brief  nmea_decode_test 解码GPS模块信息
  * @param  无
  * @retval 无
  */
//int nmea_decode_test(void)
//{
//		OS_ERR      err;
//	
//    nmeaPARSER parser;      //解码时使用的数据结构  
//    uint8_t new_parse=0;    //是否有新的解码数据标志
//  
//    nmeaTIME beiJingTime;    //北京时间 

//    /* 设置用于输出调试信息的函数 */
//    nmea_property()->trace_func = &trace;
//    nmea_property()->error_func = &error;
//    nmea_property()->info_func = &gps_info;

//    /* 初始化GPS数据结构 */
//    nmea_zero_INFO(&info);
//    nmea_parser_init(&parser);

//    while(1)
//    {
//      if(GPS_HalfTransferEnd)     /* 接收到GPS_RBUFF_SIZE一半的数据 */
//      {
//        /* 进行nmea格式解码 */
//        nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);
//        
//        GPS_HalfTransferEnd = 0;   //清空标志位
//        new_parse = 1;             //设置解码消息标志 
//      }
//      else if(GPS_TransferEnd)    /* 接收到另一半数据 */
//      {

//        nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
//       
//        GPS_TransferEnd = 0;
//        new_parse =1;
//      }
//      
//      if(new_parse )                //有新的解码消息   
//      {    
//        /* 对解码后的时间进行转换，转换成北京时间 */
//        GMTconvert(&info.utc,&beiJingTime,8,1);
//        
//        /* 输出解码得到的信息 */
//				printf("\r\n时间%d-%02d-%02d,%d:%d:%d\r\n", beiJingTime.year+1900, beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
//				
//				//info.lat lon中的格式为[degree][min].[sec/60]，使用以下函数转换成[degree].[degree]格式
//				deg_lat = nmea_ndeg2degree(info.lat);
//				deg_lon = nmea_ndeg2degree(info.lon);
//				
//				printf("\r\n纬度：%f,经度%f\r\n",deg_lat,deg_lon);
//        printf("\r\n海拔高度：%f 米 ", info.elv);
//        printf("\r\n速度：%f km/h ", info.speed);
//        printf("\r\n航向：%f 度", info.direction);
//				
//				printf("\r\n正在使用的GPS卫星：%d,可见GPS卫星：%d",info.satinfo.inuse,info.satinfo.inview);

//				printf("\r\n正在使用的北斗卫星：%d,可见北斗卫星：%d",info.BDsatinfo.inuse,info.BDsatinfo.inview);
//				printf("\r\nPDOP：%f,HDOP：%f，VDOP：%f",info.PDOP,info.HDOP,info.VDOP);
//        
//        new_parse = 0;
//      }
//		OSTimeDly ( 30, OS_OPT_TIME_DLY, & err );
//	}

//    /* 释放GPS数据结构 */
//    // nmea_parser_destroy(&parser);

//    
//    //  return 0;
//}

#endif






/**************************************************end of file****************************************/

