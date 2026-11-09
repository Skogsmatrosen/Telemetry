#pragma once
#include <Arduino.h>

// Pins
namespace Pin {
    constexpr int SDA = 4;  // IMU: Blå
    constexpr int SCL = 3;  // IMU: Gul
    constexpr int BMS = 5;  // BMS
}

namespace Cfg {
    constexpr float BMS_R1 = 46.5e3;  // BMS: R1
    constexpr float BMS_R2 = 9.9e3;   // BMS R2
    constexpr uint8_t RECEIVER_MAC[6] = {0x20, 0x6E, 0xF1, 0x0C, 0x7B, 0x50};
    // constexpr uint8_t RECEIVER_MAC[6] = {0x58, 0xE6, 0xC5, 0x20, 0x48, 0xF4};    // Radio MAC-adresse
}