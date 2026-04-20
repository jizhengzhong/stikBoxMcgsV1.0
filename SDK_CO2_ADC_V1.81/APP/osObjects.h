/*----------------------------------------------------------------------------
 * osObjects.h: CMSIS-RTOS global object definitions for an application
 *----------------------------------------------------------------------------
 *
 * This header file defines global RTOS objects used throughout a project
 *
 * #define osObjectsPublic indicates that objects are defined; without that
 * definition the objects are defined as external symbols.
 *
 *--------------------------------------------------------------------------*/


#ifndef __osObjects
#define __osObjects


#ifdef osObjectsCreat
#define OBJ_EXT  
#else
#define OBJ_EXT extern 
#endif

#include "cmsis_os.h"               // CMSIS RTOS header file


// global 'thread' functions ---------------------------------------------------

int Init_Thread (void);

OBJ_EXT osThreadId  tid_GuiTask;                                  // thread id
OBJ_EXT osThreadId  tid_SysTask;                                  // thread id
OBJ_EXT osThreadId  tid_LedTask;                                  // thread id
OBJ_EXT osThreadId  tid_StrTask;                                  // thread id 
OBJ_EXT osThreadId  tid_modbus; 
OBJ_EXT osThreadId  tid_appman; 
OBJ_EXT osThreadId  tid_Control;
OBJ_EXT osThreadId  tid_Gprs;
OBJ_EXT osThreadId  tid_Print;
OBJ_EXT osThreadId  tid_SenModule;
OBJ_EXT osThreadId  tid_HumiGet;
OBJ_EXT osThreadId  tid_Co2Get;
OBJ_EXT osThreadId  tid_Rtc;

// global 'semaphores' ----------------------------------------------------------

OBJ_EXT osMutexId mid_sdcard;
OBJ_EXT osMutexId mid_iichal;
OBJ_EXT osMutexId mid_malloc;

// global 'message queues' -------------------------------------------------------
OBJ_EXT osMessageQId msgid_music;

// global 'mail queues' ----------------------------------------------------------


#endif  // __osObjects
