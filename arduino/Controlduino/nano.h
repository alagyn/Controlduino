#ifndef CONTROLDUINO_PINS
#define CONTROLDUINO_PINS

namespace pins
{
    constexpr uint8_t LeftStick_X = PIN_A3;
    constexpr uint8_t LeftStick_Y = PIN_A1;
    constexpr uint8_t LeftStick_Btn = PIN_A2;

    constexpr uint8_t RightStick_X = PIN_A6;
    constexpr uint8_t RightStick_Y = PIN_A7;
    constexpr uint8_t RightStick_Btn = PIN_A5;

    constexpr uint8_t DPad_Up = 12;
    constexpr uint8_t DPad_Right = 11;
    constexpr uint8_t DPad_Down = 10;
    constexpr uint8_t DPad_Left = 9;

    constexpr uint8_t Button_B = 7;
    constexpr uint8_t Button_A = 6;
    constexpr uint8_t Button_X = 5;
    constexpr uint8_t Button_Y = 4;

    constexpr uint8_t BumperL = 8;
    constexpr uint8_t BumperR = 3;

    constexpr uint8_t Start = 2;
    constexpr uint8_t Back = 13;

    constexpr uint8_t TriggerL = PIN_A0;
    constexpr uint8_t TriggerR = PIN_A4;
}

#endif