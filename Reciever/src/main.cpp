#include <Arduino.h>
#include "radio.h"

Receiver receiver;

void setup() {
    Serial.begin(115200);
    delay(1000);
    receiver.setup();
}

void loop() {
    TelemetryPacket p;
    while (receiver.read(p)) {
        Serial.printf("%lu,%lu,%.3f,%.3f,%.3f,%.2f\n",
                      (unsigned long)p.seq, (unsigned long)p.timeMs,
                      p.x, p.y, p.z, p.voltage);
    }
}