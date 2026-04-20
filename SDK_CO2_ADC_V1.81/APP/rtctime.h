#ifndef _RTC_TIME_H_
#define	_RTC_TIME_H_

#include "stdint.h"

#ifdef   _RTC_TIME_MODULE_
#define  TIME_EXT
#else
#define  TIME_EXT  extern
#endif      
    
    typedef struct {
        uint8_t Sec; 		/*!< Seconds Register */
        uint8_t Min; 		/*!< Minutes Register */
        uint8_t Hour; 		/*!< Hours Register */
		uint8_t Week;       /*!< Week Register */
        uint8_t Day;		/*!< Day of Month Register */
        uint8_t Mon; 	    /*!< Months Register */
        uint16_t Year; 		/*!< Years Register */
    } TIME_Type;

//*********************************Time ***********************************//
    typedef union{
        uint32_t val;
        struct {
            uint32_t sec2t:5;
            uint32_t min  :6;
            uint32_t hour :5;
            uint32_t day  :5;
            uint32_t mon  :4;
            uint32_t year :7;
        } time;
    } sig_time_type;

    typedef union{
        uint32_t val;
        struct {
            uint32_t min  :7;
            uint32_t hour :6;
            uint32_t day  :6;
            uint32_t mon  :5;
            uint32_t year :8;
        } time;
    } bcd_time_type;

    TIME_EXT TIME_Type TimeCur;
    TIME_EXT TIME_Type TimeStart;  //order time for start
    TIME_EXT TIME_Type TimeStop;   //order time for stop
    TIME_EXT uint16_t CenturyCur;
    
    TIME_EXT uint8_t RtcTimeSecLast;
    TIME_EXT uint8_t bRtcPulse_1s;
    
    void TimeRun(void);
    void TimeSet(TIME_Type *pTime);
    void TimeGet(TIME_Type *pTime);
    int8_t TimeCompare(TIME_Type *pTime1,TIME_Type *pTime2);//* @pTime1-pTime2¡¾  == : 0        >: 1           <: -1 ¡¿
    void TimeCopy(TIME_Type *pTimeSrc,TIME_Type *pTimeDes);
    void RtcTimePulseCreat(void);
    
    bcd_time_type time_sig_to_bcd( sig_time_type sig_time);
    sig_time_type  time_bcd_to_sig(bcd_time_type  bcd_time);
    bcd_time_type time_to_bcd( TIME_Type * time);
    
#undef TIME_EXT

#endif	/* _RTC_TIME_H_ */

