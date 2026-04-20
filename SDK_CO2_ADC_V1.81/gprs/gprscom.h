#ifndef _COM_GPRS_H_
#define	_COM_GPRS_H_

#include "stdint.h"
#include "com_hal.h"

void GprsComInit(uint32_t ulBaudRate, ComParity_t eParity);
void GprsComParity(ComParity_t eParity);
void GprsComStringSend(uint8_t * pData);
void GprsComDataSend(uint8_t * pData, uint16_t Len);
uint8_t GprsComStCheck(void); //¶Ë¿Ú¿ÕÏÐ×´Ì¬¼ì²é


#endif	/* _COM_GPRS_H_ */

