#include "drivetrain.h"

Drivetrain::Drivetrain(int pinLeft, int pinRight, bool invertLeft, bool invertRight)
    : left(pinLeft, 1500), right(pinRight, 1500),
      invertLeft(invertLeft), invertRight(invertRight) {}

bool Drivetrain::setup() {
    bool okLeft = left.setup();       // begge settes opp, også om den første feiler
    bool okRight = right.setup();
    return okLeft && okRight;
}

int16_t Drivetrain::toSigned(uint16_t raw) const {
    raw = constrain(raw, Cfg::CRSF_MIN, Cfg::CRSF_MAX);
    int32_t delta = (int32_t)raw - Cfg::CRSF_CENTER;

    if (abs(delta) <= deadband) return 0;

    if (delta > 0)
        return map(raw, Cfg::CRSF_CENTER + deadband, Cfg::CRSF_MAX, 0, 1000);
    return map(raw, Cfg::CRSF_MIN, Cfg::CRSF_CENTER - deadband, -1000, 0);
}

int Drivetrain::toPulse(int16_t value) {
    value = constrain(value, -1000, 1000);
    return 1500 + value / 2;          // -1000 -> 1000 µs, 0 -> 1500 µs, +1000 -> 2000 µs
}

void Drivetrain::update(uint16_t leftRaw, uint16_t rightRaw) {
    int16_t leftValue  = toSigned(leftRaw);    // -1000 … +1000, med deadband
    int16_t rightValue = toSigned(rightRaw);

    if (invertLeft)  leftValue  = -leftValue;
    if (invertRight) rightValue = -rightValue;

    left.writeUs(toPulse(leftValue));
    right.writeUs(toPulse(rightValue));
}

void Drivetrain::stop() {
    left.stop();
    right.stop();
}