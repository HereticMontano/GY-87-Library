#pragma once
#include "Common.h"
#include "I2C.h"
#include <array>
#include <esp_rom_sys.h>

class Acelerometro
{
private:
    static constexpr uint8_t ADD_MPU6050 = 0x68;

    static constexpr uint8_t REG_CONFIG = 0x1A;
    static constexpr uint8_t REG_INT_PIN_CFG = 0x37;
    static constexpr uint8_t REG_ACCEL_XOUT_H = 0x3B;
    static constexpr uint8_t REG_GYRO_XOUT_H = 0x43;
    static constexpr uint8_t REG_PWR_MGMT_1 = 0x6B;

    static constexpr uint8_t MPU_WAKE_UP = 0x00;
    static constexpr uint8_t MPU_I2C_BYPASS_EN = 0x02;
    static constexpr uint8_t MODE_CONFIG = 0x03; // Medition 44hz

    int16_t _offSetX = 0;
    int16_t _offSetY = 0;
    int16_t _offSetZ = 0;

    I2C _communicationI2C = I2C(ADD_MPU6050);

    void SetOffSet()
    {
        int32_t sumX = 0;
        int32_t sumY = 0;
        int32_t sumZ = 0;
        constexpr uint8_t takes = 50;

        for (uint8_t i = 0; i < takes; i++)
        {
            Vector3 acc = GetRawAcceleration();
            sumX += acc.X;
            sumY += acc.Y;
            sumZ += acc.Z;
            esp_rom_delay_us(2000); // Pause to the sensor have a new data
        }

        _offSetX = static_cast<int16_t>(sumX / takes);
        _offSetY = static_cast<int16_t>(sumY / takes);

        _offSetZ = static_cast<int16_t>(sumZ / takes);
    }

public:
    void Initialice()
    {
        _communicationI2C.Init();

        _communicationI2C.I2cWriteReg(REG_PWR_MGMT_1, MPU_WAKE_UP);
        _communicationI2C.I2cWriteReg(REG_INT_PIN_CFG, MPU_I2C_BYPASS_EN);
        esp_rom_delay_us(10000);

        _communicationI2C.I2cWriteReg(REG_CONFIG, MODE_CONFIG);
        SetOffSet();
    };

    Vector3 GetRawAcceleration()
    {
        std::array<uint8_t, 6> dataAcceleration = _communicationI2C.I2cReadRegs<6>(REG_ACCEL_XOUT_H);

        return {
            .X = static_cast<int16_t>(dataAcceleration[0] << 8 | dataAcceleration[1]),
            .Y = static_cast<int16_t>(dataAcceleration[2] << 8 | dataAcceleration[3]),
            .Z = static_cast<int16_t>(dataAcceleration[4] << 8 | dataAcceleration[5])};
    };

    Vector3 GetAcceleration()
    {
        Vector3 vectorAcceleration = GetRawAcceleration();
        vectorAcceleration.X -= _offSetX;
        vectorAcceleration.Y -= _offSetY;
        vectorAcceleration.Z -= _offSetZ;        

        return vectorAcceleration;
    };

    Vector3 GetGyro()
    {
        std::array<uint8_t, 6> dataGyro = _communicationI2C.I2cReadRegs<6>(REG_GYRO_XOUT_H);

        return {
            .X = static_cast<int16_t>(dataGyro[0] << 8 | dataGyro[1]),
            .Y = static_cast<int16_t>(dataGyro[2] << 8 | dataGyro[3]),
            .Z = static_cast<int16_t>(dataGyro[4] << 8 | dataGyro[5])};
    };
};