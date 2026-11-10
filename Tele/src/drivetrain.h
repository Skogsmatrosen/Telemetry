#pragma once
#include <Arduino.h>
#include "esc.h"
#include "config.h"

class Drivetrain {
private:
    Esc left;
    Esc right;
    bool invertLeft;
    bool invertRight;
    uint16_t deadband = 25;           // rå CRSF-enheter rundt midten

    int16_t toSigned(uint16_t raw) const;   // rå CRSF -> -1000 … +1000
    static int toPulse(int16_t value);      // -1000 … +1000 -> 1000 … 2000 µs
public:
    Drivetrain(int pinLeft, int pinRight, bool invertLeft = false, bool invertRight = false);
    bool setup();
    void update(uint16_t forwardRaw, uint16_t turnRaw);
    void stop();
    int getLeftUs() const { return left.getUs(); }
    int getRightUs() const { return right.getUs(); }
};