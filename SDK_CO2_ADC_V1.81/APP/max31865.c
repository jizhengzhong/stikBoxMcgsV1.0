#include "sys.h"
#include "pincfg.h"

#include "max31865.h"
#include "pincfg.h"

//////////////////////////////////////////////////////////////////////////////////	 
// 模块特征
// 连续转换大约 每次转换大约21ms
// 单次转换 每次大约 52ms(60HZ滤波) 62ms(50HZ滤波)
////////////////////////////////////////////////////////////////////////////////// 	

void temp_AlSpiInit(void);
uint8_t temp_AlSpiRw(uint8_t wData);
void temp_AlSpiCs(uint8_t Cs);

#define SpiRw(x)       temp_AlSpiRw(x)   

#define max31865_enable()       temp_AlSpiCs(0)
#define max31865_disable()      temp_AlSpiCs(1)

/*****************************************************************************//*!
* @brief   	max38165 data read.
*               
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/ 
uint8_t max31865_read(uint8_t addr){
    
    uint8_t Src[2];
    uint8_t Des[2];
    
    max31865_disable();
    NOP20();
    max31865_enable();
    NOP20();
    
    Src[0] = addr;
    Src[1] = 0xFF;
    
    Des[0] = SpiRw(Src[0]);
    Des[1] = SpiRw(Src[1]);
    
    max31865_disable();
    
    return Des[1];
}


/*****************************************************************************//*!
* @brief   	max38165 data wrire.
*               
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/ 
uint8_t max31865_write(uint8_t addr, uint8_t data){
    uint8_t Src[2];
    uint8_t Des[2];
    
    max31865_disable();
    NOP20();
    max31865_enable();
    NOP20();
    
    Src[0] = addr;
    Src[1] = data;
    
    Des[0] = SpiRw(Src[0]);
    Des[1] = SpiRw(Src[1]);
    
    max31865_disable();
    
    return Des[1];
}

/*****************************************************************************//*!
* @brief   	max38165 config.
*               
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/ 
uint8_t max31865_Cfg(uint8_t cfg){
    uint8_t cfgR;
    
    for(uint8_t faultCnt = 0;faultCnt<3;faultCnt++){
        max31865_write(MAX31865_CFG|0x80, cfg); //写入配置
        cfgR = max31865_read(MAX31865_CFG);     //读回配置
        if((cfgR & 0xDD) == (cfg & 0xDD)){
            cfgR &= 0xDD;
            break;
        }
    } 
    return cfgR;
}

/*****************************************************************************//*!
* @brief   	max38165 rdt read.
*               
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/ 
uint16_t max31865_rtd_read(void){
    uint16_t resl,resh;
    
    resh = max31865_read(MAX31865_RTD_MSB);
    resl = max31865_read(MAX31865_RTD_LSB);
    
    return (((resh << 8) | (resl & 0xFF)) >> 1);
}

/*****************************************************************************//*!
* @brief   	max38165 high val set.
*               
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/ 
uint16_t max31865_high_set(uint16_t val){
    uint8_t dr,dw;
    uint16_t fault;
    
    fault = val;
    
    dw = val & 0xFF;
    for(uint8_t faultCnt = 0;faultCnt<5;faultCnt++){
        max31865_write(MAX31865_HIGH_LSB|0x80, dw);  //写入配置
        dr = max31865_read(MAX31865_HIGH_LSB);     //读回配置
        if(dr == dw){
            fault &= 0xFF00;
            break;
        }
    } 
    
    dw = (val>>8) & 0xFF;
    for(uint8_t faultCnt = 0;faultCnt<5;faultCnt++){
        max31865_write(MAX31865_HIGH_MSB|0x80, dw);  //写入配置
        dr = max31865_read(MAX31865_HIGH_MSB);     //读回配置
        if(dr == dw){
            fault &= 0x00FF;
            break;
        }
    } 
    
    return fault;
}

/*****************************************************************************//*!
* @brief   	max38165 low val set.
*               
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/ 
uint16_t max31865_low_set(uint16_t val){
    uint8_t dr,dw;
    uint16_t fault;
    
    fault = val;
    
    dw = val & 0xFF;
    for(uint8_t faultCnt = 0;faultCnt<5;faultCnt++){
        max31865_write(MAX31865_LOW_LSB|0x80, dw);  //写入配置
        dr = max31865_read(MAX31865_LOW_LSB);     //读回配置
        if(dr == dw){
            fault &= 0xFF00;
            break;
        }
    } 
    
    dw = (val>>8) & 0xFF;
    for(uint8_t faultCnt = 0;faultCnt<5;faultCnt++){
        max31865_write(MAX31865_LOW_MSB|0x80, dw);  //写入配置
        dr = max31865_read(MAX31865_LOW_MSB);     //读回配置
        if(dr == dw){
            fault &= 0x00FF;
            break;
        }
    } 
    
    return fault;
}

/*****************************************************************************//*!
* @brief   	max38165 fault read.
*               
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/ 
uint8_t max31865_fault_read(void){
    return max31865_read(MAX31865_FAULT);
}
