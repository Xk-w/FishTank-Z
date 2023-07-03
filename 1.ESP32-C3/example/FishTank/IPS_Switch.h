/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2022-11-22 21:38:55
 * @LastEditors: None
 * @LastEditTime: 2022-11-23 11:25:39
 */
#pragma

enum IPS_Switch_Tag
{
    IPS_Switch_Up=1,
    IPS_Switch_Down,
    IPS_Switch_Left,
    IPS_Switch_Right,
    IPS_Switch_Centre
};

extern unsigned char IPS_Switch_Flag;

class IPS_Switch
{
public:
    void IPS_Switch_Init(void);

private:
};