#include "crsf.h"
#include "config.h"

Crsf::Crsf(HardwareSerial& serial, int rxPin, int txPin)
    : serial(serial), rxPin(rxPin), txPin(txPin) {
    for (auto& c : ch) c = Cfg::CRSF_CENTER;
}

void Crsf::setup() {
    serial.setRxBufferSize(1024);     // må stå før begin()
    serial.begin(420000, SERIAL_8N1, rxPin, txPin);
}

uint8_t Crsf::crc8(const uint8_t* data, uint8_t len) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t b = 0; b < 8; b++)
            crc = (crc & 0x80) ? (crc << 1) ^ 0xD5 : crc << 1;
    }
    return crc;
}

void Crsf::update() {
    while (serial.available() > 0) {
        uint8_t b = serial.read();

        if (pos == 0) {
            if (b != 0xC8) continue;            // vent på startbyte
            frame[pos++] = b;
            continue;
        }
        if (pos == 1) {
            if (b < 2 || b > 64) { pos = 0; continue; }
            frame[pos++] = b;
            size = b + 2;
            continue;
        }
        frame[pos++] = b;
        if (pos >= size) {
            processFrame();
            pos = 0;
        }
    }
}

void Crsf::processFrame() {
    uint8_t length = frame[1];
    uint8_t type = frame[2];
    if (crc8(&frame[2], length - 1) != frame[length + 1]) return;   // ødelagt ramme

    if (type == 0x16 && length == 24) decodeChannels(&frame[3]);    // kanaldata
}

void Crsf::decodeChannels(const uint8_t* payload) {
    uint32_t buf = 0;
    uint8_t bits = 0, idx = 0;
    for (uint8_t i = 0; i < 22; i++) {
        buf |= (uint32_t)payload[i] << bits;
        bits += 8;
        while (bits >= 11 && idx < 16) {
            ch[idx++] = buf & 0x07FF;
            buf >>= 11;
            bits -= 11;
        }
    }
    if (idx != 16) return;

    haveChannels = true;
    lastFrameMs = millis();
    frameCount++;
}

bool Crsf::linkUp() const {
    return haveChannels && (millis() - lastFrameMs <= timeoutMs);
}

uint16_t Crsf::channel(uint8_t n) const {
    if (n < 1 || n > 16) return Cfg::CRSF_CENTER;
    return ch[n - 1];
}