/* 
 * File:   templib.h
 * Author: apleilx
 */

#ifndef TEMPLIB_H_
#define	TEMPLIB_H_

#ifdef   _TEMPLIB_MODULE_
#define  TEMP_EXT
#else
#define  TEMP_EXT  extern
#endif

#include "stdint.h"

#ifdef	__cplusplus
extern "C" {
#endif

    
#define TEMP_ERR_LIMITE_H_MASK          0x01
#define TEMP_ERR_LIMITE_L_MASK          0x10
#define TEMP_ERR_TABLE_H_MASK           0x02
#define TEMP_ERR_TABLE_L_MASK           0x20
    
#define TEMP_DIVISION_MAX               (20000L)
#define TEMP_DIVISION_MIN               (0L)

#define TEMP_TAB_DATA_END               INT32_MAX

/*-----------------------------------------------------------------------------*
 * 
 * ADMarkH    -->     DivisionMax    
 * ADMarkL    -->     DivisionMin
 * 
 * Dx = (ADx - ADMarkL) * (DivisionMax - DivisionMin) / (ADMarkH -  ADMarkL)
 * Tx = (Dx - x0) * (y1 - y0) / (x1 - x0) 
 *-----------------------------------------------------------------------------*/    
    //最后一个分度固定为 INT32_MAX
    typedef struct
    {
        int32_t Division;
        int32_t Temp;
        
    } TEMP_TABLE_Type;
    
    typedef struct
    {
        int32_t MarkADL;
        int32_t MarkADH;
    } TEMP_AD_MARK_Type;
    
    typedef struct
    {
        int32_t MarkTempL;
        int32_t MarkTempH;
    } TEMP_VAL_MARK_Type;	
	
    typedef struct
    {
        int32_t PB;
        int32_t PK;
    } TEMP_CORRECT_Type;
    
    typedef struct
    {
        int32_t LimiteH;
        int32_t LimiteL;
    } TEMP_LIMITE_Type;
    
	
	typedef struct
	{
		uint8_t Err;
        uint8_t bCFEn;
		int16_t ADVal;
		float Temp;
		float TempReal;
		float TempDis;
        float Flt;
		const TEMP_TABLE_Type *TempTab;
		TEMP_AD_MARK_Type Mark;
		TEMP_CORRECT_Type Correct;
		TEMP_LIMITE_Type Limite;
	} TEMP_OBJ_Type;
    
    void TempCal(TEMP_OBJ_Type* TempObj);
    float  TempGet(TEMP_OBJ_Type* TempObj);
	int16_t TempIntGet(TEMP_OBJ_Type* TempObj);
	int16_t  DispTempGet(TEMP_OBJ_Type* TempObj);
    void TempADSet(TEMP_OBJ_Type* TempObj ,int16_t AD_Val);
    uint8_t TempErrGet(TEMP_OBJ_Type* TempObj);
	float Pt100_ResToTemp(float m_Res);
    float Pt100_TempToRes(float m_Temp);

#ifdef	__cplusplus
}
#endif

#undef TEMP_EXT

#endif	/* TEMPLIB_H_ */

