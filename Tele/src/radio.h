#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "telemetry.h"
#include "config.h"

class Radio {
private:
    uint8_t receiverMac[6];
    uint32_t seq = 0;
public:
    Radio(const uint8_t mac[6]);
    bool setup();
    bool send(TelemetryPacket& p);
};