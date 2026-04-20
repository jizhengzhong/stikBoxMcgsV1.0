
#ifndef  _APP_INTERFACE__
#define  _APP_INTERFACE__

#include "stdint.h"
#include "dataproc.h"
#include "pulse.h"
#include "errhandle.h"
#include "flashtoeep.h"
#include "extnorflashlog.h"
#include "mbcom.h"
#include "mbmaster.h"
#include "humitempctrlib.h"
#include "templib.h"

typedef struct {
    int32_t (*Sub2Add8)(float f_real, int32_t i_now);
    int32_t (*SubAdd_x)(float f_real, int32_t i_now, float val_sub);
    uint32_t (*LedBreathHandle)(led_breath_type *Brj);
    void (*pulse_init)(timer_pulse_type *pulse, uint16_t uTick);
    void (*PulseCreat)(timer_pulse_type *pulse, uint16_t uTick);
    void (*i16_to_led_code)(int16_t Data, uint8_t *Des, uint8_t zero_clr, uint8_t PointSite, uint8_t nodisp);
    
    void (*key_st_handle)(key_type *key);
    void (*key_st_isr)(key_type *key, uint32_t key_read);
    void (*switch_handle)(switch_st_type *swobj, uint8_t sw_now);
    void (*out_st_handle)(out_st_type *obj, uint8_t out_now);
    
    bcd_time_type (*time_sig_to_bcd)( sig_time_type sig_time);
    sig_time_type  (*time_bcd_to_sig)(bcd_time_type  bcd_time);
    bcd_time_type (*time_to_bcd)( TIME_Type * time);
    void (*Int16ToAscii)(int16_t Num,uint8_t Attr,uint8_t *Des);
	void (*Uint16ToAscii)(uint16_t Num,uint8_t Attr,uint8_t *Des);
    
    void (*err_init)(sys_err_type *sys_err);
    void (*err_poll)(sys_err_type *sys_err);
    
    uint16_t (*FlashEepRead)(FlashEepCfg_Type *EepCfg);
    uint16_t (*FlashEepSave)(FlashEepCfg_Type *EepCfg);
    uint16_t (*FlashMulEepSave)(FlashEepCfg_Type *EepCfg);
    uint16_t (*FlashMulEepRead)(FlashEepCfg_Type *EepCfg);
    uint16_t (*ExtFlashMulEepSave)(ExtFlashEepCfg_Type *EepCfg);
    uint16_t (*ExtFlashMulEepRead)(ExtFlashEepCfg_Type *EepCfg);

    uint16_t (*FlashLogSave)(flash_log_man_cfg_Type *EepCfg);
    uint16_t (*FlashLogInit)(flash_log_man_cfg_Type *EepCfg);
    uint16_t (*FlashLogSectorAlign)(flash_log_man_cfg_Type *EepCfg);
    uint16_t (*ExtFlashLogSave)(ext_flash_log_man_cfg_Type *EepCfg);
    uint16_t (*ExtFlashLogInit)(ext_flash_log_man_cfg_Type *EepCfg);
    uint16_t (*ExtFlashLogSectorAlign)(ext_flash_log_man_cfg_Type *EepCfg);
    
    uint16_t (*ext_nor_log_FlashCheck)(ext_nor_log_Cfg_Type *EepCfg);
    uint16_t (*ext_nor_log_FlashSave)(ext_nor_log_Cfg_Type *EepCfg);
    uint16_t (*ext_nor_log_SectorAlign)(ext_nor_log_Cfg_Type *EepCfg);
    
    void (*DUGSII_MasterPoll)(void *mbObj); 
    void (*DUGS_MasterPoll)(void *mbObj);
    void (*mb_poll)(mbObjType *mbObj);   
    
    void (*mbCmdBuffInit)(mbObjType *mbObj, mbCmdBuffType * pBuff, uint8_t mbCBuffSize, uint8_t mbSCBuffSize);
    uint8_t (*mbStaticCmdSet)(mbObjType *mbObj, uint8_t CmdSite, uint8_t DeviceId, mbCmdType Cmd, uint16_t *Data, uint16_t mbAddr, uint16_t amount, void (*CallBack)(uint8_t,uint8_t));
    uint8_t (*mbDynaCmdReq)(mbObjType *mbObj, uint8_t DeviceId, mbCmdType Cmd, uint16_t *Data, uint16_t mbAddr, uint16_t amount, void (*CallBack)(uint8_t,uint8_t));
    
    
    uint8_t (*mbStaticCmdSetNoOS)(mbObjType *mbObj, uint8_t CmdSite, uint8_t DeviceId, mbCmdType Cmd, uint16_t *Data, uint16_t mbAddr, uint16_t amount, void(*CallBack)(uint8_t, uint8_t));
    uint8_t (*mbDynaCmdReqNoOS)(mbObjType *mbObj, uint8_t DeviceId, mbCmdType Cmd, uint16_t *Data, uint16_t mbAddr, uint16_t amount, void(*CallBack)(uint8_t, uint8_t));
    uint8_t (*NumOfMmbCmdReady)(mbObjType *mbObj);
    uint8_t (*NumOfMmbCmdIdle)(mbObjType *mbObj);
    void (*mbCmdStClrAll)(mbObjType *mbObj);
    uint32_t (*mbCmdHandleGet)(mbObjType *mbObj, mbCmdBuffType * pBuff);
    void (*mb_obj_init)(mbObjType *mbObj);
    
    int32_t (*int_Sub2Add8)(int32_t real, int32_t i_now);
    int32_t (*int_SubAdd_x)(int32_t real, int32_t i_now, int32_t val_sub);
    
    void (*PidClr)(PID_Type* pid);//clear (dv,uk)    set(OutCoef = 1)
    void (*PidDvClr)(PID_Type* pid); //clear dv
    void (*PidPvSet)(PID_Type* pid,float Pv); //Set pv
    void (*PidSvSet)(PID_Type* pid,float Sv); //Set Sv
    void (*IndPid)(PID_Type* pid);
    void (*PidUpdate)(PID_Type* pid,uint16_t *ParTab,int32_t DutyMin,int32_t DutyMax);
    
    uint8_t (*PidAutoSet)(PID_AUTO_SET_Type *AutoSetPar);
    void (*PidAutoSetStart)(PID_Type *Pid, PID_AUTO_SET_Type *AutoSetPar);
    void (*PidAutoSetExit)(PID_AUTO_SET_Type *AutoSetPar);
    
    void (*HumiTempCtr)(HUMITEMP_CTR_PAR_Type *HTempCtrPar);   
    void (*HT_Control)(HUMITEMP_CTR_PAR_Type *CtrPar);
    void (*TempCtrAutoCool)(TEMP_CTR_PAR_Type *TempCtrPar);
    void (*TempCal)(TEMP_OBJ_Type* TempObj);
} app_interface_list_type;

extern const unsigned long int hex_array[];
#define sysapi   ((app_interface_list_type *)(hex_array[8]))

#endif
