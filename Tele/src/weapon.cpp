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

// Fart styrt fra glidebryteren, brukes både i fyring og manuell modus
int Weapon::fireSpeed(uint16_t potRaw) const {
    uint16_t pot = constrain(potRaw, potLow, Cfg::CRSF_MAX);
    return map(pot, potLow, Cfg::CRSF_MAX, fireMinUs, fireMaxUs);
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
    case MANUAL:
        // Motoren går bare mens fyr holdes inne, med samme fart som ved vanlig fyring.
        // Etter at battleshort slås på må fyr først slippes.
        if (!manualReady) {
            esc.stop();
            if (!fireButton) manualReady = true;
            break;
        }
        if (fireButton) {
            esc.writeUs(fireSpeed(potRaw));
        } else {
            esc.stop();
        }
        break;

    case LOADED:
        esc.stop();
        if (newPress) {
            state = FIRING;
            switchReleased = false;
            fireStart = millis();
        }
        break;

    case FIRING:
        esc.writeUs(fireSpeed(potRaw));
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
        esc.stop();
        if (newPress) {                // nytt trykk = nytt forsøk
            state = FIRING;
            switchReleased = false;
            fireStart = millis();
        }
        break;
    }
}

void Weapon::stop() {
    esc.stop();
    if (state == FIRING) state = LOADED;   // avbryt en pågående runde
}