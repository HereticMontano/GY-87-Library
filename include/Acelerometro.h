#pragma once
#include <Wire.h>
#include "Common.h"
#include "I2C.h"

class Acelerometro
{
private:
    static constexpr uint8_t ADD_MPU6050 = 0x68;

    static constexpr uint8_t REG_INT_PIN_CFG = 0x37;
    static constexpr uint8_t REG_ACCEL_XOUT_H = 0x3B;    
    static constexpr uint8_t REG_GYRO_XOUT_H = 0x43;
    static constexpr uint8_t REG_PWR_MGMT_1 = 0x6B;

    static constexpr uint8_t MPU_WAKE_UP = 0x00;
    static constexpr uint8_t MPU_I2C_BYPASS_EN = 0x02;
    

    I2C _communicationI2C;

public:
    void Initialice()
    {
        _communicationI2C.I2cWriteReg(ADD_MPU6050, REG_PWR_MGMT_1, MPU_WAKE_UP);
        _communicationI2C.I2cWriteReg(ADD_MPU6050, REG_INT_PIN_CFG, MPU_I2C_BYPASS_EN);
        delay(10);
    };

    Vector3 GetAcceleration()
    {
        uint8_t dataAcceleration[6];
        _communicationI2C.I2cReadRegs(ADD_MPU6050, REG_ACCEL_XOUT_H, dataAcceleration);

        Vector3 vectorAcceleration;
        vectorAcceleration.X = static_cast<int16_t>(dataAcceleration[0] << 8 | dataAcceleration[1]);
        vectorAcceleration.Y = static_cast<int16_t>(dataAcceleration[2] << 8 | dataAcceleration[3]);
        vectorAcceleration.Z = static_cast<int16_t>(dataAcceleration[4] << 8 | dataAcceleration[5]);
        
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