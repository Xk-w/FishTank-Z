/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2022-11-09 16:51:15
 * @LastEditors: None
 * @LastEditTime: 2022-11-30 12:41:06
 */
#include <Arduino.h>
#include "ADC.h"

extern float DS18B20_Data_f;

void ADC::ADC_Init(unsigned char Resolution, unsigned char pin)
{
    pinMode(pin, INPUT_PULLDOWN);

    analogReadResolution(Resolution);
    analogSetPinAttenuation(pin, ADC_ATTENDB_MAX);
    adcAttachPin(pin);
}

unsigned short ADC::ADC_Read(unsigned char pin)
{
    return analogRead(pin);
}

unsigned int ADC::ADC_ReadMilliVolts(unsigned char pin)
{
    return analogReadMilliVolts(pin);
}

float ADC::ADC_ReadPH(unsigned char pin)
{
    float Voltage = (float)analogReadMilliVolts(pin) / 1000;
    float PHValue = (-0.853f * (Voltage * Voltage) - 7.7294f * Voltage + 26.29f);

    return PHValue;
}

float ADC::ADC_ReadTDS(unsigned char pin)
{
    float Voltage = (float)analogReadMilliVolts(pin) / 1000;
    Voltage = Voltage * (1 + 0.02 * (DS18B20_Data_f - 26)); //温度修正

    float TDSValue = (-7624.0f * (Voltage * Voltage * Voltage * Voltage * Voltage * Voltage) + 31005.0f * (Voltage * Voltage * Voltage * Voltage * Voltage) - 42523.0f * (Voltage * Voltage * Voltage * Voltage) + 25929.0f * (Voltage * Voltage * Voltage) - 6788.8f * (Voltage * Voltage) + 1071.0f * Voltage + 8.6592f);

    return TDSValue;
}
