/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2022-10-18 00:15:35
 * @LastEditors: None
 * @LastEditTime: 2022-11-30 23:45:02
 */
#ifndef __WS2812B_PWM_DMA_H__
#define __WS2812B_PWM_DMA_H__

#define TIM_Handle htim2
#define TIM_CHANNEL TIM_CHANNEL_2

#define CODE_1 68          // 1 码所需的占空比(按90%来切分)68%
#define CODE_0 22          // 0 码所需的占空比(按90%来切分)22%
#define RESET_PULSE 225    // 225复位电平个数
#define NUM_LEDS 64        // Led总个数
#define LED_DATA_LENGTH 24 // 单个Led数据长度,WS2812B单个Led一般需要24个字节

#define Shift_Left 1
#define Shift_Right 2

/// Predefined RGB colors
typedef enum
{
    AliceBlue = 0xF0F8FF,
    Amethyst = 0x9966CC,
    AntiqueWhite = 0xFAEBD7,
    Aqua = 0x00FFFF,
    Aquamarine = 0x7FFFD4,
    Azure = 0xF0FFFF,
    Beige = 0xF5F5DC,
    Bisque = 0xFFE4C4,
    Black = 0x000000,
    BlanchedAlmond = 0xFFEBCD,
    Blue = 0x0000FF,
    BlueViolet = 0x8A2BE2,
    Brown = 0xA52A2A,
    BurlyWood = 0xDEB887,
    CadetBlue = 0x5F9EA0,
    Chartreuse = 0x7FFF00,
    Chocolate = 0xD2691E,
    Coral = 0xFF7F50,
    CornflowerBlue = 0x6495ED,
    Cornsilk = 0xFFF8DC,
    Crimson = 0xDC143C,
    Cyan = 0x00FFFF,
    DarkBlue = 0x00008B,
    DarkCyan = 0x008B8B,
    DarkGoldenrod = 0xB8860B,
    DarkGray = 0xA9A9A9,
    DarkGrey = 0xA9A9A9,
    DarkGreen = 0x006400,
    DarkKhaki = 0xBDB76B,
    DarkMagenta = 0x8B008B,
    DarkOliveGreen = 0x556B2F,
    DarkOrange = 0xFF8C00,
    DarkOrchid = 0x9932CC,
    DarkRed = 0x8B0000,
    DarkSalmon = 0xE9967A,
    DarkSeaGreen = 0x8FBC8F,
    DarkSlateBlue = 0x483D8B,
    DarkSlateGray = 0x2F4F4F,
    DarkSlateGrey = 0x2F4F4F,
    DarkTurquoise = 0x00CED1,
    DarkViolet = 0x9400D3,
    DeepPink = 0xFF1493,
    DeepSkyBlue = 0x00BFFF,
    DimGray = 0x696969,
    DimGrey = 0x696969,
    DodgerBlue = 0x1E90FF,
    FireBrick = 0xB22222,
    FloralWhite = 0xFFFAF0,
    ForestGreen = 0x228B22,
    Fuchsia = 0xFF00FF,
    Gainsboro = 0xDCDCDC,
    GhostWhite = 0xF8F8FF,
    Gold = 0xFFD700,
    Goldenrod = 0xDAA520,
    Gray = 0x808080,
    Grey = 0x808080,
    Green = 0x008000,
    GreenYellow = 0xADFF2F,
    Honeydew = 0xF0FFF0,
    HotPink = 0xFF69B4,
    IndianRed = 0xCD5C5C,
    Indigo = 0x4B0082,
    Ivory = 0xFFFFF0,
    Khaki = 0xF0E68C,
    Lavender = 0xE6E6FA,
    LavenderBlush = 0xFFF0F5,
    LawnGreen = 0x7CFC00,
    LemonChiffon = 0xFFFACD,
    LightBlue = 0xADD8E6,
    LightCoral = 0xF08080,
    LightCyan = 0xE0FFFF,
    LightGoldenrodYellow = 0xFAFAD2,
    LightGreen = 0x90EE90,
    LightGrey = 0xD3D3D3,
    LightPink = 0xFFB6C1,
    LightSalmon = 0xFFA07A,
    LightSeaGreen = 0x20B2AA,
    LightSkyBlue = 0x87CEFA,
    LightSlateGray = 0x778899,
    LightSlateGrey = 0x778899,
    LightSteelBlue = 0xB0C4DE,
    LightYellow = 0xFFFFE0,
    Lime = 0x00FF00,
    LimeGreen = 0x32CD32,
    Linen = 0xFAF0E6,
    Magenta = 0xFF00FF,
    Maroon = 0x800000,
    MediumAquamarine = 0x66CDAA,
    MediumBlue = 0x0000CD,
    MediumOrchid = 0xBA55D3,
    MediumPurple = 0x9370DB,
    MediumSeaGreen = 0x3CB371,
    MediumSlateBlue = 0x7B68EE,
    MediumSpringGreen = 0x00FA9A,
    MediumTurquoise = 0x48D1CC,
    MediumVioletRed = 0xC71585,
    MidnightBlue = 0x191970,
    MintCream = 0xF5FFFA,
    MistyRose = 0xFFE4E1,
    Moccasin = 0xFFE4B5,
    NavajoWhite = 0xFFDEAD,
    Navy = 0x000080,
    OldLace = 0xFDF5E6,
    Olive = 0x808000,
    OliveDrab = 0x6B8E23,
    Orange = 0xFFA500,
    OrangeRed = 0xFF4500,
    Orchid = 0xDA70D6,
    PaleGoldenrod = 0xEEE8AA,
    PaleGreen = 0x98FB98,
    PaleTurquoise = 0xAFEEEE,
    PaleVioletRed = 0xDB7093,
    PapayaWhip = 0xFFEFD5,
    PeachPuff = 0xFFDAB9,
    Peru = 0xCD853F,
    Pink = 0xFFC0CB,
    Plaid = 0xCC5533,
    Plum = 0xDDA0DD,
    PowderBlue = 0xB0E0E6,
    Purple = 0x800080,
    Red = 0xFF0000,
    RosyBrown = 0xBC8F8F,
    RoyalBlue = 0x4169E1,
    SaddleBrown = 0x8B4513,
    Salmon = 0xFA8072,
    SandyBrown = 0xF4A460,
    SeaGreen = 0x2E8B57,
    Seashell = 0xFFF5EE,
    Sienna = 0xA0522D,
    Silver = 0xC0C0C0,
    SkyBlue = 0x87CEEB,
    SlateBlue = 0x6A5ACD,
    SlateGray = 0x708090,
    SlateGrey = 0x708090,
    Snow = 0xFFFAFA,
    SpringGreen = 0x00FF7F,
    SteelBlue = 0x4682B4,
    Tan = 0xD2B48C,
    Teal = 0x008080,
    Thistle = 0xD8BFD8,
    Tomato = 0xFF6347,
    Turquoise = 0x40E0D0,
    Violet = 0xEE82EE,
    Wheat = 0xF5DEB3,
    White = 0xFFFFFF,
    WhiteSmoke = 0xF5F5F5,
    Yellow = 0xFFFF00,
    YellowGreen = 0x9ACD32,

    // LED RGB color that roughly approximates
    // the color of incandescent fairy lights,
    // assuming that you're using FastLED
    // color correction on your LEDs (recommended).
    FairyLight = 0xFFE42D,
    // If you are using no color correction, use this
    FairyLightNCC = 0xFF9D2A

} HTMLColorCode;

