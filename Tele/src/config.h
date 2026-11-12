#pragma once
#include <Arduino.h>

// ---------- Pinner ----------
namespace Pin {
    // Sensorer
    constexpr int SDA = 4;            // IMU: blå
    constexpr int SCL = 5;            // IMU: gul
    constexpr int BMS = 6;            // Batterimåling (må være ADC, GPIO 0–6)

    // ELRS-mottaker (CRSF)
    constexpr int CRSF_RX = 18;       // ← TX på ELRS-mottakeren
    constexpr int CRSF_TX = 19;       // → RX på ELRS-mottakeren (valgfri)

    // Motorer
    constexpr int ESC_LEFT   = 20;
    constexpr int ESC_RIGHT  = 21;
    constexpr int ESC_WEAPON = 22;

    // Våpen
    constexpr int WEAPON_LIMIT = 23;  // Endebryter mellom pinne og GND
}

// ---------- Kanaler på senderen (1–16) ----------
namespace Ch {
    constexpr uint8_t LEFT        = 3;   // Venstre hjul (ferdig mikset på senderen)
    constexpr uint8_t RIGHT       = 2;   // Høyre hjul
    constexpr uint8_t MODE        = 6;
    constexpr uint8_t FIRE        = 9;
    constexpr uint8_t BATTLESHORT = 8;
    constexpr uint8_t WEAPON_POT  = 10;
}

// ---------- Innstillinger ----------
namespace Cfg {
    // Spenningsdeler (målte verdier)
    constexpr float BMS_R1 = 46.5e3f;
    constexpr float BMS_R2 = 9.9e3f;

    // MAC-adressen til mottaker-ESP-en
    constexpr uint8_t RECEIVER_MAC[6] = {0x20, 0x6E, 0xF1, 0x0C, 0x7B, 0x50};
    // constexpr uint8_t RECEIVER_MAC[6] = {0x58, 0xE6, 0xC5, 0x20, 0x48, 0xF4};

    // CRSF-kanalverdier (rå)
    constexpr uint16_t CRSF_MIN    = 172;
    constexpr uint16_t CRSF_CENTER = 992;
    constexpr uint16_t CRSF_MAX    = 1811;

    // Grenser for 3-posisjons modusbryter (rå CRSF)
    constexpr uint16_t MODE_SAFE_MAX  = 600;
    constexpr uint16_t MODE_DRIVE_MAX = 1400;
}