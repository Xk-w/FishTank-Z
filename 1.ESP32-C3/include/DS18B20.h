/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2022-10-27 15:29:17
 * @LastEditors: None
 * @LastEditTime: 2022-10-29 12:31:34
 */
#pragma

class DS18B20
{
public:
    void DS18B20_ReadT();
    static float celsius;
    static float fahrenheit;

private:
    unsigned char present = 0;
    unsigned char type_s;
    unsigned char data[9];
    unsigned char addr[8];
};
