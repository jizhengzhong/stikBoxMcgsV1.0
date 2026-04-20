/*
 * Modbus slave mode data control.
 * Copyright (c) 2012  apleilx
 * All rights reserved.
 * no limite
 */

#define _MBDATACB_MODULE_
#include "mbdatacb.h"
#include "mbinputdatacfg.h"
#include "partab.h"
#include "systask.h"
#include "sysfun.h"
#include "pincfg.h"
#include "ledctr.h"


extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;


uint8_t bParSaveReq = 0;
uint16_t RegHoldRead(uint16_t Index);
uint16_t RegHoldWrite(uint16_t Index, uint16_t Val);

#if MB_REG_INPUT_CB_ENABLE >0 
uint8_t eMBRegInputCB(uint8_t * pucRegBuffer, uint16_t usAddress, uint16_t usNRegs)
{
    uint16_t             iRegIndex;
    mbU16Type           Data;
       
        iRegIndex = (int)(usAddress - REG_INPUT_START);
        while (usNRegs > 0)
        {
            if(iRegIndex < REG_INPUT_NREGS)
                Data.Val = RegInput[iRegIndex++].uVal;
            else
                Data.Val = 0;
            
            *pucRegBuffer++ = Data.byte.HB;
            *pucRegBuffer++ = Data.byte.LB;
            usNRegs--;
        }
    
        return MB_ERR_NONE;
}

#endif

