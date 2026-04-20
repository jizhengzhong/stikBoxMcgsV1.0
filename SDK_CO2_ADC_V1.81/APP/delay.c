#include "delay.h"
#include "sys.h"
#include "pincfg.h"
#include "cmsis_os.h"
////////////////////////////////////////////////////////////////////////////////// 	 

							      								   
void delay_us(u32 nus)
{		
    while(nus--){
        NOP40();NOP40();
        NOP40();NOP40();
        NOP20();
    }
}  


//—” ±nms 
//nms:0~65535
void delay_ms(u16 nms)
{	 	 
    osDelay(nms+1);
} 

			 



































