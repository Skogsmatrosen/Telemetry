#include <Arduino.h>
#include "config.h"
#include "accel.h"
#include "battery.h"
#include "radio.h"
#include "telemetry.h"
#include "crsf.h"
#include "drivetrain.h"
#include "weapon.h"

enum RobotMode { MODE_SAFE, MODE_DRIVE, MODE_DRIVE_WEAPON };

// ---------- Objekter ----------
HardwareSerial crsfSerial(1);
Crsf crsf(crsfSerial, Pin::CRSF_RX, Pin::CRSF_TX);

Drivetrain drive(Pin::ESC_LEFT, Pin::ESC_RIGHT);
Weapon weapon(Pin::ESC_WEAPON, Pin::WEAPON_LIMIT);

Accel accel(Pin::SDA, Pin::SCL);
Battery battery(Pin::BMS, Cfg::BMS_R1, Cfg::BMS_R2);
Radio radio(Cfg::RECEIVER_MAC);

// ---------- Tilstand ----------
RobotMode mode = MODE_SAFE;
bool batteryDead = false;
uint32_t lastTelemetry = 0;

// ---------- Hjelpefunksjoner ----------
RobotMode readMode() {
    uint16_t v = crsf.channel(Ch::MODE);
    if (v <= Cfg::MODE_SAFE_MAX) return MODE_SAFE;
    if (v <= Cfg::MODE_DRIVE_MAX) return MODE_DRIVE;
    return MODE_DRIVE_WEAPON;
}

bool switchOn(uint8_t channel) {
    return crsf.channel(channel) > Cfg::CRSF_CENTER;
}

void printStatus() {
    static uint32_t last = 0;
    if (millis() - last < 500) return;
    last = millis();

    const char* modeName[]   = {"SAFE", "DRIVE", "DRIVE+WEAPON"};
    const char* weaponName[] = {"LOADED", "FIRING", "FAULT", "MANUAL"};

    Serial.printf("Link: %-4s | Mode: %-12s | Batt: %5.2f V%s\n",
                  crsf.linkUp() ? "OK" : "LOST",
                  modeName[mode],
                  battery.getVoltage(),
                  batteryDead ? " (DEAD)" : "");
    Serial.printf("Left %4d us | Right %4d us | Weapon %4d us %s\n",
                  drive.getLeftUs(), drive.getRightUs(),
                  weapon.getUs(), weaponName[weapon.getState()]);

    // Midlertidig: vis alle kanalene for å finne ut hva som er hva
    for (int i = 1; i <= 16; i++) Serial.printf("CH%d:%u ", i, crsf.channel(i));
    Serial.println("\n");
}

// ---------- Setup ----------
void setup() {
    Serial.begin(115200);
    delay(1000);

    // ESC-ene først, så de får stoppsignal med en gang
    if (!drive.setup() || !weapon.setup()) {
        Serial.println("ERROR: ESC init failed!");
        while (true) delay(1000);
    }

    crsf.setup();
    accel.setup();
    battery.setup();
    radio.setup();

    Serial.println("Robot ready, waiting for radio...");
}

// ---------- Loop ----------
void loop() {
    crsf.update();
    accel.update();

    // 20 Hz: batteri og telemetri
    if (millis() - lastTelemetry >= 50) {
        lastTelemetry = millis();
        battery.update();

        TelemetryPacket p;
        p.x = accel.getX();
        p.y = accel.getY();
        p.z = accel.getZ();
        p.voltage = battery.getVoltage();
        radio.send(p);
    }

    // Sikkerhet
    bool link = crsf.linkUp();
    mode = link ? readMode() : MODE_SAFE;

    // Lavt batteri legger boten død. Slippes først når spenningen
    // er tilbake OG føreren har satt modus til SAFE.
    if (battery.isLow()) batteryDead = true;
    if (batteryDead && !battery.isLow() && mode == MODE_SAFE) batteryDead = false;

    bool driveAllowed  = link && !batteryDead && mode != MODE_SAFE;
    bool weaponAllowed = link && !batteryDead && mode == MODE_DRIVE_WEAPON;

    // Motorer
    if (driveAllowed) {
        drive.update(crsf.channel(Ch::LEFT), crsf.channel(Ch::RIGHT));
    } else {
        drive.stop();
    }

    weapon.update(switchOn(Ch::FIRE),
                  switchOn(Ch::BATTLESHORT),
                  crsf.channel(Ch::WEAPON_POT),
                  weaponAllowed);

    printStatus();
}