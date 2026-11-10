#pragma once
#include <Arduino.h>

class Esc {
private:
    int pin;
    int stopUs;           // 1500 for toveis, 1000 for enveis
    int currentUs;        // siste verdi som ble sendt

    static constexpr int FREQ_HZ = 50;
    static constexpr int RES_BITS = 14;
    static constexpr int MIN_US = 1000;
    static constexpr int MAX_US = 2000;
public:
    Esc(int pin, int stopUs);
    bool setup();
    void writeUs(int us);
    void stop();
    int getUs() const { return currentUs; }
};