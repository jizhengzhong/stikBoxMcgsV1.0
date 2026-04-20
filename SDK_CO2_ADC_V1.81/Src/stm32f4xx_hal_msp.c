/**
  ******************************************************************************
  * File Name          : stm32f4xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization
  *                      and de-Initialization codes.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#define ADC_EV_Pin GPIO_PIN_0
#define ADC_EV_GPIO_Port GPIOC
#define ADC_HUMI_Pin GPIO_PIN_0
#define ADC_HUMI_GPIO_Port GPIOA
#define ADC_CO2_Pin GPIO_PIN_1
#define ADC_CO2_GPIO_Port GPIOA
#define ADC_BOX_Pin GPIO_PIN_0
#define ADC_BOX_GPIO_Port GPIOB
#define ADC_DOOR_Pin GPIO_PIN_1
#define ADC_DOOR_GPIO_Port GPIOB
extern DMA_HandleTypeDef hdma_spi2_tx;

extern DMA_HandleTypeDef hdma_sdio;
extern DMA_HandleTypeDef hdma_adc1;

extern void _Error_Handler(char *, int);
/* USER CODE BEGIN 0 */
static void Error_Handler(void);
/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
    /* USER CODE BEGIN MspInit 0 */

    /* USER CODE END MspInit 0 */

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* System interrupt init*/
    /* MemoryManagement_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    /* BusFault_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    /* UsageFault_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
    /* SVCall_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
    /* DebugMonitor_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
    /* PendSV_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

    /* USER CODE BEGIN MspInit 1 */

    /* USER CODE END MspInit 1 */
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{

     RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();//使能电源时钟PWR
	HAL_PWR_EnableBkUpAccess();//取消备份区域写保护
    
    RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_LSE;//LSE配置
    RCC_OscInitStruct.PLL.PLLState=RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState=RCC_LSE_ON;                  //RTC使用LSE
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    PeriphClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_RTC;//外设为RTC
    PeriphClkInitStruct.RTCClockSelection=RCC_RTCCLKSOURCE_LSE;//RTC时钟源为LSE
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
        
    __HAL_RCC_RTC_ENABLE();//RTC时钟使能

}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{

    if (hrtc->Instance == RTC)
    {
        /* USER CODE BEGIN RTC_MspDeInit 0 */

        /* USER CODE END RTC_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_RTC_DISABLE();
        /* USER CODE BEGIN RTC_MspDeInit 1 */

        /* USER CODE END RTC_MspDeInit 1 */
    }

}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if (huart->Instance == UART7)
    {
        /* USER CODE BEGIN UART7_MspInit 0 */

        /* USER CODE END UART7_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_UART7_CLK_ENABLE();

        /**UART7 GPIO Configuration
        PE7     ------> UART7_RX
        PE8     ------> UART7_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART7;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        /* USER CODE BEGIN UART7_MspInit 1 */

        /* USER CODE END UART7_MspInit 1 */
    }
    else if (huart->Instance == UART8)
    {
        /* USER CODE BEGIN UART8_MspInit 0 */

        /* USER CODE END UART8_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_UART8_CLK_ENABLE();

        /**UART8 GPIO Configuration
        PE0     ------> UART8_RX
        PE1     ------> UART8_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART8;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        /* USER CODE BEGIN UART8_MspInit 1 */

        /* USER CODE END UART8_MspInit 1 */
    }
    else if (huart->Instance == USART6)
    {
        /* USER CODE BEGIN USART1_MspInit 0 */

        /* USER CODE END USART1_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        /**USART1 GPIO Configuration
        PC6     ------> USART1_TX
        PC7     ------> USART1_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        /* USER CODE BEGIN USART1_MspInit 1 */

        /* USER CODE END USART1_MspInit 1 */
    }
    else if (huart->Instance == USART2)
    {
        /* USER CODE BEGIN USART2_MspInit 0 */

        /* USER CODE END USART2_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_USART2_CLK_ENABLE();

        /**USART2 GPIO Configuration
        PA3     ------> USART2_RX
        PA2     ------> USART2_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USER CODE BEGIN USART2_MspInit 1 */
        
         HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
         HAL_NVIC_EnableIRQ(USART2_IRQn);
	 huart->Instance->CR1 |= USART_CR1_RXNEIE;

        /* USER CODE END USART2_MspInit 1 */
    }
    else if (huart->Instance == USART3)
    {
        /* USER CODE BEGIN USART3_MspInit 0 */

        /* USER CODE END USART3_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_USART3_CLK_ENABLE();

        /**USART3 GPIO Configuration
        PB10     ------> USART3_TX
        PB11     ------> USART3_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USER CODE BEGIN USART3_MspInit 1 */

        /* USER CODE END USART3_MspInit 1 */
    }

}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

    if (huart->Instance == UART7)
    {
        /* USER CODE BEGIN UART7_MspDeInit 0 */

        /* USER CODE END UART7_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_UART7_CLK_DISABLE();

        /**UART7 GPIO Configuration
        PE7     ------> UART7_RX
        PE8     ------> UART7_TX
        */
        HAL_GPIO_DeInit(GPIOE, GPIO_PIN_7 | GPIO_PIN_8);

        /* USER CODE BEGIN UART7_MspDeInit 1 */

        /* USER CODE END UART7_MspDeInit 1 */
    }
    else if (huart->Instance == UART8)
    {
        /* USER CODE BEGIN UART8_MspDeInit 0 */

        /* USER CODE END UART8_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_UART8_CLK_DISABLE();

        /**UART8 GPIO Configuration
        PE0     ------> UART8_RX
        PE1     ------> UART8_TX
        */
        HAL_GPIO_DeInit(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);

        /* USER CODE BEGIN UART8_MspDeInit 1 */

        /* USER CODE END UART8_MspDeInit 1 */
    }
    else if (huart->Instance == USART1)
    {
        /* USER CODE BEGIN USART1_MspDeInit 0 */

        /* USER CODE END USART1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);

        /* USER CODE BEGIN USART1_MspDeInit 1 */

        /* USER CODE END USART1_MspDeInit 1 */
    }
    else if (huart->Instance == USART2)
    {
        /* USER CODE BEGIN USART2_MspDeInit 0 */

        /* USER CODE END USART2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART2_CLK_DISABLE();

        /**USART2 GPIO Configuration
        PA3     ------> USART2_RX
        PD5     ------> USART2_TX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);

        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_5);

        /* USER CODE BEGIN USART2_MspDeInit 1 */

        /* USER CODE END USART2_MspDeInit 1 */
    }
    else if (huart->Instance == USART3)
    {
        /* USER CODE BEGIN USART3_MspDeInit 0 */

        /* USER CODE END USART3_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART3_CLK_DISABLE();

        /**USART3 GPIO Configuration
        PD8     ------> USART3_TX
        PD9     ------> USART3_RX
        */
        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8 | GPIO_PIN_9);

        /* USER CODE BEGIN USART3_MspDeInit 1 */

        /* USER CODE END USART3_MspDeInit 1 */
    }

}


