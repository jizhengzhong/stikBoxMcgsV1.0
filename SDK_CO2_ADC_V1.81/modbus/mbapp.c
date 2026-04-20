/*
 * Modbus app for RTU.
 * Copyright (c) 2012  apleilx
 * All rights reserved.
 * no limite
 */

/* ----------------------- Platform includes --------------------------------*/
#define _MBAPP_MODULE_
#include "mbapp.h"
#include "osObjects.h"                                          // RTOS object definitions
#include "systask.h"
#include "sys.h"
#include "main.h"


Telephone_Def TelephoneDefine;
uint8_t mbTestErrCnt = 0;
uint8_t mbTestOkCnt = 0;
uint32_t TimeCnt;

extern volatile uint32_t uwTick;
extern uint8_t SenSndBuff[];
extern uint8_t gprs_info_buff[256];

void GprsComDataSend(uint8_t * pData, uint16_t Len);
uint16_t MiddleValSearch(uint16_t* Src);
void Co2TempDataHandle(void);
void Co2TempCmdSend(void);
void Co2AdjHandle(void);

void mbTestCallBack(uint8_t CmdSite, uint8_t ErrSt);
void Co2TempModuleHandle(void);



/*****************************************************************************//*!
* @brief   	Slave device handle.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/


void mbTask(void const *argument)
{

    //从机接口
    mbxInit(MB_RTU_SLAVE, 19200, MB_PAR_EVEN);
    mbxObj.SlaveId = 2;

    //从机接口
//    mbyInit(MB_RTU_SLAVE, 9600, MB_PAR_NONE);
//    mbyObj.SlaveId = 3;

    for (;;)
    {
        osThreadFlagsWait(MBAPP_EVENT_MSK, osFlagsWaitAny, 500);
        osDelay(3);
        sysapi->mb_poll(&mbxObj);
//        sysapi->mb_poll(&mbyObj);
    }
}

