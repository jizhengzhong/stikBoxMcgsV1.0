/*
* 温度和CO2  PID 控制
*/

#define _CONTROL_MODULE_
#include "osObjects.h"
#include "control.h"
#include "pincfg.h"
#include "pulse.h"
#include "pid.h"
#include "partab.h"
#include "systask.h"
#include "templib.h"
#include "sys.h"
#include "math.h"
#include "sysfun.h"


timer_pulse_type CtrPls;

PID_Type TempPid = { 0 }, Co2Pid = { 0 };
PID_AUTO_SET_Type TempPidAutoSet = { 0 }, Co2PidAutoSet = { 0 };

//PID索引映射
static uint8_t PAR_POS_TEMP_P;
static uint8_t PAR_POS_TEMP_I;
static uint8_t PAR_POS_TEMP_D;
static uint8_t PAR_POS_TEMP_T;

static uint8_t PAR_POS_COL_P;
static uint8_t PAR_POS_COL_I;
static uint8_t PAR_POS_COL_D;
static uint8_t PAR_POS_COL_T;

extern volatile uint32_t uwTick;

void TempCtr(void);
void Co2Ctr(void);

/////////////////ji_start//////////////////////////////

PID_Type DoorPid = { 0 };
PID_AUTO_SET_Type DoorPidAutoSet = { 0 };

void DoorCtr(void);
void DoorAutoSetParSave(void);

static uint8_t PAR_POS_DOOR_P;
static uint8_t PAR_POS_DOOR_I;
static uint8_t PAR_POS_DOOR_D;
static uint8_t PAR_POS_DOOR_T;
//////////////////ji_end///////////////////////////

void TempAutoSetParSave(void);
void Co2AutoSetParSave(void);

uint8_t pidtime_list[256];
uint8_t pidtime_list_index;
void CtrUpdate(void);

