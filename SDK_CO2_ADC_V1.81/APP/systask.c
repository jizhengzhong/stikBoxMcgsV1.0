/*****************************************************************************//*!
* file  : systask.c
* creat : apleilx
* data  : 2017/11/05
*******************************************************************************/
#define _RTC_TIME_MODULE_
#include "osObjects.h"
#include "pincfg.h"
#define _SYSTASK_MODULE_
#include "systask.h"
#undef _SYSTASK_MODULE_
#include "iichal.h"
#include "malloc.h"
#include "myiic.h"
#include "partab.h"
#include "sysfun.h"
#include "flashtoeep.h"
#include "stmflash.h"
#include "rtctime.h"
#include "main.h"
#include "string.h"
#include <stdbool.h>

#define SYS_TSK_PERIOD         20

/*****************************************************************************/
extern const partab_attr_type ParTabAttr[];
extern IWDG_HandleTypeDef hiwdg;

/*****************************************************************************/
/*****************************************************************************/

uint32_t SysTaskCnt;
uint32_t PowerTimeCnt;
uint8_t SysTaskAutoCnt;
uint8_t bFlashEepSaveReq=0;

void SysTimePulseCreat(void);
void sd_check(void);
void io_manage(void);
void SysParInit(void);
void hpvol_manage(void);
void MX_IWDG_Init(void);
void ErrHandle(void);

