/**
  ******************************************************************************
  * @file    nmea_decode_test.c
  * @author  fire
  * @version V1.0
  * @date    2016-07-xx
  * @brief   ����NEMA�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��F103������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./gps/gps_config.h"
#include "nmea.h"
#include <string.h>
#include <math.h>

nmeaINFO info;          //GPS�����õ�����Ϣ
double deg_lat = 0;//ת����[degree].[degree]��ʽ��γ��
double deg_lon = 0;//ת����[degree].[degree]��ʽ�ľ���	
	

nmea_msg gnssx;

unsigned char NMEA_Comma_Pos(unsigned char *buf,unsigned char cx)
{
    unsigned char *p=buf;
    while(cx)
    {
        if(*buf=='*'||*buf>'z')return 0XFF;//����'*'���߷Ƿ��ַ�,�򲻴��ڵ�cx������
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
	
  posx=NMEA_Comma_Pos(p1,1);								//�õ�UTCʱ��
	if(posx!=0XFF)
	{
		rs = NMEA_Str2num(p1+posx,&dx);	 	//�õ�UTCʱ��,ȥ��ms
		gpsx->utc.hour= ((int)rs)/10000;
		gpsx->utc.min= (((int)rs)/100)%100;
		gpsx->utc.sec= ((int)rs)%100;	 
	}
	posx=NMEA_Comma_Pos(p1,2);							
	if(posx!=0XFF)gpsx->posstate=*(p1+posx);		//�õ���λ״̬
	
	
	posx=NMEA_Comma_Pos(p1,3);								//�õ�γ��
	if(posx!=0XFF)
	{
		rs=NMEA_Str2num(p1+posx,&dx);
		
		llh = rs/100.0;	//�õ���
		rs=(rs-llh*100.0)/60.0;				//�õ�'		 
		gpsx->lat = rs + llh;//ת��Ϊ��			
	}
	posx=NMEA_Comma_Pos(p1,4);								//��γ���Ǳ�γ 
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);
	
 	posx=NMEA_Comma_Pos(p1,5);								//�õ�����
	if(posx!=0XFF)
	{												  
		rs=NMEA_Str2num(p1+posx,&dx);

		llh = rs/100.0;	//�õ���
		rs=(rs-llh*100.0)/60.0;				//�õ�'		 
		gpsx->lon = rs + llh;//ת��Ϊ��	
	}
	posx=NMEA_Comma_Pos(p1,6);								//������������
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);	

	posx=NMEA_Comma_Pos(p1,7);					//�õ��ٶȣ�KM/H
	if(posx!=0XFF)
	{	
		rs=NMEA_Str2num(p1+posx,&dx);
		gpsx->speed=(rs*1.852)/3.6;
	}

	posx=NMEA_Comma_Pos(p1,9);								//�õ�UTC����
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//�õ�UTC����
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
	
	posx=NMEA_Comma_Pos(p1,6);								//�õ�GPS״̬
	if(posx!=0XFF) 
	{
		gpsx->gpssta=*(p1+posx);
	}
	
	posx=NMEA_Comma_Pos(p1,7);                 //ʹ���е�������
	if(posx!=0XFF)
		gpsx->gnssvnum=NMEA_Str2num(p1+posx,&dx);
	
	posx=NMEA_Comma_Pos(p1,8);                 //HDOP
	if(posx!=0XFF)
		gpsx->hdop=NMEA_Str2num(p1+posx,&dx);
	
	posx=NMEA_Comma_Pos(p1,9);								//�õ��߶�
	if(posx!=0XFF)
	{
		rs=NMEA_Str2num(p1+posx,&dx);	
		gpsx->altitude=rs;
	}
	posx=NMEA_Comma_Pos(p1,11);								//�õ��߳��쳣ֵ
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
	
	posx=NMEA_Comma_Pos(p1,11);								//�õ�INS STA
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
	
	
#ifdef __GPS_LOG_FILE             //��SD���ϵ�gpslog.txt�ļ����н��룻����Ҫ��sd���ϴ��gpslog.txt�ļ���

FATFS fs;
FIL log_file;
FRESULT res; 
UINT br, bw;            					/* File R/W count */

/**
  * @brief  nmea_decode_test ����GPS�ļ���Ϣ
  * @param  ��
  * @retval ��
  */
