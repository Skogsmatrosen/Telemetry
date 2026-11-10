#include "weapon.h"

Weapon::Weapon(int escPin, int limitPin) : esc(escPin, 1000), limitPin(limitPin) {}

bool Weapon::setup() {
    pinMode(limitPin, INPUT_PULLUP);  // bryter mellom pinne og GND
    lastRaw = stable = (digitalRead(limitPin) == LOW);
    stableSince = millis();
    return esc.setup();
}

// Avprellet lesing av endebryteren
bool Weapon::limitPressed() {
    bool raw = digitalRead(limitPin) == LOW;
    uint32_t now = millis();

    if (raw != lastRaw) {
        lastRaw = raw;
        stableSince = now;
    }
    if (now - stableSince >= debounceMs) stable = raw;
    return stable;
}

void Weapon::update(bool fireButton, bool battleshort, uint16_t potRaw, bool enabled) {
    bool pressed = limitPressed();
    bool newPress = fireButton && !lastFire;   // bare kanten, ikke mens knappen holdes
    lastFire = fireButton;

    // SAFE, DRIVE eller failsafe: alt stopper
    if (!enabled) {
        stop();
        manualReady = false;
        return;
    }

    // Inn og ut av battleshort
    if (battleshort && state != MANUAL) {
        state = MANUAL;
        manualReady = false;
    } else if (!battleshort && state == MANUAL) {
        esc.stop();
        state = LOADED;                // neste fyr kjører uansett frem til bryteren
    }

    switch (state) {
    case MANUAL: {
        if (!manualReady) {
            esc.stop();
            if (potRaw <= potLow) manualReady = true;
            break;
        }
        uint16_t pot = constrain(potRaw, potLow, Cfg::CRSF_MAX);
        esc.writeUs(map(pot, potLow, Cfg::CRSF_MAX, 1000, manualMaxUs));
        break;
    }

    case LOADED:
        esc.stop();
        if (newPress) {
            state = FIRING;
            switchReleased = false;
            fireStart = millis();
        }
        break;

    case FIRING:
        esc.writeUs(runUs);
        if (!pressed) switchReleased = true;            // har forlatt hvileposisjonen
        if (switchReleased && pressed) {                 // tilbake i ladet posisjon
            esc.stop();
            state = LOADED;
        } else if (millis() - fireStart > timeoutMs) {   // noe sitter fast
            esc.stop();
            state = FAULT;
        }
        break;

    case FAULT:
        esc.stop();                    // står stille til battleshort eller resetFault()
        break;
    }
}

void Weapon::stop() {
    esc.stop();
    if (state == FIRING) state = LOADED;   // avbryt en pågående runde
}