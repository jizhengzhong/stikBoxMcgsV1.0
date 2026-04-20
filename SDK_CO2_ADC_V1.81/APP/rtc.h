
#ifndef RTC_H
#define	RTC_H

#include <stdint.h>
#include "stm32f4xx_ll_rcc.h"

#ifdef	__cplusplus
extern "C" {
#endif
	
#ifdef   _RTC_MODULE_
#define  RTC_EXT
#else
#define  RTC_EXT  extern
#endif  
 extern void MX_RTC_Init(void);
 extern HAL_StatusTypeDef RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm);
 extern HAL_StatusTypeDef RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week);
	

#undef LED_EXT

#ifdef	__cplusplus
}
#endif

#endif	/* LEDCTR_H */
