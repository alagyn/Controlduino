#pragma once
#include <stdint.h>

typedef struct
{
    uint16_t buttons;
    uint8_t lTrigger;
    uint8_t rTrigger;

    // Unlike the actual XInputState struct, these are raw values from the sensors
    // Calibration/deadzone is handled by the computer
    uint16_t lStickX;
    uint16_t lStickY;
    uint16_t rStickX;
    uint16_t rStickY;
} Ard_XInput;

constexpr uint8_t START_RUN = 'A';
constexpr uint8_t STOP_RUN = 'B';

namespace button {
    constexpr uint16_t Up = 0x0001;
    constexpr uint16_t Down = 0x0002;
    constexpr uint16_t Left = 0x0004;
    constexpr uint16_t Right = 0x0008;
    constexpr uint16_t Start = 0x0010;
    constexpr uint16_t Back = 0x0020;
    constexpr uint16_t L3 = 0x0040;
    constexpr uint16_t R3 = 0x0080;
    constexpr uint16_t LB = 0x0100;
    constexpr uint16_t RB = 0x0200;
    constexpr uint16_t A = 0x1000;
    constexpr uint16_t B = 0x2000;
    constexpr uint16_t X = 0x4000;
    constexpr uint16_t Y = 0x8000;
} //namespace button