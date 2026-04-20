/*****************************************************************************//*!
* file  : test.c
* creat : apleilx
* data  : 2017/03/02
*******************************************************************************/
#define   osObjectsCreat                                        // define objects in rtxapp module
#include "osObjects.h"                                          // RTOS object definitions
#include "pincfg.h"

void RtcTask(void const *argument);

void SysTask (void  const *argument);
void LedCtrTask(void const *argument);
void StrTask(void const *argument);
void mbTask(void const *argument);
void AppManTask(void const *argument);
void ControlTask(void const *argument);
void GprsTask(void const *argument);
void PrintTask(void const *argument);
void SenModuleTask(void const *argument);
void HumiGetTask(void const *argument);
void Co2GetTask(void const *argument);
void StartDefaultTask(void const * argument);

//独立堆栈 需定义
osThreadDef(SysTask, osPriorityHigh, 1, 0x800);
osThreadDef(mbTask, osPriorityAboveNormal, 1, 0x800);
osThreadDef(LedCtrTask, osPriorityNormal, 1, 0x800);
osThreadDef(AppManTask, osPriorityBelowNormal, 1, 0x800);
osThreadDef(StrTask, osPriorityLow, 1, 0x2000);
osThreadDef(ControlTask, osPriorityNormal, 1, 0x800);
osThreadDef(GprsTask, osPriorityNormal, 1, 0x800);
osThreadDef(PrintTask, osPriorityNormal, 1, 0x800);
osThreadDef(SenModuleTask, osPriorityNormal, 1, 0x800);
osThreadDef(HumiGetTask, osPriorityNormal, 1, 0x800);
osThreadDef(Co2GetTask, osPriorityNormal, 1, 0x800);
osThreadDef(StartDefaultTask, osPriorityNormal, 1, 0x800);

osThreadDef(RtcTask, osPriorityNormal, 1, 0x800);

osMutexDef(Mut_Malloc);
osMutexDef(Mut_IICHAL);
osMutexDef(Mut_SDCARD);

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/

int Init_Thread (void)
{
    osThreadCreate(osThread(StartDefaultTask),NULL);   
    tid_SysTask = osThreadCreate(osThread(SysTask),NULL);
    tid_LedTask = osThreadCreate(osThread(LedCtrTask),NULL);
    tid_StrTask = osThreadCreate(osThread(StrTask),NULL);
    tid_modbus = osThreadCreate(osThread(mbTask),NULL);
    tid_appman = osThreadCreate(osThread(AppManTask),NULL);
    tid_Control = osThreadCreate(osThread(ControlTask), NULL);
    tid_Gprs = osThreadCreate(osThread(GprsTask), NULL);
    tid_Print = osThreadCreate(osThread(PrintTask), NULL);
    tid_SenModule = osThreadCreate(osThread(SenModuleTask), NULL);
    tid_HumiGet = osThreadCreate(osThread(HumiGetTask), NULL);
    tid_Co2Get = osThreadCreate(osThread(Co2GetTask), NULL);
  
    tid_Rtc=osThreadCreate(osThread(RtcTask),NULL);
    
    mid_malloc = osMutexCreate(osMutex(Mut_Malloc));
    mid_iichal = osMutexCreate(osMutex(Mut_IICHAL));
    mid_sdcard = osMutexCreate(osMutex(Mut_SDCARD));
    
    return(0);
}

