#include <Arduino.h>
#include "config.h"
#include "accel.h"
#include "battery.h"
#include "radio.h"
#include "telemetry.h"

Accel accel(Pin::SDA, Pin::SCL);
Battery battery(Pin::BMS, Cfg::BMS_R1, Cfg::BMS_R2);
Radio radio(Cfg::RECEIVER_MAC);

uint32_t last = 0;

void setup() {
  Serial.begin(115200);
  accel.setup();
  battery.setup();
  radio.setup();
}

void loop() {
  accel.update();
  if(millis() - last >= 50) {
    last = millis();
    battery.update();
    battery.printVoltage();
    TelemetryPacket p;
    p.x = accel.getX();
    p.y = accel.getY();
    p.z = accel.getZ();
    p.voltage = battery.getVoltage();
    radio.send(p);
  }
}