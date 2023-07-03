/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2022-09-12 20:53:45
 * @LastEditors: None
 * @LastEditTime: 2022-11-28 12:10:20
 */
#pragma

#define PWMA GPIO_NUM_19
#define PWMB GPIO_NUM_18
#define TB6612_OxygenChannel 0
#define TB6612_FilterChannel 1
#define TB6612_Accuracy10 10
#define TB6612_Frequency 15000
#define TB6612_ON 1
#define TB6612_OFF 0

class PWM
{
public:
    void PWM_InitConfig(unsigned char Pin, unsigned char Channel, unsigned int Freq, unsigned char Resolution_bits);
    void PWM_Set(unsigned char Channel, bool State, unsigned char Resolution_bits, unsigned int Duty);

private:
};
