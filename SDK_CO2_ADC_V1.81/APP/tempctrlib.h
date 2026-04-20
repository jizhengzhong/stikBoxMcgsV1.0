/* 
 * File:   tempctr.h
 * Author: apleilx
 */

#ifndef TEMPCTRLIB_H_
#define	TEMPCTRLIB_H_

#include <stdint.h>
#include "pid.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define PidCal(x)             IndPid(x)
  
typedef enum
{
    TEMP_HEAT_INIT=0,
    TEMP_HEAT_FULL = 1,
    TEMP_COOL,
    TEMP_HEAT,
    TEMP_AUTO,
    TEMP_BACKUP
} TEMP_CTR_STEP_Type;

typedef enum {
    TEMP_DIR_DOWN  =     0,
    TEMP_DIR_UP    =     1,
       
} TempDirType;

//制冷判定优先级 bCoolForce最高，TempAir次之，自动判定最低

typedef struct
{
    uint8_t bTempManOn;                                  //手动输出控制 //每次执行后程序自动清除此位  //输出手动值，阶段回INIT,清微分，其余保持  Y
    uint8_t bManTempCoolReq;                             //外部更新                                     Y
    uint16_t ManHeatOut;                                 //外部更新                                     Y
    uint8_t WaveCnt;                                     //制冷次数 外部只读 
    int8_t StCoolEnd;                                    //外部只读
    TEMP_CTR_STEP_Type  TempCtrStep;                     //外部初始化为  TEMP_HEAT_FULL                 Y
    TempDirType bTempDir;                                //外部只读
    TempDirType bTempDirLast;                            //外部只读
    TempDirType bDelayEndDir;                            //外部只读
    uint8_t TempKeepCnt;                                 //
    uint8_t TempDirKeepCnt;                              //
    uint8_t bCoolForce;                                  //强制制冷，制冷判定优先级最好，加热固定在全加热阶段
    uint8_t bTempCoolDis;                                //温度制冷禁用，根据TempAir,会差0.5 //外部只读
    uint8_t bCoolReq;                                    //外部只读
    uint8_t bCoolReqLast;                                //外部只读 
    uint8_t bTempCoolReq;                                //外部只读
    uint8_t bPulseCoolDelayEnd;                          //外部只读
    int16_t TempCur;                                     //外部设定           Y
    int16_t TempSet;                                     //外部设定           Y
    int16_t TempForDir;                                  //外部只读
    int16_t TempCoolDelayEnd;                            //外部只读
    int16_t TempCoolEnd;                                 //外部只读
    int16_t StCoolDelayEnd;                              //外部只读
    int16_t TempOffsetCoolEn;                            //外部初始化          Y        相对于SET
    int16_t TempOffsetCoolDis;                           //外部初始化          Y        相对于SET
    int16_t TempOffsetHeatClose;                         //外部初始化          Y        相对于SET
    int16_t TempOffsetLimite;                            //外部设定                     相对于SET
    int16_t TempMax;                                     //外部初始化为0x0000 Y
    int16_t TempMin;                                     //外部初始化为0x7FFF Y
    int16_t TempLast;                                    //禁止读写
    int16_t TempSetLast;                                 //禁止读写
    int16_t TempAir;                                     //环境温度                     高于设定值高于此温度禁止制冷  Y
    int16_t CoolTimeSet;                                 //外部只读
    int16_t CoolTimeCntOld;                              //外部只读
    int16_t CoolTimeCnt;                                 //外部只读
    int16_t CoolDelayTimeSv;                            //外部设定           Y
    int16_t CoolDelayTimeCnt;                           //外部初始化 为 CoolDelayTimeSv
    int16_t NoTempCoolTimeCnt;                          //记录两次制冷之间的间隔，初始化为最大值
    int32_t HeatOut;                                    //外部只读，加热占空比
	PID_Type TempPid;                                    //外部设定，指向温度PID，用于自动调整PID内部数据
    uint32_t TimeCnt;
} TEMP_CTR_PAR_Type;


void TempCtrAutoCool(TEMP_CTR_PAR_Type *TempCtrPar);    //启停式控制程序，主循环调用,每1s调用一次


#ifdef	__cplusplus
}
#endif



#endif	/* TEMPCTRLIB_H_ */