void nmea_decode_test(void)
{
		double deg_lat;//ת����[degree].[degree]��ʽ��γ��
		double deg_lon;//ת����[degree].[degree]��ʽ�ľ���

    nmeaINFO info;          //GPS�����õ�����Ϣ
    nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ  
    
    nmeaTIME beiJingTime;    //����ʱ�� 

    char buff[2048];
  
    	/* ע���̷� */
    res = f_mount(&fs,"0:",1);	
  
    if(res != FR_OK)
    {
      printf("\r\n����SD�������ļ�ϵͳʧ�ܡ�(%d)��������������SD��\r\n",res);
      while(1);
    }

    /* �򿪼�¼��GPS��Ϣ���ļ� */

		res = f_open(&log_file,"0:gpslog.txt", FA_OPEN_EXISTING|FA_READ);

    if(!(res == FR_OK))
    {
        printf("\r\n��gpslog.txt�ļ�ʧ�ܣ�����SD���ĸ�Ŀ¼�Ƿ�����gpslog.txt�ļ�!\r\n");
        return ;      
    }
    

    /* �����������������Ϣ�ĺ��� */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;
    nmea_property()->info_func = &gps_info;
		
    /* ��ʼ��GPS���ݽṹ */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

    while(!f_eof(&log_file))
    {
      
        f_read(&log_file, &buff[0], 100, &br);

        /* ����nmea��ʽ���� */
        nmea_parse(&parser, &buff[0], br, &info);
      
        /* �Խ�����ʱ�����ת����ת���ɱ���ʱ�� */
        GMTconvert(&info.utc,&beiJingTime,8,1);
        
        /* �������õ�����Ϣ */
				printf("\r\nʱ��%d-%02d-%02d,%d:%d:%d\r\n", beiJingTime.year+1900, beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
        
				//info.lat lon�еĸ�ʽΪ[degree][min].[sec/60]��ʹ�����º���ת����[degree].[degree]��ʽ
				deg_lat = nmea_ndeg2degree(info.lat);
				deg_lon = nmea_ndeg2degree(info.lon);
			
				printf("\r\nγ�ȣ�%f,����%f\r\n",deg_lat,deg_lon);
        printf("\r\n���θ߶ȣ�%f �� ", info.elv);
        printf("\r\n�ٶȣ�%f km/h ", info.speed);
        printf("\r\n����%f ��", info.direction);
				
				printf("\r\n����ʹ�õ�GPS���ǣ�%d,�ɼ�GPS���ǣ�%d",info.satinfo.inuse,info.satinfo.inview);

				printf("\r\n����ʹ�õı������ǣ�%d,�ɼ��������ǣ�%d",info.BDsatinfo.inuse,info.BDsatinfo.inview);
				printf("\r\nPDOP��%f,HDOP��%f��VDOP��%f",info.PDOP,info.HDOP,info.VDOP);

	
	}

    f_lseek(&log_file, f_size(&log_file));

    /* �ͷ�GPS���ݽṹ */
    nmea_parser_destroy(&parser);
  
    /* �ر��ļ� */
    f_close(&log_file);
    
   
}
#else       //��GPSģ�鴫�ص���Ϣ���н���

/**
  * @brief  nmea_decode_test ����GPSģ����Ϣ
  * @param  ��
  * @retval ��
  */
//int nmea_decode_test(void)
//{
//		OS_ERR      err;
//	
//    nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ  
//    uint8_t new_parse=0;    //�Ƿ����µĽ������ݱ�־
//  
//    nmeaTIME beiJingTime;    //����ʱ�� 

//    /* �����������������Ϣ�ĺ��� */
//    nmea_property()->trace_func = &trace;
//    nmea_property()->error_func = &error;
//    nmea_property()->info_func = &gps_info;

//    /* ��ʼ��GPS���ݽṹ */
//    nmea_zero_INFO(&info);
//    nmea_parser_init(&parser);

//    while(1)
//    {
//      if(GPS_HalfTransferEnd)     /* ���յ�GPS_RBUFF_SIZEһ������� */
//      {
//        /* ����nmea��ʽ���� */
//        nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);
//        
//        GPS_HalfTransferEnd = 0;   //��ձ�־λ
//        new_parse = 1;             //���ý�����Ϣ��־ 
//      }
//      else if(GPS_TransferEnd)    /* ���յ���һ������ */
//      {

//        nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
//       
//        GPS_TransferEnd = 0;
//        new_parse =1;
//      }
//      
//      if(new_parse )                //���µĽ�����Ϣ   
//      {    
//        /* �Խ�����ʱ�����ת����ת���ɱ���ʱ�� */
//        GMTconvert(&info.utc,&beiJingTime,8,1);
//        
//        /* �������õ�����Ϣ */
//				printf("\r\nʱ��%d-%02d-%02d,%d:%d:%d\r\n", beiJingTime.year+1900, beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
//				
//				//info.lat lon�еĸ�ʽΪ[degree][min].[sec/60]��ʹ�����º���ת����[degree].[degree]��ʽ
//				deg_lat = nmea_ndeg2degree(info.lat);
//				deg_lon = nmea_ndeg2degree(info.lon);
//				
//				printf("\r\nγ�ȣ�%f,����%f\r\n",deg_lat,deg_lon);
//        printf("\r\n���θ߶ȣ�%f �� ", info.elv);
//        printf("\r\n�ٶȣ�%f km/h ", info.speed);
//        printf("\r\n����%f ��", info.direction);
//				
//				printf("\r\n����ʹ�õ�GPS���ǣ�%d,�ɼ�GPS���ǣ�%d",info.satinfo.inuse,info.satinfo.inview);

//				printf("\r\n����ʹ�õı������ǣ�%d,�ɼ��������ǣ�%d",info.BDsatinfo.inuse,info.BDsatinfo.inview);
//				printf("\r\nPDOP��%f,HDOP��%f��VDOP��%f",info.PDOP,info.HDOP,info.VDOP);
//        
//        new_parse = 0;
//      }
//		OSTimeDly ( 30, OS_OPT_TIME_DLY, & err );
//	}

//    /* �ͷ�GPS���ݽṹ */
//    // nmea_parser_destroy(&parser);

//    
//    //  return 0;
//}

#endif






/**************************************************end of file****************************************/

