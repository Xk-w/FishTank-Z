/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f1xx_it.c
 * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "TMC2225.h"
#include "tim.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_tim2_ch2_ch4;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
    /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

    /* USER CODE END NonMaskableInt_IRQn 0 */
    /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
    while (1)
    {
    }
    /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
    /* USER CODE BEGIN HardFault_IRQn 0 */

    /* USER CODE END HardFault_IRQn 0 */
    while (1)
    {
        /* USER CODE BEGIN W1_HardFault_IRQn 0 */
        /* USER CODE END W1_HardFault_IRQn 0 */
    }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
    /* USER CODE BEGIN MemoryManagement_IRQn 0 */

    /* USER CODE END MemoryManagement_IRQn 0 */
    while (1)
    {
        /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
        /* USER CODE END W1_MemoryManagement_IRQn 0 */
    }
}

/**
 * @brief This function handles Prefetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
    /* USER CODE BEGIN BusFault_IRQn 0 */

    /* USER CODE END BusFault_IRQn 0 */
    while (1)
    {
        /* USER CODE BEGIN W1_BusFault_IRQn 0 */
        /* USER CODE END W1_BusFault_IRQn 0 */
    }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
    /* USER CODE BEGIN UsageFault_IRQn 0 */

    /* USER CODE END UsageFault_IRQn 0 */
    while (1)
    {
        /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
        /* USER CODE END W1_UsageFault_IRQn 0 */
    }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
    /* USER CODE BEGIN SVCall_IRQn 0 */

    /* USER CODE END SVCall_IRQn 0 */
    /* USER CODE BEGIN SVCall_IRQn 1 */

    /* USER CODE END SVCall_IRQn 1 */
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
    /* USER CODE BEGIN DebugMonitor_IRQn 0 */

    /* USER CODE END DebugMonitor_IRQn 0 */
    /* USER CODE BEGIN DebugMonitor_IRQn 1 */

    /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
    /* USER CODE BEGIN PendSV_IRQn 0 */

    /* USER CODE END PendSV_IRQn 0 */
    /* USER CODE BEGIN PendSV_IRQn 1 */

    /* USER CODE END PendSV_IRQn 1 */
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
    /* USER CODE BEGIN SysTick_IRQn 0 */

    /* USER CODE END SysTick_IRQn 0 */
    HAL_IncTick();
    /* USER CODE BEGIN SysTick_IRQn 1 */

    /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles DMA1 channel4 global interrupt.
 */
void DMA1_Channel4_IRQHandler(void)
{
    /* USER CODE BEGIN DMA1_Channel4_IRQn 0 */

    /* USER CODE END DMA1_Channel4_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_usart1_tx);
    /* USER CODE BEGIN DMA1_Channel4_IRQn 1 */

    /* USER CODE END DMA1_Channel4_IRQn 1 */
}

/**
 * @brief This function handles DMA1 channel5 global interrupt.
 */
void DMA1_Channel5_IRQHandler(void)
{
    /* USER CODE BEGIN DMA1_Channel5_IRQn 0 */

    /* USER CODE END DMA1_Channel5_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
    /* USER CODE BEGIN DMA1_Channel5_IRQn 1 */

    /* USER CODE END DMA1_Channel5_IRQn 1 */
}

/**
 * @brief This function handles DMA1 channel7 global interrupt.
 */
void DMA1_Channel7_IRQHandler(void)
{
    /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

    /* USER CODE END DMA1_Channel7_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_tim2_ch2_ch4);
    /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

    /* USER CODE END DMA1_Channel7_IRQn 1 */
}

/**
 * @brief This function handles TIM1 update interrupt.
 */
void TIM1_UP_IRQHandler(void)
{
    /* USER CODE BEGIN TIM1_UP_IRQn 0 */

    /* USER CODE END TIM1_UP_IRQn 0 */
    HAL_TIM_IRQHandler(&htim1);
    /* USER CODE BEGIN TIM1_UP_IRQn 1 */

    /* USER CODE END TIM1_UP_IRQn 1 */
}

/**
 * @brief This function handles TIM3 global interrupt.
 */
void TIM3_IRQHandler(void)
{
    /* USER CODE BEGIN TIM3_IRQn 0 */

    /* USER CODE END TIM3_IRQn 0 */
    HAL_TIM_IRQHandler(&htim3);
    /* USER CODE BEGIN TIM3_IRQn 1 */

    /* USER CODE END TIM3_IRQn 1 */
}

/**
 * @brief This function handles USART1 global interrupt.
 */
