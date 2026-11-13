#pragma once
#include <cstdint>
#include <Wire.h>

class I2C
{
public:
    void I2cWrite(uint8_t devAddr, uint8_t data)
    {
        Wire.beginTransmission(devAddr);
        Wire.write(data);
        Wire.endTransmission();
    }

    void I2cWriteReg(uint8_t devAddr, uint8_t regAddr, uint8_t data)
    {
        Wire.beginTransmission(devAddr);
        Wire.write(regAddr);
        Wire.write(data);
        Wire.endTransmission();
    }

    template <uint8_t N>
    void I2cRead(uint8_t devAddr, uint8_t (&dest)[N])
    {
        Wire.requestFrom(devAddr, N);

        for (uint8_t i = 0; i < N && Wire.available(); i++)
        {
            dest[i] = static_cast<uint8_t>(Wire.read());
        }
    }

    template <uint8_t N>
    void I2cReadRegs(uint8_t devAddr, uint8_t startReg, uint8_t (&dest)[N])
    {
        Wire.beginTransmission(devAddr);
        Wire.write(startReg);
        Wire.endTransmission();
        Wire.requestFrom(devAddr, N);

        for (uint8_t i = 0; i < N && Wire.available(); i++)
        {
            dest[i] = static_cast<uint8_t>(Wire.read());
        }
    }
};
