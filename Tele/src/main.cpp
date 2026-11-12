#include <Arduino.h>
#include <esp_task_wdt.h>
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
bool startupLock = true;    // krever SAFE én gang etter oppstart
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

// Til feilsøking på benken. Kallet i loop() er kommentert ut.
void printStatus() {
    static uint32_t last = 0;
    if (millis() - last < 500) return;
    last = millis();

    const char* modeName[]   = {"SAFE", "DRIVE", "DRIVE+WEAPON"};
    const char* weaponName[] = {"LOADED", "FIRING", "FAULT", "MANUAL"};

    if (startupLock) Serial.println("Set mode switch to SAFE to unlock");

    Serial.printf("Link: %-4s | Mode: %-12s | Batt: %5.2f V%s\n",
                  crsf.linkUp() ? "OK" : "LOST",
                  modeName[mode],
                  battery.getVoltage(),
                  batteryDead ? " (DEAD)" : "");
    Serial.printf("Left %4d us | Right %4d us | Weapon %4d us %s\n\n",
                  drive.getLeftUs(), drive.getRightUs(),
                  weapon.getUs(), weaponName[weapon.getState()]);
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

    // Watchdog: restarter ESP-en hvis loop() henger i mer enn 500 ms
    esp_task_wdt_config_t wdt = {.timeout_ms = 500, .idle_core_mask = 0, .trigger_panic = true};
    esp_task_wdt_reconfigure(&wdt);
    esp_task_wdt_add(NULL);
}

// ---------- Loop ----------
void loop() {
    esp_task_wdt_reset();

    crsf.update();
    accel.update();

    // Sikkerhet
    bool link = crsf.linkUp();
    RobotMode requested = link ? readMode() : MODE_SAFE;

    // Føreren har aktivt satt bryteren til SAFE (krever radioforbindelse)
    bool driverSafe = link && requested == MODE_SAFE;

    // Oppstartslås: boten kjører ikke før føreren har satt SAFE én gang
    if (driverSafe) startupLock = false;
    mode = startupLock ? MODE_SAFE : requested;

    // SAFE nullstiller våpenfeil
    if (driverSafe) weapon.resetFault();

    // Lavt batteri legger boten død. Slippes først når spenningen
    // er tilbake OG føreren har satt bryteren til SAFE.
    if (battery.isLow()) batteryDead = true;
    if (batteryDead && !battery.isLow() && driverSafe) batteryDead = false;

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

    // 20 Hz: batteri og telemetri
    if (millis() - lastTelemetry >= 50) {
        lastTelemetry = millis();
        battery.update();

        TelemetryPacket p;
        p.x = accel.getX();
        p.y = accel.getY();
        p.z = accel.getZ();
        p.voltage = battery.getVoltage();

        p.leftUs = drive.getLeftUs();
        p.rightUs = drive.getRightUs();
        p.weaponUs = weapon.getUs();
        p.mode = mode;
        p.weaponState = weapon.getState();

        p.flags = 0;
        if (link)        p.flags |= FLAG_LINK;
        if (batteryDead) p.flags |= FLAG_BATTERY_DEAD;
        if (startupLock) p.flags |= FLAG_STARTUP_LOCK;

        radio.send(p);
    }

    // printStatus();
}