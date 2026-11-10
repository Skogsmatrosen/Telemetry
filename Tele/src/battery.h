#pragma once
#include <Arduino.h>

class Battery {
private:
    int pin;
    float ratio;                  // (R1 + R2) / R2
    float calib = 8.85f / 8.99f;  // multimetermåling / kodemåling
    float voltage = 0.0f;

    // Lavspenning
    float cutoff = 11.6f;         // under dette …
    float resume = 11.9f;         // … og tilbake over dette
    uint32_t lowDelayMs = 2000;   // må være lav så lenge før den regnes som lav
    uint32_t lowSince = 0;
    bool low = false;
public:
    Battery(int pin, float r1, float r2);
    void setup();
    void update();
    float getVoltage() const { return voltage; }
    bool isLow() const { return low; }
    void printVoltage();
};