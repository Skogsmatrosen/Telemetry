#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "telemetry.h"

// MAC-adressen til mottakeren
uint8_t receiverMac[6] = {0x58, 0xE6, 0xC5, 0x20, 0x48, 0xD8};

// Tilfeldig float mellom lo og hi
float randf(float lo, float hi) {
    return lo + (hi - lo) * (esp_random() / (float)UINT32_MAX);
}

uint32_t seq = 0;
uint32_t last = 0;
uint32_t nextHit = 0;
float voltage = 12.7f;

void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error: ESP-NOW init failed!");
        return;
    }

    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, receiverMac, 6);
    peer.channel = 0;
    peer.encrypt = false;
    if (esp_now_add_peer(&peer) != ESP_OK) {
        Serial.println("Error: Could not add receiver as peer!");
        return;
    }

    nextHit = millis() + 3000;
    Serial.println("Dummy sender ready!");
}

void loop() {
    if (millis() - last < 50) return;          // 20 Hz
    last = millis();

    TelemetryPacket p;
    p.seq = seq++;
    p.timeMs = last;

    // Tyngdekraft langs Z, rolig "kjørebevegelse" og litt støy
    float t = last / 1000.0f;
    p.x = 0.3f * sinf(t * 0.8f) + randf(-0.05f, 0.05f);
    p.y = 0.2f * sinf(t * 1.3f) + randf(-0.05f, 0.05f);
    p.z = 1.0f + randf(-0.05f, 0.05f);

    // Simulert slag med jevne mellomrom
    if ((int32_t)(last - nextHit) >= 0) {
        p.x += randf(-12.0f, 12.0f);
        p.y += randf(-12.0f, 12.0f);
        p.z += randf(-4.0f, 4.0f);
        nextHit = last + (uint32_t)randf(2000.0f, 6000.0f);
        Serial.println("SLAG!");
    }

    // Batteriet synker sakte og starter på nytt når det er "tomt"
    voltage -= 0.0005f;                        // ca. 0,01 V per sekund
    if (voltage < 11.6f) voltage = 12.7f;
    p.voltage = voltage + randf(-0.02f, 0.02f);

    esp_now_send(receiverMac, (const uint8_t*)&p, sizeof(p));
}