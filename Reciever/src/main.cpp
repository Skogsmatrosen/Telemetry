#include <Arduino.h>
#include "radio.h"
#include <math.h>

Receiver receiver;

void setup() {
    Serial.begin(115200);
    delay(1000);
    receiver.setup();
}

void loop() {
    TelemetryPacket p;

    while (receiver.read(p)) {
        if (!isfinite(p.x) ||
            !isfinite(p.y) ||
            !isfinite(p.voltage)) {
            continue;
        }

        Serial.printf(
            "{\"time_ms\":%lu,"
            "\"ax_g\":%.3f,"
            "\"ay_g\":%.3f,"
            "\"battery_v\":%.2f}\n",
            (unsigned long)p.timeMs,
            p.x,
            p.y,
            p.voltage
        );
    }
}