void WS2812B_Shift1(unsigned char Direction, unsigned char N);
void WS2812B_Shift2(unsigned char Direction, unsigned char N);
void WS2812B_SetRGB(unsigned char R, unsigned char G, unsigned char B, unsigned short Location);
void WS2812B_SetColour(unsigned int RGB, unsigned short Location);
void WS2812B_SetHSV(unsigned short H, unsigned short S, unsigned short V, unsigned short Location);
void WS2812B_Show(void);
void WS2812B_ClearAll(void);
void WS2812B_SetSequentialRGB(unsigned char R, unsigned char G, unsigned char B, unsigned short Sequential);
void WS2812B_SetSequentialColour(unsigned int RGB, unsigned short Sequential);
void WS2812B_SetSequentialHSV(unsigned short H, unsigned short S, unsigned short V, unsigned short Sequential);
void WS2812B_Draw8x8RGBChar(unsigned int RGB, unsigned char *Data);
void WS2812B_DrawRGBString_InfiniteX(unsigned int RGB, unsigned short *Data, unsigned short X_Deviation, unsigned short Delay);
void WS2812B_DrawHSVString_InfiniteX(unsigned short H, unsigned short S, unsigned short V, unsigned short *Data, unsigned short X_Deviation, unsigned short Delay);
void WS2812B_DrawColourString_InfiniteX(unsigned short V, unsigned short *Data, unsigned short X_Deviation, unsigned short Delay);

void WS2812B_Test1(void);
void WS2812B_Test2(unsigned int Test2_Colour1, unsigned int Test2_Colour2, unsigned int Test2_Colour3, unsigned short Test2_Delay1);
void WS2812B_Test3(void);
void WS2812B_Test4(void);
void WS2812B_Test5(void);
void WS2812B_Test6(void);
void WS2812B_Test7(unsigned char N);
void WS2812B_Test8(void);
void WS2812B_Test9(void);

#endif
