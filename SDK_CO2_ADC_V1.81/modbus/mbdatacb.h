/* 
 * File:   mbdatacb.h
 * Author: Administrator
 *
 * Created on 2015年8月24日, 下午8:38
 */

#ifndef MBDATACB_H
#define	MBDATACB_H

#include "mbdef.h"

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef   _MBDATACB_MODULE_
#define  DATACB_EXT
#else
#define  DATACB_EXT  extern
#endif  

#define REG_DISCRETE_START          0
#define REG_DISCRETE_NREGS          16         // Amount base on U8


#define REG_HOLDING_START           0
#define REG_HOLDING_NREGS           (0xF000)        // Amount base on U16
     
#define REG_COILS_START             0
#define REG_COILS_NREGS             16         // Amount base on U8

#if MB_REG_DISCRETE_CB_ENABLE > 0
DATACB_EXT signed char    RegDiscrete[REG_DISCRETE_NREGS + 1];
#endif

#if MB_REG_COILS_CB_ENABLE > 0
DATACB_EXT signed char    RegCoil[REG_COILS_NREGS + 1]; 
#endif

uint8_t eMBRegInputCB( uint8_t * pucRegBuffer, uint16_t usAddress, uint16_t usNRegs );
uint8_t eMBRegHoldingCB( uint8_t  * pucRegBuffer, uint16_t usAddress, uint16_t usNRegs, mbRegModeType eMode );
uint8_t eMBRegDiscreteCB( uint8_t  *pucRegBuffer, uint16_t usAddress, uint16_t usNCoils );
uint8_t eMBRegCoilsCB( uint8_t  * pucRegBuffer, uint16_t usAddress, uint16_t usNCoils, mbRegModeType eMode );


#undef  DATACB_EXT
#ifdef	__cplusplus
}
#endif

#endif	/* MBDATACB_H */

