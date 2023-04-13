/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "dma.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f1xx_it.h"
#include <stdio.h>
#include <string.h>
#include "TMC2225.h"
#include "WS2812B_PWM_DMA.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char Uart1_Data_Check[2] = {0}; //串口数据接收检查位

unsigned char RTC_Data[5] = {0}; // RTC的数据

char WS2812B_ModeEnable_Flag = 0;
unsigned short WS2812B_ModeBrightnessData = 20;
unsigned short WS2812B_Mode1_HSV_HData = 0;
unsigned short WS2812B_Mode2_SingleRefresh = 0;
unsigned short WS2812B_Mode3_SingleRefresh = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */
    RTC_TimeTypeDef RTC_TimeStruct; //获取时间结构体
    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();
    MX_RTC_Init();
    MX_TIM1_Init();
    MX_TIM3_Init();
    MX_TIM2_Init();
    /* USER CODE BEGIN 2 */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);                        //使能串口空闲IDLE中断实现自由数据长度接收
    HAL_UART_Receive_DMA(&huart1, Uart1_DataMax, Uart1_DataMax_Length); //每次需要进中断接收时候,都需要调用此函数

    __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE); //清除IT标志位
    HAL_TIM_Base_Start_IT(&htim1);             //启动时基

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {

        /*Usart_Receive------------------------------------------------------------------------------------------------*/
        if (Uart1_Rx_Flag == 1)
        {
            Uart1_Rx_Flag = 0; //清除接收结束标志位

            sprintf(&Uart1_Data_Check[0], "%c", Uart1_DataMax[0]);
            sprintf(&Uart1_Data_Check[1], "%c", Uart1_DataMax[1]);

            if (Uart1_Data_Check[0] == 'R' && Uart1_Data_Check[1] == 'M')
            {
                RTC_TimeStruct.Hours = Uart1_DataMax[2];
                RTC_TimeStruct.Minutes = Uart1_DataMax[3];
                RTC_TimeStruct.Seconds = Uart1_DataMax[4];

                HAL_RTC_SetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN);
                // printf("yes\n");
            }

            if (Uart1_Data_Check[0] == 'M' && Uart1_Data_Check[1] == 'F')
            {
                if (Uart1_DataMax[2] == '1')
                {
                    TMC2225_SetState(TMC2225_Motor_EN, TMC2225_Motor_Forward, 4, 0.1);
                }
                if (Uart1_DataMax[2] == '2')
                {
                    TMC2225_SetState(TMC2225_Motor_EN, TMC2225_Motor_Forward, 4, 0.5);
                }
                if (Uart1_DataMax[2] == '3')
                {
                    TMC2225_SetState(TMC2225_Motor_EN, TMC2225_Motor_Forward, 4, 1);
                }
            }

            if (Uart1_Data_Check[0] == 'L' && Uart1_Data_Check[1] == 'M')
            {
                if (Uart1_DataMax[2] == 0)
                {
                    WS2812B_ModeEnable_Flag = 0;
                    WS2812B_Mode1Count1_Flag = 0;
                    WS2812B_Mode2Count1_Flag = 0;
                    WS2812B_Mode3Count1_Flag = 0;
                    WS2812B_Mode2_SingleRefresh = 0;
                    WS2812B_Mode3_SingleRefresh = 0;
                    WS2812B_ClearAll();

                    WS2812B_ModeBrightnessData = 0;
                }
                if (Uart1_DataMax[2] == 1)
                {
                    WS2812B_ModeEnable_Flag = 1;
                    WS2812B_Mode1Count1_Flag = 0;
                    WS2812B_Mode2Count1_Flag = 0;
                    WS2812B_Mode3Count1_Flag = 0;
                    WS2812B_Mode2_SingleRefresh = 0;
                    WS2812B_Mode3_SingleRefresh = 0;
                    WS2812B_ClearAll();

                    if (Uart1_DataMax[3] == 'B')
                    {
                        WS2812B_ModeBrightnessData = Uart1_DataMax[4];
                    }
                    else
                    {
                        WS2812B_ModeBrightnessData = 20;
                    }
                }
                if (Uart1_DataMax[2] == 2)
                {
                    WS2812B_ModeEnable_Flag = 2;
                    WS2812B_Mode1Count1_Flag = 0;
                    WS2812B_Mode2Count1_Flag = 0;
                    WS2812B_Mode3Count1_Flag = 0;
                    WS2812B_Mode2_SingleRefresh = 0;
                    WS2812B_Mode3_SingleRefresh = 0;
                    WS2812B_ClearAll();

                    if (Uart1_DataMax[3] == 'B')
                    {
                        WS2812B_ModeBrightnessData = Uart1_DataMax[4];
                    }
                    else
                    {
                        WS2812B_ModeBrightnessData = 20;
                    }
                }
                if (Uart1_DataMax[2] == 3)
                {
                    WS2812B_ModeEnable_Flag = 3;
                    WS2812B_Mode1Count1_Flag = 0;
                    WS2812B_Mode2Count1_Flag = 0;
                    WS2812B_Mode3Count1_Flag = 0;
                    WS2812B_Mode2_SingleRefresh = 0;
                    WS2812B_Mode3_SingleRefresh = 0;
                    WS2812B_ClearAll();

                    if (Uart1_DataMax[3] == 'B')
                    {
                        WS2812B_ModeBrightnessData = Uart1_DataMax[4];
                    }
                    else
                    {
                        WS2812B_ModeBrightnessData = 20;
                    }
                }
            }
            // HAL_UART_Transmit_DMA(&huart1, Uart1_DataMax, Uart1_DataValid_Length); //串口回调

            Uart1_DataValid_Length = 0; //清除数据长度

            //每次接收替换上一次的数据不会造成数据堆积错误
            HAL_UART_Receive_DMA(&huart1, Uart1_DataMax, Uart1_DataMax_Length); //重新打开DMA接收
        }
        /*Usart_Receive------------------------------------------------------------------------------------------------*/

        /*RTC_ScheduledTransmission------------------------------------------------------------------------------------------------*/
        if (RTC_Flag == 1)
        {
            RTC_Flag = 0;

            HAL_RTC_GetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN); //获取时间

            RTC_Data[0] = 'R';
            RTC_Data[1] = '0';
            RTC_Data[2] = RTC_TimeStruct.Hours;
            RTC_Data[3] = RTC_TimeStruct.Minutes;
            RTC_Data[4] = RTC_TimeStruct.Seconds;

            // 数据转换,设计由ESP32进行时间数据转换故STM32这边注释
            // sprintf(RTC_Buffer, "%d", RTC_TimeStruct.Seconds);
            // for (unsigned char i = 1; i < 3; i++)
            // {
            //     RTC_Data[i] = RTC_Buffer[i - 1];
            // }
            // RTC_Data[0] = 'R';

            HAL_UART_Transmit_DMA(&huart1, RTC_Data, 5);

            // 串口打印测试
            // printf("%02d:%02d:%02d\r\n\r\n",RTC_Data[1],RTC_Data[2],RTC_Data[3]);
            // printf("%02d:%02d:%02d\r\n\r\n", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
        }

        /*RTC_ScheduledTransmission-----------------------------------------------------------------------------------------------*/

        /*TMC2225------------------------------------------------------------------------------------------------*/
        // 无
        /*TMC2225------------------------------------------------------------------------------------------------*/

        /*WS2812B------------------------------------------------------------------------------------------------*/

        // if (WS2812B_ModeEnable_Flag == 0) //清除灯模式标志位
        // {
        //     WS2812B_Mode1Count1_Flag = 0;
        //     WS2812B_Mode2Count1_Flag = 0;
        //     WS2812B_Mode3Count1_Flag = 0;
        //     WS2812B_Mode2_SingleRefresh = 0;
        //     WS2812B_Mode3_SingleRefresh = 0;
        // }

        if (WS2812B_ModeEnable_Flag == 1) //灯模式1使能标志位
        {
            WS2812B_SetSequentialHSV(WS2812B_Mode1_HSV_HData, 100, WS2812B_ModeBrightnessData, NUM_LEDS);
            if (WS2812B_Mode1Count1_Flag == 1)
            {
                WS2812B_Mode1Count1_Flag = 0;
                WS2812B_Mode1_HSV_HData++;
                if (WS2812B_Mode1_HSV_HData > 360)
                {
                    WS2812B_Mode1_HSV_HData = 0;
                }
            }

            WS2812B_Mode2_SingleRefresh = 0;
            WS2812B_Mode3_SingleRefresh = 0;
        }

        if (WS2812B_ModeEnable_Flag == 2) //灯模式2使能标志位
        {
            for (unsigned char i = WS2812B_Mode2_SingleRefresh; i < NUM_LEDS; i++)
            {
                WS2812B_SetHSV(i * 5.625f, 100, WS2812B_ModeBrightnessData, i);
                WS2812B_Show();
                WS2812B_Mode2_SingleRefresh++;
            }

            if (WS2812B_Mode2Count1_Flag == 1)
            {
                WS2812B_Mode2Count1_Flag = 0;

                // WS2812B_Shift1(Shift_Left, 1);
                WS2812B_Shift2(Shift_Right, 1);
                WS2812B_Show();
            }

            WS2812B_Mode3_SingleRefresh = 0;
        }

        if (WS2812B_ModeEnable_Flag == 3) //灯模式3使能标志位
        {
            for (unsigned char i = WS2812B_Mode3_SingleRefresh; i < NUM_LEDS; i++)
            {
                if (i == 0 || i == 25 || i == 35 || i == 45 || i == 55)
                {
                    for (unsigned char j = 0; j < 15; j++) //减小数字15可减少白灯数量
                    {
                        WS2812B_SetHSV(i * 5.625f, 0, 0, i++);
                        WS2812B_Show();
                        WS2812B_Mode3_SingleRefresh++;
                    }
                }
                WS2812B_SetHSV(i * 5.625f, 0, WS2812B_ModeBrightnessData, i);
                WS2812B_Show();
                WS2812B_Mode3_SingleRefresh++;
            }

            if (WS2812B_Mode3Count1_Flag == 1)
            {
                WS2812B_Mode3Count1_Flag = 0;

                // WS2812B_Shift1(Shift_Left, 1);
                WS2812B_Shift2(Shift_Right, 1);
                WS2812B_Show();
            }

            WS2812B_Mode2_SingleRefresh = 0;
        }

        /*WS2812B------------------------------------------------------------------------------------------------*/

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
