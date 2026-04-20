/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 */

#ifndef _MB_APP_H
#define _MB_APP_H

#include "mbdef.h"
#include "mbportx.h"
#include "mbporty.h"
#include "mbportz.h"
#include "mbdatacb.h"
#include "mbcrc.h"
#include "mbslave.h"
#include "mbmaster.h"
#include "mbinputdatacfg.h"


#ifdef   _MAPP_MODULE_
#define  MAPP_EXT
#else
#define  MAPP_EXT  extern
#endif

#define MBAPP_COMX_EVENT_RTUEND                0x01UL
#define MBAPP_COMX_EVENT_TIMEOUT               0x02UL
#define MBAPP_COMY_EVENT_RTUEND                0x04UL
#define MBAPP_COMY_EVENT_TIMEOUT               0x08UL
#define MBAPP_COMZ_EVENT_RTUEND                0x10UL
#define MBAPP_COMZ_EVENT_TIMEOUT               0x20UL
    
#define MBAPP_EVENT_MSK \
(MBAPP_COMX_EVENT_RTUEND\
|MBAPP_COMX_EVENT_TIMEOUT\
|MBAPP_COMY_EVENT_RTUEND\
|MBAPP_COMY_EVENT_TIMEOUT\
|MBAPP_COMZ_EVENT_RTUEND\
|MBAPP_COMZ_EVENT_TIMEOUT\
)

#undef MAPP_EXT

#endif
