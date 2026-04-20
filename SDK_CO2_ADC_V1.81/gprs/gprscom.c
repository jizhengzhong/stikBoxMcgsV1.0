/* ------------------------------------------------------------------------*
 *
 * ------------------------------------------------------------------------*/
#include "gprscom.h"
#include "osObjects.h"  
#include "pincfg.h"

static void GprsComSndEnable(uint8_t xTxEnable);
static void GprsComRcvEnable(uint8_t xRxEnable);

static COM_MAN_Type GprsCom;

#define COM                       USART3
#define ComIRQn                   USART3_IRQn
#define PortMan   GprsCom

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
__STATIC_INLINE void GprsComTimerEnable(void) {
    PortMan.TimeCnt=0;
    PortMan.bTimerOn = 1;
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

__STATIC_INLINE void GprsComTimerDisable(void) {
    PortMan.bTimerOn = 0;
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

void GprsComTimerRun() {

    if(PortMan.bTimerOn) {
        PortMan.TimeCnt++;
        if(PortMan.TimeCnt >= PortMan.TimeSv) {
            //关闭定时器
            GprsComTimerDisable();
            if (COM_RX_RCV == PortMan.RcvSt) {
                //端口接收结束，告诉数据处理单元可以处理数据
                PortMan.RcvSt = COM_RX_RCVEND;
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

void USART3_IRQHandler() {
    volatile uint32_t IntSt;
	volatile uint8_t Data; 
     
    IntSt = COM->SR;
	
    if(IntSt & USART_SR_RXNE)
    {
        GprsComTimerEnable();
        
        Data = COM->DR;
            
        if( USART2->SR & USART_SR_TXE)
            USART2->DR = Data;
         
        if(COM_RX_RCVEND == PortMan.RcvSt)
            return;

        PortMan.RcvSt = COM_RX_RCV;                                                    //指示正在接收数据
        
        if(PortMan.RcvCnt >= COM_BUFF_SIZE)
            return;                                                                  //指针越界检查
       
        if((IntSt & (USART_SR_FE | USART_SR_ORE | USART_SR_PE) ) != 0)                 //帧错误 //只读，由硬件管理
        {
            PortMan.St.bits.ErrHal = 1;
        }
        
	    PortMan.Buff[PortMan.RcvCnt ++] = Data;  
        
    }
	else if( (0 == PortMan.SndCnt) && (IntSt & USART_SR_TC))
    {
        PortMan.SndSt = COM_TX_IDLE;                                               //发送结束    
        COM->CR1   &= ~((uint32_t)USART_CR1_TCIE | USART_CR1_TXEIE);
    }
    else if( IntSt & USART_SR_TXE)
    {
		COM->DR  =  *PortMan.pSndCur++;
        PortMan.SndCnt--;
		if(0 == PortMan.SndCnt)                                      //写缓冲区结束
		{
            COM->CR1   &= ~((uint32_t)(USART_CR1_TXEIE));
            COM->CR1   |= (uint32_t)(USART_CR1_TCIE);
		}        
    }
    else
    {
        
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

void GprsComInit(uint32_t ulBaudRate, ComParity_t eParity) {
    PortMan.SndSt = COM_TX_IDLE;
    PortMan.RcvSt = COM_RX_IDLE;


    GprsComSndEnable(ENABLE);
    GprsComRcvEnable(ENABLE);
    
    PortMan.TimeSv = 50;
    
    //允许中断
    NVIC_SetPriority(ComIRQn, 0x6);
	NVIC_ClearPendingIRQ(ComIRQn);
	NVIC_EnableIRQ(ComIRQn);	
}

static void GprsComSndEnable(uint8_t xTxEnable) {
    if (xTxEnable) {
		COM->CR1   |= (uint32_t)USART_CR1_TE;		
        
        if(  PortMan.SndCnt ){
            PortMan.SndCnt--;
            COM->DR     =  *PortMan.pSndCur++;
            COM->CR1   |= (uint32_t)(USART_CR1_TXEIE);
        }
    } else {
        COM->CR1   &= ~(uint32_t)USART_CR1_TE;	
        COM->CR1   &= ~((uint32_t)USART_CR1_TCIE | USART_CR1_TXEIE);
    }
}

static void GprsComRcvEnable(uint8_t xRxEnable) {
    volatile uint8_t u8Temp;

    if (xRxEnable) {
		COM->CR1   |= (uint32_t)USART_CR1_RE;
		
		u8Temp        = COM->DR;
		u8Temp        = COM->SR;		
		while((COM->SR & (USART_SR_ORE | USART_SR_RXNE)) != 0)
		{
			u8Temp        = COM->DR;
			u8Temp        = COM->SR;	
		}
		
		COM->CR1   |= USART_CR1_RXNEIE;
    } else {
        COM->CR1   &= ~((uint32_t)USART_CR1_RXNEIE);	    
		COM->CR1   &= ~((uint32_t) USART_CR1_RE );
    }
}

/*****************************************************************************//*!
*
* @brief   Send a string.
*             
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void GprsComStringSend(uint8_t * pData) {
    if (COM_TX_IDLE == PortMan.SndSt) {
        PortMan.pSndCur = pData;
        PortMan.SndCnt = 0;
        while(*pData++)
            PortMan.SndCnt++;
        PortMan.SndSt = COM_TX_XMIT;
        GprsComSndEnable(ENABLE);
    } else {

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

void GprsComDataSend(uint8_t * pData, uint16_t Len) {
    if (COM_TX_IDLE == PortMan.SndSt) {
        PortMan.SndCnt = Len;
        PortMan.SndSt = COM_TX_XMIT;
        PortMan.pSndCur = pData;
        GprsComSndEnable(ENABLE);
    } else {

    }
}

/*****************************************************************************//*!
*
* @brief   Port state check.
*             
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

uint8_t GprsComStCheck(void) {
    if (PortMan.SndSt != COM_TX_IDLE)
        return 0;
    if (PortMan.RcvSt != COM_RX_IDLE)
        return 0;
    return 1;
}
