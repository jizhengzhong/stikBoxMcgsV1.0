/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 */

#ifndef _GPRS_API_H
#define _GPRS_API_H

#include "stdint.h"

#ifdef   _GPRS_API_MODULE_
#define  MY_EXT
#else
#define  MY_EXT  extern
#endif

typedef struct {
    uint8_t TelNumLocal[16]; //telphone number Local
    uint8_t TelNumRemot[16]; //telphone number remote
    uint8_t IMEI[16];//
    uint8_t IMSI[16];//
    uint8_t CMGF[16];//message mode
} GprsInfo_Type;


#undef MY_EXT

#endif
