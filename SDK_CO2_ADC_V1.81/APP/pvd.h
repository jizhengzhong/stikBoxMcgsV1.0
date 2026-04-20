
#ifndef PVD_H
#define	PVD_H

#include <stdint.h>
#include "stm32f4xx_ll_pwr.h"

#ifdef	__cplusplus
extern "C" {
#endif
	
#ifdef   _RTC_MODULE_
#define  PVD_EXT
#else
#define  PVD_EXT  extern
#endif  

  
 PVD_EXT  void PVD_Init(void);

#undef LED_EXT

#ifdef	__cplusplus
}
#endif

#endif	/* LEDCTR_H */
