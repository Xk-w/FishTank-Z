/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2022-08-01 14:15:06
 * @LastEditors: None
 * @LastEditTime: 2022-12-02 20:37:37
 */
#include <Arduino.h>
#include "TFT_eSPI.h"
#include "IPS_Switch.h"
#include "DS18B20.h"
#include "PWM.h"
#include "ADC.h"

#define FishFeed_RunALittleCommand 1
#define FishFeed_RunAHalfCommand 2
#define FishFeed_RunALabCommand 3

char RTC_SendData[5] = {0};
char FishFeed_SendData[3] = {0};

short Windows = 0; //窗口切换定义,0为主窗口
bool Windows_EditMode = 0;
short Windows0_EditPosition = 0;
short Windows1_EditPosition = 0;
short Windows2_EditPosition = 0;
short Windows3_EditPosition = 0;
short Windows4_EditPosition = 0;
short Windows5_EditPosition = 0;
bool Windows4_FILAutoMoodEnable_Flag = 0;
bool Windows4_OXYAutoMoodEnable_Flag = 0;

char Serial_Data[5] = {0};
char Serial_Data_Check[2] = {0}; //串口数据接收检查位
int RTC_Data_Hours_d = 0;        //时_整型
int RTC_Data_Minutes_d = 0;      //分_整型
int RTC_Data_Seconds_d = 0;      //秒_整型
bool RTC_DataUpdate_Flag = 0;

float DS18B20_Data_f = 0;
char DS18B20_Data_c[5] = {0};

float PH_Data_f = 0;
char PH_Data_c[6] = {0};
short PH_LimitMinData = 0;
short PH_LimitMaxData = 14; //越界极限默认14
bool PH_DataTiming_Flag = 0;
bool PH_OutOfRange_Flag = 0;
bool PH_OutOfRange_LoopFlag = 0;
short PH_OutOfRange_Data = 0;
char PH_OutOfRange_Clock[3] = {0};

short TDS_Data_d = 0;
short TDS_LimitMinData = 0;
short TDS_LimitMaxData = 1500; //越界极限默认1500
bool TDS_DataTiming_Flag = 0;
bool TDS_OutOfRange_Flag = 0;
bool TDS_OutOfRange_LoopFlag = 0;
short TDS_OutOfRange_Data = 0;
char TDS_OutOfRange_Clock[3] = {0};

short FishFeedTimes_Data = 0;           //鱼饲料投食次数预寄存显示数据
char FishFeedTimes_DataEnable_Flag = 0; //鱼饲料投食次数执行处理数据
short FishFeed_TimeDataP1_Hours = 0;    //鱼饲料投食时间点1预寄存显示数据
short FishFeed_TimeDataP1_Minutes = 0;
short FishFeed_TimeDataP1_Seconds = 0;
short FishFeed_TimeDataP2_Hours = 0; //鱼饲料投食时间点2预寄存显示数据
short FishFeed_TimeDataP2_Minutes = 0;
short FishFeed_TimeDataP2_Seconds = 0;
short FishFeed_TimeDataP3_Hours = 0; //鱼饲料投食时间点3预寄存显示数据
short FishFeed_TimeDataP3_Minutes = 0;
short FishFeed_TimeDataP3_Seconds = 0;
char FishFeed_TimeDataP1_Clock[3] = {0}; //投食时间的执行处理数据(时+分+秒)
char FishFeed_TimeDataP2_Clock[3] = {0};
char FishFeed_TimeDataP3_Clock[3] = {0};
bool FishFeed_EndOfDayP1_Flag = 0;
bool FishFeed_EndOfDayP2_Flag = 0;
bool FishFeed_EndOfDayP3_Flag = 0;
short FishFeed_AutoQuantityData = 2; //默认一次自动投食量为步进电机旋转半圈
short FishFeed_CompleteTimes = 0;    //一天已经投食的次数

short FIL_Mood_Flag = 1; //过滤电机自动模式手动模式切换(0自动,1手动)
short OXY_Mood_Flag = 1; //供氧电机自动模式手动模式切换(0自动,1手动)
short FIL_Speed_Data = 0;
short OXY_Speed_Data = 0;

bool FishFeed_ExecutionPlan_Flag = 0;                           //定时投食的执行标志
char FishFeed_OtherMotorControlDuringFeeding_Flag = 1;          //默认开启投食前5分钟关闭其他电机,已经投食后10分钟(等待鱼吃完)开启其他电机
bool FishFeed_OtherMotorControlEnable_Flag = 0;                 //触发定时投食控制其他电机
char FishFeed_OtherMotorControl_OriginalMotorModeData[2] = {0}; // 数据1为FIL模式原始数据,2为OXY模式原始数据
char FishFeed_OtherMotorControl_OriginalMotorSpeed[2] = {0};    // 数据1为FIL电机速度原始数据,2为OXY电机速度原始数据
bool FishFeed_OtherMotorControl_Time1Flag = 0;                  //单次进入时间点1标志位
bool FishFeed_OtherMotorControl_Time2Flag = 0;                  //单次进入时间点2标志位
char FishFeed_OtherMotorControl_Time1Data[3] = {0};             //在这个定时时间点开启投食(原时间+5分钟)
char FishFeed_OtherMotorControl_Time2Data[3] = {0};             //在这个定时时间点重新开启其他电机(原时间+15分钟)
short FishFeed_OtherMotorControl_WaitingTime = 10;              //投食后其他电机的等待开启时间

short Light_ModeData = 0;
short Light_BrightnessData = 20; //默认状态20亮度
char Light_SendData[5] = {0};

bool RTC_DataTiming_Flag = 0;
hw_timer_t *Tim0 = NULL;

TFT_eSPI TFT_eSPIClass1;
IPS_Switch IPS_SwitchClass1;
DS18B20 DS18B20Class1;
PWM TB6612_FILClass1;
PWM TB6612_OXYClass1;
ADC PH_Class1;
ADC TDS_Class1;

/**
 * @brief Tim0中断服务函数(IRAM_ATTR我们声明编译后的代码将放置在ESP32 的内部
 *            RAM (IRAM) 中,否则,代码将放在 Flash 中,ESP32 上的闪存比内部 RAM 慢得多)
 *            合宙ESP-C3定时器中断里最好不放串口打印函数,否则可能会卡机
 * @return
 * @Date 2022-08-04 14:30:29
 */
void IRAM_ATTR Tim0_Interrupt(void)
{
    static unsigned short Tim0_Count1 = 0;
    static unsigned short Tim0_Count2 = 0;

    Tim0_Count1++;
    Tim0_Count2++;
    if (Tim0_Count1 >= 1000) //计时1000ms
    {
        Tim0_Count1 = 0;
        RTC_DataTiming_Flag = 1;
    }
    if (Tim0_Count2 >= 500) //计时500ms
    {
        Tim0_Count2 = 0;
        PH_DataTiming_Flag = 1;
        TDS_DataTiming_Flag = 1;
    }
}

/*Usart_SendCommand-----------------------------------------------------------------------------------------------*/

void FishFeed_SendCommand(char Command)
{
    switch (Command)
    {
    case 0:
        break;
    case 1:
        FishFeed_SendData[0] = 'M';
        FishFeed_SendData[1] = 'F';
        FishFeed_SendData[2] = '1';

        Serial.write(FishFeed_SendData, 3);
        break;
    case 2:
        FishFeed_SendData[0] = 'M';
        FishFeed_SendData[1] = 'F';
        FishFeed_SendData[2] = '2';

        Serial.write(FishFeed_SendData, 3);
        break;
    case 3:
        FishFeed_SendData[0] = 'M';
        FishFeed_SendData[1] = 'F';
        FishFeed_SendData[2] = '3';

        Serial.write(FishFeed_SendData, 3);
        break;

    default:
        break;
    }
}
/*Usart_SendCommand-----------------------------------------------------------------------------------------------*/

void setup()
{
    Serial.begin(115200);
    IPS_SwitchClass1.IPS_Switch_Init();

    // 1.定时器编号0到3,对应全部4个硬件定时器
    // 2.预分频器数值,ESP32计数器基频为80M,80分频单位是微秒
    // 3.计数器向上(true)或向下(false)计数的标志
    Tim0 = timerBegin(0, 80, true);
    // 1.指向已初始化定时器的指针(Tim0)
    // 2.中断处理函数的地址
    // 3.表示中断触发类型是边沿(true)还是电平(false)的标志
    timerAttachInterrupt(Tim0, &Tim0_Interrupt, true);
    // 1. 指向已初始化定时器的指针(Tim0)
    // 2.第二个参数是触发中断的计数器值(1000000 us -> 1s)
    // 3.定时器在产生中断时是否重新加载的标志
    timerAlarmWrite(Tim0, 1000, true);
    timerAlarmEnable(Tim0); //使能定时器Tim0

    TFT_eSPIClass1.begin();
    TFT_eSPIClass1.setRotation(3);
    TFT_eSPIClass1.setTextFont(2);
    // TFT_eSPIClass1.setTextColor(TFT_BROWN);
    // TFT_eSPIClass1.setTextColor(TFT_WHITE);
    TFT_eSPIClass1.fillScreen(TFT_BLACK);
    TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
    // TFT_eSPIClass1.drawString("Ciallo!!", 1, 0);

    PH_Class1.ADC_Init(12, PH_Pin);
    TDS_Class1.ADC_Init(12, TDS_Pin);

    // 过滤器PWM初始化
    TB6612_FILClass1.PWM_InitConfig(PWMA, TB6612_FilterChannel, TB6612_Frequency, TB6612_Accuracy10);
    // 供氧泵PWM初始化
    TB6612_OXYClass1.PWM_InitConfig(PWMB, TB6612_OxygenChannel, TB6612_Frequency, TB6612_Accuracy10);
}

