/* ------------------------------------------------------------------------*
 *
 * ------------------------------------------------------------------------*/
#define _MBPORTX_MODULE_
#include "stm32f4xx.h"
#include "stm32f4xx_ll_usart.h"
#include "mbportx.h"
#include "pincfg.h"
#include "osObjects.h"
#include "mbdatacb.h"
#include "mbapp.h"

#define mbPORT                       USART6
#define mbURT_IRQn                   USART6_IRQn


//RS485 Dir-Pin
#define mbxRxEnable()
#define mbxTxEnable()
#define mbxDE(x)      //( (x)?(( alDirx = 0 )):(( alDirx = 1))) 

static void mbxEnable(uint8_t xRxEnable, uint8_t xTxEnable);
static void mbxDataSend(void);

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
void mbxTimeoutHandle(void)
{

    if (mbxObj.RunSt.bits.MasterMode && (mbxObj.RcvSt == MB_RX_WAIT))
    {
        mbxObj.TimeOutCnt++;
        if (mbxObj.TimeOutCnt > MMB_TIME_OUT_MS)
        {
            mbxObj.TimeOutCnt = 0;
            mbxObj.RcvSt = MB_RX_TIMEOUT;
            osSignalSet(tid_modbus, MBAPP_COMX_EVENT_TIMEOUT);
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

__STATIC_INLINE void mbxTimerEnable(void)
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

__STATIC_INLINE void mbxTimerDisable(void)
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

void mbxRtuTimerRun()
{

    if (bRtuTimerOn)
    {
        RtuTimerCnt++;
        if (RtuTimerCnt > RtuTimerSv)
        {
            //关闭定时器
            mbxTimerDisable();
            if (MB_RX_RCV == mbxObj.RcvSt)
            {

                //端口接收结束，告诉数据处理单元可以处理数据
                mbxObj.RcvSt = MB_RX_RCVEND;
                osSignalSet(tid_modbus, MBAPP_COMX_EVENT_RTUEND);
            }
        }
    }
}

/*****************************************************************************//*!
* @brief UART6 RX interrupt routine.
* @brief   Uart interrupt.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void USART6_IRQHandler()
{
    volatile uint32_t IntSt;
    volatile uint8_t Data;

    mbxObj.TimeOutCnt = 0;

    IntSt = mbPORT->SR;

    if (IntSt & USART_SR_RXNE)
    {
        mbxObj.RunSt.bits.PortSt = 1;

        Data = mbPORT->DR;

        if (MB_RX_RCVEND == mbxObj.RcvSt)
            return;

        mbxObj.RcvSt = MB_RX_RCV;                                                    //指示正在接收数据
        mbxTimerEnable();

        if (mbxObj.RcvCnt >= MB_BUFF_SIZE)
            return;                                                                  //指针越界检查

        if ((IntSt & (USART_SR_FE | USART_SR_ORE | USART_SR_PE)) != 0)                 //帧错误 //只读，由硬件管理
        {
            mbxObj.ErrSt.bits.ErrHal = 1;
        }

        mbxObj.AduBuff[mbxObj.RcvCnt++] = Data;

    }
    else if ((mbxObj.SndSize <= mbxObj.SndCnt) && (IntSt & USART_SR_TC))
    {
        mbxObj.SndSt = MB_TX_IDLE;                                               //发送结束   
        if (mbxObj.RunSt.bits.MasterMode)                                         //主机进入接收等待状态
            mbxObj.RcvSt = MB_RX_WAIT;
        mbxEnable(ENABLE, DISABLE);
    }
    else if (IntSt & USART_SR_TXE)
    {
        mbPORT->DR = mbxObj.AduBuff[mbxObj.SndCnt++];
        if (mbxObj.SndSize <= mbxObj.SndCnt)                                      //写缓冲区结束
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

void mbxObjInit(mbModeType mbMode)
{
    mbxObj.RunSt.St = 0;
    mbxObj.ErrSt.St = 0;
    mbxObj.SlaveId = 0;
    mbxObj.SndSt = MB_TX_IDLE;
    mbxObj.RcvSt = MB_RX_IDLE;
    mbxObj.TimeOutCnt = 0;
    mbxObj.RcvCnt = 0;
    mbxObj.SndCnt = 0;
    mbxObj.SndSize = 0;
    mbxObj.AduSend = &mbxDataSend;

    mbxObj.RunSt.bits.RTUMode = 1;

    //SLAVE FUNCTION INIT
    if (MB_RTU_SLAVE == mbMode)
    {
        mbxObj.Fun.Slave.RegCoilsCB = &eMBRegCoilsCB;
        mbxObj.Fun.Slave.RegDiscreteCB = &eMBRegDiscreteCB;
        mbxObj.Fun.Slave.RegHoldingCB = &eMBRegHoldingCB;
        mbxObj.Fun.Slave.RegInputCB = &eMBRegInputCB;
    }
    else
    {
        //MASER FUNCTION INIT
        mbxObj.RunSt.bits.MasterMode = 1;
        sysapi->mbCmdBuffInit(&mbxObj, mbxCmdBuff, 16, 6);
        mbxObj.Fun.Master.ErrCnt = 0;
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

void mbxInit(mbModeType mbMode, uint32_t ulBaudRate, mbParityType eParity)
{
    LL_USART_InitTypeDef UPar;
    uint32_t usTimerT35_50us;

    __HAL_RCC_USART6_CLK_ENABLE();
    __HAL_RCC_USART6_FORCE_RESET();
    NOP10();
    __HAL_RCC_USART6_RELEASE_RESET();

    mbxObjInit(mbMode);

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

    mbxEnable(ENABLE, DISABLE);

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

static void mbxEnable(uint8_t xRxEnable, uint8_t xTxEnable)
{
    volatile uint8_t u8Temp;

    if (xRxEnable)
    {
        mbPORT->CR1 &= ~((uint32_t)USART_CR1_TCIE | USART_CR1_TXEIE);

        mbxRxEnable();

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

        mbxTxEnable();

        mbPORT->CR1 &= ~((uint32_t)USART_CR1_RE);
        mbPORT->CR1 |= (uint32_t)USART_CR1_TE;

        mbPORT->DR = mbxObj.AduBuff[mbxObj.SndCnt++];
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

void mbxDataSend(void)
{
    mbxEnable(DISABLE, ENABLE);
}

