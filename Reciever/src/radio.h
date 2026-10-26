#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "telemetry.h"

class Receiver {
private:
    static QueueHandle_t queue;
    static void onRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len);
public:
    bool setup();
    bool read(TelemetryPacket& p);
};