void loop()
{
    /*Windows-----------------------------------------------------------------------------------------------*/

    //窗口数目越界重置
    if (Windows > 5)
    {
        Windows = 0;
    }
    if (Windows < 0)
    {
        Windows = 5;
    }

    // 窗口修改模式绘制修改边框
    if (Windows == 0 && Windows_EditMode == 1) // 主窗口修改模式选定框
    {
        TFT_eSPIClass1.drawString(":", 58, 1, 4);
        TFT_eSPIClass1.drawString(":", 96, 1, 4);

        // 时针数据刷新
        if (RTC_Data_Hours_d < 10)
        {
            TFT_eSPIClass1.drawNumber(0, 29, 3, 4);
            TFT_eSPIClass1.drawNumber(RTC_Data_Hours_d, 43, 3, 4); //(4字体原x位置+14,2字体原x位置+8)
        }
        else
        {
            TFT_eSPIClass1.drawNumber(RTC_Data_Hours_d, 29, 3, 4);
        }
        // 分针数据刷新
        if (RTC_Data_Minutes_d < 10)
        {
            TFT_eSPIClass1.drawNumber(0, 67, 3, 4);
            TFT_eSPIClass1.drawNumber(RTC_Data_Minutes_d, 81, 3, 4); //(4字体原x位置+14,2字体原x位置+8)
        }
        else
        {
            TFT_eSPIClass1.drawNumber(RTC_Data_Minutes_d, 67, 3, 4);
        }
        // 秒针数据刷新
        if (RTC_Data_Seconds_d < 10)
        {
            TFT_eSPIClass1.drawNumber(0, 105, 3, 4);
            TFT_eSPIClass1.drawNumber(RTC_Data_Seconds_d, 119, 3, 4); //(4字体原x位置+14,2字体原x位置+8)
        }
        else
        {
            TFT_eSPIClass1.drawNumber(RTC_Data_Seconds_d, 105, 3, 4);
        }

        switch (Windows0_EditPosition)
        {
        case 0:
            TFT_eSPIClass1.drawRect(113, 28, 11, 13, TFT_BLACK);
            TFT_eSPIClass1.drawRect(29, 4, 28, 19, TFT_BROWN);
            break;
        case 1:
            TFT_eSPIClass1.drawRect(67, 4, 28, 19, TFT_BROWN);
            break;
        case 2:
            TFT_eSPIClass1.drawRect(113, 28, 11, 13, TFT_BLACK);
            TFT_eSPIClass1.drawRect(105, 4, 28, 19, TFT_BROWN);
            break;
        case 3:
            TFT_eSPIClass1.drawRect(113, 28, 11, 13, TFT_BROWN);
            break;

        default:
            break;
        }
    }

    if (Windows == 1 && Windows_EditMode == 1) // 窗口1修改模式选定框
    {
        switch (Windows1_EditPosition)
        {
        case 0:
            TFT_eSPIClass1.drawRect(110, 31, 21, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(67, 31, 21, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(117, 5, 38, 20, TFT_BROWN);
            break;
        case 1:
            TFT_eSPIClass1.drawRect(117, 5, 38, 20, TFT_BLACK);
            TFT_eSPIClass1.drawRect(110, 31, 21, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(67, 31, 21, 18, TFT_BROWN);
            break;
        case 2:
            TFT_eSPIClass1.drawRect(67, 31, 21, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(117, 5, 38, 20, TFT_BLACK);
            TFT_eSPIClass1.drawRect(110, 31, 21, 18, TFT_BROWN);
            break;

        default:
            break;
        }
    }

    if (Windows == 2 && Windows_EditMode == 1) // 窗口2修改模式选定框
    {
        switch (Windows2_EditPosition)
        {
        case 0:
            TFT_eSPIClass1.drawRect(110, 31, 37, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(52, 31, 37, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(117, 5, 38, 20, TFT_BROWN);
            break;
        case 1:
            TFT_eSPIClass1.drawRect(117, 5, 38, 20, TFT_BLACK);
            TFT_eSPIClass1.drawRect(110, 31, 37, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(52, 31, 37, 18, TFT_BROWN);
            break;
        case 2:
            TFT_eSPIClass1.drawRect(52, 31, 37, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(117, 5, 38, 20, TFT_BLACK);
            TFT_eSPIClass1.drawRect(110, 31, 37, 18, TFT_BROWN);
            break;

        default:
            break;
        }
    }

    if (Windows == 3 && Windows_EditMode == 1) // 窗口3修改模式选定框
    {
        switch (FishFeedTimes_Data)
        {
        case 0: // 0times

            switch (Windows3_EditPosition)
            {
            case 0:
                TFT_eSPIClass1.drawRect(41, 57, 77, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(39, 21, 81, 19, TFT_BLACK);
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BROWN);
                break;
            case 1:
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(41, 38, 77, 19, TFT_BLACK);
                TFT_eSPIClass1.drawRect(39, 21, 81, 19, TFT_BROWN);
                break;
            case 2:
                TFT_eSPIClass1.drawRect(39, 21, 81, 19, TFT_BLACK);
                TFT_eSPIClass1.drawRect(41, 57, 77, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(41, 38, 77, 19, TFT_BROWN);
                break;
            case 3:
                TFT_eSPIClass1.drawRect(41, 38, 77, 19, TFT_BLACK);
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(41, 57, 77, 18, TFT_BROWN);
                break;
            default:
                break;
            }

            break;
        case 1: // 1times

            switch (Windows3_EditPosition)
            {
            case 0:
                TFT_eSPIClass1.drawRect(126, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BROWN);
                break;
            case 1:
                TFT_eSPIClass1.drawRect(99, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 24, 21, 18, TFT_BROWN);
                break;
            case 2:
                TFT_eSPIClass1.drawRect(126, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 24, 21, 18, TFT_BROWN);
                break;
            case 3:
                TFT_eSPIClass1.drawRect(103, 2, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(126, 24, 21, 18, TFT_BROWN);
                break;
            default:
                break;
            }

            break;
        case 2: // 2times

            switch (Windows3_EditPosition)
            {
            case 0:
                TFT_eSPIClass1.drawRect(126, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BROWN);
                break;
            case 1:
                TFT_eSPIClass1.drawRect(99, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 24, 21, 18, TFT_BROWN);
                break;
            case 2:
                TFT_eSPIClass1.drawRect(126, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 24, 21, 18, TFT_BROWN);
                break;
            case 3:
                TFT_eSPIClass1.drawRect(72, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(126, 24, 21, 18, TFT_BROWN);
                break;
            case 4:
                TFT_eSPIClass1.drawRect(99, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(126, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 41, 21, 18, TFT_BROWN);
                break;
            case 5:
                TFT_eSPIClass1.drawRect(126, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 41, 21, 18, TFT_BROWN);
                break;
            case 6:
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(126, 41, 21, 18, TFT_BROWN);
                break;
            default:
                break;
            }

            break;
        case 3: // 3times

            switch (Windows3_EditPosition)
            {
            case 0:
                TFT_eSPIClass1.drawRect(126, 58, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BROWN);
                break;
            case 1:
                TFT_eSPIClass1.drawRect(99, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 24, 21, 18, TFT_BROWN);
                break;
            case 2:
                TFT_eSPIClass1.drawRect(126, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 24, 21, 18, TFT_BROWN);
                break;
            case 3:
                TFT_eSPIClass1.drawRect(72, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(126, 24, 21, 18, TFT_BROWN);
                break;
            case 4:
                TFT_eSPIClass1.drawRect(99, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(126, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 41, 21, 18, TFT_BROWN);
                break;
            case 5:
                TFT_eSPIClass1.drawRect(126, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 41, 21, 18, TFT_BROWN);
                break;
            case 6:
                TFT_eSPIClass1.drawRect(72, 58, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(126, 41, 21, 18, TFT_BROWN);
                break;
            case 7:
                TFT_eSPIClass1.drawRect(99, 58, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(126, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 58, 21, 18, TFT_BROWN);
                break;
            case 8:
                TFT_eSPIClass1.drawRect(126, 58, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 58, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 58, 21, 18, TFT_BROWN);
                break;
            case 9:
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 58, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(126, 58, 21, 18, TFT_BROWN);
                break;
            default:
                break;
            }

            break;

        default:
            break;
        }
    }

    if (Windows == 4 && Windows_EditMode == 1) // 窗口4修改模式选定框
    {
        switch (FIL_Mood_Flag)
        {
        case 0:

            switch (OXY_Mood_Flag)
            {
            case 0: // FIL和OXY都为AT的时候

                switch (Windows4_EditPosition)
                {
                case 0:
                    TFT_eSPIClass1.drawRect(98, 39, 29, 18, TFT_BLACK); //清除向下边框
                    TFT_eSPIClass1.drawRect(98, 2, 29, 18, TFT_BROWN);
                    TFT_eSPIClass1.drawRect(98, 39, 29, 18, TFT_BLACK);
                    break;
                case 1:
                    break;
                case 2:
                    TFT_eSPIClass1.drawRect(98, 2, 29, 18, TFT_BLACK);
                    TFT_eSPIClass1.drawRect(98, 39, 29, 18, TFT_BROWN);
                    TFT_eSPIClass1.drawRect(98, 2, 29, 18, TFT_BLACK);
                    break;
                case 3:
                    break;
                default:
                    break;
                }

                break;
            case 1: // FIL为AT的时候

                switch (Windows4_EditPosition)
                {
                case 0:
                    TFT_eSPIClass1.drawRect(98, 57, 29, 18, TFT_BLACK); //清除向下边框
                    TFT_eSPIClass1.drawRect(98, 2, 29, 18, TFT_BROWN);
                    TFT_eSPIClass1.drawRect(98, 39, 29, 18, TFT_BLACK); //清除向上边框
                    break;
                case 1:
                    break;
                case 2:
                    TFT_eSPIClass1.drawRect(98, 2, 29, 18, TFT_BLACK);
                    TFT_eSPIClass1.drawRect(98, 39, 29, 18, TFT_BROWN);
                    TFT_eSPIClass1.drawRect(98, 57, 29, 18, TFT_BLACK);
                    break;
                case 3:
                    TFT_eSPIClass1.drawRect(98, 39, 29, 18, TFT_BLACK);
                    TFT_eSPIClass1.drawRect(98, 57, 29, 18, TFT_BROWN);
                    TFT_eSPIClass1.drawRect(98, 2, 29, 18, TFT_BLACK);
                    break;
                default:
                    break;
                }

                break;

            default:
                break;
            }

            break;
        case 1:

            switch (OXY_Mood_Flag)
            {
            case 0: // OXY为AT的时候

                switch (Windows4_EditPosition)
                {
                case 0:
                    TFT_eSPIClass1.drawRect(98, 39, 29, 18, TFT_BLACK); //清除向下边框
                    TFT_eSPIClass1.drawRect(98, 2, 29, 18, TFT_BROWN);
                    TFT_eSPIClass1.drawRect(98, 20, 29, 19, TFT_BLACK); //清除向上边框
                    break;
                case 1:
                    TFT_eSPIClass1.drawRect(98, 2, 29, 18, TFT_BLACK);
                    TFT_eSPIClass1.drawRect(98, 20, 29, 19, TFT_BROWN);
                    TFT_eSPIClass1.drawRect(98, 39, 29, 18, TFT_BLACK);
                    break;
                case 2:
                    TFT_eSPIClass1.drawRect(98, 20, 29, 19, TFT_BLACK);
                    TFT_eSPIClass1.drawRect(98, 39, 29, 18, TFT_BROWN);
                    TFT_eSPIClass1.drawRect(98, 2, 29, 18, TFT_BLACK);
                    break;
                case 3:
                    break;
                default:
                    break;
                }

                break;
            case 1: // FIL和OXY都不为AT的时候

                switch (Windows4_EditPosition)
                {
                case 0:
                    TFT_eSPIClass1.drawRect(98, 57, 29, 18, TFT_BLACK); //清除向下边框
                    TFT_eSPIClass1.drawRect(98, 2, 29, 18, TFT_BROWN);
                    TFT_eSPIClass1.drawRect(98, 20, 29, 19, TFT_BLACK); //清除向上边框
                    break;
                case 1:
                    TFT_eSPIClass1.drawRect(98, 2, 29, 18, TFT_BLACK);
                    TFT_eSPIClass1.drawRect(98, 20, 29, 19, TFT_BROWN);
                    TFT_eSPIClass1.drawRect(98, 39, 29, 18, TFT_BLACK);
                    break;
                case 2:
                    TFT_eSPIClass1.drawRect(98, 20, 29, 19, TFT_BLACK);
                    TFT_eSPIClass1.drawRect(98, 39, 29, 18, TFT_BROWN);
                    TFT_eSPIClass1.drawRect(98, 57, 29, 18, TFT_BLACK);
                    break;
                case 3:
                    TFT_eSPIClass1.drawRect(98, 39, 29, 18, TFT_BLACK);
                    TFT_eSPIClass1.drawRect(98, 57, 29, 18, TFT_BROWN);
                    TFT_eSPIClass1.drawRect(98, 2, 29, 18, TFT_BLACK);
                    break;
                default:
                    break;
                }

                break;

            default:
                break;
            }

            break;

        default:
            break;
        }
    }

    if (Windows == 5 && Windows_EditMode == 1) // 窗口4修改模式选定框
    {
        switch (Windows5_EditPosition)
        {
        case 0:
            TFT_eSPIClass1.drawRect(128, 21, 19, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(103, 40, 19, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(128, 2, 19, 18, TFT_BROWN);
            break;
        case 1:
            TFT_eSPIClass1.drawRect(66, 40, 19, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(128, 2, 19, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(128, 21, 19, 18, TFT_BROWN);
            break;
        case 2:
            TFT_eSPIClass1.drawRect(103, 40, 19, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(128, 21, 19, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(66, 40, 19, 18, TFT_BROWN);
            break;
        case 3:
            TFT_eSPIClass1.drawRect(128, 2, 19, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(66, 40, 19, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(103, 40, 19, 18, TFT_BROWN);
            break;

        default:
            break;
        }
    }
    /*Windows-----------------------------------------------------------------------------------------------*/

    /*Usart-----------------------------------------------------------------------------------------------*/

    // 获取可用于从串行端口读取的字节数(字符)
    // 这是已到达并存储在串行接收缓冲区(包含 64 个字节)中的数据
    // reply only when you receive data:
    if (Serial.available() > 0)
    {
        // read the incoming byte:
        Serial.read(Serial_Data, 5);

        sprintf(&Serial_Data_Check[0], "%c", Serial_Data[0]);
        sprintf(&Serial_Data_Check[1], "%c", Serial_Data[1]);

        // 接收数据是否是RTC数据校验
        if (Serial_Data_Check[0] == 'R' && Serial_Data_Check[1] == '0' && Windows_EditMode == 0)
        {
            RTC_Data_Hours_d = Serial_Data[2];
            RTC_Data_Minutes_d = Serial_Data[3];
            RTC_Data_Seconds_d = Serial_Data[4];

            RTC_DataUpdate_Flag = 1;
        }
    }
    // Serial.print(millis()); //返回当前系统运行时间(ms)
    // Serial.println("ms");

    /*Usart-----------------------------------------------------------------------------------------------*/

    /*DS18B20-----------------------------------------------------------------------------------------------*/

    //获取温度数据
    DS18B20Class1.DS18B20_ReadT();
    DS18B20_Data_f = DS18B20Class1.celsius;
    sprintf(DS18B20_Data_c, "%.1f", DS18B20_Data_f); //转换温度数据
    DS18B20_Data_c[4] = 0x00;

    /*DS18B20-----------------------------------------------------------------------------------------------*/

    /*PH-----------------------------------------------------------------------------------------------*/
    // 定时测量PH值
    if (PH_DataTiming_Flag == 1)
    {
        PH_DataTiming_Flag = 0;

        PH_Data_f = PH_Class1.ADC_ReadPH(PH_Pin);
        if (PH_Data_f < 0) //数值越界判断
        {
            PH_Data_f = 0;
        }
        if (PH_Data_f > 14)
        {
            PH_Data_f = 14;
        }

        sprintf(PH_Data_c, "%.2f", PH_Data_f); //转换PH数据
        PH_Data_c[5] = 0x00;
    }

    if (PH_LimitMinData > PH_Data_f || PH_LimitMaxData < PH_Data_f)
    {
        PH_OutOfRange_Flag = 1;
    }
    /*PH-----------------------------------------------------------------------------------------------*/

    /*TDS-----------------------------------------------------------------------------------------------*/
    // 定时测量TDS值
    if (TDS_DataTiming_Flag == 1)
    {
        TDS_DataTiming_Flag = 0;

        TDS_Data_d = TDS_Class1.ADC_ReadTDS(TDS_Pin);

        if (TDS_Data_d < 30) //数值越界判断
        {
            TDS_Data_d = 0;
        }
        if (TDS_Data_d > 1500)
        {
            TDS_Data_d = 1500;
        }
    }

    if (TDS_LimitMinData > TDS_Data_d || TDS_LimitMaxData < TDS_Data_d)
    {
        TDS_OutOfRange_Flag = 1;
    }
    /*TDS-----------------------------------------------------------------------------------------------*/

    /*IPS_Show-----------------------------------------------------------------------------------------------*/
    /*TIM0_RTC_ShowIPS-----------------------------------------------------------------------------------------------*/

    if (Windows == 0 && Windows_EditMode == 0) //主窗口定时获取RTC数据
    {

        if (RTC_DataTiming_Flag == 1)
        {
            RTC_DataTiming_Flag = 0;

            // 串口如果接收到RTC数据的话RTC_DataUpdate_Flag = 1
            if (RTC_DataUpdate_Flag == 1)
            {
                RTC_DataUpdate_Flag = 0;

                TFT_eSPIClass1.drawString(":", 58, 1, 4);
                TFT_eSPIClass1.drawString(":", 96, 1, 4);

                // 时针数据刷新
                if (RTC_Data_Hours_d < 10)
                {
                    TFT_eSPIClass1.drawNumber(0, 29, 3, 4);
                    TFT_eSPIClass1.drawNumber(RTC_Data_Hours_d, 43, 3, 4); //(4字体原x位置+14,2字体原x位置+8)
                }
                else
                {
                    TFT_eSPIClass1.drawNumber(RTC_Data_Hours_d, 29, 3, 4);
                }
                // 分针数据刷新
                if (RTC_Data_Minutes_d < 10)
                {
                    TFT_eSPIClass1.drawNumber(0, 67, 3, 4);
                    TFT_eSPIClass1.drawNumber(RTC_Data_Minutes_d, 81, 3, 4); //(4字体原x位置+14,2字体原x位置+8)
                }
                else
                {
                    TFT_eSPIClass1.drawNumber(RTC_Data_Minutes_d, 67, 3, 4);
                }
                // 秒针数据刷新
                if (RTC_Data_Seconds_d < 10)
                {
                    TFT_eSPIClass1.drawNumber(0, 105, 3, 4);
                    TFT_eSPIClass1.drawNumber(RTC_Data_Seconds_d, 119, 3, 4); //(4字体原x位置+14,2字体原x位置+8)
                }
                else
                {
                    TFT_eSPIClass1.drawNumber(RTC_Data_Seconds_d, 105, 3, 4);
                }
            }
            else
            {
                TFT_eSPIClass1.drawString("00", 28 + 1, 3, 4);
                TFT_eSPIClass1.drawString(":", 59 - 1, 1, 4);
                TFT_eSPIClass1.drawString("00", 66 + 1, 3, 4);
                TFT_eSPIClass1.drawString(":", 97 - 1, 1, 4);
                TFT_eSPIClass1.drawString("00", 104 + 1, 3, 4);
            }
        }
    }

    /*TIM0_RTC_ShowIPS-----------------------------------------------------------------------------------------------*/

    if (Windows == 0) //主窗口
    {
        // 横屏显示
        // TFT_eSPIClass1.drawString("PH:", 25, 25, 2);
        // TFT_eSPIClass1.drawString("TDS:", 82, 25, 2);
        // TFT_eSPIClass1.drawString("00.0", 45, 25, 2);
        // TFT_eSPIClass1.drawString("0000", 110, 25, 2);

        // TFT_eSPIClass1.drawString("FS:", 24, 42, 2);
        // TFT_eSPIClass1.drawString("OS:", 89, 42, 2);
        // TFT_eSPIClass1.drawString("000", 45, 42, 2);
        // TFT_eSPIClass1.drawString("000", 110, 42, 2);
        // TFT_eSPIClass1.drawString("%", 70, 42, 2);
        // TFT_eSPIClass1.drawString("%", 135, 42, 2);

        // TFT_eSPIClass1.drawString("Fish Feed:", 30, 58, 2);
        // TFT_eSPIClass1.drawString("0", 96, 58, 2);
        // TFT_eSPIClass1.drawString("times", 107, 58, 2);

        TFT_eSPIClass1.drawString("TEMP:", 5, 58, 2);
        TFT_eSPIClass1.drawString(DS18B20_Data_c, 43, 58, 2);
        TFT_eSPIClass1.drawString("^C", 73, 58, 2);

        TFT_eSPIClass1.drawString("PH:", 5, 26, 2);
        TFT_eSPIClass1.drawString(PH_Data_c, 25, 26, 2);

        TFT_eSPIClass1.drawString("TDS:", 5, 42, 2);

        // TDS越界显示
        if (TDS_Data_d < 10)
        {
            TFT_eSPIClass1.drawNumber(0, 33, 42, 2);
            TFT_eSPIClass1.drawNumber(0, 41, 42, 2);
            TFT_eSPIClass1.drawNumber(0, 49, 42, 2);
            TFT_eSPIClass1.drawNumber(TDS_Data_d, 57, 42, 2);
        }
        else if (TDS_Data_d < 100)
        {
            TFT_eSPIClass1.drawNumber(0, 33, 42, 2);
            TFT_eSPIClass1.drawNumber(0, 41, 42, 2);
            TFT_eSPIClass1.drawNumber(TDS_Data_d, 49, 42, 2);
        }
        else if (TDS_Data_d < 1000)
        {
            TFT_eSPIClass1.drawNumber(0, 33, 42, 2);
            TFT_eSPIClass1.drawNumber(TDS_Data_d, 41, 42, 2);
        }
        else
        {
            TFT_eSPIClass1.drawNumber(TDS_Data_d, 33, 42, 2);
        }

        TFT_eSPIClass1.drawString("FS:", 100, 42, 2);
        TFT_eSPIClass1.drawString("OS:", 100, 58, 2);
        TFT_eSPIClass1.drawString("%", 145, 42, 2);
        TFT_eSPIClass1.drawString("%", 145, 58, 2);

        // 主界面已经完成投食次数越界判断
        if (FishFeed_CompleteTimes > 9)
        {
            FishFeed_CompleteTimes = 0;
        }
        if (FishFeed_CompleteTimes < 0)
        {
            FishFeed_CompleteTimes = 9;
        }

        TFT_eSPIClass1.drawString("Feed:", 80, 26, 2);
        TFT_eSPIClass1.drawString("times", 125, 26, 2);
        TFT_eSPIClass1.drawNumber(FishFeed_CompleteTimes, 115, 26, 2);

        // FIL电机速度显示越界判断
        if (FIL_Speed_Data < 10)
        {
            TFT_eSPIClass1.drawNumber(0, 120, 42, 2);
            TFT_eSPIClass1.drawNumber(0, 128, 42, 2);
            TFT_eSPIClass1.drawNumber(FIL_Speed_Data, 136, 42, 2);
        }
        else if (FIL_Speed_Data < 100)
        {
            TFT_eSPIClass1.drawNumber(0, 120, 42, 2);
            TFT_eSPIClass1.drawNumber(FIL_Speed_Data, 128, 42, 2);
        }
        else
        {
            TFT_eSPIClass1.drawNumber(FIL_Speed_Data, 120, 42, 2);
        }

        // OXY电机速度显示越界判断
        if (OXY_Speed_Data < 10)
        {
            TFT_eSPIClass1.drawNumber(0, 120, 58, 2);
            TFT_eSPIClass1.drawNumber(0, 128, 58, 2);
            TFT_eSPIClass1.drawNumber(OXY_Speed_Data, 136, 58, 2);
        }
        else if (OXY_Speed_Data < 100)
        {
            TFT_eSPIClass1.drawNumber(0, 120, 58, 2);
            TFT_eSPIClass1.drawNumber(OXY_Speed_Data, 128, 58, 2);
        }
        else
        {
            TFT_eSPIClass1.drawNumber(OXY_Speed_Data, 120, 58, 2);
        }
    }

    if (Windows == 1) // 窗口1
    {
        TFT_eSPIClass1.drawString("PH:", 5, 5, 4);
        TFT_eSPIClass1.drawString(PH_Data_c, 47, 5, 4);

        TFT_eSPIClass1.drawString("Clean", 120, 7, 2);
        TFT_eSPIClass1.drawString("Limit:", 35, 32, 2);
        TFT_eSPIClass1.drawString("<->", 90, 32, 2);

        // PH显示0判断
        if (PH_LimitMinData < 10)
        {
            TFT_eSPIClass1.drawNumber(0, 70, 32, 2);
            TFT_eSPIClass1.drawNumber(PH_LimitMinData, 78, 32, 2);
        }
        else
        {
            TFT_eSPIClass1.drawNumber(PH_LimitMinData, 70, 32, 2);
        }

        if (PH_LimitMaxData < 10)
        {
            TFT_eSPIClass1.drawNumber(0, 113, 32, 2);
            TFT_eSPIClass1.drawNumber(PH_LimitMaxData, 121, 32, 2);
        }
        else
        {
            TFT_eSPIClass1.drawNumber(PH_LimitMaxData, 113, 32, 2);
        }

        if (PH_OutOfRange_Flag == 1)
        {
            PH_OutOfRange_Flag = 0;
            PH_OutOfRange_LoopFlag = 1;

            PH_OutOfRange_Data = PH_Data_f; //越界PH值记录

            PH_OutOfRange_Clock[0] = RTC_Data_Hours_d;
            PH_OutOfRange_Clock[1] = RTC_Data_Minutes_d;
            PH_OutOfRange_Clock[2] = RTC_Data_Seconds_d;
        }
        // PH循环刷新数据
        if (PH_OutOfRange_LoopFlag == 1)
        {
            TFT_eSPIClass1.drawString("OOR", 20, 53, 2); // Out Of Range(OOR)
            TFT_eSPIClass1.drawString("[", 45, 53, 2);
            TFT_eSPIClass1.drawString("]:", 67, 53, 2);
            TFT_eSPIClass1.drawString(":", 95, 53, 2);
            TFT_eSPIClass1.drawString(":", 118, 53, 2);

            // 小于10个位数填充0
            if (PH_OutOfRange_Data < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 50, 53, 2);
                TFT_eSPIClass1.drawNumber(PH_OutOfRange_Data, 58, 53, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(PH_OutOfRange_Data, 50, 53, 2);
            }

            if (PH_OutOfRange_Clock[0] < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 77, 53, 2);
                TFT_eSPIClass1.drawNumber(PH_OutOfRange_Clock[0], 85, 53, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(PH_OutOfRange_Clock[0], 77, 53, 2);
            }

            if (PH_OutOfRange_Clock[1] < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 100, 53, 2);
                TFT_eSPIClass1.drawNumber(PH_OutOfRange_Clock[1], 108, 53, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(PH_OutOfRange_Clock[1], 100, 53, 2);
            }

            if (PH_OutOfRange_Clock[2] < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 124, 53, 2);
                TFT_eSPIClass1.drawNumber(PH_OutOfRange_Clock[2], 132, 53, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(PH_OutOfRange_Clock[2], 124, 53, 2);
            }
        }
    }

    if (Windows == 2) // 窗口2
    {
        TFT_eSPIClass1.drawString("TDS:", 5, 5, 4);

        // TDS越界显示
        if (TDS_Data_d < 10)
        {
            TFT_eSPIClass1.drawNumber(0, 60, 5, 4);
            TFT_eSPIClass1.drawNumber(0, 74, 5, 4);
            TFT_eSPIClass1.drawNumber(0, 88, 5, 4);
            TFT_eSPIClass1.drawNumber(TDS_Data_d, 102, 5, 4);
        }
        else if (TDS_Data_d < 100)
        {
            TFT_eSPIClass1.drawNumber(0, 60, 5, 4);
            TFT_eSPIClass1.drawNumber(0, 74, 5, 4);
            TFT_eSPIClass1.drawNumber(TDS_Data_d, 88, 5, 4);
        }
        else if (TDS_Data_d < 1000)
        {
            TFT_eSPIClass1.drawNumber(0, 60, 5, 4);
            TFT_eSPIClass1.drawNumber(TDS_Data_d, 74, 5, 4);
        }
        else
        {
            TFT_eSPIClass1.drawNumber(TDS_Data_d, 60, 5, 4);
        }

        TFT_eSPIClass1.drawString("Clean", 120, 7, 2);
        TFT_eSPIClass1.drawString("Limit:", 20, 32, 2);
        TFT_eSPIClass1.drawString("<->", 91, 32, 2);

        // TDS显示0判断
        if (TDS_LimitMinData < 10)
        {
            TFT_eSPIClass1.drawNumber(0, 55, 32, 2);
            TFT_eSPIClass1.drawNumber(0, 63, 32, 2);
            TFT_eSPIClass1.drawNumber(0, 71, 32, 2);
            TFT_eSPIClass1.drawNumber(TDS_LimitMinData, 79, 32, 2);
        }
        else if (TDS_LimitMinData < 100)
        {
            TFT_eSPIClass1.drawNumber(0, 55, 32, 2);
            TFT_eSPIClass1.drawNumber(0, 63, 32, 2);
            TFT_eSPIClass1.drawNumber(TDS_LimitMinData, 71, 32, 2);
        }
        else if (TDS_LimitMinData < 1000)
        {
            TFT_eSPIClass1.drawNumber(0, 55, 32, 2);
            TFT_eSPIClass1.drawNumber(TDS_LimitMinData, 63, 32, 2);
        }
        else
        {
            TFT_eSPIClass1.drawNumber(TDS_LimitMinData, 55, 32, 2);
        }

        if (TDS_LimitMaxData < 10)
        {
            TFT_eSPIClass1.drawNumber(0, 113, 32, 2);
            TFT_eSPIClass1.drawNumber(0, 121, 32, 2);
            TFT_eSPIClass1.drawNumber(0, 129, 32, 2);
            TFT_eSPIClass1.drawNumber(TDS_LimitMaxData, 137, 32, 2);
        }
        else if (TDS_LimitMaxData < 100)
        {
            TFT_eSPIClass1.drawNumber(0, 113, 32, 2);
            TFT_eSPIClass1.drawNumber(0, 121, 32, 2);
            TFT_eSPIClass1.drawNumber(TDS_LimitMaxData, 129, 32, 2);
        }
        else if (TDS_LimitMaxData < 1000)
        {
            TFT_eSPIClass1.drawNumber(0, 113, 32, 2);
            TFT_eSPIClass1.drawNumber(TDS_LimitMaxData, 121, 32, 2);
        }
        else
        {
            TFT_eSPIClass1.drawNumber(TDS_LimitMaxData, 113, 32, 2);
        }

        if (TDS_OutOfRange_Flag == 1)
        {
            TDS_OutOfRange_Flag = 0;
            TDS_OutOfRange_LoopFlag = 1;

            TDS_OutOfRange_Data = TDS_Data_d; //越界TDS值记录

            TDS_OutOfRange_Clock[0] = RTC_Data_Hours_d;
            TDS_OutOfRange_Clock[1] = RTC_Data_Minutes_d;
            TDS_OutOfRange_Clock[2] = RTC_Data_Seconds_d;
        }

        // TDS循环刷新数据
        if (TDS_OutOfRange_LoopFlag == 1)
        {
            TFT_eSPIClass1.drawString("OOR", 14, 53, 2); // Out Of Range(OOR)
            TFT_eSPIClass1.drawString("[", 39, 53, 2);
            TFT_eSPIClass1.drawString("]:", 77, 53, 2);
            TFT_eSPIClass1.drawString(":", 105, 53, 2);
            TFT_eSPIClass1.drawString(":", 128, 53, 2);

            // TDS显示0判断
            if (TDS_OutOfRange_Data < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 44, 53, 2);
                TFT_eSPIClass1.drawNumber(0, 52, 53, 2);
                TFT_eSPIClass1.drawNumber(0, 60, 53, 2);
                TFT_eSPIClass1.drawNumber(TDS_OutOfRange_Data, 68, 53, 2);
            }
            else if (TDS_OutOfRange_Data < 100)
            {
                TFT_eSPIClass1.drawNumber(0, 44, 53, 2);
                TFT_eSPIClass1.drawNumber(0, 52, 53, 2);
                TFT_eSPIClass1.drawNumber(TDS_OutOfRange_Data, 60, 53, 2);
            }
            else if (TDS_OutOfRange_Data < 1000)
            {
                TFT_eSPIClass1.drawNumber(0, 44, 53, 2);
                TFT_eSPIClass1.drawNumber(TDS_OutOfRange_Data, 52, 53, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(TDS_OutOfRange_Data, 44, 53, 2);
            }

            if (TDS_OutOfRange_Clock[0] < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 87, 53, 2);
                TFT_eSPIClass1.drawNumber(TDS_OutOfRange_Clock[0], 95, 53, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(TDS_OutOfRange_Clock[0], 87, 53, 2);
            }

            if (TDS_OutOfRange_Clock[1] < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 110, 53, 2);
                TFT_eSPIClass1.drawNumber(TDS_OutOfRange_Clock[1], 118, 53, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(TDS_OutOfRange_Clock[1], 110, 53, 2);
            }

            if (TDS_OutOfRange_Clock[2] < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 134, 53, 2);
                TFT_eSPIClass1.drawNumber(TDS_OutOfRange_Clock[2], 142, 53, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(TDS_OutOfRange_Clock[2], 134, 53, 2);
            }
        }
    }

    if (Windows == 3) // 窗口3
    {
        TFT_eSPIClass1.drawString("Fish Feed/day:", 6, 3, 2);
        TFT_eSPIClass1.drawNumber(FishFeedTimes_Data, 105, 3, 2);
        TFT_eSPIClass1.drawString("times", 120, 3, 2);

        switch (FishFeedTimes_Data)
        {
        case 0: // 0times界面显示
            TFT_eSPIClass1.drawString("Run a little", 45, 22, 2);
            TFT_eSPIClass1.drawString("Run a half", 48, 40, 2);
            TFT_eSPIClass1.drawString("Run a lab", 50, 58, 2);
            break;
        case 1: // 1times界面显示
            TFT_eSPIClass1.drawString("Time P1:", 6, 25, 2);
            TFT_eSPIClass1.drawString(":", 95, 25, 2);
            TFT_eSPIClass1.drawString(":", 122, 25, 2);

            if (FishFeed_TimeDataP1_Hours < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 75, 25, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Hours, 83, 25, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Hours, 75, 25, 2);
            }
            if (FishFeed_TimeDataP1_Minutes < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 102, 25, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Minutes, 110, 25, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Minutes, 102, 25, 2);
            }
            if (FishFeed_TimeDataP1_Seconds < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 129, 25, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Seconds, 137, 25, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Seconds, 129, 25, 2);
            }

            break;
        case 2: // 2times界面显示
            TFT_eSPIClass1.drawString("Time P1:", 6, 25, 2);
            TFT_eSPIClass1.drawString(":", 95, 25, 2);
            TFT_eSPIClass1.drawString(":", 122, 25, 2);

            if (FishFeed_TimeDataP1_Hours < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 75, 25, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Hours, 83, 25, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Hours, 75, 25, 2);
            }
            if (FishFeed_TimeDataP1_Minutes < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 102, 25, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Minutes, 110, 25, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Minutes, 102, 25, 2);
            }
            if (FishFeed_TimeDataP1_Seconds < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 129, 25, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Seconds, 137, 25, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Seconds, 129, 25, 2);
            }

            TFT_eSPIClass1.drawString("Time P2:", 6, 42, 2);
            TFT_eSPIClass1.drawString(":", 95, 42, 2);
            TFT_eSPIClass1.drawString(":", 122, 42, 2);

            if (FishFeed_TimeDataP2_Hours < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 75, 42, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP2_Hours, 83, 42, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP2_Hours, 75, 42, 2);
            }
            if (FishFeed_TimeDataP2_Minutes < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 102, 42, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP2_Minutes, 110, 42, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP2_Minutes, 102, 42, 2);
            }
            if (FishFeed_TimeDataP2_Seconds < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 129, 42, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP2_Seconds, 137, 42, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP2_Seconds, 129, 42, 2);
            }

            break;
        case 3: // 3times界面显示
            TFT_eSPIClass1.drawString("Time P1:", 6, 25, 2);
            TFT_eSPIClass1.drawString(":", 95, 25, 2);
            TFT_eSPIClass1.drawString(":", 122, 25, 2);

            if (FishFeed_TimeDataP1_Hours < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 75, 25, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Hours, 83, 25, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Hours, 75, 25, 2);
            }
            if (FishFeed_TimeDataP1_Minutes < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 102, 25, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Minutes, 110, 25, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Minutes, 102, 25, 2);
            }
            if (FishFeed_TimeDataP1_Seconds < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 129, 25, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Seconds, 137, 25, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP1_Seconds, 129, 25, 2);
            }

            TFT_eSPIClass1.drawString("Time P2:", 6, 42, 2);
            TFT_eSPIClass1.drawString(":", 95, 42, 2);
            TFT_eSPIClass1.drawString(":", 122, 42, 2);

            if (FishFeed_TimeDataP2_Hours < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 75, 42, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP2_Hours, 83, 42, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP2_Hours, 75, 42, 2);
            }
            if (FishFeed_TimeDataP2_Minutes < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 102, 42, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP2_Minutes, 110, 42, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP2_Minutes, 102, 42, 2);
            }
            if (FishFeed_TimeDataP2_Seconds < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 129, 42, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP2_Seconds, 137, 42, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP2_Seconds, 129, 42, 2);
            }

            TFT_eSPIClass1.drawString("Time P3:", 6, 59, 2);
            TFT_eSPIClass1.drawString(":", 95, 59, 2);
            TFT_eSPIClass1.drawString(":", 122, 59, 2);

            if (FishFeed_TimeDataP3_Hours < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 75, 59, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP3_Hours, 83, 59, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP3_Hours, 75, 59, 2);
            }
            if (FishFeed_TimeDataP3_Minutes < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 102, 59, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP3_Minutes, 110, 59, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP3_Minutes, 102, 59, 2);
            }
            if (FishFeed_TimeDataP3_Seconds < 10)
            {
                TFT_eSPIClass1.drawNumber(0, 129, 59, 2);
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP3_Seconds, 137, 59, 2);
            }
            else
            {
                TFT_eSPIClass1.drawNumber(FishFeed_TimeDataP3_Seconds, 129, 59, 2);
            }

            break;

        default:
            break;
        }
    }

    if (Windows == 4) // 窗口4
    {

        TFT_eSPIClass1.drawString("FIL_Mood:", 11, 3, 2);
        TFT_eSPIClass1.drawString("OXY_Mood:", 8, 40, 2);

        switch (FIL_Mood_Flag) // AT或者MT显示
        {
        case 0:
            TFT_eSPIClass1.drawString("AT", 105, 3, 2);
            break;

        case 1:
            TFT_eSPIClass1.drawString("MT", 105, 3, 2);
            break;

        default:
            break;
        }

        switch (OXY_Mood_Flag) // AT或者MT显示
        {
        case 0:
            TFT_eSPIClass1.drawString("AT", 105, 40, 2);
            break;

        case 1:
            TFT_eSPIClass1.drawString("MT", 105, 40, 2);
            break;

        default:
            break;
        }

        switch (FIL_Mood_Flag) //自动模式过滤&供氧速度隐藏
        {
        case 0:

            switch (OXY_Mood_Flag)
            {
            case 0:
                break;

            case 1:
                TFT_eSPIClass1.drawString("OXY_Speed:", 7, 57, 2);
                TFT_eSPIClass1.drawString("%", 130, 58, 2);

                if (OXY_Speed_Data < 10) //显示越界判断
                {
                    TFT_eSPIClass1.drawNumber(0, 101, 58, 2);
                    TFT_eSPIClass1.drawNumber(0, 109, 58, 2);
                    TFT_eSPIClass1.drawNumber(OXY_Speed_Data, 117, 58, 2);
                }
                else if (OXY_Speed_Data < 100)
                {
                    TFT_eSPIClass1.drawNumber(0, 101, 58, 2);
                    TFT_eSPIClass1.drawNumber(OXY_Speed_Data, 109, 58, 2);
                }
                else
                {
                    TFT_eSPIClass1.drawNumber(OXY_Speed_Data, 101, 58, 2);
                }
                break;

            default:
                break;
            }

            break;
        case 1:

            switch (OXY_Mood_Flag)
            {
            case 0:
                TFT_eSPIClass1.drawString("FIL_Speed:", 10, 20, 2);
                TFT_eSPIClass1.drawString("%", 130, 21, 2);

                if (FIL_Speed_Data < 10) //显示越界判断
                {
                    TFT_eSPIClass1.drawNumber(0, 101, 21, 2);
                    TFT_eSPIClass1.drawNumber(0, 109, 21, 2);
                    TFT_eSPIClass1.drawNumber(FIL_Speed_Data, 117, 21, 2);
                }
                else if (FIL_Speed_Data < 100)
                {
                    TFT_eSPIClass1.drawNumber(0, 101, 21, 2);
                    TFT_eSPIClass1.drawNumber(FIL_Speed_Data, 109, 21, 2);
                }
                else
                {
                    TFT_eSPIClass1.drawNumber(FIL_Speed_Data, 101, 21, 2);
                }
                break;

            case 1:
                TFT_eSPIClass1.drawString("FIL_Speed:", 10, 20, 2);
                TFT_eSPIClass1.drawString("OXY_Speed:", 7, 57, 2);
                TFT_eSPIClass1.drawString("%", 130, 21, 2);
                TFT_eSPIClass1.drawString("%", 130, 58, 2);

                if (FIL_Speed_Data < 10) //显示越界判断
                {
                    TFT_eSPIClass1.drawNumber(0, 101, 21, 2);
                    TFT_eSPIClass1.drawNumber(0, 109, 21, 2);
                    TFT_eSPIClass1.drawNumber(FIL_Speed_Data, 117, 21, 2);
                }
                else if (FIL_Speed_Data < 100)
                {
                    TFT_eSPIClass1.drawNumber(0, 101, 21, 2);
                    TFT_eSPIClass1.drawNumber(FIL_Speed_Data, 109, 21, 2);
                }
                else
                {
                    TFT_eSPIClass1.drawNumber(FIL_Speed_Data, 101, 21, 2);
                }

                if (OXY_Speed_Data < 10) //显示越界判断
                {
                    TFT_eSPIClass1.drawNumber(0, 101, 58, 2);
                    TFT_eSPIClass1.drawNumber(0, 109, 58, 2);
                    TFT_eSPIClass1.drawNumber(OXY_Speed_Data, 117, 58, 2);
                }
                else if (OXY_Speed_Data < 100)
                {
                    TFT_eSPIClass1.drawNumber(0, 101, 58, 2);
                    TFT_eSPIClass1.drawNumber(OXY_Speed_Data, 109, 58, 2);
                }
                else
                {
                    TFT_eSPIClass1.drawNumber(OXY_Speed_Data, 101, 58, 2);
                }
                break;

            default:
                break;
            }

            break;

        default:
            break;
        }
    }

    if (Windows == 5) // 窗口5
    {
        TFT_eSPIClass1.drawString("1.Feeding Quantity:", 3, 3, 2);
        TFT_eSPIClass1.drawNumber(FishFeed_AutoQuantityData, 130, 3, 2);

        TFT_eSPIClass1.drawString("2.Feed Waiting:", 3, 22, 2);
        if (FishFeed_OtherMotorControl_WaitingTime < 10)
        {
            TFT_eSPIClass1.drawNumber(0, 130, 22, 2);
            TFT_eSPIClass1.drawNumber(FishFeed_OtherMotorControl_WaitingTime, 138, 22, 2);
        }
        else
        {
            TFT_eSPIClass1.drawNumber(FishFeed_OtherMotorControl_WaitingTime, 130, 22, 2);
        }

        TFT_eSPIClass1.drawString("3.Light M:", 3, 41, 2);
        TFT_eSPIClass1.drawNumber(Light_ModeData, 68, 41, 2);

        TFT_eSPIClass1.drawString("B:", 90, 41, 2);
        TFT_eSPIClass1.drawString("%", 127, 41, 2);

        TFT_eSPIClass1.drawString("[FISH TANK-Z v1.0.0]", 12, 60, 2);

        if (Light_BrightnessData < 10)
        {
            TFT_eSPIClass1.drawNumber(0, 105, 41, 2);
            TFT_eSPIClass1.drawNumber(Light_BrightnessData, 113, 41, 2);
        }
        else
        {
            TFT_eSPIClass1.drawNumber(Light_BrightnessData, 105, 41, 2);
        }
    }
    /*IPS_Show------------------------------------------------------------------------------------------------*/

    /*IPS_Switch-----------------------------------------------------------------------------------------------*/
    switch (IPS_Switch_Flag)
    {
    case 0:
        break;
    case IPS_Switch_Up:

        //窗口向上切换
        if (Windows_EditMode == 0)
        {
            Windows--; //窗口向上切换
            TFT_eSPIClass1.fillScreen(TFT_BLACK);
            TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
        }

        //主窗口时钟修改++
        if (Windows == 0 && Windows_EditMode == 1)
        {
            switch (Windows0_EditPosition)
            {
            case 0:
                RTC_Data_Hours_d++;
                if (RTC_Data_Hours_d > 23) //主窗口RTC时钟修改模式时间数值越界重置
                {
                    RTC_Data_Hours_d = 0;
                }
                break;
            case 1:
                RTC_Data_Minutes_d++;
                if (RTC_Data_Minutes_d > 59)
                {
                    RTC_Data_Minutes_d = 0;
                }
                break;
            case 2:
                RTC_Data_Seconds_d++;
                if (RTC_Data_Seconds_d > 59)
                {
                    RTC_Data_Seconds_d = 0;
                }
                break;
            case 3:
                FishFeed_CompleteTimes++;
                break;

            default:
                break;
            }
        }

        //窗口1PH限制值修改++
        if (Windows == 1 && Windows_EditMode == 1)
        {
            switch (Windows1_EditPosition)
            {
            case 0:
                PH_LimitMinData = 0;
                PH_LimitMaxData = 14;
                PH_OutOfRange_LoopFlag = 0;
                TFT_eSPIClass1.fillScreen(TFT_BLACK);
                TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                break;
            case 1:
                PH_LimitMinData++;
                if (PH_LimitMinData > 14)
                {
                    PH_LimitMinData = 0;
                }
                break;
            case 2:
                PH_LimitMaxData++;
                if (PH_LimitMaxData > 14)
                {
                    PH_LimitMaxData = 0;
                }
                break;

            default:
                break;
            }
        }

        //窗口2TDS限制值修改++
        if (Windows == 2 && Windows_EditMode == 1)
        {
            switch (Windows2_EditPosition)
            {
            case 0:
                TDS_LimitMinData = 0;
                TDS_LimitMaxData = 1500;
                TDS_OutOfRange_LoopFlag = 0;
                TFT_eSPIClass1.fillScreen(TFT_BLACK);
                TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                break;
            case 1:
                TDS_LimitMinData = TDS_LimitMinData + 50;
                if (TDS_LimitMinData > 1000)
                {
                    TDS_LimitMinData = 0;
                }
                break;
            case 2:
                TDS_LimitMaxData = TDS_LimitMaxData + 50;
                if (TDS_LimitMaxData > 1000)
                {
                    TDS_LimitMaxData = 0;
                }
                break;

            default:
                break;
            }
        }

        //窗口3时钟修改++
        if (Windows == 3 && Windows_EditMode == 1)
        {
            switch (FishFeedTimes_Data)
            {
            case 0:
                // 手动投食模式越界++判断
                switch (Windows3_EditPosition)
                {
                case 0:
                    FishFeedTimes_Data++;
                    if (FishFeedTimes_Data > 3)
                    {
                        FishFeedTimes_Data = 0;
                    }
                    TFT_eSPIClass1.fillScreen(TFT_BLACK);
                    TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                default:
                    break;
                }

                break;
            case 1:
                // 定时自动投食1times模式时间越界++判断
                switch (Windows3_EditPosition)
                {
                case 0:
                    FishFeedTimes_Data++;
                    if (FishFeedTimes_Data > 3)
                    {
                        FishFeedTimes_Data = 0;
                    }
                    TFT_eSPIClass1.fillScreen(TFT_BLACK);
                    TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                    break;
                case 1:
                    FishFeed_TimeDataP1_Hours++;
                    if (FishFeed_TimeDataP1_Hours > 23)
                    {
                        FishFeed_TimeDataP1_Hours = 0;
                    }
                    break;
                case 2:
                    FishFeed_TimeDataP1_Minutes++;
                    if (FishFeed_TimeDataP1_Minutes > 59)
                    {
                        FishFeed_TimeDataP1_Minutes = 0;
                    }
                    break;
                case 3:
                    FishFeed_TimeDataP1_Seconds++;
                    if (FishFeed_TimeDataP1_Seconds > 59)
                    {
                        FishFeed_TimeDataP1_Seconds = 0;
                    }
                    break;

                default:
                    break;
                }

                break;
            case 2:
                // 定时自动投食2times模式时间越界++判断
                switch (Windows3_EditPosition)
                {
                case 0:
                    FishFeedTimes_Data++;
                    if (FishFeedTimes_Data > 3)
                    {
                        FishFeedTimes_Data = 0;
                    }
                    TFT_eSPIClass1.fillScreen(TFT_BLACK);
                    TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                    break;
                case 1:
                    FishFeed_TimeDataP1_Hours++;
                    if (FishFeed_TimeDataP1_Hours > 23)
                    {
                        FishFeed_TimeDataP1_Hours = 0;
                    }
                    break;
                case 2:
                    FishFeed_TimeDataP1_Minutes++;
                    if (FishFeed_TimeDataP1_Minutes > 59)
                    {
                        FishFeed_TimeDataP1_Minutes = 0;
                    }
                    break;
                case 3:
                    FishFeed_TimeDataP1_Seconds++;
                    if (FishFeed_TimeDataP1_Seconds > 59)
                    {
                        FishFeed_TimeDataP1_Seconds = 0;
                    }
                    break;
                case 4:
                    FishFeed_TimeDataP2_Hours++;
                    if (FishFeed_TimeDataP2_Hours > 23)
                    {
                        FishFeed_TimeDataP2_Hours = 0;
                    }
                    break;
                case 5:
                    FishFeed_TimeDataP2_Minutes++;
                    if (FishFeed_TimeDataP2_Minutes > 59)
                    {
                        FishFeed_TimeDataP2_Minutes = 0;
                    }
                    break;
                case 6:
                    FishFeed_TimeDataP2_Seconds++;
                    if (FishFeed_TimeDataP2_Seconds > 59)
                    {
                        FishFeed_TimeDataP2_Seconds = 0;
                    }
                    break;

                default:
                    break;
                }

                break;
            case 3:
                // 定时自动投食3times模式时间越界++判断
                switch (Windows3_EditPosition)
                {
                case 0:
                    FishFeedTimes_Data++;
                    if (FishFeedTimes_Data > 3)
                    {
                        FishFeedTimes_Data = 0;
                    }
                    TFT_eSPIClass1.fillScreen(TFT_BLACK);
                    TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                    break;
                case 1:
                    FishFeed_TimeDataP1_Hours++;
                    if (FishFeed_TimeDataP1_Hours > 23)
                    {
                        FishFeed_TimeDataP1_Hours = 0;
                    }
                    break;
                case 2:
                    FishFeed_TimeDataP1_Minutes++;
                    if (FishFeed_TimeDataP1_Minutes > 59)
                    {
                        FishFeed_TimeDataP1_Minutes = 0;
                    }
                    break;
                case 3:
                    FishFeed_TimeDataP1_Seconds++;
                    if (FishFeed_TimeDataP1_Seconds > 59)
                    {
                        FishFeed_TimeDataP1_Seconds = 0;
                    }
                    break;
                case 4:
                    FishFeed_TimeDataP2_Hours++;
                    if (FishFeed_TimeDataP2_Hours > 23)
                    {
                        FishFeed_TimeDataP2_Hours = 0;
                    }
                    break;
                case 5:
                    FishFeed_TimeDataP2_Minutes++;
                    if (FishFeed_TimeDataP2_Minutes > 59)
                    {
                        FishFeed_TimeDataP2_Minutes = 0;
                    }
                    break;
                case 6:
                    FishFeed_TimeDataP2_Seconds++;
                    if (FishFeed_TimeDataP2_Seconds > 59)
                    {
                        FishFeed_TimeDataP2_Seconds = 0;
                    }
                    break;
                case 7:
                    FishFeed_TimeDataP3_Hours++;
                    if (FishFeed_TimeDataP3_Hours > 23)
                    {
                        FishFeed_TimeDataP3_Hours = 0;
                    }
                    break;
                case 8:
                    FishFeed_TimeDataP3_Minutes++;
                    if (FishFeed_TimeDataP3_Minutes > 59)
                    {
                        FishFeed_TimeDataP3_Minutes = 0;
                    }
                    break;
                case 9:
                    FishFeed_TimeDataP3_Seconds++;
                    if (FishFeed_TimeDataP3_Seconds > 59)
                    {
                        FishFeed_TimeDataP3_Seconds = 0;
                    }
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
            }
        }

        //窗口4电机数据修改++
        if (Windows == 4 && Windows_EditMode == 1)
        {

            switch (Windows4_EditPosition)
            {
            case 0:
                FIL_Mood_Flag++;
                if (FIL_Mood_Flag > 1)
                {
                    FIL_Mood_Flag = 0;
                }

                // 窗口4刷新显示
                TFT_eSPIClass1.fillScreen(TFT_BLACK);
                TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                break;
            case 1:
                FIL_Speed_Data++;
                if (FIL_Speed_Data > 10)
                {
                    FIL_Speed_Data = 0;
                }
                break;
            case 2:
                OXY_Mood_Flag++;
                if (OXY_Mood_Flag > 1)
                {
                    OXY_Mood_Flag = 0;
                }

                // 窗口4刷新显示
                TFT_eSPIClass1.fillScreen(TFT_BLACK);
                TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                break;
            case 3:
                OXY_Speed_Data++;
                if (OXY_Speed_Data > 50)
                {
                    OXY_Speed_Data = 0;
                }
                break;

            default:
                break;
            }
        }

        //窗口5数据修改++
        if (Windows == 5 && Windows_EditMode == 1)
        {
            switch (Windows5_EditPosition)
            {
            case 0:
                FishFeed_AutoQuantityData++;
                if (FishFeed_AutoQuantityData > 3)
                {
                    FishFeed_AutoQuantityData = 1;
                }
                break;
            case 1:
                FishFeed_OtherMotorControl_WaitingTime++;
                if (FishFeed_OtherMotorControl_WaitingTime > 99)
                {
                    FishFeed_OtherMotorControl_WaitingTime = 0;
                }
                break;
            case 2:
                Light_ModeData++;
                if (Light_ModeData > 3)
                {
                    Light_ModeData = 0;
                }
                break;
            case 3:
                Light_BrightnessData++;
                if (Light_BrightnessData > 99)
                {
                    Light_BrightnessData = 0;
                }
                break;

            default:
                break;
            }
        }

        delay(200); //消抖
        IPS_Switch_Flag = 0;

        break;
    case IPS_Switch_Down:

        //窗口向下切换
        if (Windows_EditMode == 0)
        {
            Windows++; //窗口向下切换
            TFT_eSPIClass1.fillScreen(TFT_BLACK);
            TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
        }

        //主窗口时钟修改--
        if (Windows == 0 && Windows_EditMode == 1)
        {
            switch (Windows0_EditPosition)
            {
            case 0:
                RTC_Data_Hours_d--;
                if (RTC_Data_Hours_d < 0) //主窗口RTC时钟修改模式时间数值越界重置
                {
                    RTC_Data_Hours_d = 23;
                }
                break;
            case 1:
                RTC_Data_Minutes_d--;
                if (RTC_Data_Minutes_d < 0)
                {
                    RTC_Data_Minutes_d = 59;
                }
                break;
            case 2:
                RTC_Data_Seconds_d--;
                if (RTC_Data_Seconds_d < 0)
                {
                    RTC_Data_Seconds_d = 59;
                }
                break;
            case 3:
                FishFeed_CompleteTimes--;
                break;

            default:
                break;
            }
        }

        //窗口1PH限制值修改--
        if (Windows == 1 && Windows_EditMode == 1)
        {
            switch (Windows1_EditPosition)
            {
            case 0:
                PH_LimitMinData = 0;
                PH_LimitMaxData = 14;
                PH_OutOfRange_LoopFlag = 0;
                TFT_eSPIClass1.fillScreen(TFT_BLACK);
                TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                break;
            case 1:
                PH_LimitMinData--;
                if (PH_LimitMinData < 0)
                {
                    PH_LimitMinData = 14;
                }
                break;
            case 2:
                PH_LimitMaxData--;
                if (PH_LimitMaxData < 0)
                {
                    PH_LimitMaxData = 14;
                }
                break;

            default:
                break;
            }
        }

        //窗口2TDS限制值修改++
        if (Windows == 2 && Windows_EditMode == 1)
        {
            switch (Windows2_EditPosition)
            {
            case 0:
                TDS_LimitMinData = 0;
                TDS_LimitMaxData = 1500;
                TDS_OutOfRange_LoopFlag = 0;
                TFT_eSPIClass1.fillScreen(TFT_BLACK);
                TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                break;
            case 1:
                TDS_LimitMinData = TDS_LimitMinData - 50;
                if (TDS_LimitMinData < 0)
                {
                    TDS_LimitMinData = 1000;
                }
                break;
            case 2:
                TDS_LimitMaxData = TDS_LimitMaxData - 50;
                if (TDS_LimitMaxData < 0)
                {
                    TDS_LimitMaxData = 1000;
                }
                break;

            default:
                break;
            }
        }

        //窗口3时钟修改--
        if (Windows == 3 && Windows_EditMode == 1)
        {
            switch (FishFeedTimes_Data)
            {
            case 0:
                // 手动投食模式越界--判断
                switch (Windows3_EditPosition)
                {
                case 0:
                    FishFeedTimes_Data--;
                    if (FishFeedTimes_Data < 0)
                    {
                        FishFeedTimes_Data = 3;
                    }
                    TFT_eSPIClass1.fillScreen(TFT_BLACK);
                    TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                default:
                    break;
                }

                break;
            case 1:
                // 定时自动投食1times模式时间越界--判断
                switch (Windows3_EditPosition)
                {
                case 0:
                    FishFeedTimes_Data--;
                    if (FishFeedTimes_Data < 0)
                    {
                        FishFeedTimes_Data = 3;
                    }
                    TFT_eSPIClass1.fillScreen(TFT_BLACK);
                    TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                    break;
                case 1:
                    FishFeed_TimeDataP1_Hours--;
                    if (FishFeed_TimeDataP1_Hours < 0)
                    {
                        FishFeed_TimeDataP1_Hours = 23;
                    }
                    break;
                case 2:
                    FishFeed_TimeDataP1_Minutes--;
                    if (FishFeed_TimeDataP1_Minutes < 0)
                    {
                        FishFeed_TimeDataP1_Minutes = 59;
                    }
                    break;
                case 3:
                    FishFeed_TimeDataP1_Seconds--;
                    if (FishFeed_TimeDataP1_Seconds < 0)
                    {
                        FishFeed_TimeDataP1_Seconds = 59;
                    }
                    break;

                default:
                    break;
                }

                break;
            case 2:
                // 定时自动投食2times模式时间越界--判断
                switch (Windows3_EditPosition)
                {
                case 0:
                    FishFeedTimes_Data--;
                    if (FishFeedTimes_Data < 0)
                    {
                        FishFeedTimes_Data = 3;
                    }
                    TFT_eSPIClass1.fillScreen(TFT_BLACK);
                    TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                    break;
                case 1:
                    FishFeed_TimeDataP1_Hours--;
                    if (FishFeed_TimeDataP1_Hours < 0)
                    {
                        FishFeed_TimeDataP1_Hours = 23;
                    }
                    break;
                case 2:
                    FishFeed_TimeDataP1_Minutes--;
                    if (FishFeed_TimeDataP1_Minutes < 0)
                    {
                        FishFeed_TimeDataP1_Minutes = 59;
                    }
                    break;
                case 3:
                    FishFeed_TimeDataP1_Seconds--;
                    if (FishFeed_TimeDataP1_Seconds < 0)
                    {
                        FishFeed_TimeDataP1_Seconds = 59;
                    }
                    break;
                case 4:
                    FishFeed_TimeDataP2_Hours--;
                    if (FishFeed_TimeDataP2_Hours < 0)
                    {
                        FishFeed_TimeDataP2_Hours = 23;
                    }
                    break;
                case 5:
                    FishFeed_TimeDataP2_Minutes--;
                    if (FishFeed_TimeDataP2_Minutes < 0)
                    {
                        FishFeed_TimeDataP2_Minutes = 59;
                    }
                    break;
                case 6:
                    FishFeed_TimeDataP2_Seconds--;
                    if (FishFeed_TimeDataP2_Seconds < 0)
                    {
                        FishFeed_TimeDataP2_Seconds = 59;
                    }
                    break;

                default:
                    break;
                }

                break;
            case 3:
                // 定时自动投食3times模式时间越界++判断
                switch (Windows3_EditPosition)
                {
                case 0:
                    FishFeedTimes_Data--;
                    if (FishFeedTimes_Data < 0)
                    {
                        FishFeedTimes_Data = 3;
                    }
                    TFT_eSPIClass1.fillScreen(TFT_BLACK);
                    TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                    break;
                case 1:
                    FishFeed_TimeDataP1_Hours--;
                    if (FishFeed_TimeDataP1_Hours < 0)
                    {
                        FishFeed_TimeDataP1_Hours = 23;
                    }
                    break;
                case 2:
                    FishFeed_TimeDataP1_Minutes--;
                    if (FishFeed_TimeDataP1_Minutes < 0)
                    {
                        FishFeed_TimeDataP1_Minutes = 59;
                    }
                    break;
                case 3:
                    FishFeed_TimeDataP1_Seconds--;
                    if (FishFeed_TimeDataP1_Seconds < 0)
                    {
                        FishFeed_TimeDataP1_Seconds = 59;
                    }
                    break;
                case 4:
                    FishFeed_TimeDataP2_Hours--;
                    if (FishFeed_TimeDataP2_Hours < 0)
                    {
                        FishFeed_TimeDataP2_Hours = 23;
                    }
                    break;
                case 5:
                    FishFeed_TimeDataP2_Minutes--;
                    if (FishFeed_TimeDataP2_Minutes < 0)
                    {
                        FishFeed_TimeDataP2_Minutes = 59;
                    }
                    break;
                case 6:
                    FishFeed_TimeDataP2_Seconds--;
                    if (FishFeed_TimeDataP2_Seconds < 0)
                    {
                        FishFeed_TimeDataP2_Seconds = 59;
                    }
                    break;
                case 7:
                    FishFeed_TimeDataP3_Hours--;
                    if (FishFeed_TimeDataP3_Hours < 0)
                    {
                        FishFeed_TimeDataP3_Hours = 23;
                    }
                    break;
                case 8:
                    FishFeed_TimeDataP3_Minutes--;
                    if (FishFeed_TimeDataP3_Minutes < 0)
                    {
                        FishFeed_TimeDataP3_Minutes = 59;
                    }
                    break;
                case 9:
                    FishFeed_TimeDataP3_Seconds--;
                    if (FishFeed_TimeDataP3_Seconds < 0)
                    {
                        FishFeed_TimeDataP3_Seconds = 59;
                    }
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
            }
        }

        //窗口4电机数据修改--
        if (Windows == 4 && Windows_EditMode == 1)
        {
            switch (Windows4_EditPosition)
            {
            case 0:
                FIL_Mood_Flag--;
                if (FIL_Mood_Flag < 0)
                {
                    FIL_Mood_Flag = 1;
                }

                // 窗口4刷新显示
                TFT_eSPIClass1.fillScreen(TFT_BLACK);
                TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                break;
            case 1:
                FIL_Speed_Data--;
                if (FIL_Speed_Data < 0)
                {
                    FIL_Speed_Data = 10;
                }
                break;
            case 2:
                OXY_Mood_Flag--;
                if (OXY_Mood_Flag < 0)
                {
                    OXY_Mood_Flag = 1;
                }

                // 窗口4刷新显示
                TFT_eSPIClass1.fillScreen(TFT_BLACK);
                TFT_eSPIClass1.drawRect(0, 0, 160, 80, TFT_BROWN);
                break;
            case 3:
                OXY_Speed_Data--;
                if (OXY_Speed_Data < 0)
                {
                    OXY_Speed_Data = 50;
                }
                break;

            default:
                break;
            }
        }

        //窗口5数据修改--
        if (Windows == 5 && Windows_EditMode == 1)
        {
            switch (Windows5_EditPosition)
            {
            case 0:
                FishFeed_AutoQuantityData--;
                if (FishFeed_AutoQuantityData < 1)
                {
                    FishFeed_AutoQuantityData = 3;
                }
                break;
            case 1:
                FishFeed_OtherMotorControl_WaitingTime--;
                if (FishFeed_OtherMotorControl_WaitingTime < 0)
                {
                    FishFeed_OtherMotorControl_WaitingTime = 99;
                }
                break;
            case 2:
                Light_ModeData--;
                if (Light_ModeData < 0)
                {
                    Light_ModeData = 3;
                }
                break;
            case 3:
                Light_BrightnessData--;
                if (Light_BrightnessData < 0)
                {
                    Light_BrightnessData = 99;
                }
                break;
            default:
                break;
            }
        }

        delay(200); //消抖
        IPS_Switch_Flag = 0;

        break;
    case IPS_Switch_Left:

        //主窗口修改框位置左移动
        if (Windows == 0 && Windows_EditMode == 1)
        {
            Windows0_EditPosition--;
            if (Windows0_EditPosition < 0) //主窗口RTC时钟修改模式位置越界重置
            {
                Windows0_EditPosition = 3;
            }
        }

        //窗口1修改框位置左移动
        if (Windows == 1 && Windows_EditMode == 1)
        {
            Windows1_EditPosition--;
            if (Windows1_EditPosition < 0)
            {
                Windows1_EditPosition = 2;
            }
        }

        //窗口2修改框位置左移动
        if (Windows == 2 && Windows_EditMode == 1)
        {
            Windows2_EditPosition--;
            if (Windows2_EditPosition < 0)
            {
                Windows2_EditPosition = 2;
            }
        }

        //窗口3修改框位置左移动
        if (Windows == 3 && Windows_EditMode == 1)
        {
            Windows3_EditPosition--;

            switch (FishFeedTimes_Data)
            {
            case 0:
                if (Windows3_EditPosition < 0) //窗口3修改模式位置越界重置
                {
                    Windows3_EditPosition = 3;
                }
                break;
            case 1:
                if (Windows3_EditPosition < 0) //窗口3修改模式位置越界重置
                {
                    Windows3_EditPosition = 3;
                }
                break;
            case 2:
                if (Windows3_EditPosition < 0) //窗口3修改模式位置越界重置
                {
                    Windows3_EditPosition = 6;
                }
                break;
            case 3:
                if (Windows3_EditPosition < 0) //窗口3修改模式位置越界重置
                {
                    Windows3_EditPosition = 9;
                }
                break;

            default:
                break;
            }
        }

        //窗口4修改框位置上移动
        if (Windows == 4 && Windows_EditMode == 1)
        {
            Windows4_EditPosition--;

            switch (FIL_Mood_Flag)
            {
            case 0:

                switch (OXY_Mood_Flag)
                {
                case 0:

                    switch (Windows4_EditPosition)
                    {
                    case -1:
                        Windows4_EditPosition = 2;
                        break;
                    case 0:
                        break;
                    case 1:
                        Windows4_EditPosition--;
                        break;
                    case 2:
                        break;

                    default:
                        break;
                    }

                    break;

                case 1:

                    switch (Windows4_EditPosition)
                    {
                    case -1:
                        Windows4_EditPosition = 3;
                        break;
                    case 0:
                        break;
                    case 1:
                        Windows4_EditPosition--;
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    default:
                        break;
                    }

                    break;

                default:
                    break;
                }

                break;
            case 1:

                switch (OXY_Mood_Flag)
                {
                case 0:
                    if (Windows4_EditPosition < 0) //窗口4修改模式位置越界重置
                    {
                        Windows4_EditPosition = 2;
                    }
                    break;

                case 1:
                    if (Windows4_EditPosition < 0) //窗口4修改模式位置越界重置
                    {
                        Windows4_EditPosition = 3;
                    }
                    break;

                default:
                    break;
                }

                break;

            default:
                break;
            }
        }

        //窗口5修改框位置上移动
        if (Windows == 5 && Windows_EditMode == 1)
        {
            Windows5_EditPosition--;
            if (Windows5_EditPosition < 0)
            {
                Windows5_EditPosition = 3;
            }
        }

        delay(200); //消抖
        IPS_Switch_Flag = 0;

        break;
    case IPS_Switch_Right:

        //主窗口修改框位置右移动
        if (Windows == 0 && Windows_EditMode == 1)
        {
            Windows0_EditPosition++;
            if (Windows0_EditPosition > 3) //主窗口RTC时钟修改模式位置越界重置
            {
                Windows0_EditPosition = 0;
            }
        }

        //窗口1修改框位置右移动
        if (Windows == 1 && Windows_EditMode == 1)
        {
            Windows1_EditPosition++;
            if (Windows1_EditPosition > 2)
            {
                Windows1_EditPosition = 0;
            }
        }

        //窗口2修改框位置右移动
        if (Windows == 2 && Windows_EditMode == 1)
        {
            Windows2_EditPosition++;
            if (Windows2_EditPosition > 2)
            {
                Windows2_EditPosition = 0;
            }
        }

        //窗口3修改框位置右移动
        if (Windows == 3 && Windows_EditMode == 1)
        {
            Windows3_EditPosition++;

            switch (FishFeedTimes_Data)
            {
            case 0:
                if (Windows3_EditPosition > 3) //窗口3修改模式位置越界重置
                {
                    Windows3_EditPosition = 0;
                }
                break;
            case 1:
                if (Windows3_EditPosition > 3) //窗口3修改模式位置越界重置
                {
                    Windows3_EditPosition = 0;
                }
                break;
            case 2:
                if (Windows3_EditPosition > 6) //窗口3修改模式位置越界重置
                {
                    Windows3_EditPosition = 0;
                }
                break;
            case 3:
                if (Windows3_EditPosition > 9) //窗口3修改模式位置越界重置
                {
                    Windows3_EditPosition = 0;
                }
                break;

            default:
                break;
            }
        }

        //窗口4修改框位置下移动
        if (Windows == 4 && Windows_EditMode == 1)
        {
            Windows4_EditPosition++;

            switch (FIL_Mood_Flag)
            {
            case 0:

                switch (OXY_Mood_Flag)
                {
                case 0:

                    switch (Windows4_EditPosition)
                    {
                    case 0:
                        break;

                    case 1:
                        Windows4_EditPosition++;
                        break;
                    case 2:
                        break;
                    case 3:
                        Windows4_EditPosition = 0;
                        break;

                    default:
                        break;
                    }

                    break;
                case 1:

                    switch (Windows4_EditPosition)
                    {
                    case 0:
                        break;

                    case 1:
                        Windows4_EditPosition++;
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    case 4:
                        Windows4_EditPosition = 0;
                        break;
                    default:
                        break;
                    }

                    break;

                default:
                    break;
                }

                break;
            case 1:

                switch (OXY_Mood_Flag)
                {
                case 0:
                    if (Windows4_EditPosition > 2) //窗口4修改模式位置越界重置
                    {
                        Windows4_EditPosition = 0;
                    }
                    break;
                case 1:
                    if (Windows4_EditPosition > 3) //窗口4修改模式位置越界重置
                    {
                        Windows4_EditPosition = 0;
                    }
                    break;

                default:
                    break;
                }

                break;

            default:
                break;
            }
        }

        //窗口5修改框位置下移动
        if (Windows == 5 && Windows_EditMode == 1)
        {
            Windows5_EditPosition++;
            if (Windows5_EditPosition > 3)
            {
                Windows5_EditPosition = 0;
            }
        }

        delay(200); //消抖
        IPS_Switch_Flag = 0;

        break;
    case IPS_Switch_Centre:

        //窗口修改模式切换
        Windows_EditMode = !Windows_EditMode;

        // 主窗口修改数据结束后发送修改后的RTC时间数据
        if (Windows == 0 && Windows_EditMode == 0)
        {
            if (Windows0_EditPosition != 3)
            {
                // 寄存修改后的数据
                RTC_SendData[0] = 'R';
                RTC_SendData[1] = 'M';
                RTC_SendData[2] = RTC_Data_Hours_d;
                RTC_SendData[3] = RTC_Data_Minutes_d;
                RTC_SendData[4] = RTC_Data_Seconds_d;

                Serial.write(RTC_SendData, 5);
            }
            else
            {
                //清除残留
                TFT_eSPIClass1.drawRect(113, 28, 11, 13, TFT_BLACK);
            }
        }

        // 窗口1修改数据结束后
        if (Windows == 1 && Windows_EditMode == 0)
        {
            //清除修改边框
            TFT_eSPIClass1.drawRect(117, 5, 38, 20, TFT_BLACK);
            TFT_eSPIClass1.drawRect(67, 31, 21, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(110, 31, 21, 18, TFT_BLACK);
        }

        // 窗口2修改数据结束后
        if (Windows == 2 && Windows_EditMode == 0)
        {
            //清除修改边框
            TFT_eSPIClass1.drawRect(117, 5, 38, 20, TFT_BLACK);
            TFT_eSPIClass1.drawRect(52, 31, 37, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(110, 31, 37, 18, TFT_BLACK);
        }

        // 窗口3修改数据结束后
        if (Windows == 3 && Windows_EditMode == 0)
        {

            // 窗口3的0times如果选中手动控制投食,则选择发送要控制电机旋转多少角度的数据
            if (FishFeedTimes_Data == 0)
            {
                switch (Windows3_EditPosition)
                {
                case 0:
                    break;
                case 1: //选中Run a little投食器运行一点点
                    FishFeed_SendCommand(FishFeed_RunALittleCommand);
                    FishFeed_CompleteTimes++; //投食次数显示
                    break;
                case 2: //选中Run a half投食器运行半圈
                    FishFeed_SendCommand(FishFeed_RunAHalfCommand);
                    FishFeed_CompleteTimes++; //投食次数显示
                    break;
                case 3: //选中Run a lab投食器运行一圈
                    FishFeed_SendCommand(FishFeed_RunALabCommand);
                    FishFeed_CompleteTimes++; //投食次数显示
                    break;

                default:
                    break;
                }
            }

            // 窗口3的投食次数标志数据使能,一个计划每天一次的定时标志位和1-3times定时投食数据储存
            if (FishFeedTimes_Data != 0)
            {
                // 投食次数标志数据使能
                FishFeedTimes_DataEnable_Flag = FishFeedTimes_Data;

                // 一个计划每天一次的定时标志位重置
                FishFeed_EndOfDayP1_Flag = 0;
                FishFeed_EndOfDayP2_Flag = 0;
                FishFeed_EndOfDayP3_Flag = 0;

                // 1-3times定时投食数据储存
                FishFeed_TimeDataP1_Clock[0] = FishFeed_TimeDataP1_Hours;
                FishFeed_TimeDataP1_Clock[1] = FishFeed_TimeDataP1_Minutes;
                FishFeed_TimeDataP1_Clock[2] = FishFeed_TimeDataP1_Seconds;

                FishFeed_TimeDataP2_Clock[0] = FishFeed_TimeDataP2_Hours;
                FishFeed_TimeDataP2_Clock[1] = FishFeed_TimeDataP2_Minutes;
                FishFeed_TimeDataP2_Clock[2] = FishFeed_TimeDataP2_Seconds;

                FishFeed_TimeDataP3_Clock[0] = FishFeed_TimeDataP3_Hours;
                FishFeed_TimeDataP3_Clock[1] = FishFeed_TimeDataP3_Minutes;
                FishFeed_TimeDataP3_Clock[2] = FishFeed_TimeDataP3_Seconds;
            }

            // 窗口3清除修改边框残留
            switch (FishFeedTimes_Data)
            {
            case 0:

                TFT_eSPIClass1.drawRect(41, 57, 77, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(39, 21, 81, 19, TFT_BLACK);
                TFT_eSPIClass1.drawRect(41, 38, 77, 19, TFT_BLACK);
                break;
            case 1:
                TFT_eSPIClass1.drawRect(126, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 24, 21, 18, TFT_BLACK);

                break;
            case 2:
                TFT_eSPIClass1.drawRect(126, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(126, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 41, 21, 18, TFT_BLACK);
                break;
            case 3:
                TFT_eSPIClass1.drawRect(126, 58, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(103, 2, 11, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(126, 24, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(126, 41, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(72, 58, 21, 18, TFT_BLACK);
                TFT_eSPIClass1.drawRect(99, 58, 21, 18, TFT_BLACK);

                break;

            default:
                break;
            }
        }

        // 窗口4修改数据结束后,修改手动模式电机速度
        if (Windows == 4 && Windows_EditMode == 0)
        {
            // 窗口4清除修改边框残留
            TFT_eSPIClass1.drawRect(98, 57, 29, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(98, 2, 29, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(98, 20, 29, 19, TFT_BLACK);
            TFT_eSPIClass1.drawRect(98, 39, 29, 18, TFT_BLACK);

            switch (FIL_Mood_Flag)
            {
            case 0:
                Windows4_FILAutoMoodEnable_Flag = 1;
                break;
            case 1:
                Windows4_FILAutoMoodEnable_Flag = 0;
                if (FIL_Speed_Data == 0) //过滤器只有在90-100%的PWM才会启动
                {
                    TB6612_FILClass1.PWM_Set(TB6612_FilterChannel, TB6612_OFF, TB6612_Accuracy10, 0);
                }
                else
                {
                    TB6612_FILClass1.PWM_Set(TB6612_FilterChannel, TB6612_ON, TB6612_Accuracy10, 90 + FIL_Speed_Data);
                }
                break;

            default:
                break;
            }

            switch (OXY_Mood_Flag)
            {
            case 0:
                Windows4_OXYAutoMoodEnable_Flag = 1;
                break;
            case 1:
                Windows4_OXYAutoMoodEnable_Flag = 0;
                if (OXY_Speed_Data == 0) //供氧器只有在50-100%的PWM才会启动
                {
                    TB6612_OXYClass1.PWM_Set(TB6612_OxygenChannel, TB6612_OFF, TB6612_Accuracy10, 0);
                }
                else
                {
                    TB6612_OXYClass1.PWM_Set(TB6612_OxygenChannel, TB6612_ON, TB6612_Accuracy10, 50 + OXY_Speed_Data);
                }
                break;

            default:
                break;
            }
        }

        // 窗口5修改数据结束后
        if (Windows == 5 && Windows_EditMode == 0)
        {
            //清除修改边框
            TFT_eSPIClass1.drawRect(128, 2, 19, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(128, 21, 19, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(66, 40, 19, 18, TFT_BLACK);
            TFT_eSPIClass1.drawRect(103, 40, 19, 18, TFT_BLACK);

            Light_SendData[0] = 'L';
            Light_SendData[1] = 'M';
            Light_SendData[2] = Light_ModeData;
            Light_SendData[3] = 'B';
            Light_SendData[4] = Light_BrightnessData;

            Serial.write(Light_SendData, 5);
        }

        delay(200); //消抖
        IPS_Switch_Flag = 0;

        break;

    default:
        break;
    }
    /*IPS_Switch-----------------------------------------------------------------------------------------------*/

    /*FishFeed-----------------------------------------------------------------------------------------------*/
    // 手动投食程序写在IPS_Switch里头

    if (FishFeed_OtherMotorControl_WaitingTime == 0) //如果设置等待时间为0,那么就关闭投食控制其他电机关闭模式,即投食不等待
    {
        FishFeed_OtherMotorControlDuringFeeding_Flag = 0;
    }
    else
    {
        FishFeed_OtherMotorControlDuringFeeding_Flag = 1;
    }

    // 自动定时投食响应区(如果FishFeedTimes_DataEnable_Flag>0且<=3就使能定时投食)
    //  使用FishFeed_ExecutionPlan_Flag = 1;作为任务响应避免了同一定时时间多个相同任务的重复执行
    // 主函数循环扫描以下switch (FishFeedTimes_DataEnable_Flag)程序
    switch (FishFeedTimes_DataEnable_Flag)
    {
    case 0:
        break;

    case 1:
        if (FishFeed_EndOfDayP1_Flag == 0)
        {
            if (FishFeed_TimeDataP1_Clock[0] == RTC_Data_Hours_d && FishFeed_TimeDataP1_Clock[1] == RTC_Data_Minutes_d && FishFeed_TimeDataP1_Clock[2] == RTC_Data_Seconds_d)
            {
                FishFeed_ExecutionPlan_Flag = 1; //执行标志
                FishFeed_EndOfDayP1_Flag = 1;    //表示计划定时1已经投食完成,今天起不再触发计划定时1投食,在第二天00:00:00后或者重新设置定时时间重置
            }
        }

        break;
    case 2:
        if (FishFeed_EndOfDayP1_Flag == 0)
        {
            if (FishFeed_TimeDataP1_Clock[0] == RTC_Data_Hours_d && FishFeed_TimeDataP1_Clock[1] == RTC_Data_Minutes_d && FishFeed_TimeDataP1_Clock[2] == RTC_Data_Seconds_d)
            {
                FishFeed_ExecutionPlan_Flag = 1; //执行标志
                FishFeed_EndOfDayP1_Flag = 1;    //表示计划定时1已经投食完成,今天起不再触发计划定时1投食,在第二天00:00:00后或者重新设置定时时间重置
            }
        }

        if (FishFeed_EndOfDayP2_Flag == 0)
        {
            if (FishFeed_TimeDataP2_Clock[0] == RTC_Data_Hours_d && FishFeed_TimeDataP2_Clock[1] == RTC_Data_Minutes_d && FishFeed_TimeDataP2_Clock[2] == RTC_Data_Seconds_d)
            {
                FishFeed_ExecutionPlan_Flag = 1; //执行标志
                FishFeed_EndOfDayP2_Flag = 1;
            }
        }
        break;
    case 3:
        if (FishFeed_EndOfDayP1_Flag == 0)
        {
            if (FishFeed_TimeDataP1_Clock[0] == RTC_Data_Hours_d && FishFeed_TimeDataP1_Clock[1] == RTC_Data_Minutes_d && FishFeed_TimeDataP1_Clock[2] == RTC_Data_Seconds_d)
            {
                FishFeed_ExecutionPlan_Flag = 1; //执行标志
                FishFeed_EndOfDayP1_Flag = 1;    //表示计划定时1已经投食完成,今天起不再触发计划定时1投食,在第二天00:00:00后或者重新设置定时时间重置
            }
        }

        if (FishFeed_EndOfDayP2_Flag == 0)
        {
            if (FishFeed_TimeDataP2_Clock[0] == RTC_Data_Hours_d && FishFeed_TimeDataP2_Clock[1] == RTC_Data_Minutes_d && FishFeed_TimeDataP2_Clock[2] == RTC_Data_Seconds_d)
            {
                FishFeed_ExecutionPlan_Flag = 1; //执行标志
                FishFeed_EndOfDayP2_Flag = 1;
            }
        }

        if (FishFeed_EndOfDayP3_Flag == 0)
        {
            if (FishFeed_TimeDataP3_Clock[0] == RTC_Data_Hours_d && FishFeed_TimeDataP3_Clock[1] == RTC_Data_Minutes_d && FishFeed_TimeDataP3_Clock[2] == RTC_Data_Seconds_d)
            {
                FishFeed_ExecutionPlan_Flag = 1; //执行标志
                FishFeed_EndOfDayP3_Flag = 1;
            }
        }
        break;

    default:
        break;
    }

    // 自动定时投食执行区(是否开启投食期间关闭其他电机模式)
    if (FishFeed_ExecutionPlan_Flag == 1) //触发投食器
    {
        // 记录原始状态程序
        switch (FIL_Mood_Flag) //记录过滤电机原始状态
        {
        case 0:
            FishFeed_OtherMotorControl_OriginalMotorModeData[0] = 0;
            break;
        case 1: //手动模式一并记录过滤电机原始速度
            FishFeed_OtherMotorControl_OriginalMotorModeData[0] = 1;
            FishFeed_OtherMotorControl_OriginalMotorSpeed[0] = FIL_Speed_Data;
            break;

        default:
            break;
        }
        switch (OXY_Mood_Flag) //记录供氧电机原始状态
        {
        case 0:
            FishFeed_OtherMotorControl_OriginalMotorModeData[1] = 0;
            break;
        case 1: //手动模式一并记录供氧电机原始速度
            FishFeed_OtherMotorControl_OriginalMotorModeData[1] = 1;
            FishFeed_OtherMotorControl_OriginalMotorSpeed[1] = OXY_Speed_Data;
            break;

        default:
            break;
        }

        //是否开启投食电机控制(0关闭,1开启)
        switch (FishFeed_OtherMotorControlDuringFeeding_Flag)
        {
        case 0: //关闭投食控制其他电机模式
            FishFeed_ExecutionPlan_Flag = 0;
            FishFeed_OtherMotorControlEnable_Flag = 0; //失能自动定时投食关闭其他电机模式

            FishFeed_SendCommand(FishFeed_AutoQuantityData);
            FishFeed_CompleteTimes++; //投食次数显示
            // Serial.println("ts"); // 投食
            break;
        case 1: //开启投食控制其他电机模式
            FishFeed_ExecutionPlan_Flag = 0;

            // 其他电机停止
            FIL_Mood_Flag = 1; //开启投食控制其他电机模式第一步必需把其他电机的模式转为手动模式
            OXY_Mood_Flag = 1;
            FIL_Speed_Data = 0;
            OXY_Speed_Data = 0;
            Windows4_FILAutoMoodEnable_Flag = 0;
            Windows4_OXYAutoMoodEnable_Flag = 0;
            // 其他电机停止
            TB6612_FILClass1.PWM_Set(TB6612_FilterChannel, TB6612_OFF, TB6612_Accuracy10, 0);
            TB6612_OXYClass1.PWM_Set(TB6612_OxygenChannel, TB6612_OFF, TB6612_Accuracy10, 0);

            // 投食开始时间点
            FishFeed_OtherMotorControl_Time1Data[0] = RTC_Data_Hours_d;
            FishFeed_OtherMotorControl_Time1Data[1] = RTC_Data_Minutes_d + 5; //测试一分钟
            FishFeed_OtherMotorControl_Time1Data[2] = RTC_Data_Seconds_d;

            // 其他电机重新开启时间点
            FishFeed_OtherMotorControl_Time2Data[0] = RTC_Data_Hours_d;
            FishFeed_OtherMotorControl_Time2Data[1] = RTC_Data_Minutes_d + FishFeed_OtherMotorControl_WaitingTime + 5; //测试一分钟
            FishFeed_OtherMotorControl_Time2Data[2] = RTC_Data_Seconds_d;

            FishFeed_OtherMotorControlEnable_Flag = 1; //使能自动定时投食关闭其他电机模式
            FishFeed_OtherMotorControl_Time1Flag = 0;  //标志位重置
            FishFeed_OtherMotorControl_Time2Flag = 0;

            break;

        default:
            break;
        }
    }

    // 自动定时投食关闭其他电机模式执行区(如果开启了投食期间关闭其他电机模式就使能以下代码)
    if (FishFeed_OtherMotorControlEnable_Flag == 1)
    {
        //等待时间点1触发
        if (FishFeed_OtherMotorControl_Time1Data[0] == RTC_Data_Hours_d && FishFeed_OtherMotorControl_Time1Data[1] == RTC_Data_Minutes_d && FishFeed_OtherMotorControl_Time1Data[2] == RTC_Data_Seconds_d)
        {
            if (FishFeed_OtherMotorControl_Time1Flag == 0) //只执行一次
            {
                FishFeed_SendCommand(FishFeed_AutoQuantityData); //投食器执行区
                FishFeed_CompleteTimes++;                        //投食次数显示
                // Serial.println("ts,time1");
                FishFeed_OtherMotorControl_Time1Flag = 1;
            }
        }

        //等待时间点2触发
        if (FishFeed_OtherMotorControl_Time2Data[0] == RTC_Data_Hours_d && FishFeed_OtherMotorControl_Time2Data[1] == RTC_Data_Minutes_d && FishFeed_OtherMotorControl_Time2Data[2] == RTC_Data_Seconds_d)
        {
            FishFeed_OtherMotorControlEnable_Flag = 0; //在第二次时间点触发后失能此次循环判断并还原电机原来的工作状态和速度

            if (FishFeed_OtherMotorControl_Time2Flag == 0) //只执行一次
            {
                switch (FishFeed_OtherMotorControl_OriginalMotorModeData[0]) //还原原来的工作模式和速度
                {
                case 0:
                    FIL_Mood_Flag = 0;
                    Windows4_FILAutoMoodEnable_Flag = 1;
                    break;
                case 1:
                    FIL_Mood_Flag = 1;
                    FIL_Speed_Data = FishFeed_OtherMotorControl_OriginalMotorSpeed[0];
                    break;

                default:
                    break;
                }
                switch (FishFeed_OtherMotorControl_OriginalMotorModeData[1]) //还原原来的工作模式和速度
                {
                case 0:
                    OXY_Mood_Flag = 0;
                    Windows4_OXYAutoMoodEnable_Flag = 1;
                    break;
                case 1:
                    OXY_Mood_Flag = 1;
                    OXY_Speed_Data = FishFeed_OtherMotorControl_OriginalMotorSpeed[1];
                    break;

                default:
                    break;
                }

                // 其他电机执行区
                if (FIL_Speed_Data == 0)
                {
                    TB6612_FILClass1.PWM_Set(TB6612_FilterChannel, TB6612_OFF, TB6612_Accuracy10, 0);
                }
                else
                {
                    TB6612_FILClass1.PWM_Set(TB6612_FilterChannel, TB6612_ON, TB6612_Accuracy10, 90 + FIL_Speed_Data);
                }
                if (OXY_Speed_Data == 0)
                {
                    TB6612_OXYClass1.PWM_Set(TB6612_OxygenChannel, TB6612_OFF, TB6612_Accuracy10, 0);
                }
                else
                {
                    TB6612_OXYClass1.PWM_Set(TB6612_OxygenChannel, TB6612_ON, TB6612_Accuracy10, 50 + OXY_Speed_Data);
                }
                // Serial.println("ts,time2");

                FishFeed_OtherMotorControl_Time2Flag = 1;
            }
        }
    }

    // 每天的00:00:00清除一天一次定时投食标志
    if (RTC_Data_Hours_d == 0 && RTC_Data_Minutes_d == 0 && RTC_Data_Seconds_d == 0)
    {
        // 一个计划每天一次的定时标志位重置
        FishFeed_EndOfDayP1_Flag = 0;
        FishFeed_EndOfDayP2_Flag = 0;
        FishFeed_EndOfDayP3_Flag = 0;

        FishFeed_CompleteTimes = 0; //投食次数清零
    }

    /*FishFeed-----------------------------------------------------------------------------------------------*/

    /*Motor_FIL&OXY-----------------------------------------------------------------------------------------------*/
    // 手动控制程序写在IPS_Switch里头(因为手动控制的数据只要设置一次就行所以直接写在IPS_Switch里头)

    // 自动控制模式(因为自动控制的数据要实时修改所以在IPS_Switch里头使能,外部刷新数据)
    if (Windows4_FILAutoMoodEnable_Flag == 1)
    {
        if (TDS_Data_d <= 10 || TDS_Data_d >= 1500)
        {
            FIL_Speed_Data = 0;
            TB6612_FILClass1.PWM_Set(TB6612_FilterChannel, TB6612_OFF, TB6612_Accuracy10, 0);
        }
        else if (TDS_Data_d <= 100 && TDS_Data_d > 10)
        {
            FIL_Speed_Data = 3;
            TB6612_FILClass1.PWM_Set(TB6612_FilterChannel, TB6612_ON, TB6612_Accuracy10, 90 + FIL_Speed_Data);
        }
        else if (TDS_Data_d <= 200 && TDS_Data_d > 100)
        {
            FIL_Speed_Data = 6;
            TB6612_FILClass1.PWM_Set(TB6612_FilterChannel, TB6612_ON, TB6612_Accuracy10, 90 + FIL_Speed_Data);
        }
        else if (TDS_Data_d > 200)
        {
            FIL_Speed_Data = 10;
            TB6612_FILClass1.PWM_Set(TB6612_FilterChannel, TB6612_ON, TB6612_Accuracy10, 90 + FIL_Speed_Data);
        }
    }

    if (Windows4_OXYAutoMoodEnable_Flag == 1)
    {
        if (TDS_Data_d <= 10 || TDS_Data_d >= 1500)
        {
            OXY_Speed_Data = 0;
            TB6612_OXYClass1.PWM_Set(TB6612_OxygenChannel, TB6612_OFF, TB6612_Accuracy10, 0);
        }
        else if (TDS_Data_d <= 100 && TDS_Data_d > 10)
        {
            OXY_Speed_Data = 10;
            TB6612_OXYClass1.PWM_Set(TB6612_OxygenChannel, TB6612_ON, TB6612_Accuracy10, 50 + OXY_Speed_Data);
        }
        else if (TDS_Data_d <= 200 && TDS_Data_d > 100)
        {
            OXY_Speed_Data = 20;
            TB6612_OXYClass1.PWM_Set(TB6612_OxygenChannel, TB6612_ON, TB6612_Accuracy10, 50 + OXY_Speed_Data);
        }
        else if (TDS_Data_d > 200)
        {
            OXY_Speed_Data = 30;
            TB6612_OXYClass1.PWM_Set(TB6612_OxygenChannel, TB6612_ON, TB6612_Accuracy10, 50 + OXY_Speed_Data);
        }
    }

    /*Motor_FIL&OXY-----------------------------------------------------------------------------------------------*/
}
