#include "osObjects.h"
#include "pvd.h"
#include "pincfg.h"
#include "dataproc.h"
#include "systask.h"
#include "mbapp.h"
#include "partab.h"
#include "iichal.h"



void PVD_Init(void)
{
  PWR_PVDTypeDef PvdStruct;
 
  HAL_PWR_EnablePVD();
  
  PvdStruct.PVDLevel = PWR_PVDLEVEL_5;
  PvdStruct.Mode     = PWR_PVD_MODE_IT_RISING;
  HAL_PWR_ConfigPVD(&PvdStruct);
  
  HAL_NVIC_SetPriority(PVD_IRQn,0,0);
  HAL_NVIC_EnableIRQ(PVD_IRQn);

}

void PVD_IRQHandler(void)
{
  static uint8_t temp[4]={0};
  temp[0] = SysRunSt.TimeCnt;
  temp[1] = SysRunSt.TimeCnt>>8;
  temp[2] = SysRunSt.TimeCnt>>16;
  temp[3] = SysRunSt.TimeCnt>>24;
    
   if( __HAL_PWR_GET_FLAG(PWR_FLAG_PVDO) )
   {
       Nw_eep_Send(0xa0, PAR_POS_END<<1, &temp[0], 4);
   }
}









