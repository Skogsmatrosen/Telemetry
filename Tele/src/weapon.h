#pragma once
#include <Arduino.h>
#include "esc.h"
#include "config.h"

class Weapon {
public:
    enum State { LOADED, FIRING, FAULT, MANUAL };

private:
    Esc esc;
    int limitPin;
    State state = LOADED;

    // Fyring
    bool switchReleased = false;      // har endebryteren sluppet siden motoren startet?
    bool lastFire = false;            // for å oppdage nytt trykk på fyr-knappen
    uint32_t fireStart = 0;

    // Avprelling av endebryteren
    bool lastRaw = false;
    bool stable = false;
    uint32_t stableSince = 0;

    // Manuell modus (battleshort)
    bool manualReady = false;         // pot har vært i bunn siden manuell ble valgt

    // Innstillinger
    int runUs = 1700;                 // motorfart under fyring
    int manualMaxUs = 1600;           // maks fart i manuell modus
    uint32_t timeoutMs = 2000;        // maks tid for én runde
    uint32_t debounceMs = 10;
    uint16_t potLow = Cfg::CRSF_MIN + 60;

    bool limitPressed();

public:
    Weapon(int escPin, int limitPin);
    bool setup();
    void update(bool fireButton, bool battleshort, uint16_t potRaw, bool enabled);
    void stop();
    void resetFault() { if (state == FAULT) state = LOADED; }
    State getState() const { return state; }
    int getUs() const { return esc.getUs(); }
};