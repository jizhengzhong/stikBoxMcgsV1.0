#ifndef _MBPORTZ_HAL_H_
#define	_MBPORTZ_HAL_H_

#include "stdint.h"
#include "mbdef.h"

#ifdef   _MBPORTZ_MODULE_
#define  MB_EXT
#else
#define  MB_EXT  extern
#endif      


MB_EXT mbObjType mbzObj;
MB_EXT mbCmdBuffType mbzCmdBuff[16];

void mbzInit(mbModeType mbMode, uint32_t ulBaudRate, mbParityType eParity);
void mbzTimeoutHandle(void);
void mbzRtuTimerRun(void);
void mbzDataSend(void);

#undef MB_EXT

#endif	/* _MBPORTZ_HAL_H_ */

