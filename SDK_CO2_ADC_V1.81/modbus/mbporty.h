#ifndef _MBPORTY_HAL_H_
#define	_MBPORTY_HAL_H_

#include "stdint.h"
#include "mbdef.h"

#ifdef   _MBPORTY_MODULE_
#define  MB_EXT
#else
#define  MB_EXT  extern
#endif      

#define MBY_CMD_BUFF_SIZE       16
#define MBY_SCMD_BUFF_SIZE       6

MB_EXT mbObjType mbyObj;
MB_EXT mbCmdBuffType mbyCmdBuff[MBY_CMD_BUFF_SIZE];

void mbyInit(mbModeType mbMode, uint32_t ulBaudRate, mbParityType eParity);
void mbyTimeoutHandle(void);
void mbyRtuTimerRun(void);

#undef MB_EXT

#endif	/* _MBPORTY_HAL_H_ */

