#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
   	   		   
//IO方向设置
#define SDA_IN()  {__disable_irq(); GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;__enable_irq();}	//PH5输入模式
#define SDA_OUT() {__disable_irq(); GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;__enable_irq();} //PH5输出模式
//IO操作函数	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //输入SDA 


void IIC_Init(void);

#endif
