/*****************************************************************************//*!
*
* @brief   control task.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void ControlTask(void const *argument)
{
    osDelay(5000);

    sysapi->pulse_init(&CtrPls, uwTick);//uwTick功能1ms加1

    for (;;)
    {

        sysapi->PulseCreat(&CtrPls, uwTick);//脉冲创建

        CtrUpdate();

        TempCtr();
        
        DoorCtr();

        Co2Ctr();
        
        osDelay(50);
    }
}

/*****************************************************************************//*!
*
* @brief   control.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void CtrUpdate(void)
{
    if (SysRunSt.bSysRun)
    {
        //常规运行，温度+CO2
        SysRunSt.bTempRun = 1;
        SysRunSt.bCo2Run = 1;
        SysRunSt.TempMainSet = ParTab[PAR_POS_TEMP_SET].iVal;
        SysRunSt.ColSet = ParTab[PAR_POS_COL_SET].iVal*10;
 //       SysRunSt.TempDoorSet = ParTab[PAR_POS_COL_SET].iVal;
       
    }
    else if (SysRunSt.bMjRun)
    {
        //消毒运行，仅温度
        SysRunSt.bTempRun = 1;
        SysRunSt.bCo2Run = 0;
        if( ParTab[PAR_POS_MJ_MODE].iVal != 0 )
        SysRunSt.TempMainSet = ParTab[PAR_POS_MJ_TEMP].iVal;
        else
        SysRunSt.TempMainSet = 0;
    }
    else
    {
        //停止状态
        SysRunSt.bTempRun = SysRunSt.bCo2Run = 0;
    }
    if( ParTab[PAR_POS_TEMP_SET].iVal > ParTab[PAR_POS_TEMP_RH].iVal )
    {
       ParTab[PAR_POS_TEMP_SET].iVal = ParTab[PAR_POS_TEMP_RH].iVal;
    }

    //运行计时
    if (SysRunSt.bSysRun)
    {         
        SysRunSt.TimeSet = ParTab[PAR_POS_TIME_SET].iVal * 60;
        if (SysRunSt.TimeSet)
        {
            if (CtrPls.bPulse_1s)
            {
              if( ParTab[PAR_POS_RECKON_TIME].uVal == 0)
              {
                SysRunSt.TimeCnt++;
              }else
              {
                 if( SysRunSt.TempMainDisp - SysRunSt.TempMainSet <= ParTab[PAR_POS_SET_CONS_TEMP_DEV].uVal+0.5   && \
                     SysRunSt.TempMainDisp - SysRunSt.TempMainSet >= -ParTab[PAR_POS_SET_CONS_TEMP_DEV].uVal   ) //建议上下温差
                   SysRunSt.TimeCnt++;
              }
            }
            if (SysRunSt.TimeCnt >= SysRunSt.TimeSet)
            {
                SysRunSt.bSysRun = 0;
            }
        }
        else
        {
            SysRunSt.TimeCnt = 0;
        }
    }
    else
    {
      if (!SysRunSt.bMjRun)
         SysRunSt.TimeCnt = 0;
    }

    //灭菌计时
    if (SysRunSt.bMjRun)
    {
        SysRunSt.MjTimeSet = ParTab[PAR_POS_MJ_TIME].iVal * 60;
        ParTab[PAR_POS_TIME_SET].iVal = ParTab[PAR_POS_MJ_TIME].iVal;
        if (SysRunSt.MjTimeSet)
        {
            if (CtrPls.bPulse_1s)
            {
              SysRunSt.Io.OutPut.bits.out_WATER_PUMP=1;
              if( (SysRunSt.TempMainDisp - ParTab[PAR_POS_MJ_TEMP].uVal <= ParTab[PAR_POS_SET_CONS_TEMP_DEV].uVal+5   && \
                     SysRunSt.TempMainDisp - ParTab[PAR_POS_MJ_TEMP].uVal>= -ParTab[PAR_POS_SET_CONS_TEMP_DEV].uVal)  ||
                   ( ParTab[PAR_POS_MJ_MODE].iVal == 0 )) //建议上下温差紫外
              { 
                SysRunSt.MjTimeCnt++;
                SysRunSt.TimeCnt =  SysRunSt.MjTimeCnt;
              }
            }
            if (SysRunSt.MjTimeCnt >= SysRunSt.MjTimeSet)
            {
                SysRunSt.bMjRun = 0;
                 SysRunSt.Io.OutPut.bits.out_WATER_PUMP=0;
            }
        }
        else
        {
            SysRunSt.MjTimeCnt = 0;
        }
    }
    else
    {
        SysRunSt.MjTimeCnt = 0;
//        SysRunSt.Io.OutPut.bits.out_WATER_PUMP=0;   //阀切换时注释掉
    }
    
     
}
///////////////////////ji_start/////////////////////////////////

/*****************************************************************************//*!
* @brief   door control.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void DoorCtr(void)
{
    static uint8_t bDoorRunLast = 0;
    static uint16_t DoorCtrDelay = 5000;
    
   if (SysRunSt.TempDoorSet <= 600)
    {    
///////////////ji_start//////////////////////
 
    PAR_POS_DOOR_P = PAR_POS_DOOR_P1;
    PAR_POS_DOOR_I = PAR_POS_DOOR_I1;
    PAR_POS_DOOR_D = PAR_POS_DOOR_D1;
    PAR_POS_DOOR_T = PAR_POS_DOOR_T1; 
    
///////////////ji_end//////////////////////
    }  
    //测量值更新
    DoorPid.Pv = SysRunSt.Io.fTempDoor;

    //PID参数设置
    DoorPid.DeadZone = 0.1f;//乘以0.1，和目标值相差0.01认为稳定；
    DoorPid.Cycle = 1;   //计时单位1S
    DoorPid.Sv = SysRunSt.TempMainSet;  //设定目标温度
    DoorPid.outFac = ParTab[PAR_POS_DOOR_T].uVal;  //输出量化系数对应为设置的时间周期，如加热设置为2，即量化后2s
    DoorPid.outFac_offs = 0;  //起始输出功率偏移量；对应的加热输出功率初始化为0，但是电机起始位置可能不是0，
    DoorPid.OutCoefFlt = 0.8;//输出弱化，一般在制冷上，
    DoorPid.OutCoef = 1.0f;  
    DoorPid.DvWeakMax = 3.0;//单一方向上升的快降的慢，或者升的慢，降的快
    DoorPid.DvWeak = 0.2;
    DoorPid.DvWeakType = DWK_NONE;
    DoorPid.q15_DirAdjZone = Q_15_VAL_0_25F;//观察窗，
    DoorPid.ILimiteLineL = 0;
    DoorPid.ILimiteLineH = 0;

    if (SysRunSt.bTempRun)
    {
        //运行状态
        sysapi->PidUpdate(&DoorPid, &ParTab[PAR_POS_DOOR_P].uVal, 0, 1000);//结合TempPid.outFac       
          
        if (!bDoorRunLast)//记录上次门运行
        {
            sysapi->PidClr(&DoorPid);
            DoorPid.KtCnt = DoorPid.Kt; //直接开始计算，Kt是屏幕上设置的周期
        }
        if (CtrPls.bPulse_1s)
        {
            pidtime_list[pidtime_list_index++] = DoorPid.KtCnt; //调试时看PID是否计时
            if (SysRunSt.Io.InPut.bits.in_DOOR_SW)  //如果门开关状态为1  检测到关门
            {
                //关门30s内不计算PID
                if(DoorCtrDelay < 30)
                {
                    sysapi->PidClr(&DoorPid);
                    DoorPid.KtCnt = DoorPid.Kt;
                }
                else
                {
                    sysapi->IndPid(&DoorPid); //PID运算  输入微分
                }
                
                //关门60s内不加热
                if(DoorCtrDelay < 60)
                {
                    DoorPid.Out = 0;
                }
            }
            else
            {
                //开门不计算PID
                sysapi->PidClr(&DoorPid);
                DoorPid.KtCnt = DoorPid.Kt;
            }
        }
    }
    else
    {
        sysapi->PidClr(&DoorPid);
        DoorCtrDelay = 10 * 60;
    }
    
    if(CtrPls.bPulse_1s && (DoorCtrDelay<10000) )
    {
        DoorCtrDelay ++;
    }
    
    if (!SysRunSt.Io.InPut.bits.in_DOOR_SW)
    {
        DoorCtrDelay = 0;
    }
    
    SysRunSt.Io.AdjDorOutDuty = DoorPid.Out;  //调节温度输出占空比
    SysRunSt.Io.AdjDorPeriod = 1000 * DoorPid.Kt;

    bDoorRunLast = SysRunSt.bTempRun; //记录运行状态

}
//////////////////////ji_end////////////////////////////////////

/*****************************************************************************//*!
* @brief   temp control.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void TempCtr(void)
{
    static uint8_t bTempRunLast = 0;
    static uint16_t ParAutoSetLast = 0;
    static uint16_t DoorCtrDelay = 5000;
    static uint8_t bDoorRunOpnState=0;
    static uint8_t  DoorOpenAlarm=0;
    //温控PID切换
    if (SysRunSt.TempMainSet <= 500)
    {
      
///////////////ji_start//////////////////////

        if( TempPid.Sv-SysRunSt.TempMainDisp>35 )
          {
            PAR_POS_TEMP_P = PAR_POS_TEMP_P1;
            PAR_POS_TEMP_I = PAR_POS_TEMP_I1;
            PAR_POS_TEMP_D = PAR_POS_TEMP_D1;
            PAR_POS_TEMP_T = PAR_POS_TEMP_T1;
          
          }
        else 
          {
            if(bDoorRunOpnState==1)
            {
                PAR_POS_TEMP_P = PAR_POS_TEMP_P2;
                PAR_POS_TEMP_I = PAR_POS_TEMP_I2;
                PAR_POS_TEMP_D = PAR_POS_TEMP_D2;
                PAR_POS_TEMP_T = PAR_POS_TEMP_T2;
            }
            else
            {
              
              PAR_POS_TEMP_P = PAR_POS_TEMP_P1;
              PAR_POS_TEMP_I = PAR_POS_TEMP_I1;
              PAR_POS_TEMP_D = PAR_POS_TEMP_D1;
              PAR_POS_TEMP_T = PAR_POS_TEMP_T1;

            }
 
          }       
    }
    else
    {
        PAR_POS_TEMP_P = PAR_POS_MJ_P;
        PAR_POS_TEMP_I = PAR_POS_MJ_I;
        PAR_POS_TEMP_D = PAR_POS_MJ_D;
        PAR_POS_TEMP_T = PAR_POS_MJ_T;
    }

    //自整定控制
    TempPidAutoSet.CtrDir = 0; //控制加热一般为0  制冷反作用设为1
    TempPidAutoSet.CtrZone = 1.0f; //乘以0.1控制带；
    TempPidAutoSet.IdCoef = 0.8f;//微分是积分的0.8倍
    TempPidAutoSet.Pid = &TempPid;
    TempPidAutoSet.ParSave = &TempAutoSetParSave;
    if (ParAutoSetLast != ParTab[PAR_POS_TEMP_AT].uVal)
    {
        if (ParTab[PAR_POS_TEMP_AT].uVal)
        {
            //开启自整定
            sysapi->PidAutoSetStart(&TempPid, &TempPidAutoSet);
        }
        else
        {
            //关闭自整定
            sysapi->PidAutoSetExit(&TempPidAutoSet);
        }
    }

    //测量值更新
    TempPid.Pv = SysRunSt.Io.fTempMain;

    //PID参数设置
    TempPid.DeadZone = 0.1f;//乘以0.1，和目标值相差0.01认为稳定；
    TempPid.Cycle = 1;   //计时单位1S
    TempPid.Sv = SysRunSt.TempMainSet;  //设定目标温度
    TempPid.outFac = ParTab[PAR_POS_TEMP_T].uVal;  //输出量化系数对应为设置的时间周期，如加热设置为2，即量化后2s
    TempPid.outFac_offs = 0;  //起始输出功率偏移量；对应的加热输出功率初始化为0，但是电机起始位置可能不是0，
    TempPid.OutCoefFlt = 0.8;//输出弱化，一般在制冷上，
    TempPid.OutCoef = 1.0f;  //
    TempPid.DvWeakMax = 3.0;//单一方向上升的快降的慢，或者升的慢，降的快
    TempPid.DvWeak = 0.2;
    TempPid.DvWeakType = DWK_NONE;
    TempPid.q15_DirAdjZone = Q_15_VAL_0_25F;//观察窗，
    TempPid.ILimiteLineL = 0;
    TempPid.ILimiteLineH = 0;

    //输出控制
    if (TempPidAutoSet.State)
    {
        //自整定状态
        if (CtrPls.bPulse_500ms)
        {
            if (sysapi->PidAutoSet(&TempPidAutoSet))//返回值判断要不要输出
            {
                TempPid.Out = (int32_t)(TempPid.outFac * TempPid.OutMax);
            }
            else
            {
                TempPid.Out = 0;
            }
        }

        if (TempPidAutoSet.State == 0xFF)
        {
            sysapi->PidAutoSetExit(&TempPidAutoSet);
            sysapi->PidUpdate(&TempPid, &ParTab[PAR_POS_TEMP_P].uVal, 0, 1000);
            sysapi->PidClr(&TempPid);
            ParTab[PAR_POS_TEMP_AT].uVal = 0;  //开启自整定的变量清零，退出自整定；
        }
    }
    else if (SysRunSt.bTempRun)
    {
              ///////////////////////////jiadd/////////////////
       if( SysRunSt.ColDisp  >= SysRunSt.ColSet - 10 && DoorCtrDelay > 180 ) //建议上下温差
            SysRunSt.bCo2StableSta = 1 ;
        if( SysRunSt.TempMainDisp - SysRunSt.TempMainSet <= ParTab[PAR_POS_SET_CONS_TEMP_DEV].uVal+0.5   && \
           SysRunSt.TempMainDisp - SysRunSt.TempMainSet >= -ParTab[PAR_POS_SET_CONS_TEMP_DEV].uVal  && DoorCtrDelay > 180  ) //建议上下温差
            SysRunSt.bTempStableSta = 1 ;
      
        //运行状态
        sysapi->PidUpdate(&TempPid, &ParTab[PAR_POS_TEMP_P].uVal, 0, 1000);//结合TempPid.outFac

        if (!bTempRunLast)
        {
            sysapi->PidClr(&TempPid);
            TempPid.KtCnt = TempPid.Kt; //直接开始计算，Kt是屏幕上设置的周期
        }
        if (CtrPls.bPulse_1s)
        {
            pidtime_list[pidtime_list_index++] = TempPid.KtCnt; //调试时看PID是否计时
            if (SysRunSt.Io.InPut.bits.in_DOOR_SW)
            {
              DoorOpenAlarm=0; 
              SysRunSt.door_open_state=0;
              OUT_FSCRL3 = 1;

                //关门30s内不计算PID
                if(DoorCtrDelay < 30)
                {
                    sysapi->PidClr(&TempPid); //清楚PID参数
                    TempPid.KtCnt = TempPid.Kt;
                }
                else
                {
                    sysapi->IndPid(&TempPid); //PID运算  输入微分
                }
                
                //关门60s内不加热
                if(DoorCtrDelay < 60)
                {
                    TempPid.Out = 0; 
                    
                    bDoorRunOpnState=1;//ji add
                }
            }
            else
            {
                //开门不计算PID
                sysapi->PidClr(&TempPid);
                TempPid.KtCnt = TempPid.Kt;
//////////////////////////ji_add//////////////////////
              SysRunSt.bCo2StableSta = 0 ;
              SysRunSt.bTempStableSta = 0;
              OUT_FSCRL3 = 0;
////////////////////// ///ji_end//////////////////////  
            }
        }
    }
    else
    {
        sysapi->PidClr(&TempPid);
        DoorCtrDelay = 10 * 60;
        bDoorRunOpnState=0;//ji add
           OUT_FSCRL3 = 0;
    }
    
    if(CtrPls.bPulse_1s && (DoorCtrDelay<1000) )
    {
        DoorCtrDelay ++;
    }
    
    if (!SysRunSt.Io.InPut.bits.in_DOOR_SW)
    {
        DoorCtrDelay = 0;
       if(CtrPls.bPulse_1s && (DoorOpenAlarm<99) )
       {
        DoorOpenAlarm++;
       }
        if( DoorOpenAlarm > ParTab[PAR_POS_Door_Open_DLY].uVal  )
        {
          SysRunSt.door_open_state = 1;
        }
    }
     /////////////////////////ji_add////////////////////////
          if( !SysRunSt.Io.InPut.bits.in_RHW_SW )
          {
            SysRunSt.rhw_sw_state++;
          }
          else
          {
            SysRunSt.rhw_sw_state = 0;
            SysErrClr(ERR_SW_AL);
          }
          
 ////////////////////////ji_end////////////////////////     
    
    SysRunSt.Io.AdjTempOutDuty = TempPid.Out;  //调节温度输出占空比
    SysRunSt.Io.AdjTempPeriod = 1000 * TempPid.Kt;

    bTempRunLast = SysRunSt.bTempRun; //记录运行状态
    ParAutoSetLast = ParTab[PAR_POS_TEMP_AT].uVal; //记录自整定状态
}
/*****************************************************************************//*!
* @brief   co2 control.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void Co2Ctr(void)
{
    static uint8_t bCo2RunLast = 0;
    static uint16_t ParAutoSetLast = 0;

    //CO2 PID 切换
    if (SysRunSt.ColSet < 100)
    {
        PAR_POS_COL_P = PAR_POS_COL_P1;
        PAR_POS_COL_I = PAR_POS_COL_I1;
        PAR_POS_COL_D = PAR_POS_COL_D1;
        PAR_POS_COL_T = PAR_POS_COL_T1;
    }
    else
    {
        PAR_POS_COL_P = PAR_POS_COL_P2;
        PAR_POS_COL_I = PAR_POS_COL_I2;
        PAR_POS_COL_D = PAR_POS_COL_D2;
        PAR_POS_COL_T = PAR_POS_COL_T2;
    }

    //自整定控制
    Co2PidAutoSet.CtrDir = 0;
    Co2PidAutoSet.CtrZone = 1.0f;
    Co2PidAutoSet.IdCoef = 0.8f;
    Co2PidAutoSet.Pid = &Co2Pid;
    Co2PidAutoSet.ParSave = &Co2AutoSetParSave;
    if (ParAutoSetLast != ParTab[PAR_POS_COL_AT].uVal)
    {
        if (ParTab[PAR_POS_COL_AT].uVal)
        {
            sysapi->PidAutoSetStart(&Co2Pid, &Co2PidAutoSet);
        }
        else
        {
            sysapi->PidAutoSetExit(&Co2PidAutoSet);
        }
    }

    //测量值更新
    Co2Pid.Pv = SysRunSt.Io.fCol;//浓度更新

    //PID参数设置
    Co2Pid.DeadZone = 0.1f;
    Co2Pid.Cycle = 1;
    Co2Pid.Sv = SysRunSt.ColSet;
    Co2Pid.Sv /= 10; //转换为1位小数 
    Co2Pid.outFac = ParTab[PAR_POS_COL_T].uVal;
    Co2Pid.outFac_offs = 0;
    Co2Pid.OutCoefFlt = 0.8;
    Co2Pid.OutCoef = 1.0f;
    Co2Pid.DvWeakMax = 3.0;
    Co2Pid.DvWeak = 0.2;
    Co2Pid.DvWeakType = DWK_NONE;
    Co2Pid.q15_DirAdjZone = Q_15_VAL_0_25F;
    Co2Pid.ILimiteLineL = 0;
    Co2Pid.ILimiteLineH = 0;

    //输出控制
    if (Co2PidAutoSet.State)
    {
        if (CtrPls.bPulse_500ms)
        {
            if (sysapi->PidAutoSet(&Co2PidAutoSet))
            {
                Co2Pid.Out = (int32_t)(Co2Pid.outFac * Co2Pid.OutMax);
            }
            else
            {
                Co2Pid.Out = 0;
            }
        }

        if (Co2PidAutoSet.State == 0xFF)
        {
            sysapi->PidAutoSetExit(&Co2PidAutoSet);
            sysapi->PidUpdate(&Co2Pid, &ParTab[PAR_POS_COL_P].uVal, 0, 1000);
            sysapi->PidClr(&Co2Pid);
            ParTab[PAR_POS_COL_AT].uVal = 0;
        }
    }
    else if (SysRunSt.bCo2Run)
    {
        sysapi->PidUpdate(&Co2Pid, &ParTab[PAR_POS_COL_P].uVal, 0, 1000);        

        if (!bCo2RunLast)
        {
            sysapi->PidClr(&Co2Pid);
            Co2Pid.KtCnt = Co2Pid.Kt;
        }

        if (CtrPls.bPulse_1s)
        {
            if (SysRunSt.Io.InPut.bits.in_DOOR_SW)
            {
                sysapi->IndPid(&Co2Pid);
            }
            else
            {
                sysapi->PidClr(&Co2Pid);
                Co2Pid.KtCnt = Co2Pid.Kt;
                Co2Pid.OutCoef = 0.0f;
                Co2Pid.OutCoefFlt = 0.95;
            }
        }
    }
    else
    {
        sysapi->PidClr(&Co2Pid);
    }

    SysRunSt.Io.AdjColOutDuty = Co2Pid.Out;
    SysRunSt.Io.AdjColPeriod = 1000 * Co2Pid.Kt;

    bCo2RunLast = SysRunSt.bTempRun;
    ParAutoSetLast = ParTab[PAR_POS_COL_AT].uVal;
}

/*****************************************************************************//*!
* @brief   temp auto set par save.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void TempAutoSetParSave(void)
{
    /*需要保存的参数要同时保存在ParReceive中*/  
    ParTab[PAR_POS_TEMP_P].uVal = ParReceive[PAR_POS_TEMP_P].uVal = TempPidAutoSet.P;
    ParTab[PAR_POS_TEMP_I].uVal = ParReceive[PAR_POS_TEMP_I].uVal = TempPidAutoSet.I;
    ParTab[PAR_POS_TEMP_D].uVal = ParReceive[PAR_POS_TEMP_D].uVal = TempPidAutoSet.D;
}

