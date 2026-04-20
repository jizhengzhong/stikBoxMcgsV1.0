/* 
 * File:   dataProc.h
 * Author: Administrator
 *
 * Created on 2014年9月15日, 下午8:29
 */

#ifndef DATAPROC_H
#define	DATAPROC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "rtctime.h"    
    
    
#define DATA_PROC_ERR_NONE           0x0
#define DATA_PROC_ERR_RES            0x21
    
    extern uint8_t const led_code[];
    extern uint8_t const ascii_code[];
    
    typedef struct{
        uint8_t IncCoef;  //32-255
        uint8_t bDir; 
        uint16_t min; 
        uint16_t max; 
        uint16_t Out;
        uint16_t min_keep_time_sv; 
        uint16_t max_keep_time_sv;
        uint16_t min_keep_time_cnt; 
        uint16_t max_keep_time_cnt;
        uint32_t Cal;
    } led_breath_type;
    
    uint16_t AsciiToUpper(uint8_t *pSrc, uint16_t Len); //转换ascii为大写
    uint16_t AsciiToLower(uint8_t *pSrc, uint16_t Len); //转换ascii为小写
    
    void Hex4ToBCD4(uint16_t Data ,uint8_t *Des );
    void Hex4ToBCD8(uint16_t Data ,uint8_t *Des );
    void Hex2ToBCD2(uint8_t Data ,uint8_t *Des );
    void Hex2ToBCD4(uint8_t Data ,uint8_t *Des );
    
    uint8_t BCD2ToHex(uint8_t Data, uint8_t *Des);
    uint8_t BCD4ToHex(uint16_t Data, uint8_t *Des);
    uint8_t BCD8ToHex(uint32_t Data, uint8_t *Des);
    
    void Hex8ToAscii(uint32_t Num,uint8_t *Des);
    void Hex4ToAscii(uint16_t Num,uint8_t *Des);
    void Hex2ToAscii(uint8_t Num,uint8_t *Des);
    
/*****************************************************************************
*          转换结果右对齐
* @param   Attr:
*          [7]:   1消零，0补零
*          [6]:
*          [5-4]: 小数点位数
*          [3]:
*          [2-0]: 十进制结果位数,x+1位
*****************************************************************************/    
    void Int16ToAscii(int16_t Num,uint8_t Attr,uint8_t *Des);
	void Uint16ToAscii(uint16_t Num,uint8_t Attr,uint8_t *Des);
    
/*****************************************************************************
*          转换结果左对齐
* @param   Attr:
*          [6]:
*          [5-4]: 小数点位数
*          [3]:
*          [2-0]:
*****************************************************************************/
    uint8_t Uint16ToString(uint16_t Num,uint8_t Attr,uint8_t *Des);
    uint8_t Int16ToString(int16_t Num,uint8_t Attr,uint8_t *Des);
    
    int16_t AsciiToInt16(uint8_t *pSrc, uint8_t Len);
    int16_t AsciiToInt16_P0(uint8_t *pSrc, uint8_t Len); //无小数
    int16_t AsciiToInt16_P1(uint8_t *pSrc, uint8_t Len); //一位小数
    
    //zero_clr : 控制是否清零
    //PointSite: 小数点位置
    //nodisp :隐藏位控制，bit[7]控制是否隐藏，bit[1:0]指示隐藏的位 0代表个位，3代表千位
    void i16_to_led_code(int16_t Data, uint8_t *Des, uint8_t zero_clr, uint8_t PointSite, uint8_t nodisp);
    
    //软件奇偶校验
    uint8_t EvenU8(uint8_t Data);                 //返回8位数据的偶校验结果，0表示偶数个1，1表示奇数个1
    uint8_t OddU8(uint8_t Data);                  //返回8位数据的奇校验结果，0表示奇数个1，0表示偶数个1
    uint8_t NoneParity(uint8_t Data);             //返回8位数据的奇校验结果
    
    //数组比较
    int8_t ArrayComp(uint8_t *Array1, uint8_t *Array2, uint16_t Len); //数组比较
    
    //ascii 转 hex
    uint8_t AsciiToHex2(uint8_t *Src);
    
    int8_t AsciiToTime(uint8_t *Src,TIME_Type *pTime);
    uint8_t TimeToAscii(TIME_Type *pTime,uint8_t *Des);
    uint8_t TimeNoSecToAscii(TIME_Type *pTime,uint8_t *Des);
    uint8_t DateToAscii(TIME_Type *pTime,uint8_t *Des);
    uint8_t ClockTimeToAscii(TIME_Type *pTime,uint8_t *Des);
    uint8_t bcd_time_to_ascii(bcd_time_type  bcd_time, uint8_t *des);
    
    //灯光渐变控制
    uint32_t LedBreathHandle(led_breath_type *Brj);
    
    //数据舍入
    int32_t Sub2Add8(float f_real, int32_t i_now); //二舍八入
    int32_t SubAdd_x(float f_real, int32_t i_now, float val_sub); //自由舍入，舍入值val_sub[0.1-0.49]
    int32_t int_Sub2Add8(int32_t real, int32_t i_now);
    int32_t int_SubAdd_x(int32_t real, int32_t i_now, int32_t val_sub);
    
    /*key handle*/
    typedef struct {
        uint8_t  b_pulse_100ms;
        uint16_t found_time_cnt;
        uint16_t lost_time_cnt;
        uint16_t keep_time_cnt;
        uint32_t val;
        uint32_t old;
        uint32_t edge;
        uint32_t found_edge;
        uint32_t lost_edge;
        uint32_t read;
        uint32_t read_2;
        uint32_t read_3;
    } key_type;
    void key_st_handle(key_type *key);
    void key_st_isr(key_type *key, uint32_t key_read);
    
    /*io st*/
    typedef struct {
        uint8_t  b_time_pulse;  //计时脉冲
        uint8_t  b_sample; //采样脉冲    
        uint8_t  val   ;
        uint8_t  old    ;
        uint8_t found_edge;
        uint8_t lost_edge;
        uint8_t sample_old;
        uint8_t sample_keep_cnt;//0-UINT8_MAX
        uint32_t found_time_cnt;//0-UINT32_MAX
        uint32_t lost_time_cnt;//0-UINT32_MAX
    } switch_st_type;

    typedef struct {
        uint8_t  b_time_pulse;  //计时脉冲 
        uint8_t  old;
        uint8_t enable_edge;
        uint8_t disable_edge;
        uint32_t enable_time_cnt;//0-UINT32_MAX
        uint32_t disable_time_cnt;//0-UINT32_MAX
    } out_st_type;
    void switch_handle(switch_st_type *swobj, uint8_t sw_now);
    void out_st_handle(out_st_type *obj, uint8_t out_now);
    
#ifdef	__cplusplus
}
#endif

#endif	/* DATAPROC_H */

