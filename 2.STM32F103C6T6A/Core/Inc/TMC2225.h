/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2022-10-31 09:09:05
 * @LastEditors: None
 * @LastEditTime: 2022-10-31 16:04:29
 */
#ifndef __TMC2225_H__
#define __TMC2225_H__

#define TMC2225_Motor_EN 0
#define TMC2225_Motor_DISEN 1
#define TMC2225_Motor_Forward 0
#define TMC2225_Motor_Reverse 1

extern unsigned short TMC2225_Pulse;

void TMC2225_SetState(unsigned char State, unsigned char Direction, unsigned char Microstep, float Rotations);

#endif
