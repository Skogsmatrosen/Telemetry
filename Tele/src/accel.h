#pragma once
#include <Arduino.h>
#include <SparkFun_BMI270_Arduino_Library.h>
#include <Wire.h>

class Accel {
private:
    BMI270 imu;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float magnetude = 0.0f;
    int sdaPin;
    int sclPin;
    uint8_t i2cAddress = BMI2_I2C_PRIM_ADDR; // 0x68
    //uint8_t i2cAddress = BMI2_I2C_SEC_ADDR; // 0x69
    bool ok = false;
    uint8_t errors = 0;
public:
    Accel(int sda, int scl);
    bool setup();
    bool update();
    float getX() const {return x;}
    float getY() const {return y;}
    float getZ() const {return z;}
    float getMagnitude() const {return sqrtf(x*x + y*y + z*z);}
    void printValues();
};