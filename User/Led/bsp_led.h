#ifndef __LED_H_
#define	__LED_H_



#include "stm32f10x.h"
 
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入
 
/****************************** LED 引脚配置参数定义***************************************/
#define             macLED1_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macLED1_GPIO_CLK                        RCC_APB2Periph_GPIOD
#define             macLED1_GPIO_PORT                       GPIOD
#define             macLED1_GPIO_PIN                        GPIO_Pin_2

#define             macLED2_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macLED2_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macLED2_GPIO_PORT                       GPIOB
#define             macLED2_GPIO_PIN                        GPIO_Pin_0

#define             macLED3_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macLED3_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macLED3_GPIO_PORT                       GPIOB
#define             macLED3_GPIO_PIN                        GPIO_Pin_1

#define             macHCSR04_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macHCSR04_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define             macHCSR04_GPIO_PORT                       GPIOA
#define             macHCSR04_GPIO_PIN                        GPIO_Pin_1

/****************************** LED 函数宏定义***************************************/
#define             macLED1_ON()                            GPIO_ResetBits ( macLED1_GPIO_PORT, macLED1_GPIO_PIN )
#define             macLED1_OFF()                           GPIO_SetBits ( macLED1_GPIO_PORT, macLED1_GPIO_PIN )
#define             macLED1_TOGGLE()                        GPIO_ReadOutputDataBit ( macLED1_GPIO_PORT, macLED1_GPIO_PIN ) ? \
                                                            GPIO_ResetBits ( macLED1_GPIO_PORT, macLED1_GPIO_PIN ) : GPIO_SetBits ( macLED1_GPIO_PORT, macLED1_GPIO_PIN )

#define             macLED2_ON()                            GPIO_ResetBits ( macLED2_GPIO_PORT, macLED2_GPIO_PIN )
#define             macLED2_OFF()                           GPIO_SetBits ( macLED2_GPIO_PORT, macLED2_GPIO_PIN )
#define             macLED2_TOGGLE()                        GPIO_ReadOutputDataBit ( macLED2_GPIO_PORT, macLED2_GPIO_PIN ) ? \
                                                            GPIO_ResetBits ( macLED2_GPIO_PORT, macLED2_GPIO_PIN ) : GPIO_SetBits ( macLED2_GPIO_PORT, macLED2_GPIO_PIN )

#define             macLED3_ON()                            GPIO_ResetBits ( macLED3_GPIO_PORT, macLED3_GPIO_PIN )
#define             macLED3_OFF()                           GPIO_SetBits ( macLED3_GPIO_PORT, macLED3_GPIO_PIN )
#define             macLED3_TOGGLE()                        GPIO_ReadOutputDataBit ( macLED3_GPIO_PORT, macLED3_GPIO_PIN ) ? \
                                                            GPIO_ResetBits ( macLED3_GPIO_PORT, macLED3_GPIO_PIN ) : GPIO_SetBits ( macLED3_GPIO_PORT, macLED3_GPIO_PIN )

#define             macBEEP_ON()                            GPIO_SetBits ( macHCSR04_GPIO_PORT, macHCSR04_GPIO_PIN )
#define             macBEEP_OFF()                           GPIO_ResetBits ( macHCSR04_GPIO_PORT, macHCSR04_GPIO_PIN )

/************************** PAD 函数声明********************************/
void                               LED_Init                         ( void );
 
void BEEP_Init ( void );


/****************************** 继电器 引脚配置参数定义***************************************/
#define             macRELAY1_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macRELAY1_GPIO_CLK                        RCC_APB2Periph_GPIOC
#define             macRELAY1_GPIO_PORT                       GPIOC
#define             macRELAY1_GPIO_PIN                        GPIO_Pin_2

#define             macRELAY2_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macRELAY2_GPIO_CLK                        RCC_APB2Periph_GPIOC
#define             macRELAY2_GPIO_PORT                       GPIOC
#define             macRELAY2_GPIO_PIN                        GPIO_Pin_3

#define             macRELAY3_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macRELAY3_GPIO_CLK                        RCC_APB2Periph_GPIOC
#define             macRELAY3_GPIO_PORT                       GPIOC
#define             macRELAY3_GPIO_PIN                        GPIO_Pin_4

#define             macRELAY4_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macRELAY4_GPIO_CLK                        RCC_APB2Periph_GPIOC
#define             macRELAY4_GPIO_PORT                       GPIOC
#define             macRELAY4_GPIO_PIN                        GPIO_Pin_5


#define             macRELAY1_ON()                            GPIO_SetBits ( macRELAY1_GPIO_PORT, macRELAY1_GPIO_PIN )
#define             macRELAY1_OFF()                           GPIO_ResetBits ( macRELAY1_GPIO_PORT, macRELAY1_GPIO_PIN )

#define             macRELAY2_ON()                            GPIO_SetBits ( macRELAY2_GPIO_PORT, macRELAY2_GPIO_PIN )
#define             macRELAY2_OFF()                           GPIO_ResetBits ( macRELAY2_GPIO_PORT, macRELAY2_GPIO_PIN )

#define             macRELAY3_ON()                            GPIO_SetBits ( macRELAY3_GPIO_PORT, macRELAY3_GPIO_PIN )
#define             macRELAY3_OFF()                           GPIO_ResetBits ( macRELAY3_GPIO_PORT, macRELAY3_GPIO_PIN )

#define             macRELAY4_ON()                            GPIO_SetBits ( macRELAY4_GPIO_PORT, macRELAY4_GPIO_PIN )
#define             macRELAY4_OFF()                           GPIO_ResetBits ( macRELAY4_GPIO_PORT, macRELAY4_GPIO_PIN )

void RELAY_Init ( void );

#endif /* __LED_H_ */