/**
* @brief SD MSP Initialization
* This function configures the hardware resources used in this example
* @param hsd: SD handle pointer
* @retval None
*/
void HAL_SD_MspInit(SD_HandleTypeDef* hsd)
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    if (hsd->Instance == SDIO)
    {
        /* USER CODE BEGIN SDIO_MspInit 0 */

        /* USER CODE END SDIO_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_SDIO_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        /**SDIO GPIO Configuration
        PC8     ------> SDIO_D0
        PC9     ------> SDIO_D1
        PC10     ------> SDIO_D2
        PC11     ------> SDIO_D3
        PC12     ------> SDIO_CK
        PD2     ------> SDIO_CMD
        */
        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11
            | GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        /* USER CODE BEGIN SDIO_MspInit 1 */

        /* USER CODE END SDIO_MspInit 1 */
    }

}

/**
* @brief SD MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hsd: SD handle pointer
* @retval None
*/
void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{
    if (hsd->Instance == SDIO)
    {
        /* USER CODE BEGIN SDIO_MspDeInit 0 */

        /* USER CODE END SDIO_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_SDIO_CLK_DISABLE();

        /**SDIO GPIO Configuration
        PC8     ------> SDIO_D0
        PC9     ------> SDIO_D1
        PC10     ------> SDIO_D2
        PC11     ------> SDIO_D3
        PC12     ------> SDIO_CK
        PD2     ------> SDIO_CMD
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11
            | GPIO_PIN_12);

        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

        /* USER CODE BEGIN SDIO_MspDeInit 1 */

        /* USER CODE END SDIO_MspDeInit 1 */
    }

}


/* USER CODE BEGIN 1 */
/**
  * Initializes the Global MSP.
  */
//static void HAL_MspInit(void)
//{
//  /* USER CODE BEGIN MspInit 0 */
//
//  /* USER CODE END MspInit 0 */
//
//  __HAL_RCC_SYSCFG_CLK_ENABLE();
//  __HAL_RCC_PWR_CLK_ENABLE();
//
//  /* System interrupt init*/
//
//  /* USER CODE BEGIN MspInit 1 */
//
//  /* USER CODE END MspInit 1 */
//}

/**
* @brief ADC MSP Initialization
* This function configures the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PC0     ------> ADC1_IN10
    PA0/WKUP     ------> ADC1_IN0
    PA1     ------> ADC1_IN1
    PB0     ------> ADC1_IN8
    PB1     ------> ADC1_IN9
    */
    GPIO_InitStruct.Pin = ADC_EV_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC_EV_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ADC_HUMI_Pin|ADC_CO2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ADC_BOX_Pin|ADC_DOOR_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* ADC1 DMA Init */
    /* ADC1 Init */
    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc1);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }

}

/**
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PC0     ------> ADC1_IN10
    PA0/WKUP     ------> ADC1_IN0
    PA1     ------> ADC1_IN1
    PB0     ------> ADC1_IN8
    PB1     ------> ADC1_IN9
    */
    HAL_GPIO_DeInit(ADC_EV_GPIO_Port, ADC_EV_Pin);

    HAL_GPIO_DeInit(GPIOA, ADC_HUMI_Pin|ADC_CO2_Pin);

    HAL_GPIO_DeInit(GPIOB, ADC_BOX_Pin|ADC_DOOR_Pin);

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(hadc->DMA_Handle);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
static void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}
/* USER CODE END 1 */

