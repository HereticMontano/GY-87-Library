#pragma once
#include <Wire.h>
#include "Common.h"
#include "I2C.h"

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

    I2C _communicationI2C;

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
            delay(2); // Pequeña pausa para que el sensor a 44Hz tenga una muestra nueva
        }

        _offSetX = static_cast<int16_t>(sumX / takes);
        _offSetY = static_cast<int16_t>(sumY / takes);
        
        _offSetZ = static_cast<int16_t>(sumZ / takes);
    }

public:
    void Initialice()
    {
        _communicationI2C.I2cWriteReg(ADD_MPU6050, REG_PWR_MGMT_1, MPU_WAKE_UP);
        _communicationI2C.I2cWriteReg(ADD_MPU6050, REG_INT_PIN_CFG, MPU_I2C_BYPASS_EN);
        delay(10);

        _communicationI2C.I2cWriteReg(ADD_MPU6050, REG_CONFIG, MODE_CONFIG);
        SetOffSet();
    };

    Vector3 GetRawAcceleration()
    {
        uint8_t dataAcceleration[6];
        _communicationI2C.I2cReadRegs(ADD_MPU6050, REG_ACCEL_XOUT_H, dataAcceleration);

        Vector3 vectorAcceleration;
        vectorAcceleration.X = static_cast<int16_t>(dataAcceleration[0] << 8 | dataAcceleration[1]);
        vectorAcceleration.Y = static_cast<int16_t>(dataAcceleration[2] << 8 | dataAcceleration[3]);
        vectorAcceleration.Z = static_cast<int16_t>(dataAcceleration[4] << 8 | dataAcceleration[5]);

        return vectorAcceleration;
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
        uint8_t dataGyro[6];
        _communicationI2C.I2cReadRegs(ADD_MPU6050, REG_GYRO_XOUT_H, dataGyro);

        Vector3 vectorGyro;
        vectorGyro.X = static_cast<int16_t>(dataGyro[0] << 8 | dataGyro[1]);
        vectorGyro.Y = static_cast<int16_t>(dataGyro[2] << 8 | dataGyro[3]);
        vectorGyro.Z = static_cast<int16_t>(dataGyro[4] << 8 | dataGyro[5]);

        return vectorGyro;
    };
};