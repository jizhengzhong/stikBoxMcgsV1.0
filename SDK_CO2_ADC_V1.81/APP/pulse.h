
#ifndef PULSE_H_H
#define	PULSE_H_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {

    uint8_t bPulse_1s;
    uint8_t bPulse_500ms;
    uint8_t bPulse_250ms;
    uint8_t bPulse_100ms;
    
    uint8_t bPulse_50ms;
    uint8_t bPulse_30ms;
    uint8_t bPulse_20ms;
    uint8_t bPulse_10ms;
    
    uint16_t PulseTimeCnt_1s;
    uint16_t PulseTimeCnt_500ms;
    uint16_t PulseTimeCnt_250ms;
    uint16_t PulseTimeCnt_100ms;

    uint16_t PulseTimeCnt_50ms;
    uint16_t PulseTimeCnt_30ms;
    uint16_t PulseTimeCnt_20ms;
    uint16_t PulseTimeCnt_10ms;
    
    uint16_t AutoCnt; //base in 10ms

    uint16_t uTickLast;
} timer_pulse_type;

extern timer_pulse_type CtrPls;
void    pulse_init(timer_pulse_type *pulse, uint16_t tickStart);
void    PulseCreat(timer_pulse_type *pulse, uint16_t uTick);
#define pulse_create                             PulseCreat

#ifdef	__cplusplus
}
#endif

#endif	/* LEDCTR_H */
