/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2022-11-25 20:45:16
 * @LastEditors: None
 * @LastEditTime: 2022-12-01 00:07:16
 */
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f1xx_it.h
 * @brief   This file contains the headers of the interrupt handlers.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#ifndef __STM32F1xx_IT_H
#define __STM32F1xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void DMA1_Channel4_IRQHandler(void);
void DMA1_Channel5_IRQHandler(void);
void DMA1_Channel7_IRQHandler(void);
void TIM1_UP_IRQHandler(void);
void TIM3_IRQHandler(void);
void USART1_IRQHandler(void);
/* USER CODE BEGIN EFP */
    extern unsigned char Uart1_DataMax[100];
    extern unsigned char Uart1_DataMax_Length;
    extern unsigned char Uart1_DataValid_Length;
    extern unsigned char Uart1_Rx_Flag;
    extern unsigned char RTC_Flag;

    extern unsigned short Tim3_Temp1;
    extern char WS2812B_Mode1Count1_Flag;
    extern char WS2812B_Mode2Count1_Flag;
    extern char WS2812B_Mode3Count1_Flag;

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