#if MB_REG_HOLDING_CB_ENABLE > 0
uint8_t eMBRegHoldingCB(uint8_t  * pucRegBuffer, uint16_t usAddress, uint16_t usNRegs, mbRegModeType eMode)
{
    uint16_t             iRegIndex;
    mbU16Type           Data;

#if REG_HOLDING_START == 0
    if ((usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
#else
    if ((usAddress >= REG_HOLDING_START) && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
#endif        
        iRegIndex = (usAddress - REG_HOLDING_START);
        switch (eMode)
        {
            /* Pass current register values to the protocol stack. */
            case MB_REG_READ:
                while (usNRegs > 0)
                {
                    Data.Val =  RegHoldRead(iRegIndex++);
                    *pucRegBuffer++ = Data.byte.HB;
                    *pucRegBuffer++ = Data.byte.LB;
                    usNRegs--;
                }
                break;

            case MB_REG_WRITE:
                while (usNRegs > 0)
                {
                    Data.byte.HB = *pucRegBuffer++;
                    Data.byte.LB = *pucRegBuffer++;
                    RegHoldWrite(iRegIndex, Data.Val);

                    iRegIndex++;
                    usNRegs--;
                }
        }
    }
    else
        return MB_ERR_NOREG;
    return MB_ERR_NONE;
    }

/*****************************************************************************//*!
* @brief   hold read.
*
* @param   index: 数据地址
*
* @return  data:数据
* 地址区间划分：
* 0x100 获取系统参数
* 0x200 获取系统参数
* 0x280 命令/运行状态
* 0x400 调试
* @ Pass/ Fail criteria: none
*****************************************************************************/
    

    
uint16_t RegHoldRead(uint16_t Index)
{
    mbU16Type Data;

    Data.uVal = 0;

    //获取参数
    if ((Index >= 0x1000) && (Index <= (0x1000 + PAR_POS_END)))
    {
        Data.uVal = ParTab[Index - 0x1000].uVal;
        ParReadSt[Index - 0x1000] = 1;
        
        if( Index == 0x1039 )
        {  
           Data.uVal =SysRunSt.TimeCnt/60; 
        }
       
    }
    else
    {
        switch (Index)
        {
            case 0x400: //运行状态
                Data.uVal = SysRunSt.bSysRun;
                break;
            case 0x401: //杀菌状态
                Data.uVal = SysRunSt.bMjRun;
                break;
            case 0x410:
                Data.iVal = SysRunSt.TempMainDisp;
                break;
            case 0x411:
                Data.iVal = SysRunSt.TempDoorDisp;
                break;
            case 0x412:
                Data.iVal = SysRunSt.ColDisp;
                break;
            case 0x413:
                Data.iVal = SysRunSt.HumiDisp;
                break;
            case 0x414:
                 Data.iVal = SysRunSt.TempEvnDisp;
                 break;
            case 0x415:
                  Data.iVal = SysErrGet(ALARM_DOR_OPEN);
                  break;
            case 0x440:
                Data.iVal = SysErrGet(ERR_TEMP_AL);
                break;
            case 0x441:
                Data.iVal = SysErrGet(ERR_TEMP_AH);
                break;
            case 0x450:
                Data.iVal = SysErrGet(ERR_CO2_AL);
                break;
            case 0x451:
                Data.iVal = SysErrGet(ERR_CO2_AH);
                break;
            case 0x500:
                //加热输出指示
                Data.iVal = ((OUT_FRAME_HEAT == SSR_OUT_ENABLE) ? 1 : 0);
                break;
            case 0x501:
                //co2输出指示
                Data.iVal = ((OUT_CO2_CRL1 == JK_OUT_ENABLE) ? 1 : 0);
                break;
            case 0x60A:
                //水位检测
                Data.iVal = SysRunSt.Io.InPut.bits.in_RHW_SW;
                break;   
            default:
                Data.uVal = 0;
                break;
        }

    }

    return Data.uVal;
}

/*****************************************************************************//*
* @brief   hold read.
*
* @param   index: 数据地址
*
* @return  data:数据
* 地址区间划分：
* 0x100 写入系统参数
* 0x280 命令/运行状态
* 0x400 调试
* @ Pass/ Fail criteria: none
*****************************************************************************/

uint16_t RegHoldWrite(uint16_t Index, uint16_t Val)
{
    RTC_HandleTypeDef RTC_Handler;  
  
    mbU16Type   Data;

    Data.uVal = Val;

    if ((Index >= 0x1000) && (Index <= (0x1000 + PAR_POS_END)))
    {
        uint16_t pos;
        pos = Index - 0x1000;
        if (ParCheck(pos, Data.iVal))
        {                    //参数检测成功后更新数据
          ParTab[pos].uVal = ParReceive[pos].uVal = Val;
//          if(pos==PAR_POS_TEL1_5||pos==PAR_POS_TEL2_5||pos==PAR_POS_TEL5_5||  \
//             pos==PAR_POS_TEL6_5||pos==PAR_POS_TEL8_5||pos==PAR_POS_TEL9_5||
//             pos==PAR_POS_TEL10_5||pos==PAR_POS_TEL11_5||pos==PAR_POS_TEL12_5||PAR_POS_TEL7_5)

        }       

// ///////////////////////////////////////ji_add///////////////////
//        if( Index == 0x1000+ PAR_POS_ALRM_FLAG )
//          ParTab[pos].uVal = ParReceive[pos].uVal = Val;
  if( ParTab[PAR_POS_TEMP_SET].uVal > ParTab[PAR_POS_SET_MAX_TEMP].uVal)
         ParTab[PAR_POS_TEMP_SET].uVal = 0;
  if( ParTab[PAR_POS_COL_SET].uVal > ParTab[PAR_POS_COL_RH].uVal) 
     ParTab[PAR_POS_COL_SET].uVal = ParTab[PAR_POS_COL_RH].uVal;      
  if( ParTab[PAR_POS_COL_SET].uVal < ParTab[PAR_POS_COL_RL].uVal )
     ParTab[PAR_POS_COL_SET].uVal = ParTab[PAR_POS_COL_RL].uVal;
/////////////////////////////////////////ji_end/////////////////// 

    }
    else
    {
        switch (Index)
        {
            case 0x400: //启动命令
                if ((Val == 0x33) || (Val == 0x03) || (Val == 0x01)) 
                {
                   /////////////////////ji_add////////////////////// //////                  
                    SysRunSt.rhw_sw_state = 0;
                    SysRunSt.bCo2StableSta = 0;
                    SysRunSt.bTempStableSta = 0;

///////////////////////ji_end///////////////////////////   
                    SysRunSt.bSysRun = 1;

                }
                else if ((Val == 0xCC) || (Val == 0x0C) || (Val == 0x00)) 
                {
                    SysRunSt.bSysRun = 0;
                }
                else if ((Val == 0xFF)|| (Val == 0x0F) || (Val == 0x02)) 
                {
                    if (SysRunSt.bSysRun)
                    {
                        SysRunSt.bSysRun = 0;
                    }
                    else
                    {
                       if( SysRunSt.bMjRun != 1 )
                       {
                        SysRunSt.bSysRun = 1;
                        SysRunSt.bMjRun = 0;
                        SysRunSt.TimeCnt =0;
                       }

                    }
                }
        ParTab[PAR_POS_START_STOP].uVal = ParReceive[PAR_POS_START_STOP].uVal = SysRunSt.bSysRun;
          
                break;
            case 0x401:
                if ((Val == 0x33) || (Val == 0x03) || (Val == 0x01)) 
                {
                    SysRunSt.bMjRun = 1;
                }
                else if ((Val == 0xCC) || (Val == 0x0C) || (Val == 0x00)) 
                {
                    SysRunSt.bMjRun = 0;
                }
                else if ((Val == 0xFF)|| (Val == 0x0F) || (Val == 0x02)) 
                {
                    if (SysRunSt.bMjRun)
                    {
                        SysRunSt.bMjRun = 0;
                    }
                    else
                    {
                        SysRunSt.bMjRun = 1;
 //                       SysRunSt.bSysRun = 0;
                        SysRunSt.TimeCnt = 0;
                    }
                  if( SysRunSt.bSysRun == 1)
                    {
                     SysRunSt.bMjRun = 0;
                    }
                }
                break;
            case 0x402:

                break;
            default:
                break;
        }
    }

    return Val;
}

#endif

#if MB_REG_COILS_CB_ENABLE > 0
uint8_t eMBRegCoilsCB(uint8_t  * pucRegBuffer, uint16_t usAddress, uint16_t usNCoils, mbRegModeType eMode)
{

    mbU16Type          DataWR;
    uint16_t          iRegIndex;
    uint8_t           BitOffset;
    uint8_t           ReadMask;
    mbU16Type          WriteMask;

#if REG_COILS_START == 0
    if ((usAddress + usNCoils <= REG_COILS_START + REG_COILS_NREGS * 8))
    {
#else
    if ((usAddress >= REG_COILS_START) && (usAddress + usNCoils <= REG_COILS_START + REG_COILS_NREGS * 8))
    {
#endif
        BitOffset = usAddress % 8;
        iRegIndex = (usAddress - REG_COILS_START) / 8;

        switch (eMode)
        {
            /* Pass current register values to the protocol stack. */
            case MB_REG_READ:
                DataWR.byte.HB = RegCoil[iRegIndex++];
                DataWR.Val >>= (BitOffset);
                while (usNCoils > 0)
                {
                    DataWR.Val >>= (8 - BitOffset);
                    DataWR.byte.HB = RegCoil[iRegIndex++];
                    DataWR.Val >>= (BitOffset);
                    if (usNCoils < 8)
                    {
                        ReadMask = 0xFF;
                        DataWR.byte.LB &= ~(ReadMask << usNCoils);
                        usNCoils = 0;
                    }
                    else
                        usNCoils -= 8;
                    *pucRegBuffer++ = DataWR.byte.LB;
                }
                break;

                /* Update current register values with new values from the
                 * protocol stack. */
            case MB_REG_WRITE:
                while (usNCoils > 0)
                {
                    DataWR.byte.LB = *pucRegBuffer++;
                    DataWR.Val <<= BitOffset;
                    WriteMask.Val = 0xFF;
                    if (usNCoils < 8)
                    {
                        WriteMask.Val >>= (8 - usNCoils);
                        usNCoils = 0;
                    }
                    else
                        usNCoils -= 8;
                    WriteMask.Val <<= BitOffset;

                    DataWR.Val &= WriteMask.Val;
                    RegCoil[iRegIndex] &= ~WriteMask.byte.LB;
                    RegCoil[iRegIndex + 1] &= ~WriteMask.byte.HB;
                    RegCoil[iRegIndex] |= DataWR.byte.LB;
                    RegCoil[iRegIndex + 1] |= DataWR.byte.HB;
                    iRegIndex++;
                }
        }
    }
    else
        return MB_ERR_NOREG;
    return MB_ERR_NONE;
    }

#endif

#if MB_REG_DISCRETE_CB_ENABLE > 0
uint8_t eMBRegDiscreteCB(uint8_t  *pucRegBuffer, uint16_t usAddress, uint16_t usNCoils)
{

    mbU16Type          DataWR;
    uint16_t          iRegIndex;
    uint8_t           BitOffset;
    uint8_t           ReadMask;

#if REG_DISCRETE_START == 0
    if ((usAddress + usNCoils <= REG_DISCRETE_START + REG_DISCRETE_NREGS * 8))
    {
#else
    if ((usAddress >= REG_DISCRETE_START) && (usAddress + usNCoils <= REG_DISCRETE_START + REG_DISCRETE_NREGS * 8))
    {
#endif        
        BitOffset = usAddress % 8;
        iRegIndex = (usAddress - REG_DISCRETE_START) / 8;

        DataWR.byte.HB = RegDiscrete[iRegIndex++];
        DataWR.Val >>= (BitOffset);
        while (usNCoils > 0)
        {
            DataWR.Val >>= (8 - BitOffset);
            DataWR.byte.HB = RegDiscrete[iRegIndex++];
            DataWR.Val >>= (BitOffset);
            if (usNCoils < 8)
            {
                ReadMask = 0xFF;
                DataWR.byte.LB &= ~(ReadMask << usNCoils);
                usNCoils = 0;
            }
            else
                usNCoils -= 8;
            *pucRegBuffer++ = DataWR.byte.LB;
        }
    }
    else
        return MB_ERR_NOREG;
    return MB_ERR_NONE;
    }

#endif


