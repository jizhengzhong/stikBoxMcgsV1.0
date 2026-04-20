
#ifndef __SYSTASK_H
#define	__SYSTASK_H

#include <stdint.h>
#include <dataproc.h>

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef   _SYSTASK_MODULE_
#define  MY_EXT
#else
#define  MY_EXT  extern
#endif  

    #define ERR_TEMP_AL     0x0001
    #define ERR_TEMP_AH     0x0004
  
    #define ERR_CO2_AL      0x0101
    #define ERR_CO2_AH      0x0110
  
    #define ERR_SW_AL       0x0100
    #define ALARM_DOR_OPEN  0x0010
  
    //错误代码定义
#define ERR_G1_MSK      0x0008      // 0x0003冀替换成0x0008
#define ERR_G2_MSK      0x0008      // 0x0003冀替换成0x0008
#define ERR_G3_MSK      0x0000
#define ERR_G4_MSK      0x0000
    
    MY_EXT uint8_t bSysPulse_1s;
    MY_EXT uint8_t bSysPulse_500ms;
    MY_EXT uint8_t bSysPulse_250ms;
    MY_EXT uint8_t bSysPulse_100ms;

    MY_EXT uint16_t SysPulseTimeCnt_1s;
    MY_EXT uint16_t SysPulseTimeCnt_500ms;
    MY_EXT uint16_t SysPulseTimeCnt_250ms;
    MY_EXT uint16_t SysPulseTimeCnt_100ms;
    
    MY_EXT uint8_t bSysWave_500ms;
    MY_EXT uint8_t bSysWave_250ms;
    
    typedef union {
        uint16_t St;
        uint8_t Stv[2];

        struct {
            uint16_t in_YL : 1;
            uint16_t bit1 : 1;
            uint16_t in_RHW_SW : 1;
            uint16_t bit3 : 1;
            uint16_t in_DOOR_SW : 1;
            uint16_t bit5 : 1;        
            uint16_t in_CO2TANK1 : 1;
            uint16_t bit7 : 1;
            uint16_t in_CO2TANK2 : 1;
            uint16_t bit9 : 1;
            uint16_t TEMP_REACH_SV : 1; //////////ji_xiugai 原来为备用变量；
            uint16_t bit11 : 1;
            uint16_t CO2_REACH_SV : 1;//////////ji_xiugai 原来为备用变量；
            uint16_t bit13 : 1;      //////////ji_xiugai 原来为备用变量；
            uint16_t bit14 : 1;      //////////ji_xiugai 原来为备用变量；
            uint16_t bit15 : 1;
        } bits;
    } sys_in_type;

    typedef union {
        uint16_t St;
        uint8_t Stv[2];

        struct {
            uint16_t out_ALARM : 1;
            uint16_t out_RH_H : 1;
            uint16_t out_BOTTOM_H : 1;
            uint16_t out_LR_H : 1;
            uint16_t out_TOP_H : 1;
            uint16_t out_REAR_H : 1;
            uint16_t out_FRAME_H : 1;
            uint16_t out_WATER_PUMP : 1;
            uint16_t out_CO2_CRL1 : 1;
            uint16_t out_CO2_CRL2 : 1;
            uint16_t out_PUBZZ : 1;
            uint16_t out_DOOR_H : 1;
            uint16_t out_IR_ZERO : 1;
            uint16_t out_COOL_PUMP : 1;
            uint16_t bit14 : 1;
            uint16_t bit15 : 1;
        } bits;
    } sys_out_type;
    
    typedef struct {
        //数字输入
        sys_in_type InPut;
        sys_in_type InPutList[4];
        sys_in_type LostEdge;
        sys_in_type FoundEdge;
        sys_out_type OutPut;
        sys_out_type OutPutOld;
        //AD转换数值获取
        uint16_t Pt100AdValList[5]; //pt 测量AD
        uint16_t Pt100AdValIndex;
        uint16_t Pt100AdVal;
        uint16_t TcdMainAdValList[5]; //TCD 主测量
        uint16_t TcdMainAdValIndex;
        uint16_t TcdMainAdVal;
        uint16_t DigHumiAdValList[5]; //数字湿度
        uint16_t DigHumiAdValIndex;
        uint16_t DigHumiAdVal;
        uint16_t DigTempAdValList[5]; //数字温度
        uint16_t DigTempAdValIndex;
        uint16_t DigTempAdVal;
        uint16_t TcdMeasureAdValList[5]; //TCD 测量臂
        uint16_t TcdMeasureAdValIndex;
        uint16_t TcdMeasureAdVal;
        uint16_t TcdRefAdValList[5]; //TCD参考臂
        uint16_t TcdRefAdValIndex;
        uint16_t TcdRefAdVal;
        
        //模拟输入
        int32_t iTempMain;
        float fTempMain;
        int32_t iTempDoor;
        float fTempDoor;
////////////////ji_add/////////////////
        int32_t iTempEvn;
        float fTempEvn;
//////////////ji_end////////////////        
        int32_t iCol;
        float fCol;
        float fHumi;
        
        float SHT85_fHumi;
        float SHT85_fTemp;
        
        //模拟输出
        int32_t AdjTempOutDuty;
        int32_t AdjColOutDuty;
/////////////ji_start//////////////////////        
        int32_t AdjDorOutDuty;
/////////////ji_end///////////////////////        
        int32_t AdjTempPeriod;
        int32_t AdjColPeriod;
      /////////////ji_start//////////////////////        
         int32_t AdjDorPeriod;
/////////////ji_end///////////////////////       
        int32_t AdjTempOutCnt;
        int32_t AdjColOutCnt;
        /////////////ji_start//////////////////////        
        int32_t AdjDorOutCnt;
/////////////ji_end/////////////////////// 
    } sys_io_type;

    typedef struct {
        uint8_t ErrSt[4][16];
        uint8_t ErrStLast[4][16];
        uint8_t err_global;
        
        uint8_t sd_cd_st; //sd 插卡状态
        uint8_t sd_state; //sd 初始化完成
        uint8_t door_open_state;
/////////////////ji_add/////////////////////
        uint8_t rhw_sw_state;
/////////////////ji_end/////////////////////
        
        uint8_t bLoginSt;
        uint32_t PowerUpTimeCnt; //单位s
        sys_io_type Io;

        //控制设定
        uint8_t bTempStableSta;
        uint8_t bCo2StableSta;
        uint8_t bTempRun;
        uint8_t bCo2Run;
        int16_t TempMainSet;
        int16_t TempDoorSet;
        int16_t ColSet;
        
        //运行控制
        uint8_t bSysRun;
        uint32_t TimeSet;
        uint32_t TimeCnt;
        
        //灭菌控制
        uint8_t bMjRun;
        uint32_t MjTimeSet;
        uint32_t MjTimeCnt;
        
        //控制值 / 显示值
        int16_t TempMainDisp;
        int16_t TempDoorDisp;
///////////////////ji_add//////////////////////
        int16_t TempEvnDisp;
//////////////////ji_end//////////////////////        
        int16_t ColDisp;
        int16_t LstColDis;
        int16_t HumiDisp;
        
        //北京时间-压缩码
        sig_time_type SigTime;
        sig_time_type OrderStartTime;
        sig_time_type OrderStopTime;
        uint8_t bOrderStartTimeOn;
        uint8_t bOrderStopTimeOn;
        uint8_t bStOrderStartDone;
        uint8_t bStOrderStopDone;
        
        //传感器通信控制
        uint8_t bSenSndReq;
        uint8_t StSenBack;
        
        //co2传感器自校准
        volatile uint8_t bCo2AutoAdjSt; //0-:标准 1-启动 2-收到回执 3-收到结束标志
        
    } sys_run_st_type;

    MY_EXT sys_run_st_type SysRunSt;
    
    

#undef MY_EXT

#ifdef	__cplusplus
}
#endif

#endif	/* __SYSTASK_H */
