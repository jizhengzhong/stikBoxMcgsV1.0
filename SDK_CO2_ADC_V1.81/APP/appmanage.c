#define _APP_MANAGE_MODULE_

#include "osObjects.h"
#include "appmanage.h"
#include "pincfg.h"
#include "partab.h"
#include "systask.h"
#include "iichal.h"


void ParSaveCheck(void);
void Poweroff_mode(void);

extern timer_pulse_type CtrPls;

/*****************************************************************************//*!
*
* @brief   led task.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void AppManTask(void const *argument)
{
    osDelay(2000);
    Poweroff_mode();
    for (;;){
        osDelay(20);
        ParSaveCheck();
       

    }
}

/*****************************************************************************//*!
*
* @brief   led duty control.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void ParSaveCheck(void) {
    uint32_t Index;
    
    //前两个参数是ID和LK，从第3个参数开始存储
    //检测到参数值变化就存储至EEPROM
    for(Index = 2; Index< PAR_POS_END;Index++)
    {
        if (ParSaved[Index].uVal != ParReceive[Index].uVal){
            ParSaved[Index].uVal = ParReceive[Index].uVal;
            ParSave(Index);
            break;
        }
    }
}

void Poweroff_mode(void)
{
   static unsigned char pvd_temp[4]={0};
    
   switch( ParTab[PAR_POS_PWROFF_MODE].uVal )
   {
     case 0:
       SysRunSt.bSysRun = 0; 
       break;     
     case 1:
       SysRunSt.bSysRun = 1; 
       break;      
     case 2:
       
    
       Nw_eep_Rcv(0xa0, PAR_POS_END<<1, &pvd_temp[0], 4);//避免每次开机都写AT24CXX
        if( ParTab[PAR_POS_START_STOP].uVal == 0x01 ) 
             SysRunSt.bSysRun = 1; 
        else
             SysRunSt.bSysRun = 0; 
       SysRunSt.TimeCnt = pvd_temp[0] | pvd_temp[1]<<8 |         
                          pvd_temp[2]<<16 | pvd_temp[3] << 24;
       break;
    default : 
      SysRunSt.bSysRun = 0; 
      break;
   }
}

