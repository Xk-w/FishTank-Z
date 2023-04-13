/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2022-11-09 16:37:32
 * @LastEditors: None
 * @LastEditTime: 2022-11-30 10:42:54
 */
#pragma

#define PH_Pin GPIO_NUM_0
#define TDS_Pin GPIO_NUM_1

class ADC
{
public:
    void ADC_Init(unsigned char Resolution, unsigned char pin);
    unsigned short ADC_Read(unsigned char pin);
    unsigned int ADC_ReadMilliVolts(unsigned char pin);
    float ADC_ReadPH(unsigned char pin);
    float ADC_ReadTDS(unsigned char pin);

private:
};