void USART1_IRQHandler(void)
{
    /* USER CODE BEGIN USART1_IRQn 0 */
    //因为使用的是DMA串口通信,所以只能在这里写接收数据的函数

    //如果串口空闲标志位被置位,说明串口数据已经接收完毕,可以进行数据处理了
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) == SET)
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1); //清除空闲标志位
        HAL_UART_DMAStop(&huart1);          //停止DMA传输防止数据干扰

        Uart1_DataValid_Length = Uart1_DataMax_Length - (__HAL_DMA_GET_COUNTER(&hdma_usart1_rx)); //总计数减去未传输的数据个数，得到已经接收的数据个数
        Uart1_Rx_Flag = 1;                                                                        // 接受完成标志位置1
    }

    /* USER CODE END USART1_IRQn 0 */
    HAL_UART_IRQHandler(&huart1);
    /* USER CODE BEGIN USART1_IRQn 1 */

    /* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

unsigned char Uart1_DataMax[100] = {0};
unsigned char Uart1_DataMax_Length = 100;
unsigned char Uart1_DataValid_Length = 0; //真正的数据长度
unsigned char Uart1_Rx_Flag = 0;          // 接受完成标志位

/**
 * @brief DMA串口回调函数
 * @param *huart:串口
 * @return
 * @Date 2022-11-03 09:40:30
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{ //如果不开启串口中断,则程序只能发送一次数据,程序不能判断DMA传输是否完成USART一直处于busy状态
    if (huart->Instance == huart1.Instance)
    {
        //因为使用的是DMA串口通信,所以这里的回调不一定能触发
    }
}

extern char WS2812B_ModeEnable_Flag;
unsigned char RTC_Flag = 0;
unsigned short Tim3_Temp1 = 0;
char WS2812B_Mode1Count1_Flag = 0;
char WS2812B_Mode2Count1_Flag = 0;
char WS2812B_Mode3Count1_Flag = 0;
/**
 * @brief 定时器回调函数(处理定时触发函数和电机脉冲计数)
 * @param *htim:定时器
 * @return
 * @Date 2022-11-30 22:41:52
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static unsigned short Tim1_Count1 = 0;
    static unsigned short WS2812B_Mode1Count1 = 0;
    static unsigned short WS2812B_Mode2Count1 = 0;
    static unsigned short WS2812B_Mode3Count1 = 0;

    // 定时器1定时发送RTC时间数据函数
    if (htim->Instance == htim1.Instance)
    {
        Tim1_Count1++;
        if (Tim1_Count1 >= 1000) // 1000ms
        {
            Tim1_Count1 = 0;
            RTC_Flag = 1;
        }

        switch (WS2812B_ModeEnable_Flag)
        {
        case 0:
            break;
        case 1:
            WS2812B_Mode1Count1++;
            if (WS2812B_Mode1Count1 >= 50) // 50ms
            {
                WS2812B_Mode1Count1 = 0;
                WS2812B_Mode1Count1_Flag = 1;
            }
            break;
        case 2:
            WS2812B_Mode2Count1++;
            if (WS2812B_Mode2Count1 >= 100) // 100ms
            {
                WS2812B_Mode2Count1 = 0;
                WS2812B_Mode2Count1_Flag = 1;
            }
            break;
        case 3:
            WS2812B_Mode3Count1++;
            if (WS2812B_Mode3Count1 >= 100) // 100ms
            {
                WS2812B_Mode3Count1 = 0;
                WS2812B_Mode3Count1_Flag = 1;
            }
            break;

        default:
            break;
        }
    }

    // 定时器3计数脉冲个数函数
    if (htim->Instance == htim3.Instance)
    {
        Tim3_Temp1++;
        if (Tim3_Temp1 >= TMC2225_Pulse)
        {
            Tim3_Temp1 = 0;

            HAL_GPIO_WritePin(TMC_2225_EN_GPIO_Port, TMC_2225_EN_Pin, GPIO_PIN_SET);
            HAL_TIM_Base_Stop_IT(&htim3);            // 关闭定时器中断(HAL_TIM_Base_Start和HAL_TIM_Base_Start_IT只要启动一个就可以了,什么模式就启动什么)
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1); //关闭PWM
            // 定时器中断和PWM中断计算多少个脉冲的原理差不多,所以这里只用定时器中断计数,当然使用PWM中断计数脉冲也是可行的
        }
    }
}

/**
 * @brief DMA 传输完成暂停回调函数
 * @param *htim
 * @return
 * @Date 2022-10-17 22:27:42
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_2);
}
/* USER CODE END 1 */
