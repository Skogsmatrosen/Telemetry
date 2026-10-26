#pragma once
#include <Arduino.h>

class Battery {
private:
    int pin;
    float ratio;          // (R1 + R2) / R2
    float calib = 8.85f/8.99f;   // justeres mot multimeter, calib = multimeter måling / kode måling
    float voltage = 0.0f;
public:
    Battery(int pin, float r1, float r2);
    void setup();
    void update();
    float getVoltage() const {return voltage;}
    void printVoltage();
};