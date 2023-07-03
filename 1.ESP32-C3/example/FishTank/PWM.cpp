/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2022-11-05 13:37:26
 * @LastEditors: None
 * @LastEditTime: 2022-11-28 10:20:35
 */
#include <Arduino.h>
#include "PWM.h"

/**
 * @brief ESP32-C3 PWM初始化
 * @param Pin:PWM输出引脚
 * @param Channel:PWM通道
 * @param Freq:配置的频率
 * @param Resolution_bits:PWM精度
 * @return
 * @Date 2022-11-24 12:21:05
 */
void PWM::PWM_InitConfig(unsigned char Pin, unsigned char Channel, unsigned int Freq, unsigned char Resolution_bits)
{
    pinMode(Pin, OUTPUT);

    // 设置通道(高速通道0 ~ 7由80MHz时钟驱动,低速通道8 ~ 15由 1MHz 时钟驱动)
    // 频率
    // 分辨率(14表示0～16383种选择,ESP32-C3可设置1~14bits)
    ledcSetup(Channel, Freq, Resolution_bits);

    // 18,19被用于JTAG内部烧录,这里用于产生PWM故JTAG烧录不可用,只能使用串口烧录
    ledcAttachPin(Pin, Channel); //将生成信号通道绑定到输出通道上

    ledcWrite(Channel, 0); //初始化高电平占空比为0
}

/**
 * @brief PWM设置占空比
 * @param Channel:PWM通道
 * @param State:设置PWM开关
 * @param Resolution_bits:该通道初始化时的精度
 * @param Duty:占空比份(0-100)
 * @return
 * @Date 2022-11-24 12:44:30
 */
void PWM::PWM_Set(unsigned char Channel, bool State, unsigned char Resolution_bits, unsigned int Duty)
{
    float Accuracy = 1;
    for (unsigned char i; i < Resolution_bits; i++)
    {
        Accuracy = Accuracy * 2;
    }

    if (State == true)
    {
        ledcWrite(Channel, (Accuracy / 100) * Duty); // (Accuracy / 1000) * Duty为高电平的持续时间 1000为切分度
    }
    else
    {
        ledcWrite(Channel, 0);
    }
}
