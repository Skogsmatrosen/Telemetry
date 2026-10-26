#include "radio.h"

QueueHandle_t Receiver::queue = nullptr;

void Receiver::onRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
    if (len != sizeof(TelemetryPacket)) return;   // ignorer ukjente pakker
    xQueueSend(queue, data, 0);
}

bool Receiver::setup() {
    queue = xQueueCreate(16, sizeof(TelemetryPacket));

    WiFi.mode(WIFI_STA);
    delay(100);
    Serial.print("Receiver MAC: ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error: ESP-NOW init failed!");
        return false;
    }
    esp_now_register_recv_cb(onRecv);
    Serial.println("Receiver ready!");
    return true;
}

bool Receiver::read(TelemetryPacket& p) {
    return xQueueReceive(queue, &p, 0) == pdTRUE;
}