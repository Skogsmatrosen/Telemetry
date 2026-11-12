#pragma once
#include <stdint.h>

// flags-bitene
constexpr uint8_t FLAG_LINK         = 1 << 0;   // radioforbindelse OK
constexpr uint8_t FLAG_BATTERY_DEAD = 1 << 1;   // lagt død pga. lavt batteri
constexpr uint8_t FLAG_STARTUP_LOCK = 1 << 2;   // venter på SAFE etter oppstart

struct __attribute__((packed)) TelemetryPacket {
    uint32_t seq;          // løpenummer, avslører tapte pakker
    uint32_t timeMs;       // millis() på boten
    float x, y, z;         // akselerasjon i g
    float voltage;         // batterispenning
    uint16_t leftUs;       // signal til venstre ESC
    uint16_t rightUs;      // signal til høyre ESC
    uint16_t weaponUs;     // signal til våpen-ESC
    uint8_t mode;          // 0 = SAFE, 1 = DRIVE, 2 = DRIVE+WEAPON
    uint8_t weaponState;   // 0 = LOADED, 1 = FIRING, 2 = FAULT, 3 = MANUAL
    uint8_t flags;         // se FLAG_* over
};