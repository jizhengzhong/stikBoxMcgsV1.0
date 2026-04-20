/* 
 */

/* ----------------------- Platform includes --------------------------------*/
#define _PRINT_APP_MODULE_
#include "printapp.h"
#include "osObjects.h"
#include "printcom.h"
#include "systask.h"
#include "stdio.h"
#include "rtctime.h"
#include "dataproc.h"

void DbgInfoPrint(void);

uint8_t PtBuff[1024];

/*****************************************************************************//*!
* @brief   	Slave device handle.
*               
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/    
void PrintTask(void const *argument)
{
    osDelay(2000);
    PtComInit(1,COM_PAR_NONE);
    for(;;){
        osDelay(1000);
        
        DbgInfoPrint();
    }
}


/*****************************************************************************//*!
* @brief   	Debug info .
*               
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/  
void DbgInfoPrint(void){
    uint32_t Index;
    
    Index = 0;
    Index += sprintf((char*)&PtBuff[Index], "print task run ok! Run Time = %d, ", SysRunSt.PowerUpTimeCnt);
    Index += sprintf((char*)&PtBuff[Index], "\r\n");
    Index += sprintf((char*)&PtBuff[Index], "Temp AD: %d, Temp cur %5.2f\r\n",SysRunSt.Io.Pt100AdVal,SysRunSt.Io.fTempMain);
    PtBuff[Index] = 0;
    PtComStringSend(PtBuff);
    
    while(!PtComSndStCheck()) osDelay(5);
    
    Index = 0;
    Index += sprintf((char*)&PtBuff[Index], "TEMP = %d, COL = %d \r\n", SysRunSt.Io.iTempMain, SysRunSt.Io.iCol);
    Index += sprintf((char*)&PtBuff[Index], "TEMPOUT = %d, COLOUT = %d \r\n\r\n", SysRunSt.Io.AdjTempOutDuty, SysRunSt.Io.AdjColOutDuty);
    PtBuff[Index] = 0;
    PtComStringSend(PtBuff);
}
