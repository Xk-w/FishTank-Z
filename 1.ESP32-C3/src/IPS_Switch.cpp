/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2022-11-22 21:38:46
 * @LastEditors: None
 * @LastEditTime: 2022-11-23 12:29:15
 */
#include <Arduino.h>
#include "IPS_Switch.h"

#define UPKEY GPIO_NUM_8
#define DWKEY GPIO_NUM_13
#define LKEY GPIO_NUM_5
#define RKEY GPIO_NUM_9
#define CENTER GPIO_NUM_4

unsigned char IPS_Switch_Flag = 0;

void IPS_Switch_UPKEY(void)
{
    if (digitalRead(UPKEY) == LOW)
    {
        IPS_Switch_Flag = IPS_Switch_Up;
    }
}
void IPS_Switch_DWKEY(void)
{
    if (digitalRead(DWKEY) == LOW)
    {
        IPS_Switch_Flag = IPS_Switch_Down;
    }
}
void IPS_Switch_LKEY(void)
{
    if (digitalRead(LKEY) == LOW)
    {
        IPS_Switch_Flag = IPS_Switch_Left;
    }
}
void IPS_Switch_RKEY(void)
{
    if (digitalRead(RKEY) == LOW)
    {
        IPS_Switch_Flag = IPS_Switch_Right;
    }
}

void IPS_Switch_CENTER(void)
{
    if (digitalRead(CENTER) == LOW)
    {
        IPS_Switch_Flag = IPS_Switch_Centre;
    }
}

void IPS_Switch::IPS_Switch_Init(void)
{
    pinMode(UPKEY, INPUT_PULLUP);
    pinMode(DWKEY, INPUT_PULLUP);
    pinMode(LKEY, INPUT_PULLUP);
    pinMode(RKEY, INPUT_PULLUP);
    pinMode(CENTER, INPUT_PULLUP);

    attachInterrupt(UPKEY, IPS_Switch_UPKEY, FALLING);
    attachInterrupt(DWKEY, IPS_Switch_DWKEY, FALLING);
    attachInterrupt(LKEY, IPS_Switch_LKEY, FALLING);
    attachInterrupt(RKEY, IPS_Switch_RKEY, FALLING);
    attachInterrupt(CENTER, IPS_Switch_CENTER, FALLING);
}