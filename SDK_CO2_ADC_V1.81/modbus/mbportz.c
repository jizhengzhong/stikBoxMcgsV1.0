/* ------------------------------------------------------------------------*
 *
 * ------------------------------------------------------------------------*/
#define _MBPORTZ_MODULE_
#include "stm32f4xx.h"
#include "stm32f4xx_ll_usart.h"
#include "mbportz.h"
#include "pincfg.h"
#include "osObjects.h"
#include "mbdatacb.h"
#include "mbapp.h"

#define mbPORT                       UART7
#define mbURT_IRQn                   UART7_IRQn

uint8_t alDirz;

//RS485 Dir-Pin
#define mbzRxEnable() //( OUT_RS485_RE = 0 ) 
#define mbzTxEnable() //( OUT_RS485_RE = 1 )
#define mbzDE(x)      //( (x)?(( OUT_RS485_RE = 0 )):(( OUT_RS485_RE = 1))) 

static void mbzEnable(uint8_t xRxEnable, uint8_t xTxEnable);
void mbzDataSend(void);

static uint8_t RtuTimerCnt;
static uint8_t RtuTimerSv;
static uint8_t bRtuTimerOn;

/*****************************************************************************//*!
*
* @brief   Timeout handle.
*          called from ext timer 1 ms
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void mbzTimeoutHandle(void)
{

    if (mbzObj.RunSt.bits.MasterMode && (mbzObj.RcvSt == MB_RX_WAIT))
    {
        mbzObj.TimeOutCnt++;
        if (mbzObj.TimeOutCnt > 100)
        {
            mbzObj.TimeOutCnt = 0;
            mbzObj.RcvSt = MB_RX_TIMEOUT;
            osSignalSet(tid_Co2Get, MBAPP_COMZ_EVENT_TIMEOUT);
        }
    }
}

/*****************************************************************************//*!
*
* @brief   RTU timer enable.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

__STATIC_INLINE void mbzTimerEnable(void)
{
    bRtuTimerOn = 1;
    RtuTimerCnt = 0;
}

/*****************************************************************************//*!
*
* @brief   RTU timer disable.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

__STATIC_INLINE void mbzTimerDisable(void)
{
    bRtuTimerOn = 0;
    RtuTimerCnt = 0;
}

/*****************************************************************************//*!
*
* @brief   com end timer.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void mbzRtuTimerRun()
{

    if (bRtuTimerOn)
    {
        RtuTimerCnt++;
        if (RtuTimerCnt > RtuTimerSv * 3 / 2)
        {
            //关闭定时器
            mbzTimerDisable();
            if (MB_RX_RCV == mbzObj.RcvSt)
            {

                //端口接收结束，告诉数据处理单元可以处理数据
                mbzObj.RcvSt = MB_RX_RCVEND;
                osSignalSet(tid_Co2Get, MBAPP_COMZ_EVENT_RTUEND);
            }
        }
    }
}

/*****************************************************************************//*!
* @brief UART0 RX interrupt routine.
* @brief   Uart interrupt.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void UART7_IRQHandler()
{
    volatile uint32_t IntSt;
    volatile uint8_t Data;

    mbzObj.TimeOutCnt = 0;

    IntSt = mbPORT->SR;

    if (IntSt & USART_SR_RXNE)
    {
        mbzObj.RunSt.bits.PortSt = 1;



        Data = mbPORT->DR;

        if (MB_RX_RCVEND == mbzObj.RcvSt)
            return;

        mbzObj.RcvSt = MB_RX_RCV;                                                    //指示正在接收数据
        mbzTimerEnable();

        if (mbzObj.RcvCnt >= MB_BUFF_SIZE)
            return;                                                                  //指针越界检查

        if ((IntSt & (USART_SR_FE | USART_SR_ORE | USART_SR_PE)) != 0)                 //帧错误 //只读，由硬件管理
        {
            mbzObj.ErrSt.bits.ErrHal = 1;
        }

        mbzObj.AduBuff[mbzObj.RcvCnt++] = Data;

    }
    else if ((mbzObj.SndSize <= mbzObj.SndCnt) && (IntSt & USART_SR_TC))
    {
        mbzObj.SndSt = MB_TX_IDLE;                                               //发送结束   
        if (mbzObj.RunSt.bits.MasterMode)                                         //主机进入接收等待状态
            mbzObj.RcvSt = MB_RX_WAIT;
        mbzEnable(ENABLE, DISABLE);
    }
    else if (IntSt & USART_SR_TXE)
    {
        mbPORT->DR = mbzObj.AduBuff[mbzObj.SndCnt++];
        if (mbzObj.SndSize <= mbzObj.SndCnt)                                      //写缓冲区结束
        {
            mbPORT->CR1 &= ~((uint32_t)(USART_CR1_TXEIE));
            mbPORT->CR1 |= (uint32_t)(USART_CR1_TCIE);
        }
    }
    else
    {

    }
}

/*****************************************************************************//*!
*
* @brief   mb object init.
*
* @param   none
*
* @return  TURE
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void mbzObjInit(mbModeType mbMode)
{
    mbzObj.RunSt.St = 0;
    mbzObj.ErrSt.St = 0;
    mbzObj.SlaveId = 0;
    mbzObj.SndSt = MB_TX_IDLE;
    mbzObj.RcvSt = MB_RX_IDLE;
    mbzObj.TimeOutCnt = 0;
    mbzObj.RcvCnt = 0;
    mbzObj.SndCnt = 0;
    mbzObj.SndSize = 0;
    mbzObj.AduSend = &mbzDataSend;

    mbzObj.RunSt.bits.RTUMode = 1;

    //SLAVE FUNCTION INIT
    if (MB_RTU_SLAVE == mbMode)
    {
        mbzObj.Fun.Slave.RegCoilsCB = &eMBRegCoilsCB;
        mbzObj.Fun.Slave.RegDiscreteCB = &eMBRegDiscreteCB;
        mbzObj.Fun.Slave.RegHoldingCB = &eMBRegHoldingCB;
        mbzObj.Fun.Slave.RegInputCB = &eMBRegInputCB;

    }
    else
    {
        //MASER FUNCTION INIT
        mbzObj.RunSt.bits.MasterMode = 1;
        sysapi->mbCmdBuffInit(&mbzObj, mbzCmdBuff, 16, 6);
        mbzObj.Fun.Master.ErrCnt = 0;
    }
}

/*****************************************************************************//*!
* @brief   	com modle init .
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void mbzInit(mbModeType mbMode, uint32_t ulBaudRate, mbParityType eParity)
{

    LL_USART_InitTypeDef UPar;
    uint32_t usTimerT35_50us;

    __HAL_RCC_UART7_CLK_ENABLE();
    __HAL_RCC_UART7_FORCE_RESET();
    NOP10();
    __HAL_RCC_UART7_RELEASE_RESET();

    mbzObjInit(mbMode);

    UPar.BaudRate = ulBaudRate;
    UPar.TransferDirection = LL_USART_DIRECTION_NONE;
    UPar.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    UPar.OverSampling = LL_USART_OVERSAMPLING_16;

    if (MB_PAR_NONE == eParity)
    {
        UPar.DataWidth = LL_USART_DATAWIDTH_8B;
        UPar.StopBits = LL_USART_STOPBITS_2;
        UPar.Parity = LL_USART_PARITY_NONE;
    }
    else if (MB_PAR_ODD == eParity)
    {
        UPar.DataWidth = LL_USART_DATAWIDTH_9B;
        UPar.StopBits = LL_USART_STOPBITS_1;
        UPar.Parity = LL_USART_PARITY_ODD;
    }
    else
    {
        UPar.DataWidth = LL_USART_DATAWIDTH_9B;
        UPar.StopBits = LL_USART_STOPBITS_1;
        UPar.Parity = LL_USART_PARITY_EVEN;
    }

    if (ulBaudRate > 19200)
    {
        usTimerT35_50us = 35; /* 1800us. */
    }
    else
    {
        usTimerT35_50us = (7UL * 220000UL) / (2UL * ulBaudRate);
    }

    RtuTimerSv = (usTimerT35_50us + 10) / 20 + 1;

    LL_USART_Init(mbPORT, &UPar);

    NVIC_SetPriority(mbURT_IRQn, 0xFF);
    NVIC_ClearPendingIRQ(mbURT_IRQn);
    NVIC_EnableIRQ(mbURT_IRQn);

    mbzEnable(ENABLE, DISABLE);

    mbPORT->CR1 |= USART_CR1_UE;
}

