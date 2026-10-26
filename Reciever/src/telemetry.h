#pragma once
#include <stdint.h>

struct __attribute__((packed)) TelemetryPacket {
    uint32_t seq;       // løpenummer, avslører tapte pakker
    uint32_t timeMs;    // millis() på boten
    float x, y, z;      // akselerasjon i g
    float voltage;      // batterispenning
};