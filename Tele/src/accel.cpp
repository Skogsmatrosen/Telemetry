#include "accel.h"

Accel::Accel(int sda, int scl) : sdaPin(sda), sclPin(scl) {}

bool Accel::setup() {
    Wire.setPins(sdaPin, sclPin);
    Wire.begin();
    Wire.setClock(400000);
    Wire.setTimeOut(10);              // kort timeout, så en skadet ledning ikke bremser styringen

    if (imu.beginI2C(i2cAddress) != BMI2_OK) {
        Serial.println("Error: BMI270 not connected, check wiring and I2C address!");
        return false;
    }
    Serial.println("BMI270 Connected!");

    bmi2_sens_config cfg = {};
    cfg.type = BMI2_ACCEL;
    cfg.cfg.acc.odr = BMI2_ACC_ODR_1600HZ;
    cfg.cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;
    cfg.cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;
    cfg.cfg.acc.range = BMI2_ACC_RANGE_16G;

    if (imu.setConfig(cfg) != BMI2_OK) {
        Serial.println("Error: Could not set accelerometer configuration!");
        return false;
    }
    ok = true;
    return true;
}

bool Accel::update() {
    if (!ok) return false;

    uint8_t status = 0;
    if (imu.getStatus(&status) != BMI2_OK) {
        if (++errors >= 20) ok = false;     // gi opp IMU-en, behold styringen
        return false;
    }
    if (!(status & BMI2_DRDY_ACC)) return false;

    if (imu.getSensorData() != BMI2_OK) {
        if (++errors >= 20) ok = false;
        return false;
    }
    errors = 0;

    x = imu.data.accelX;
    y = imu.data.accelY;
    z = imu.data.accelZ;
    return true;
}

void Accel::printValues() {
    Serial.print("x: ");
    Serial.print(x);
    Serial.print(", y: ");
    Serial.print(y);
    Serial.print(", z: ");
    Serial.print(z);
    Serial.print(", Magnetude: ");
    Serial.print(getMagnitude());
    Serial.println("g");
}