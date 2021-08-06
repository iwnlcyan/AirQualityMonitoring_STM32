#include "bsp_led.h"   



static void                         LED_GPIO_Config                  ( void );



 /**
  * @brief  Configure GPIO of LED
  * @param  
  * @retval 
  */
static void LED_GPIO_Config ( void )
{		
	GPIO_InitTypeDef GPIO_InitStructure;


	/* configure LED1 pins */
	RCC_APB2PeriphClockCmd ( macLED1_GPIO_CLK, ENABLE ); 															   
	GPIO_InitStructure.GPIO_Pin = macLED1_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init ( macLED1_GPIO_PORT, & GPIO_InitStructure );	

	/* configure LED2 pins */
//	RCC_APB2PeriphClockCmd ( macLED2_GPIO_CLK, ENABLE ); 															   
//	GPIO_InitStructure.GPIO_Pin = macLED2_GPIO_PIN;	
//	GPIO_Init ( macLED2_GPIO_PORT, & GPIO_InitStructure );	

//	/* configure LED3 pins */
//	RCC_APB2PeriphClockCmd ( macLED3_GPIO_CLK, ENABLE ); 															   
//	GPIO_InitStructure.GPIO_Pin = macLED3_GPIO_PIN;	
//	GPIO_Init ( macLED3_GPIO_PORT, & GPIO_InitStructure );		
	  		
}

/**
  * @brief  Configure GPIO of LED
  * @param  
  * @retval 
  */
static void RELAY_GPIO_Config ( void )
{		
	GPIO_InitTypeDef GPIO_InitStructure;


	/* configure RELAY1 pins */
	RCC_APB2PeriphClockCmd ( macRELAY1_GPIO_CLK, ENABLE ); 															   
	GPIO_InitStructure.GPIO_Pin = macRELAY1_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init ( macRELAY1_GPIO_PORT, & GPIO_InitStructure );	
	
	/* configure RELAY2 pins */
	RCC_APB2PeriphClockCmd ( macRELAY2_GPIO_CLK, ENABLE ); 															   
	GPIO_InitStructure.GPIO_Pin = macRELAY2_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init ( macRELAY2_GPIO_PORT, & GPIO_InitStructure );	

	/* configure RELAY3 pins */
	RCC_APB2PeriphClockCmd ( macRELAY3_GPIO_CLK, ENABLE ); 															   
	GPIO_InitStructure.GPIO_Pin = macRELAY3_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init ( macRELAY3_GPIO_PORT, & GPIO_InitStructure );	
	
	/* configure RELAY4 pins */
	RCC_APB2PeriphClockCmd ( macRELAY4_GPIO_CLK, ENABLE ); 															   
	GPIO_InitStructure.GPIO_Pin = macRELAY4_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init ( macRELAY4_GPIO_PORT, & GPIO_InitStructure );	
	  		
}

/**
  * @brief  configure GPIO of HC_SR04
  * @param  
  * @retval 
  */
static void BEEP_GPIO_Config ( void )
{		
	GPIO_InitTypeDef GPIO_InitStructure;


	/* configure BEEP pins */
	RCC_APB2PeriphClockCmd ( macHCSR04_GPIO_CLK, ENABLE ); 															   
	GPIO_InitStructure.GPIO_Pin = macHCSR04_GPIO_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init ( macHCSR04_GPIO_PORT, & GPIO_InitStructure );		
	  		
}

 /**
  * @brief  LED initialize
  * @param  
  * @retval 
  */
void LED_Init ( void )
{
  LED_GPIO_Config ();
	
	macLED1_OFF();
//	macLED2_OFF();
//	macLED3_OFF();
	
}

/**
  * @brief  BEEP initialize
  * @param  
  * @retval 
  */
void BEEP_Init ( void )
{
  BEEP_GPIO_Config ();
	
	macBEEP_OFF();
	
}

/**
  * @brief  RELAY initialize
  * @param  
  * @retval 
  */
void RELAY_Init ( void )
{
  RELAY_GPIO_Config ();
	
	macRELAY1_OFF();
	macRELAY2_OFF();
	macRELAY3_OFF();
	macRELAY4_OFF();
	
}


/*********************************************END OF FILE**********************/
