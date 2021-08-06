/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SysTick ϵͳ�δ�ʱ��10us�жϺ�����,�ж�ʱ����������ã�
  *          ���õ��� 1us 10us 1ms �жϡ�     
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./systick/bsp_SysTick.h"

static __IO u32 TimingDelay;

__IO u8 measureflag = 0;
__IO u32 TmMeasDelay = 0;
/**
  * @brief  ����ϵͳ�δ�ʱ�� SysTick
  * @param  ��
  * @retval ��
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
//	if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0��汾
	if (SysTick_Config(SystemCoreClock / 1000000))	// ST3.5.0��汾
	{ 
		/* Capture error */ 
		while (1);
	}
		// �رյδ�ʱ��  
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}

/**
  * @brief   us��ʱ����,10usΪһ����λ
  * @param  
  *		@arg nTime: Delay_us( 1 ) ��ʵ�ֵ���ʱΪ 1 * 10us = 10us
  * @retval  ��
  */
void Delay_us(__IO u32 nTime)
{ 
	TimingDelay = nTime;	

	// ʹ�ܵδ�ʱ��  
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay != 0);
}

extern double Distance;

/**
  * @brief  ��ȡ���ĳ���
  * @param  ��
  * @retval ��
  * @attention  �� SysTick �жϺ��� SysTick_Handler()����
  */
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
//	if(measureflag)
//	{
//		TmMeasDelay ++;
//		
//		if(TmMeasDelay >= 2000000)
//		{
//			TmMeasDelay = 0;
//			measureflag = 0;
//			TmMeasDelay = 0;
//			Distance = 400;
//		}
//	}
}

/**
  * @brief  ��ȡ����ʱ��
  * @param  ��
  * @retval ��
  * @attention  �� SysTick �жϺ��� SysTick_Handler()����
  */
void TmDelay_measure(void)
{
	if(measureflag)
	{
		TmMeasDelay ++;
		
		if(TmMeasDelay >= 11764)
		{
			TmMeasDelay = 0;
			measureflag = 0;
			TmMeasDelay = 0;
		}
	}
}

u32 Get_TmDelay_measure(void)
{
	return TmMeasDelay;
}

void TmDelay_measureSet(void)
{
	TmMeasDelay = 0;
	measureflag = 1;
}

void TmDelay_measureClr(void)
{
	TmMeasDelay = 0;
	measureflag = 0;
}


/*********************************************END OF FILE**********************/
