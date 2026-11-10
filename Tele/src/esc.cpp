#include "esc.h"

Esc::Esc(int pin, int stopUs) : pin(pin), stopUs(stopUs), currentUs(stopUs) {}

bool Esc::setup() {
    if (!ledcAttach(pin, FREQ_HZ, RES_BITS)) {
        Serial.printf("Error: Could not attach PWM to pin %d!\n", pin);
        return false;
    }
    stop();               // gyldig stoppsignal fra første øyeblikk
    return true;
}

void Esc::writeUs(int us) {
    us = constrain(us, MIN_US, MAX_US);
    currentUs = us;

    // En periode på 50 Hz er 20 000 µs, og 14 bit gir 16 383 steg
    uint32_t duty = (uint32_t)us * ((1 << RES_BITS) - 1) / 20000;
    ledcWrite(pin, duty);
}

void Esc::stop() {
    writeUs(stopUs);
}