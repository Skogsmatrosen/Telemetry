#include "radio.h"

Radio::Radio(const uint8_t mac[6]) {
    memcpy(receiverMac, mac, 6);
}

bool Radio::setup() {
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error: ESP-NOW init failed!");
        return false;
    }

    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, receiverMac, 6);
    peer.channel = 0;
    peer.encrypt = false;

    if (esp_now_add_peer(&peer) != ESP_OK) {
        Serial.println("Error: Could not add receiver as peer!");
        return false;
    }

    Serial.println("ESP-NOW ready!");
    return true;
}

bool Radio::send(TelemetryPacket& p) {
    p.seq = seq++;
    p.timeMs = millis();
    return esp_now_send(receiverMac, (const uint8_t*)&p, sizeof(p)) == ESP_OK;
}
