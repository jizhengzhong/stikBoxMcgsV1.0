/* 
 * File:   alspi.h
 * Author: apleilx
 */

#ifndef SPI_AL_HALL_H
#define	SPI_AL_HALL_H

#include "stdint.h"

	
#ifdef	__cplusplus
extern "C" {
#endif
    
void AlSpiInit(void);
uint8_t AlSpiRw(uint8_t wData);
void AlSpiCs(uint8_t Cs);
void AlSpiSetSpeed(uint8_t SpSv);

#ifdef	__cplusplus
}
#endif


#endif	/* SPI_AL_HALL_H */

