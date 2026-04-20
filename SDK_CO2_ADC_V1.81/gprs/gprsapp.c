/* 
 */

/* ----------------------- Platform includes --------------------------------*/
#define _GPRS_APP_MODULE_
#include "gprsapp.h"
#include "osObjects.h"
#include "gprscom.h"

uint8_t gprs_info_buff[256];

/*****************************************************************************//*!
* @brief   	Slave device handle.
*               
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/    
void GprsTask(void const *argument)
{
    osDelay(500);
    GprsComInit(1,COM_PAR_NONE);
    
    for(;;){
        osDelay(500);
        //暂时用来打印接收到的传感器信息
        GprsComStringSend("GPRS task run ok\r\n");
    }
}


