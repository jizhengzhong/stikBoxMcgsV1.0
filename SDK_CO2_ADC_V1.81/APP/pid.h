/* 
 * File:   PIDdevice.h
 * Author: apleilx
 *
 * Created on 2015年10月9日, 上午10:07
 */

#ifndef PIDDEVICE_H
#define	PIDDEVICE_H

#include "stdint.h"

#ifdef	__cplusplus
extern "C" {
#endif	
	
#ifdef   _PID_MODULE_
#define  PID_EXT
#else
#define  PID_EXT  extern
#endif

#define Q_15_VAL_1_0F                            (0x8000UL) //Q.15格式定点数 1
#define Q_15_VAL_0_5F                            (0x4000UL) //Q.15格式定点数 0.5
#define Q_15_VAL_0_25F                           (0x2000UL) //Q.15格式定点数 0.25
#define Q_15_VAL_0_125F                          (0x1000UL) //Q.15格式定点数 0.125
#define Q_15_VAL_NV1_0F                          (-0x8000L) //Q.15格式定点数 1
#define Q_15_VAL_NV0_5F                          (-0x4000L) //Q.15格式定点数 0.5
#define Q_15_VAL_NV0_25F                         (-0x2000L) //Q.15格式定点数 0.25
#define Q_15_VAL_NV0_125F                        (-0x1000L) //Q.15格式定点数 0.125

#define KI_INCRE_LIMITE_ERR                    2     // 用于抗积分饱和判定

//积分能力递增
#define PID_MODE_SIN                           0    
#define PID_MODE_LSIN                          1     //line+sin
#define PID_MODE_LINE                          2
#define PID_MODE_LCOS                          3     //line+cos
#define PID_MODE_COS                           4     

//参数表
#define TEMP_PID_P_POS     0
#define TEMP_PID_I_POS     1
#define TEMP_PID_D_POS     2
#define TEMP_PID_AR_POS    3
#define TEMP_PID_T_POS     4


typedef enum{ 
    DWK_NONE  = 0,
    DWK_DIR_UP  ,//变化方向向上时削弱[-P~+P]->[1~(1-c)]
    DWK_DIR_DOWN ,//变化方向向下时削弱[-P~+P]->[(1-c)~1]
    DWK_P_DIR_DC ,//正向变化加强抑制能力 [-P~+P]->[(1+c)~1]->[2~1] ，反向变化削弱抑制能力[-P~+P]->[1~(1-c)]->[1~0]
    DWK_P_DIR_DN ,//正向变化削弱抑制能力 [-P~+P]->[(1-c)~1]->[0~1] ，反向变化增强抑制能力[-P~+P]->[1~(1+c)]->[1~2]
} DV_WEAK_TYPE;

	typedef  struct
	{
        int8_t  bDir;    
        DV_WEAK_TYPE DvWeakType;//外部更新      
		uint16_t Kt;            //外部更新，或调用update
		uint16_t KtCnt;         //外部更新
        uint16_t Cycle;         //外部设置 //调用周期
		uint16_t P;  //外部更新，或调用update
        uint16_t I;  //外部更新，或调用update
        uint16_t D;  //外部更新，或调用update
		uint16_t AR; //外部更新，或调用update
        
		int32_t OutMax;//外部更新，或调用update
		int32_t OutMin;//外部更新，或调用update
		int32_t Out;                            //限幅输出    
		int32_t IoutMax;                     
        int32_t IoutMaxSet;             //外部更新，或调用update
        int32_t ILimiteLineL;                   //ILimiteLineH>ILimiteLineL时生效
        int32_t ILimiteLineH;     //外部更新
        int32_t q15_DirAdjZone;      //外部更新 ,Q.15
        int32_t q15_PvForDir;      //Q.15
        
        float AdjOut;
        float OutCoef;                      //0~1.0f//外部更新
        float OutCoefFlt;                   //0~0.99//外部更新  
        float DeadZone;                       //外部更新
		float DFt;
        float DvWeak;                 //外部更新
        float DvWeakMax;      //外部更新
		float Sv;              //外部更新
		float Pv;          //外部更新
		float Pvn1;
        float Pvn2;
        float Pvd;
		float Kp;
		float Ki;
		float Kd;   
		float uk; 
        float outFac;   //输出规格化
        float outFac_offs; 
		float dv;                         
	} PID_Type;

	
	typedef  struct
	{
		uint8_t  CtrDir;                //0:正向控制     1：反向控制
		uint8_t  State;                 //0:不在自整定   FF:自整定结束
		uint16_t RisingTimeCnt;
		uint16_t FallingTimeCnt;
		uint16_t AutoSetTimeCnt;
        uint16_t P;
		uint16_t I;
        uint16_t D;
		float  MeasureMin;
		float  MeasureMax;
        float  CtrZone;                 //输出回差带
        float  IdCoef;                  //微分积分比例
		PID_Type *Pid;
        void(*ParSave)(void);           //计算Pid自整定结束后调用外部保存函数
	
	} PID_AUTO_SET_Type;
	

	void SitePid(PID_Type* pid);

	void PidClr(PID_Type* pid);//clear (dv,uk)    set(OutCoef = 1)
    void PidDvClr(PID_Type* pid); //clear dv
	void PidPvSet(PID_Type* pid,float Pv); //Set pv
    void PidSvSet(PID_Type* pid,float Sv); //Set Sv
    
    void LcosPid(PID_Type* pid);
    void CosPid(PID_Type* pid);
    void SinPid(PID_Type* pid);
    void ComPid(PID_Type* pid);
	void IndPid(PID_Type* pid);
    
    void PidOutWeaken(PID_Type* pid, float Coef);
    void PidUpdate(PID_Type* pid,uint16_t *ParTab,int32_t DutyMin,int32_t DutyMax);
    
	uint8_t PidAutoSet(PID_AUTO_SET_Type *AutoSetPar);
	void PidAutoSetStart(PID_Type *Pid, PID_AUTO_SET_Type *AutoSetPar);
	void PidAutoSetExit(PID_AUTO_SET_Type *AutoSetPar);
	void PidAutoSetCal(PID_AUTO_SET_Type *AutoSetPar);
	
#undef PID_EXT

#ifdef	__cplusplus
}
#endif

#endif	/* PIDDEVICE_H */
