/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2022-10-27 15:29:05
 * @LastEditors: None
 * @LastEditTime: 2022-11-30 10:25:00
 */
#include <OneWire.h>
#include <setjmp.h>
#include "DS18B20.h"

OneWire ds(12); // on pin 12
float DS18B20::celsius = 0;
float DS18B20::fahrenheit = 0;

/**
 * @brief DS18B20读取温度函数
 * @return
 * @Date 2022-11-28 00:47:01
 */
void DS18B20::DS18B20_ReadT()
{
    ds.reset_search();
    if (!ds.search(addr))
    {
        // Serial.println("No more addresses.");
        ds.reset_search();
        // delay(250);
        return;
    }

    // Serial.print("ROM =");
    // for (unsigned char i = 0; i < 8; i++)
    // {
    //     Serial.write(' ');
    //     Serial.print(addr[i], HEX);
    // }

    if (OneWire::crc8(addr, 7) != addr[7])
    {
        // Serial.println("CRC is not valid!");
        return;
    }

    // 第一个ROM数据代表DS18B20的设备地址
    switch (addr[0])
    {
    case 0x10:
        // Serial.println("  Chip = DS18S20"); // 老式DS18B20
        type_s = 1;
        break;
    case 0x28: //本机设备地址
        // Serial.println("  Chip = DS18B20");
        type_s = 0;
        break;
    case 0x22:
        // Serial.println("  Chip = DS1822");
        type_s = 0;
        break;
    default:
        // Serial.println("Device is not a DS18x20 family device.");
        return;
    }

    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1);

    // delay(1000); // >750ms延时选择12位精度

    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE); // 读暂存器指令

    // Serial.print("  Data = ");
    // Serial.print(present, HEX);
    // Serial.print(" ");
    for (unsigned char i = 0; i < 9; i++)
    { // we need 9 bytes
        data[i] = ds.read();
        // Serial.print(data[i], HEX);
        // Serial.print(" ");
    }
    // Serial.print(" CRC=");
    // Serial.print(OneWire::crc8(data, 8), HEX);
    // Serial.println();

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s) //类型为老式DS18B20时
    {
        raw = raw << 3; // 9 bit resolution default
        if (data[7] == 0x10)
        {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
    }
    else
    {
        byte cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00)
            raw = raw & ~7; // 9 bit resolution, 93.75 ms
        else if (cfg == 0x20)
            raw = raw & ~3; // 10 bit res, 187.5 ms
        else if (cfg == 0x40)
            raw = raw & ~1; // 11 bit res, 375 ms
                            //// default is 12 bit resolution, 750 ms conversion time
    }
    celsius = (float)raw / 16.0;
    // fahrenheit = celsius * 1.8 + 32.0;
    // Serial.print("Temperature=");
    // Serial.println(celsius);
    //     Serial.print(" Celsius, ");
    //     Serial.print(fahrenheit);
    //     Serial.println(" Fahrenheit");
}
