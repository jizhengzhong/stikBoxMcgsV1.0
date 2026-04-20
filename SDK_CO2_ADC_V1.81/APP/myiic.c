#include "myiic.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//IO方向设置
#define SDA_OUT()  {__disable_irq(); GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;__enable_irq();}
#define SDA_IN()   {__disable_irq(); GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;__enable_irq();}

//IO操作函数	 
#define IIC_SCL    PBout(6) 		//SCL
#define IIC_SDA    PBout(7) 		//SDA	 
#define READ_SDA   PBin(7) 	 		//输入SDA 

/*端口位定义*/
#define SDA_OUT1()   IIC_SDA=1
#define SDA_OUT0() IIC_SDA=0
#define SDA_ST()   READ_SDA
#define SCL_OUT1()  IIC_SCL=1
#define SCL_OUT0()  IIC_SCL=0

#define IIC_SCL_OUT()  SCL_OUT()
#define IIC_SDA_OUT()  SDA_OUT()
#define IIC_SDA_IN()   SDA_IN()

//初始化IIC
void IIC_Init(void)
{					     
    GPIO_InitTypeDef GPIO_InitStruct;
    __disable_irq();
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    __enable_irq();
    
	IIC_SCL=1;
	IIC_SDA=1;
}
