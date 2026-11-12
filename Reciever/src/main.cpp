#include <Arduino.h>
#include "radio.h"
#include <math.h>

Receiver receiver;

// Utskrift: 0 = alle pakker (bruk dette til nettsiden), ellers ms mellom linjer (500 funker bra for testing)
constexpr uint32_t PRINT_INTERVAL_MS = 500;

const char* modeName(uint8_t m) {
    switch (m) {
        case 0: return "SAFE";
        case 1: return "DRIVE";
        case 2: return "DRIVE+WEAPON";
        default: return "UNKNOWN";
    }
}

const char* weaponName(uint8_t s) {
    switch (s) {
        case 0: return "LOADED";
        case 1: return "FIRING";
        case 2: return "FAULT";
        case 3: return "MANUAL";
        default: return "UNKNOWN";
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    receiver.setup();
}

void loop() {
    TelemetryPacket p;

    while (receiver.read(p)) {
        if (!isfinite(p.x) || !isfinite(p.y) ||
            !isfinite(p.z) || !isfinite(p.voltage)) {
            continue;
        }

        static uint32_t lastPrint = 0;
        if (PRINT_INTERVAL_MS > 0 && millis() - lastPrint < PRINT_INTERVAL_MS) {
            continue;   // hopp over utskrift, men pakken er lest
        }
        lastPrint = millis();

        Serial.printf(
            "{\"time_ms\":%lu,"
            "\"ax_g\":%.3f,"
            "\"ay_g\":%.3f,"
            "\"battery_v\":%.2f,"
            "\"seq\":%lu,"
            "\"az_g\":%.3f,"
            "\"mode\":\"%s\","
            "\"link\":%s,"
            "\"battery_dead\":%s,"
            "\"startup_lock\":%s,"
            "\"weapon\":\"%s\","
            "\"left_us\":%u,"
            "\"right_us\":%u,"
            "\"weapon_us\":%u}\n",
            (unsigned long)p.timeMs,
            p.x,
            p.y,
            p.voltage,
            (unsigned long)p.seq,
            p.z,
            modeName(p.mode),
            (p.flags & FLAG_LINK)         ? "true" : "false",
            (p.flags & FLAG_BATTERY_DEAD) ? "true" : "false",
            (p.flags & FLAG_STARTUP_LOCK) ? "true" : "false",
            weaponName(p.weaponState),
            p.leftUs,
            p.rightUs,
            p.weaponUs
        );
    }
}