/*****************************************************************************//*!
* @brief   co2 auto set par save.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void Co2AutoSetParSave(void)
{
    /*需要保存的参数要同时保存在ParReceive中*/
    ParTab[PAR_POS_COL_P].uVal = ParReceive[PAR_POS_COL_P].uVal = Co2PidAutoSet.P;
    ParTab[PAR_POS_COL_I].uVal = ParReceive[PAR_POS_COL_I].uVal = Co2PidAutoSet.I;
    ParTab[PAR_POS_COL_D].uVal = ParReceive[PAR_POS_COL_D].uVal = Co2PidAutoSet.D;
}



/*****************************************************************************//*!
* @brief   pwm out.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void PwmOutHandle(void)
{
    SysRunSt.Io.AdjTempOutCnt++;
    SysRunSt.Io.AdjColOutCnt++;

    if (SysRunSt.Io.AdjTempOutCnt >= SysRunSt.Io.AdjTempPeriod)
    {
        SysRunSt.Io.AdjTempOutCnt = 0;
    }
    if (SysRunSt.Io.AdjColOutCnt >= SysRunSt.Io.AdjColPeriod)
    {
        SysRunSt.Io.AdjColOutCnt = 0;
    }
    
/////////////////////ji_start  门加热移植部分/////////////////////////////////
    SysRunSt.Io.AdjDorOutCnt++;
    if (SysRunSt.Io.AdjDorOutCnt >= SysRunSt.Io.AdjDorPeriod)
    {
        SysRunSt.Io.AdjDorOutCnt = 0;
    }
    
    if (SysRunSt.Io.AdjDorOutCnt < SysRunSt.Io.AdjDorOutDuty)
    {
      if (SysRunSt.bMjRun)
         OUT_DOOR_HEAT = SSR_OUT_DISABLE;
      else
        OUT_DOOR_HEAT = SSR_OUT_ENABLE;
    }
    else
    {
        OUT_DOOR_HEAT = SSR_OUT_DISABLE;
    }

////////////////////ji_end////////////////////////////////////    
    

    if (SysRunSt.Io.AdjColOutCnt < SysRunSt.Io.AdjColOutDuty)
    {
 //       OUT_CO2_CRL1 = JK_OUT_ENABLE;
        OUT_CO2_CRL2 = JK_OUT_ENABLE;

    }
    else
    {
 //       OUT_CO2_CRL1 = JK_OUT_DISABLE;
        OUT_CO2_CRL2 = JK_OUT_DISABLE;
    }

    if (SysRunSt.Io.AdjTempOutCnt < SysRunSt.Io.AdjTempOutDuty)
    {
        OUT_FRAME_HEAT = SSR_OUT_ENABLE;
//        OUT_DOOR_HEAT = SSR_OUT_ENABLE;
    }
    else
    {

        OUT_FRAME_HEAT = SSR_OUT_DISABLE;
//        OUT_DOOR_HEAT = SSR_OUT_DISABLE;
    }
}



