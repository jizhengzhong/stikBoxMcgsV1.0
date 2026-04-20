/*****************************************************************************//*!
* file  : sysfun.c
* creat : apleilx
* data  : 2017/11/05
*******************************************************************************/

#include "osObjects.h"
#include "pincfg.h"

#define _SYSFUN_MODULE_
#include "sysfun.h"
#undef _SYSFUN_MODULE_


/*****************************************************************************//*!
*
* @brief   System error msk  .
*     
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void SysErrMsk(void){
    uint32_t Index;
    
    for(Index=0; Index<=15; Index++){
        if(0 == ((1UL << Index) & ERR_G1_MSK))
            SysRunSt.ErrSt[0][Index] = 0;
    }
    
    for(Index=0; Index<=15; Index++){
        if(0 == ((1UL << Index) & ERR_G2_MSK))
            SysRunSt.ErrSt[1][Index] = 0;
    }
    
    for(Index=0; Index<=15; Index++){
        if(0==((1UL << Index) & ERR_G3_MSK))
            SysRunSt.ErrSt[2][Index] = 0;
    }
    
    for(Index=0; Index<=15; Index++){
        if(0==((1UL << Index) & ERR_G4_MSK))
            SysRunSt.ErrSt[3][Index] = 0;
    }
}

/*****************************************************************************//*!
*
* @brief   err state check   .
*     
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
uint32_t IsSysErrFound(void){
    uint32_t i,j;
    
///////////////////////////ji_add//////////////
//   if( SysRunSt.door_open_state == 1)
//     return 1;
//    
//////////////////////////ji_end///////////
    
    for(i=0;i<=3;i++){
        for(j=0;j<=15;j++){
            
            if(SysRunSt.ErrSt[i][j]!=0)
                return 1;
        }
    }
    
    return 0;
}

/*****************************************************************************//*!
*
* @brief   err state check   .
*     
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void SysErrClrAll(void){
    uint32_t i,j;
    
    for(i=0;i<=3;i++){
        for(j=0;j<=15;j++){
            SysRunSt.ErrSt[i][j]=0;  
        }
    }
}

/*****************************************************************************//*!
*
* @brief   err state last update   .
*     
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void SysErrLastUpdate(void){
    uint32_t i,j;
    
    for(i=0;i<=2;i++){
        for(j=0;j<=15;j++){
            SysRunSt.ErrStLast[i][j]=SysRunSt.ErrSt[i][j];
        }
    }
}


/*****************************************************************************//*!
*
* @brief   simulink   .
*     
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void Simulink(void){
    
}
