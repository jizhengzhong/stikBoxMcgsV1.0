/* 
 * File:   tempctr.h
 * Author: apleilx
 */

#ifndef HUMITEMPCTRLIB_H_
#define	HUMITEMPCTRLIB_H_

#include <stdint.h>
#include "tempctrlib.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define HUMI_AUTO_BACK_MAX                           1

#define COOL_TIME_SET_EN

typedef enum
{
    HUMI_INC_INIT = 0,
    HUMI_INC_FULL,
    HUMI_CLR,
    HUMI_INC,
    HUMI_AUTO,
    HUMI_BACKUP
} HUMI_CTR_STEP_Type;

typedef enum {
    HUMI_DIR_DOWN  =     0,
    HUMI_DIR_UP    =     1,  
} HumiDirType;

typedef enum {
    HUMI_TEMP_CTR_MODE_NOCOOL_NOCLR=0,
    HUMI_TEMP_CTR_MODE_COOL,
    HUMI_TEMP_CTR_MODE_HUMICLR,    
} CtrModeType;

typedef struct
{
    uint8_t bTempManOn;                                  //温度手动控制           Y
    uint8_t bManTempCoolReq;                             //                       Y
    uint16_t ManHeatOut;                                 //                       Y
    uint8_t bHumiManOn;                                  //湿度手动控制           Y
    uint8_t bManHumiCoolReq;                             //                       Y
    uint16_t ManHumiOut;                                 //                       Y
    uint8_t bCoolForce;                                  //强制开制冷             Y
    CtrModeType CtrMode;                                 //外部初始化 0          IY
    CtrModeType CtrModeOld;                              //外部初始化 0          IY
    TEMP_CTR_STEP_Type  TempCtrStep;                     //外部初始化为TEMP_HEAT_FULL     IY
    TempDirType bTempDir;                                //外部只读 X
    TempDirType bTempDirLast;                            //外部只读 X
    TempDirType bDelayEndDir;                            //外部只读 X
    uint8_t TempKeepCnt;                                 //外部只读 X
    uint8_t TempDirKeepCnt;                              //外部只读 X
    uint8_t WaveCnt;                                     //制冷次数 外部初始化0  IY
    uint8_t SubAdjCnt;                                   //外部初始化0           IY
    uint8_t bTempCoolDis;                                //温度制冷禁用，根据TempAir,会差0.5 X
    uint8_t bCoolReq;                                    //外部只读           X
    uint8_t bCoolReqLast;                                //外部只读           X
    uint8_t bTempCoolReq;                                //外部只读           X
    uint8_t bTempAdjDone;                                //外部只读           X
    uint8_t bPulseCoolDelayEnd;                          //外部只读
    int16_t TempCur;                                     //外部设定           Y
    int16_t TempSet;                                     //外部设定           Y
    int16_t TempAtCoolStart;                             //外部只读           X
    int16_t TempForDir;                                  //外部只读           X
    int16_t TempCoolDelayEnd;                            //外部只读           X
    int16_t TempCoolEnd;                                 //外部只读           X
    int16_t StCoolDelayEnd;                              //外部只读
    int16_t TempOffsetCoolEn;                            //外部初始化         IY        相对于SET
    int16_t TempOffsetCoolDis;                           //外部初始化         IY        相对于SET
    int16_t TempOffsetHeatClose;                         //外部初始化         IY        相对于SET
    int16_t TempOffsetLimite;                            //外部设定           Y         相对于SET
    int16_t TempMax;                                     //外部初始化为0x0000 IY
    int16_t TempMin;                                     //外部初始化为0x7FFF IY
    int16_t TempMaxOld;                                  //外部初始化为0x0000 IY
    int16_t TempMinOld;                                  //外部初始化为0x7FFF IY
    int16_t TempLast;                                    //禁止读写
    int16_t TempSetLast;                                 //禁止读写
    int16_t TempAir;                                     //用于判定是否启动制冷     Y
    int16_t CoolDelayTimeSv;                            //外部设定                  Y
    int16_t CoolDelayTimeCnt;                           //上电初始化为 CoolDelayTimeSv 之后禁止修改 IIY
    int16_t NoTempCoolTimeCnt;                          //记录两次制冷之间的间隔，初始化为最大值    IY
    int16_t TempCtrSvAtCoolOffset;                      //外部只读           X
    int32_t HeatOut;                                    //外部只读，加热占空比 X
	PID_Type TempPid;                                   //外部设定，指向温度PID，用于自动调整PID内部数据
    float   TempPidDeadZone;                            ////外部只读 X
    float   TempOtCoef;                                 ////外部只读 X
    float   TempOtCoefFlt;                              ////外部只读 X
    //制冷计时
    int16_t CoolTimeCnt;                                 //实时制冷时间 X
    int16_t CoolTimeCntOld;                              //Update at Cool-End X
    int16_t CoolCloseTimeCnt;                            //无制冷时间 X
    int16_t CoolTimeSet;                                 // 制冷时间X
    //湿度控制参数
    uint8_t StHumiCtrEn;                                 //湿度控制使能 Y
    uint8_t StHumiAdjEn;                                 //调节控制     X
    uint8_t bCoolClrCloseByTemp;                         //温度原因导致除湿关闭 X
    HUMI_CTR_STEP_Type  HumiCtrStep;                     //外部初始化为HUMI_INC_FULL   X
    HumiDirType bHumiDir;                                //外部只读           X
    HumiDirType bHumiDirLast;                            //外部只读           X
    HumiDirType bDeayEndHumiDir;                         //外部只读           X
    uint8_t bHumiCoolReq;                                //外部只读           X
    uint8_t bHumiAdjDone;                                //外部只读           X
    uint8_t HumiKeepCnt;                                 //外部只读           X
    uint8_t HumiDirKeepCnt;                              //外部只读           X
    int16_t HumiForDir;                                  //外部只读           X
    int16_t HumiCur;                                     //外部设定           Y
    int16_t HumiSet;                                     //外部设定           Y
    int16_t HumiSetLast;                                 //外部只读           X
    int16_t HumiCtrTemp;                                 //外部设定           IY
    int16_t HumiIncClose;                                //外部设定           IY
    int16_t HumiClrEn;                                   //外部设定           IY
    int16_t HumiClrDis;                                  //外部设定           IY
    int16_t HumiCtrLimite;                               //外部设定           Y
    int16_t HumiDelayEnd;                                //外部只读           X
    int16_t HumiClrEnd;                                  //外部只读           X
    int16_t StHumiClrDelayEnd;                           //外部只读           X
    int16_t HumiMax;                                     //外部初始化为0x0000 IY
    int16_t HumiMin;                                     //外部初始化为0x7FFF IY 
    int16_t HumiMaxOld;                                  //外部初始化为0x0000 IY
    int16_t HumiMinOld;                                  //外部初始化为0x7FFF IY
    int16_t NoHumiCoolTimeCnt;                           //外部设定           Y
    int16_t HumiCtrSvAtCoolOffset;                       //外部只读           X
    int32_t HumiOut;                                    //外部只读，加湿占空比
    PID_Type HumiPid;                                    //外部初始化
    float   HumiPidDeadZone;                            //fault zone
    float   HumiOtCoef;
    float   HumiOtCoefFlt;
    uint32_t TimeCnt;
    uint32_t StepTimeCnt;
} HUMITEMP_CTR_PAR_Type;


void HumiTempCtr(HUMITEMP_CTR_PAR_Type *HTempCtrPar);   
void HT_Control(HUMITEMP_CTR_PAR_Type *CtrPar);

#ifdef	__cplusplus
}
#endif



#endif	/* HUMITEMPCTRLIB_H_ */

