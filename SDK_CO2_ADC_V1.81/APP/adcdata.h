
#ifndef ADCDATA_H
#define	ADCDATA_H

#include "sys.h"
#include "stm32f4xx_hal.h"


#ifdef	__cplusplus
extern "C" {
#endif
	
#ifdef   _ADCDATA_MODULE_
#define  ADCDATA_EXT
#else
#define  ADCDATA_EXT  extern
#endif  
extern  unsigned int ADC_Value[5];
ADCDATA_EXT  ADC_HandleTypeDef hadc1;
ADCDATA_EXT  DMA_HandleTypeDef hdma_adc1;
void MX_ADC1_Init(void);

 // void LedCtrTask(void const *argument);
 // void LedInit(void);
 // void PwmInit(void);
 // void DutySet(uint32_t Type,uint32_t Value);

#undef ADCDATA_EXT

#ifdef	__cplusplus
}
#endif

#endif	/* ADCDATA_H */
