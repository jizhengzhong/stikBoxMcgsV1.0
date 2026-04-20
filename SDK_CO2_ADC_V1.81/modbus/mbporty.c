/* ------------------------------------------------------------------------*
 *
 * ------------------------------------------------------------------------*/
#define _MBPORTY_MODULE_
#include "stm32f4xx.h"
#include "stm32f4xx_ll_usart.h"
#include "mbporty.h"
#include "pincfg.h"
#include "osObjects.h"
#include "mbdatacb.h"
#include "mbapp.h"
#include "main.h"

#define mbPORT                       USART2
#define mbURT_IRQn                   USART2_IRQn

uint8_t alDiry;

//RS485 Dir-Pin
#define mbyRxEnable() ( alDiry = 0 ) 
#define mbyTxEnable() ( alDiry = 1 )
#define mbyDE(x)      ( (x)?(( alDiry = 1 )):(( alDiry = 0))) 

static void mbyEnable(uint8_t xRxEnable, uint8_t xTxEnable);
static void mbyDataSend(void);

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
void mbyTimeoutHandle(void)
{

    if (mbyObj.RunSt.bits.MasterMode && (mbyObj.RcvSt == MB_RX_WAIT))
    {
        mbyObj.TimeOutCnt++;
        if (mbyObj.TimeOutCnt > MMB_TIME_OUT_MS)
        {
            mbyObj.TimeOutCnt = 0;
            mbyObj.RcvSt = MB_RX_TIMEOUT;
            osSignalSet(tid_modbus, MBAPP_COMY_EVENT_TIMEOUT);
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

__STATIC_INLINE void mbyTimerEnable(void)
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

__STATIC_INLINE void mbyTimerDisable(void)
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

void mbyRtuTimerRun()
{

    if (bRtuTimerOn)
    {
        RtuTimerCnt++;
        if (RtuTimerCnt > RtuTimerSv)
        {
            //关闭定时器
            mbyTimerDisable();

            if (MB_RX_RCV == mbyObj.RcvSt)
            {

                //端口接收结束，告诉数据处理单元可以处理数据
                mbyObj.RcvSt = MB_RX_RCVEND;
                osSignalSet(tid_modbus, MBAPP_COMY_EVENT_RTUEND);
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

void USART2_IRQHandler()
{
//    volatile uint32_t IntSt;
//    volatile uint8_t Data;
    
    if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) == SET)
    {	
            TelephoneDefine.received_house[TelephoneDefine.house_num++]  = (uint8_t)( huart2.Instance->DR&0xFF);
            if(TelephoneDefine.house_num >19)
              TelephoneDefine.house_num = 0;
//            if(RINGBUFF_OK != ring_buff_push_data(&g_ring_buff, &ch, 1))
//            {
//              //Log_e("ring buff push err"); // do not printf in isr at normal situation
//            }		
	   __HAL_UART_CLEAR_FLAG(&huart2, UART_IT_RXNE);			
    }

//    if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE) == SET)
//	{
//
//		 ATReceiveCompleteFlag= 1;	//接收完成
//                __HAL_UART_CLEAR_FLAG(&huart2, UART_IT_IDLE);
//	}


//    mbyObj.TimeOutCnt = 0;
//
//    IntSt = mbPORT->SR;
//
//    if (IntSt & USART_SR_RXNE)
//    {
//        mbyObj.RunSt.bits.PortSt = 1;
//
//        Data = mbPORT->DR;
//
//        if (MB_RX_RCVEND == mbyObj.RcvSt)
//            return;
//
//        mbyObj.RcvSt = MB_RX_RCV;                                                    //指示正在接收数据
//        mbyTimerEnable();
//
//        if (mbyObj.RcvCnt >= MB_BUFF_SIZE)
//            return;                                                                  //指针越界检查
//
//        if ((IntSt & (USART_SR_FE | USART_SR_ORE | USART_SR_PE)) != 0)                 //帧错误 //只读，由硬件管理
//        {
//            mbyObj.ErrSt.bits.ErrHal = 1;
//        }
//
//        mbyObj.AduBuff[mbyObj.RcvCnt++] = Data;
//
//    }
//    else if ((mbyObj.SndSize <= mbyObj.SndCnt) && (IntSt & USART_SR_TC))
//    {
//        mbyObj.SndSt = MB_TX_IDLE;                                               //发送结束   
//        if (mbyObj.RunSt.bits.MasterMode)                                         //主机进入接收等待状态
//            mbyObj.RcvSt = MB_RX_WAIT;
//        mbyEnable(ENABLE, DISABLE);
//    }
//    else if (IntSt & USART_SR_TXE)
//    {
//        mbPORT->DR = mbyObj.AduBuff[mbyObj.SndCnt++];
//        if (mbyObj.SndSize <= mbyObj.SndCnt)                                      //写缓冲区结束
//        {
//            mbPORT->CR1 &= ~((uint32_t)(USART_CR1_TXEIE));
//            mbPORT->CR1 |= (uint32_t)(USART_CR1_TCIE);
//        }
//    }
//    else
//    {
//
//    }
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

void mbyObjInit(mbModeType mbMode)
{
    mbyObj.RunSt.St = 0;
    mbyObj.ErrSt.St = 0;
    mbyObj.SlaveId = 0;
    mbyObj.SndSt = MB_TX_IDLE;
    mbyObj.RcvSt = MB_RX_IDLE;
    mbyObj.TimeOutCnt = 0;
    mbyObj.RcvCnt = 0;
    mbyObj.SndCnt = 0;
    mbyObj.SndSize = 0;
    mbyObj.AduSend = &mbyDataSend;

    mbyObj.RunSt.bits.RTUMode = 1;

    //SLAVE FUNCTION INIT
    if (MB_RTU_SLAVE == mbMode)
    {
        mbyObj.Fun.Slave.RegCoilsCB = &eMBRegCoilsCB;
        mbyObj.Fun.Slave.RegDiscreteCB = &eMBRegDiscreteCB;
        mbyObj.Fun.Slave.RegHoldingCB = &eMBRegHoldingCB;
        mbyObj.Fun.Slave.RegInputCB = &eMBRegInputCB;
    }
    else
    {
        //MASER FUNCTION INIT
        mbyObj.RunSt.bits.MasterMode = 1;
        sysapi->mbCmdBuffInit(&mbyObj, mbyCmdBuff, MBY_CMD_BUFF_SIZE, MBY_SCMD_BUFF_SIZE);
        mbyObj.Fun.Master.ErrCnt = 0;
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

void mbyInit(mbModeType mbMode, uint32_t ulBaudRate, mbParityType eParity)
{


    LL_USART_InitTypeDef UPar;
    uint32_t usTimerT35_50us;

    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_USART2_FORCE_RESET();
    NOP30();
    __HAL_RCC_USART2_RELEASE_RESET();

    mbPORT->CR1 &= ~USART_CR1_UE;

//    mbyObjInit(mbMode);

    UPar.BaudRate = ulBaudRate;
    UPar.TransferDirection = LL_USART_DIRECTION_NONE;
    UPar.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    UPar.OverSampling = LL_USART_OVERSAMPLING_16;

    if (MB_PAR_NONE == eParity)
    {
        UPar.DataWidth = LL_USART_DATAWIDTH_8B;
        UPar.StopBits = LL_USART_STOPBITS_1;
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

    mbyEnable(ENABLE, DISABLE);

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

static void mbyEnable(uint8_t xRxEnable, uint8_t xTxEnable)
{
    volatile uint8_t u8Temp;

    if (xRxEnable)
    {
        mbPORT->CR1 &= ~((uint32_t)USART_CR1_TCIE | USART_CR1_TXEIE);

        mbyRxEnable();

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

        mbyTxEnable();

        mbPORT->CR1 &= ~((uint32_t)USART_CR1_RE);
        mbPORT->CR1 |= (uint32_t)USART_CR1_TE;

        mbPORT->DR = mbyObj.AduBuff[mbyObj.SndCnt++];
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

void mbyDataSend(void)
{
    mbyEnable(DISABLE, ENABLE);
}

