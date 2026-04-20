/****************************************************************************
 * spi for sdcard
 * Tested MCU
 * Owner : apleilx
 * **************************************************************************/
#include "pincfg.h"

#define LOW_SPEED_CLKN      20

volatile uint8_t tcs_sel;

#define PulseDelay()\
{\
    uint8_t clk_cnt;\
    clk_cnt = LOW_SPEED_CLKN;\
            while(clk_cnt--) {\
        NOP10();\
                        }\
}\

/*-----------------------------------------------------------------------------
* MOSI -> PB1
* MISO -> PB3
* SCK -> PB2
* CS -> PB0
*-----------------------------------------------------------------------------*/
#define SCK         PAout(5)
#define MOSI        PAout(7)
#define MISO        PAin(6)
#define SS          PAout(4)
#define SS2          PCout(4)
#define SS3          PCout(5)

#define S_CS(x)     (x)?(SS=1):(SS = 0)
#define S_CS2(x)     (x)?(SS2=1):(SS2 = 0)
#define S_CS3(x)     (x)?(SS3=1):(SS3 = 0)

#define S_SCK(x)    (x)?(SCK=1):(SCK=0)
#define S_MOSI(x)   (x)?(MOSI=1):(MOSI = 0)
#define S_MISO()    MISO

/*****************************************************************************//*!
*
* @brief   api init.
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void temp_AlSpiInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    S_CS(1);
    S_CS2(1);
    S_CS3(1);
    S_MOSI(1);
    S_SCK(1);
}

void temp_cs_sel(uint8_t cs_sel)
{
    tcs_sel = cs_sel;
}


void temp_AlSpiCs(uint8_t Cs)
{
    if (Cs == 0)
    {
        switch (tcs_sel)
        {
            case 0:
                S_CS(0);
                break;
            case 1:
                S_CS2(0);
                break;
            case 2:
                S_CS3(0);
                break;
        }
    }
    else
    {
        S_CS(1);
        S_CS2(1);
        S_CS3(1);
    }

}


/*****************************************************************************//*!
*
* @brief   api rw.
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
uint8_t temp_AlSpiRw(uint8_t wData)
{
    uint8_t rData;
    uint8_t Cnt;

    rData = 0;

    for (Cnt = 0; Cnt < 8; Cnt++)
    {
        //主机发出一位数据
        S_SCK(0);
        S_MOSI(wData & 0x80);
        PulseDelay(); //数据保持
        //从机在每个上升沿采样，同时输出一位数据
        S_SCK(1);
        PulseDelay(); //等待从机输出稳定
        wData <<= 1;
        rData <<= 1;
        if (S_MISO()) //主机读取一位数据
            rData |= 0x01;
    }

    S_MOSI(1);

    return rData;
}
