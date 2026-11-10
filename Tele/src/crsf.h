#pragma once
#include <Arduino.h>

class Crsf {
private:
    HardwareSerial& serial;
    int rxPin, txPin;

    uint8_t frame[66];
    uint8_t pos = 0;
    uint8_t size = 0;

    uint16_t ch[16];
    bool haveChannels = false;
    uint32_t lastFrameMs = 0;
    uint32_t frameCount = 0;
    uint32_t timeoutMs = 150;         // ingen gyldig ramme på 150 ms = failsafe

    static uint8_t crc8(const uint8_t* data, uint8_t len);
    void processFrame();
    void decodeChannels(const uint8_t* payload);
public:
    Crsf(HardwareSerial& serial, int rxPin, int txPin);
    void setup();
    void update();
    bool linkUp() const;
    uint16_t channel(uint8_t n) const;      // n = 1 … 16, som på senderen
    uint32_t getFrameCount() const { return frameCount; }
};