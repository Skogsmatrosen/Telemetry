#include "battery.h"

Battery::Battery(int pin, float r1, float r2) : pin(pin), ratio((r1 + r2) / r2) {}

void Battery::setup() {
    pinMode(pin, INPUT);
}

void Battery::update() {
    float vAdc = analogReadMilliVolts(pin) / 1000.0f;
    float vNew = vAdc * ratio * calib;

    // Lavpass filter
    if (voltage == 0) voltage = vNew;   // første måling
    else voltage = 0.9f * voltage + 0.1f * vNew;
}

void Battery::printVoltage() {
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.println("V");
}