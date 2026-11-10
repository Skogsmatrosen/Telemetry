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

void Drivetrain::update(uint16_t forwardRaw, uint16_t turnRaw) {
    int16_t forward = toSigned(forwardRaw);
    int16_t turn = toSigned(turnRaw);

    // Positiv sving = til høyre: venstre hjul raskere, høyre saktere
    int16_t leftMix  = constrain(forward + turn, -1000, 1000);
    int16_t rightMix = constrain(forward - turn, -1000, 1000);

    if (invertLeft)  leftMix  = -leftMix;
    if (invertRight) rightMix = -rightMix;

    left.writeUs(toPulse(leftMix));
    right.writeUs(toPulse(rightMix));
}

void Drivetrain::stop() {
    left.stop();
    right.stop();
}