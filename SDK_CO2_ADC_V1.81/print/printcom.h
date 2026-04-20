#ifndef _COM_PRINT_H_
#define	_COM_PRINT_H_

#include "stdint.h"
#include "com_hal.h"

void PtComInit(uint32_t ulBaudRate, ComParity_t eParity);
void PtComParity(ComParity_t eParity);
void PtComStringSend(uint8_t * pData);
void PtComDataSend(uint8_t * pData, uint16_t Len);
uint8_t PtComStCheck(void); //¶Ë¿Ú¿ÕÏÐ×´Ì¬¼ì²é
uint8_t PtComSndStCheck(void);
uint8_t PtComRcvStCheck(void);

#endif	/* _COM_PRINT_H_ */