/*****************************************************************************//*!
*
* @brief   storage task.
*     
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void SysTask(void const *argument)
{ 
    IIC_Init();
    my_mem_init(SRAMIN);
    my_mem_init(SRAMCCM);
    SysParInit();
    SysRunSt.Io.OutPut.St = 0x0000;
    MX_IWDG_Init();
    
    for(;;){
      
        osDelay(SYS_TSK_PERIOD);
        SysTimePulseCreat();
        SysTaskCnt++;
        
        sd_check();
        io_manage();
        hpvol_manage();
        
        HAL_IWDG_Refresh(&hiwdg);
        
        ErrHandle();
    }
}

/*****************************************************************************//*!
*
* @brief   system time pulse creat.
*     
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
extern volatile uint32_t uwTick;
void SysTimePulseCreat(void){
    static uint32_t uwTickLast;
    volatile uint32_t tick;
    //sig_time_type GlbTime;
        
    uint16_t TickInc;
    
    tick = uwTick;
    TickInc = tick  - uwTickLast;
    uwTickLast = uwTick;

    bSysPulse_1s = 0;
    bSysPulse_500ms = 0;
    bSysPulse_250ms = 0;
    bSysPulse_100ms = 0;
    
    if(!TickInc)
        return;
    
    SysPulseTimeCnt_1s+=TickInc;
    SysPulseTimeCnt_500ms+=TickInc;
    SysPulseTimeCnt_250ms+=TickInc;
    SysPulseTimeCnt_100ms+=TickInc;
    
    if(SysPulseTimeCnt_1s >= (1000/1)){
        SysPulseTimeCnt_1s-= (1000/1);
        bSysPulse_1s = 1;

        if(SysRunSt.PowerUpTimeCnt<UINT32_MAX)
            SysRunSt.PowerUpTimeCnt ++;
        
        TimeCur.Sec++;
        if(TimeCur.Sec >= 60)
        {
            TimeCur.Sec=0;
            
                TimeCur.Min ++;
                {
                    if(TimeCur.Min >= 60)
                    {
                        TimeCur.Min = 0;
                    }
                }
         
        }
    }
    
    if(SysPulseTimeCnt_500ms >= (500/1)){
        SysPulseTimeCnt_500ms-= (500/1);
        bSysPulse_500ms = 1;
        bSysWave_500ms = !bSysWave_500ms;
    }
    
    if(SysPulseTimeCnt_250ms >= (250/1)){
        SysPulseTimeCnt_250ms-= (250/1);
        bSysPulse_250ms = 1;
        bSysWave_250ms = !bSysWave_250ms;
    }
    
    if(SysPulseTimeCnt_100ms >= (100/1)){
        SysPulseTimeCnt_100ms-= (100/1);
        bSysPulse_100ms = 1;
        
        SysTaskAutoCnt++;
        if(PowerTimeCnt < INT32_MAX)
            PowerTimeCnt++;
    }
}

/*****************************************************************************//*!
*
* @brief   err handle.
*        
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void ErrHandle(void)
{ 
    static bool LowTemplateBit=0;
    static bool OverTemplateBit=0;
    static bool LowCo2Bit=0;
    static bool OverCo2Bit=0;
    static bool DoorOpenBit=0;
    static unsigned char DoorCheckDelay;
  
        if( SysRunSt.rhw_sw_state > 5)
        {
          SysRunSt.bSysRun = 0;
          SysErrSet(ERR_SW_AL);
        }else
          SysErrClr(ERR_SW_AL);
    if(SysRunSt.bSysRun )
    { 
        if(  SysRunSt.TempMainDisp < SysRunSt.TempMainSet )
          {
            if(  SysRunSt.bTempStableSta ==1 && 
                 SysRunSt.TempMainDisp < SysRunSt.TempMainSet - ParTab[PAR_POS_TEMP_AL].iVal  &&
                 ParTab[PAR_POS_TEMP_AL].iVal > 0 )
            {
                SysErrSet(ERR_TEMP_AL);
                if(!LowTemplateBit)
                {
                Send_String("箱体欠温报警", strlen("箱体欠温报警"));
                LowTemplateBit = 1;
                }
            }
            else
            {
                SysErrClr(ERR_TEMP_AL);
                LowTemplateBit = 0;
            }
          }
        else if( SysRunSt.TempMainDisp > SysRunSt.TempMainSet )
          {
              if( SysRunSt.bTempStableSta ==1 && 
                  SysRunSt.TempMainDisp > SysRunSt.TempMainSet + ParTab[PAR_POS_TEMP_AH].iVal  && 
                  ParTab[PAR_POS_TEMP_AH].iVal > 0 )
              {
                SysErrSet(ERR_TEMP_AH);
                if( !OverTemplateBit)
                {
                   Send_String("箱体超温报警", strlen("箱体超温报警"));
                   OverTemplateBit=1;
                }
              }
              else
              {
                  SysErrClr(ERR_TEMP_AH);
                  OverTemplateBit=0;
              }
          }
        else
        {
         SysErrClr(ERR_TEMP_AL);
         SysErrClr(ERR_TEMP_AH);
        
        }
          
        
        if( ( ParTab[PAR_POS_DOOR_OPEN_SEL].uVal ==2  && SysRunSt.door_open_state == 0 )  \
         || (ParTab[PAR_POS_DOOR_OPEN_SEL].uVal == 1  && SysRunSt.door_open_state == 1 ) )
        {
           if(CtrPls.bPulse_1s && (DoorCheckDelay<ParTab[PAR_POS_Door_Open_DLY].uVal) )
            {
              DoorCheckDelay ++;
            }
           if(DoorCheckDelay>=ParTab[PAR_POS_Door_Open_DLY].uVal)
           {
              SysErrSet( ALARM_DOR_OPEN );
              if(!DoorOpenBit)
                {
                 Send_String("箱体开门报警", strlen("箱体开门报警"));
                  DoorOpenBit=1;
                }
           }
        }
        else 
        {
        SysErrClr(ALARM_DOR_OPEN); 
        DoorOpenBit=0;
        DoorCheckDelay = 0;
        }
    }
    else
    {
////////////////////ji_add////////////////      
        SysRunSt.bTempStableSta = 0;
///////////////////ji_end///////////////////
        SysErrClr(ERR_TEMP_AL);
        SysErrClr(ERR_TEMP_AH);
        SysErrClr(ALARM_DOR_OPEN); 
    }
    
        
    if(SysRunSt.bSysRun )
    {
      if( SysRunSt.ColDisp < SysRunSt.ColSet )
      {
          if( SysRunSt.bCo2StableSta ==1 && (SysRunSt.ColDisp < (SysRunSt.ColSet - ParTab[PAR_POS_COL_AL].iVal)) && ( ParTab[PAR_POS_COL_AL].iVal > 0))
          {
              SysErrSet(ERR_CO2_AL);
              if(!LowCo2Bit)
              {
              Send_String("箱体欠浓度报警", strlen("箱体欠浓度报警"));
              LowCo2Bit =1;
              }
          }
          else
          {
              SysErrClr(ERR_CO2_AL);
              LowCo2Bit=0;
          }
      }
      else if( SysRunSt.ColDisp > SysRunSt.ColSet )
      {
          if( SysRunSt.bCo2StableSta ==1 &&  
              SysRunSt.ColDisp > SysRunSt.ColSet + ParTab[PAR_POS_COL_AH].iVal &&   
                 ParTab[PAR_POS_COL_AH].iVal > 0 )
          {
              SysErrSet(ERR_CO2_AH);
              if(!OverCo2Bit)
              {
              Send_String("箱体超浓度报警", strlen("箱体超浓度报警"));
              OverCo2Bit= 1;
              }
          }
          else
          {
              SysErrClr(ERR_CO2_AH);
              OverCo2Bit=0;
          }
      }
      else
      {
      SysErrClr(ERR_CO2_AH);
      SysErrClr(ERR_CO2_AL);
      }
    }
    else
    {
/////////////////////////////ji_add/////////////////////////
        SysRunSt.bCo2StableSta = 0;
////////////////////////////ji_end/////////////////////////      
        SysErrClr(ERR_CO2_AL);
        SysErrClr(ERR_CO2_AH);
    }
    
    
//    SysErrMsk();
}

/*****************************************************************************//*!
*
* @brief   system par init.
*        
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void SysParInit(void)
{
    //系统设定参数加载
    ParLoad();
    ParTab[PAR_POS_COL_AT].iVal = 0;
    ParTab[PAR_POS_TEMP_AT].iVal = 0;
    
   if(ParTab[PAR_POS_VER_ID].uVal != PAR_VERSION_ID)
    {
        int Cnt;
        Cnt = PAR_POS_END;
        while(Cnt--){
            ParTab[Cnt].iVal = ParTabAttr[Cnt].init;
            ParSave(Cnt);
        }
    }
    
    //系统变量初始化
    MemClr((uint8_t*)&SysRunSt,sizeof(SysRunSt));
}

/*****************************************************************************//*!
*
* @brief   sd check.
*        
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void sd_check(void){
    static uint8_t list[3];
    
    if(SysPulseTimeCnt_100ms)
    {
        list[2] = list[1]; 
        list[1] = list[0]; 
        list[0] = IO_I_SD_ST; 
    }
    
    if(SysRunSt.sd_cd_st){
        if((list[1] != 0) && (list[0] != 0))
            SysRunSt.sd_cd_st = 0;
    } else {
        if((list[2] == 0) && (list[1] == 0) && (list[0] == 0))
            SysRunSt.sd_cd_st = 1;
    }
}

void io_manage(void)
{
    sys_in_type InStLast;
    sys_in_type InSt;
    uint32_t Cnt;
    uint32_t u32t;
    
    //记录上次值用于边沿判定
    InStLast.St = SysRunSt.Io.InPut.St;
        //状态获取
    InSt.St = 0;
    if(IN_EN_ST==IN_YL)
        InSt.bits.in_YL = 1;
    
    if(IN_EN_ST != IN_RHW_SW)
        InSt.bits.in_RHW_SW = 1;
    
    if(IN_EN_ST == IN_DOOR_SW)
        InSt.bits.in_DOOR_SW = 1;
    
    if(IN_EN_ST == IN_CO2TANK1)
        InSt.bits.in_CO2TANK1 = 1;
    
    if(IN_EN_ST == IN_CO2TANK2)
        InSt.bits.in_CO2TANK2 = 1;
    
    SysRunSt.Io.InPutList[3].St = SysRunSt.Io.InPutList[2].St;
    SysRunSt.Io.InPutList[2].St = SysRunSt.Io.InPutList[1].St;
    SysRunSt.Io.InPutList[1].St = SysRunSt.Io.InPutList[0].St;
    SysRunSt.Io.InPutList[0].St = InSt.St;
    
    //输入  状态  确认
    for(Cnt=0;Cnt<sizeof(sys_in_type);Cnt++) {
        if((SysRunSt.Io.InPutList[0].Stv[Cnt] == SysRunSt.Io.InPutList[1].Stv[Cnt])
        && (SysRunSt.Io.InPutList[0].Stv[Cnt] == SysRunSt.Io.InPutList[2].Stv[Cnt])
        && (SysRunSt.Io.InPutList[0].Stv[Cnt] == SysRunSt.Io.InPutList[3].Stv[Cnt])
        ){
            SysRunSt.Io.InPut.Stv[Cnt] = SysRunSt.Io.InPutList[0].Stv[Cnt];
         } 
    }
    //边沿
    u32t = InStLast.St ^ SysRunSt.Io.InPut.St; //边沿存在
    SysRunSt.Io.FoundEdge.St = u32t & SysRunSt.Io.InPut.St; //开关闭合边沿
    SysRunSt.Io.LostEdge.St = u32t & InStLast.St; //开关断开边沿
    
    
    //输出映射
    if(SysRunSt.Io.OutPut.bits.out_COOL_PUMP)
        OUT_COOL_PUMP = JK_OUT_ENABLE;
    else
        OUT_COOL_PUMP = JK_OUT_DISABLE;
    
    
    if(SysRunSt.Io.OutPut.bits.out_WATER_PUMP)
        OUT_WATER_PUMP = JK_OUT_ENABLE;
    else
        OUT_WATER_PUMP = JK_OUT_DISABLE;
    
    //200V供电
    JK_OUT_AC220_en();
    
    //蜂鸣器
    if(IsSysErrFound())
    {
      if( ParTab[PAR_POS_ALRM_FLAG].uVal == 0 | ParTab[PAR_POS_ALRM_FLAG].uVal == 0x55 )
      {
        if(bSysWave_500ms)
            BuzzerOff();
        else
            BuzzerOn();
        
        ParTab[PAR_POS_ALRM_FLAG].uVal = 0x55;
      }else if( ParTab[PAR_POS_ALRM_FLAG].uVal == 0x66 )//报警消音
      {
         BuzzerOff();
      }  
    }
    else
    {
        BuzzerOff();
        ParTab[PAR_POS_ALRM_FLAG].uVal = 0;
    }
}

/*****************************************************************************//*!
*
* @brief   音量管理.
*        
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void hpvol_manage(void)
{
//    static uint16_t VolNow = 0x7FFF;
    
    if(SysRunSt.PowerUpTimeCnt < 3)
        return;
    
//    if(ParTab[PAR_POS_HPVOL].iVal > ParTabAttr[PAR_POS_HPVOL].max)
//    {
//        ParTab[PAR_POS_HPVOL].iVal = ParTabAttr[PAR_POS_HPVOL].max;
//    }
//    
//    if(VolNow != ParTab[PAR_POS_HPVOL].uVal)
//    {
//    }
}

/*****************************************************************************//*!
*
* @brief   Flash .
*        
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void __FlashErase(uint32_t Addr, uint32_t Nums){
    FLASH_EraseInitTypeDef FlashEraseInit;
    u32 SectorError=0;
    
	FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //擦除类型，扇区擦除 
	FlashEraseInit.Sector=STMFLASH_GetFlashSector(Addr);    //要擦除的扇区
	FlashEraseInit.NbSectors=1;                             //一次只擦除一个扇区
	FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //电压范围，VCC=2.7~3.6V之间!!
                
	if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError) != HAL_OK) 
	{

	}
}

void __FlashProgram(uint32_t Addr, uint32_t Data){

    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,Addr,Data)!=HAL_OK)//写入数据
	{ 
		//写入异常
	}
}

void __FlashUnlock(){
    HAL_FLASH_Unlock();
}

void __FlashLock(){
    HAL_FLASH_Lock();
}

int32_t DataFlt(float f_real, int32_t i_now)
{
    int32_t i_real;
    int32_t i_real_add2;
    int32_t i_real_sub2; 
    
    if(f_real < 0)
    {
        i_real = (int32_t)f_real;
        i_real_add2 = (int32_t)(f_real - 0.2f);
        if(f_real <= -0.2f)
            i_real_sub2 = (int32_t)(f_real + 0.2f);
        else
            i_real_sub2 = 1;
        
        if(i_real_add2 < i_real)  
        {
            i_now = i_real_add2;
        } else if( i_real_sub2 > i_real)
        {
            i_now = i_real;
        } else { 
            if(i_now > i_real)
                i_now = i_real;
            else if(i_now < (i_real-1))
                i_now = i_real-1;
        }  
    } else {
        i_real = (int32_t)f_real;
        i_real_add2 = (int32_t)(f_real + 0.2f); 
        if(f_real >= 0.2f)
            i_real_sub2 = (int32_t)(f_real - 0.2f);
        else
            i_real_sub2 = -1;

        if(i_real_add2 > i_real) 
        {
            i_now = i_real_add2;
        } else if( i_real_sub2 < i_real)
        {
            i_now = i_real;
        } else { 
            if(i_now < i_real) 
                i_now = i_real;
            else if(i_now > (i_real+1))
                i_now = i_real+1;
        }        
    }

    return i_now;
}

