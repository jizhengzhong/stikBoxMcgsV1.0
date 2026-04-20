#ifndef _MBPORTX_HAL_H_
#define	_MBPORTX_HAL_H_

#include "stdint.h"
#include "mbdef.h"

#ifdef   _MBPORTX_MODULE_
#define  MB_EXT
#else
#define  MB_EXT  extern
#endif      


MB_EXT mbObjType mbxObj;
MB_EXT mbCmdBuffType mbxCmdBuff[16];

void mbxInit(mbModeType mbMode, uint32_t ulBaudRate, mbParityType eParity);
void mbxTimeoutHandle(void);
void mbxRtuTimerRun(void);

#undef MB_EXT

#endif	/* _MBPORTX_HAL_H_ */

