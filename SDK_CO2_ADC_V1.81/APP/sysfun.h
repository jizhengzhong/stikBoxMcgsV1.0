
#ifndef __SYSFUNC_H
#define	__SYSFUNC_H

#include <stdint.h>
//#include "appdefine.h"
//#include "partab.h"
#include "systask.h"

#ifdef	__cplusplus
extern "C" {
#endif
	
#ifdef   _SYSFUN_MODULE_
#define  MY_EXT
#else
#define  MY_EXT  extern
#endif  

/*****************************************************************************//*!
*
* @brief   System error set  .
*     
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
#define SysErrSet(Msk)\
    (SysRunSt.ErrSt[(Msk>>8)&0x03][Msk & 0x0F] = 1)

/*****************************************************************************//*!
*
* @brief   System error clear  .
*     
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
#define SysErrClr(Msk)\
    (SysRunSt.ErrSt[(Msk>>8)&0x03][Msk & 0x0F] = 0)

/*****************************************************************************//*!
*
* @brief   System error get  .
*     
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
#define SysErrGet(Msk)\
    (SysRunSt.ErrSt[(Msk>>8)&0x03][Msk & 0x0F])


/*****************************************************************************//*!
*
* @brief   mec clear  .
*     
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
static inline void MemClr(uint8_t* pu8Address, uint32_t u32Count)
{
    while(u32Count--)
    {
        *pu8Address++ = 0;
    }
}

/*****************************************************************************//*!
*
* @brief   mem copy  .
*     
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
static inline void MemCpy(uint8_t* Des, uint8_t* Src, uint32_t u32Count)
{
    while(u32Count--)
    {
        *Des++ = *Src++;
    }
}

void SysErrMsk(void);
uint32_t IsSysErrFound(void);
void SysErrLastUpdate(void);
void Simulink(void);
float Temp2Press(float x) ;

#undef MY_EXT

#ifdef	__cplusplus
}
#endif

#endif	/* __SYSFUNC_H */
