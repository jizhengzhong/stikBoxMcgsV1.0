/* 
 * File:   tempctr.h
 * Author: apleilx
 */

#ifndef TEMPDISPLIB_H_
#define	TEMPDISPLIB_H_

#include <stdint.h>


#ifdef	__cplusplus
extern "C" {
#endif

#define FOLLOW_LIST_SIZE          32
typedef struct {
    int16_t  SetPoint;      //外部更新      YYY
	int16_t  CurPoint;      //外部更新      YYY
    
    uint8_t  bFollowSt;     //是否开启跟踪，外部控制 + 内部检测到设定=测量自动开始 + 内部检测到设定值变化自动退出      YYY
    uint8_t  bCalPulse;     //计算脉冲    ，外部更新      YYY
    uint8_t  KeepTimeSv;    //消抖阶段保持时间，按数据记录次数，不可大于FOLLOW_LIST_SIZE-2，外部更新      YYY
    uint16_t FollowQuitPeriod; //退跟踪周期，外部更新      YYY
    uint16_t FollowEntryPeriod; //进入踪周期，外部更新      YYY
    uint16_t PulseAutoCnt;  //内部管理
	int16_t  FollowVal;     //内部管理
	int16_t  FollowValMaxSet;  //外部设置      YYY
    int16_t  FollowValMaxCur;  //内部
    int16_t  DispVal;          //内部管理，输出
    int16_t  SetPointLast;      //
    int16_t  FollowValList[FOLLOW_LIST_SIZE];//内部管理
    uint16_t DataListPeriod;    //数据记录周期      YYY
    uint16_t FollowQuitDelayCnt;//内部管理
    uint16_t FollowQuitDelaySv; //外部设置      YYY
    
} disp_flt_fixed_Type;

typedef struct {
    uint8_t bFollowSt;      //YYY
    uint8_t  bCalPulse;      //YYY
    int16_t ValSv;         //YYY
    int16_t ValPv;            //YYY
    int16_t ValDisp;
    int16_t ValFollow;
    int16_t  FollowValMaxSet;  //外部设置  //YYY
    int16_t  FollowValMaxCur;
    uint8_t Coef;  //YYY
    uint8_t  KeepTimeSv; //消抖阶段保持时间，按数据记录次数，不可大于FOLLOW_LIST_SIZE-2，外部更新  //YYY
    int16_t ValSvLast;
    int16_t  FollowValList[FOLLOW_LIST_SIZE];//内部管理
    uint16_t DataListPeriod;    //数据记录周期  //YYY
    uint16_t PulseAutoCnt;  //内部管理
    uint16_t FollowQuitPeriod; //退跟踪周期，外部更新  //YYY
    uint16_t FollowEntryPeriod; //进入踪周期，外部更新  //YYY
    uint16_t FollowQuitDelayCnt;//内部管理
    uint16_t FollowQuitDelaySv; //外部设置  //YYY
} disp_flt_dync_Type;

void DispFltFiexd(disp_flt_fixed_Type *DispObj);
void DispFltDync(disp_flt_dync_Type *DispObj);

#ifdef	__cplusplus
}
#endif



#endif	/* TEMPDISPLIB_H_ */

