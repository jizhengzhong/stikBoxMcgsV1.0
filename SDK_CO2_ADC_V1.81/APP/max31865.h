#ifndef __MAX31865_H_H
#define __MAX31865_H_H
#include "stdint.h"
//////////////////////////////////////////////////////////////////////////////////	 								  
////////////////////////////////////////////////////////////////////////////////// 	

uint8_t max31865_read(uint8_t addr);
uint8_t max31865_write(uint8_t addr,uint8_t data);

uint8_t max31865_Cfg(uint8_t cfg);
uint16_t max31865_rtd_read(void);
uint16_t max31865_high_set(uint16_t val);
uint16_t max31865_low_set(uint16_t val);
uint8_t max31865_fault_read(void);

#define MAX31865_CFG               0x00
#define MAX31865_RTD_MSB           0x01
#define MAX31865_RTD_LSB           0x02
#define MAX31865_HIGH_MSB          0x03
#define MAX31865_HIGH_LSB          0x04
#define MAX31865_LOW_MSB           0x05
#define MAX31865_LOW_LSB           0x06
#define MAX31865_FAULT             0x07

#endif
