#ifndef _COM_HAL_H_
#define	_COM_HAL_H_

#include "stdint.h"

#define COM_BUFF_SIZE             1024

typedef enum {
    COM_RX_IDLE, //!< Receiver is in idle state.
    COM_RX_RCV, //!< Frame is beeing received.
    COM_RX_RCVEND, //!< Port Receive complete .
    COM_RX_WAIT,
    COM_RX_TIMEOUT //!< Time out ,Send a command and no data return after 500ms( --> system timer)

} ComRcvState_t;

typedef enum {
    COM_TX_IDLE, /*!< Transmitter is in idle state. */
    COM_TX_XMIT /*!< Transmitter is in transfer state. */
} ComSndState_t;

typedef enum {
    COM_PAR_NONE, /*!< No parity. */
    COM_PAR_ODD, /*!< Odd parity. */
    COM_PAR_EVEN /*!< Even parity. */
} ComParity_t;

typedef union {
    uint8_t St;

    struct {
        uint8_t PortSt : 1;
        uint8_t Connect : 1;
        uint8_t ErrHal  :1;
    } bits;
} COM_ST_Type;


typedef struct {
    uint8_t Buff[COM_BUFF_SIZE + 8];
    COM_ST_Type St;

    uint8_t *pSndCur;
    uint8_t bTimerOn;
    uint8_t TimeCnt;
    uint8_t TimeSv;
    
    uint16_t SndCnt;
    uint16_t RcvCnt;

    ComSndState_t SndSt;
    ComRcvState_t RcvSt;
} COM_MAN_Type;

void ComInit(uint32_t ulBaudRate, ComParity_t eParity);
void ComParity(ComParity_t eParity);
void ComStringSend(uint8_t * pData);
void ComDataSend(uint8_t * pData, uint16_t Len);
uint8_t ComStCheck(void); //¶Ë¿Ú¿ÕÏÐ×´Ì¬¼ì²é


#endif	/* _COM_HAL_H_ */

