/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ParaTask_H__
#define __ParaTask_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
	
typedef  void (*EventFunEntry)( unsigned char evenId,manager *pFlowPcb);
	typedef enum
{
  EV_STARTSTOPCHECK=0,
  EV_E1ALARM,
	EV_E2ALARM,
  EV_E3ALARM,
	EV_E4ALARM,
  EV_E5ALARM,
	EV_E8ALARM,
  EV_E9ALARM,
	EV_E11ALARM,
  EV_E12ALARM,
} EVId;
typedef struct
{

	EVId  EventId;				/* 功能ID */
	EventFunEntry ActFun;		/* 功能函数 */
} T_ACT_FUN;

extern uint8_t ParCheck(uint16_t Pos, int16_t iVal);
extern void Init(void);
extern void ParLoad(void);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