/*****************************************************************************//*!
*
* @brief   Uart En or Dis.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

static void mbzEnable(uint8_t xRxEnable, uint8_t xTxEnable)
{
    volatile uint8_t u8Temp;

    if (xRxEnable)
    {
        mbPORT->CR1 &= ~((uint32_t)USART_CR1_TCIE | USART_CR1_TXEIE);

        mbzRxEnable();

        mbPORT->CR1 |= (uint32_t)USART_CR1_RE;

        u8Temp = mbPORT->DR;
        u8Temp = mbPORT->SR;
        while ((mbPORT->SR & (USART_SR_ORE | USART_SR_RXNE)) != 0)
        {
            u8Temp = mbPORT->DR;
            u8Temp = mbPORT->SR;
        }

        mbPORT->CR1 |= USART_CR1_RXNEIE;

    }
    else if (xTxEnable)
    {
        mbPORT->CR1 &= ~((uint32_t)USART_CR1_RXNEIE);

        mbzTxEnable();

        mbPORT->CR1 &= ~((uint32_t)USART_CR1_RE);
        mbPORT->CR1 |= (uint32_t)USART_CR1_TE;

        mbPORT->DR = mbzObj.AduBuff[mbzObj.SndCnt++];
        mbPORT->CR1 |= (uint32_t)(USART_CR1_TXEIE);
    }
    else
    {

    }
}

/*****************************************************************************//*!
*
* @brief   Send data.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void mbzDataSend(void)
{
    mbzEnable(DISABLE, ENABLE);
}

