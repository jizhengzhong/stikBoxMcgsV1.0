
#ifndef LEDCTR_H
#define	LEDCTR_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif
	
#ifdef   _LED_MODULE_
#define  LED_EXT
#else
#define  LED_EXT  extern
#endif  

  void LedCtrTask(void const *argument);
  void LedInit(void);
  void PwmInit(void);
  void DutySet(uint32_t Type,uint32_t Value);

#undef LED_EXT

#ifdef	__cplusplus
}
#endif

#endif	/* LEDCTR_H */
