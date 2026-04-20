/*
 * Modbus mutex for RTU.
 * Copyright (c) 2012  apleilx
 * All rights reserved.
 * no limite
 */

/* ----------------------- Platform includes --------------------------------*/

#include "osObjects.h"                                        
#include "mbdef.h"  

#if MB_OS_USED > 0


/*****************************************************************************//*!
*
* @brief   modbus sync mutex creat.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void mbOsSyncMutexCreat(void)
{

}

/*****************************************************************************//*!
*
* @brief   modbus sync mutex pend.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void mbOsSyncMutexPend(void)
{

}

/*****************************************************************************//*!
*
* @brief   modbus sync mutex post.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void mbOsSyncMutexPost(void)
{

}

#endif
