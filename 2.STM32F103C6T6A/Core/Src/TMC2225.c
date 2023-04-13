/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2022-10-31 09:08:56
 * @LastEditors: None
 * @LastEditTime: 2022-11-27 21:00:25
 */
#include "main.h"
#include "tim.h"
#include "stm32f1xx_it.h"
#include "TMC2225.h"

unsigned short TMC2225_Pulse = 4000; //默认配置脉冲数为4微步4000个脉冲

/**
 * @brief TMC2225电机设置函数
 * @param State:电机状态设置
 * @param Direction:电机方向设置
 * @param Microstep:电机微步值设置(这里只配置了4微步,其他微步需自行配置)
 * @param Rotations:电机旋转百分比,设置0-1之间的数
 * @return 
 * @Date 2022-11-27 20:54:25
 */
void TMC2225_SetState(unsigned char State, unsigned char Direction, unsigned char Microstep, float Rotations)
{
    unsigned short Pluse = 0;

    if (Direction == TMC2225_Motor_Reverse)
    {
        HAL_GPIO_WritePin(TMC_2225_DIR_GPIO_Port, TMC_2225_DIR_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(TMC_2225_DIR_GPIO_Port, TMC_2225_DIR_Pin, GPIO_PIN_RESET);
    }

    switch (Microstep)
    {
    // 4微步时(一个脉冲走1.8/4=0.45度),需要4000个脉冲11SRM1410001步进电机才转一圈
    //  (电机步进为15步进,一个脉冲转18度,减速比为1:50(有减速箱),360/(18/4)*50=4000个脉冲电机才能转360度一圈)
    case 4:
        HAL_GPIO_WritePin(TMC_2225_MS2_GPIO_Port, TMC_2225_MS2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TMC_2225_MS1_GPIO_Port, TMC_2225_MS1_Pin, GPIO_PIN_RESET);
        Pluse = 4000;
        
        break;
    case 8:
        HAL_GPIO_WritePin(TMC_2225_MS2_GPIO_Port, TMC_2225_MS2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TMC_2225_MS1_GPIO_Port, TMC_2225_MS1_Pin, GPIO_PIN_SET);
        break;
    case 16:
        HAL_GPIO_WritePin(TMC_2225_MS2_GPIO_Port, TMC_2225_MS2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(TMC_2225_MS1_GPIO_Port, TMC_2225_MS1_Pin, GPIO_PIN_RESET);
        break;
    case 32:
        HAL_GPIO_WritePin(TMC_2225_MS2_GPIO_Port, TMC_2225_MS2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(TMC_2225_MS1_GPIO_Port, TMC_2225_MS1_Pin, GPIO_PIN_SET);
        break;
    default: //默认配置4微步
        HAL_GPIO_WritePin(TMC_2225_MS2_GPIO_Port, TMC_2225_MS2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TMC_2225_MS1_GPIO_Port, TMC_2225_MS1_Pin, GPIO_PIN_RESET);
        Pluse = 4000;
        break;
    }

    if (State == TMC2225_Motor_EN)
    {
        HAL_GPIO_WritePin(TMC_2225_EN_GPIO_Port, TMC_2225_EN_Pin, GPIO_PIN_RESET);

        TMC2225_Pulse = Rotations * Pluse;

        __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE); //开启中断前清除标志位防止一开启定时器中断就进中断
        HAL_TIM_Base_Start_IT(&htim3);                 // HAL_TIM_Base_Start和HAL_TIM_Base_Start_IT只要启动一个就可以了,什么模式就启动什么
        HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    }
    else
    {
        HAL_GPIO_WritePin(TMC_2225_EN_GPIO_Port, TMC_2225_EN_Pin, GPIO_PIN_SET);

        Tim3_Temp1 = 0;

        HAL_TIM_Base_Stop_IT(&htim3);            // 关闭定时器中断(HAL_TIM_Base_Start和HAL_TIM_Base_Start_IT只要启动一个就可以了,什么模式就启动什么)
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1); //关闭PWM
        // 定时器中断和PWM中断计算多少个脉冲的原理差不多,所以这里只用定时器中断计数,当然使用PWM中断计数脉冲也是可行的
    }
}
