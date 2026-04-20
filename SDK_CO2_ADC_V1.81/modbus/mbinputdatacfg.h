/* 
 * File:   mbdatacfg.h
 * Author: apleilx
 *
 */
 
#ifndef _MB_INPUTDATA_CONFIG_H_
#define _MB_INPUTDATA_CONFIG_H_

#include "stdint.h"
#include "mbdef.h"

#ifdef   _MBDATACB_MODULE_
#define  EXTERN
#else
#define  EXTERN  extern
#endif    

	typedef union
	{
		int16_t Val;
        uint16_t uVal;
        int8_t  v[2];
        
		struct   
		{
			int8_t LB;
			int8_t HB;
		} bytes;
        
		struct   
		{
			uint16_t     b0 : 1;
			uint16_t     b1 : 1;
			uint16_t     b2 : 1;
			uint16_t     b3 : 1;
			uint16_t     b4 : 1;
			uint16_t     b5 : 1;
			uint16_t     b6 : 1;
			uint16_t     b7 : 1;
			uint16_t     b8 : 1;
			uint16_t     b9 : 1;
			uint16_t     b10 : 1;
			uint16_t     b11 : 1;
			uint16_t     b12 : 1;
			uint16_t     b13 : 1;
			uint16_t     b14 : 1;
			uint16_t     b15 : 1;
		} bits;
        
	} MBDataType;
    
    #define REG_INPUT_NREGS          0x50
    #define REG_INPUT_START          0
    #if MB_REG_INPUT_CB_ENABLE > 0 
    EXTERN  MBDataType   RegInput[REG_INPUT_NREGS];
    #endif
//------------------------------------------------------------------------------------------------------------------------//
//-------------------------------------------------- input register define -----------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------//

    
#undef EXTERN
#endif
