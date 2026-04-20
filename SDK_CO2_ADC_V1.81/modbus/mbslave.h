/* 
 * File:   mbslave.h
 * Author: Administrator
 *
 * Created on 2015年8月24日, 下午8:38
 */

#ifndef MBSLAVE_H
#define	MBSLAVE_H

#include "mbdef.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define    MB_SLAVE_ERR_CRC            1
#define    MB_SLAVE_ERR_HAL            2
#define    MB_SLAVE_ERR_ADDR           3
#define    MB_SLAVE_ERR_FUN_CODE       4
#define    MB_SLAVE_ERR_DATA           5
#define    MB_SLAVE_ERR_FRAME          6
#define    MB_SLAVE_ERR_UNKNOWN        127
	
void mbSlavePoll(mbObjType * mbObj);
    
#ifdef	__cplusplus
}
#endif

#endif	/* MBSLAVE_H */

