#pragma once
#include <Wire.h>
#include "Common.h"
#include "I2C.h"

class Barometro
{
private:
    static constexpr uint8_t ADD_BMP180 = 0x77;

    static constexpr uint8_t REG_CALIBRATION = 0xAA;
    static constexpr uint8_t REG_CTRL_MEAS = 0xF4;
    static constexpr uint8_t REG_OUT_MSB = 0xF6;

    static constexpr uint8_t OSS = 0b00;
    static constexpr uint8_t MOD_CTRL_OSS = (OSS << 6);
    static constexpr uint8_t MOD_CTRL_SCO = (0b1 << 5);
    static constexpr uint8_t MOD_CTRL_TEMPERATURE = 0x2E;
    static constexpr uint8_t MOD_CTRL_PRESURE = 0x34;

    struct CalibrationParam
    {
        int16_t AC1 = 0;
        int16_t AC2 = 0;
        int16_t AC3 = 0;
        uint16_t AC4 = 0;
        uint16_t AC5 = 0;
        uint16_t AC6 = 0;

        int16_t B1 = 0;
        int16_t B2 = 0;

        int16_t MB = 0;
        int16_t MC = 0;
        int16_t MD = 0;
    };
    I2C _communicationI2C;

    CalibrationParam _calibration;

    void IsReady()
    {
        uint8_t status[1];
        do
        {
            _communicationI2C.I2cReadRegs(ADD_BMP180, REG_CTRL_MEAS, status);
        } while ((status[0] & MOD_CTRL_SCO) != 0);
    };

    int32_t GetRawTemperature()
    {
        _communicationI2C.I2cWriteReg(ADD_BMP180, REG_CTRL_MEAS, MOD_CTRL_OSS | MOD_CTRL_TEMPERATURE);
        IsReady();

        uint8_t data[2];
        _communicationI2C.I2cReadRegs(ADD_BMP180, REG_OUT_MSB, data);

        return static_cast<int32_t>(data[0] << 8 | data[1]);
    }

    int32_t GetRawPresure()
    {
        _communicationI2C.I2cWriteReg(ADD_BMP180, REG_CTRL_MEAS, MOD_CTRL_OSS | MOD_CTRL_PRESURE);
        IsReady();

        uint8_t data[3];
        _communicationI2C.I2cReadRegs(ADD_BMP180, REG_OUT_MSB, data);
        return ((static_cast<int32_t>(data[0]) << 16) |
                (static_cast<int32_t>(data[1]) << 8) |
                static_cast<int32_t>(data[2])) >> (8 - OSS);
    }

    float Calculate(int32_t ut, int32_t up = 0)
    {
        // --- 1. Cálculo de B5 (usa UT: lectura cruda de temperatura) ---
        int32_t x1 = ((ut - static_cast<int32_t>(_calibration.AC6)) * static_cast<int32_t>(_calibration.AC5)) >> 15; // (UT - AC6) * AC5 / 2^15
        int32_t x2 = (static_cast<int32_t>(_calibration.MC) << 11) / (x1 + static_cast<int32_t>(_calibration.MD));   // (MC * 2^11) / (X1 + MD)
        int32_t b5 = x1 + x2;

        if (up == 0)
        {
            return static_cast<float>((b5 + 8) >> 4);
        };

        // --- 2. Cálculo de Presión (usa B5 y UP: lectura cruda de presión) ---
        int32_t b6 = b5 - 4000;
        x1 = (static_cast<int32_t>(_calibration.B2) * ((b6 * b6) >> 12)) >> 11; // (B2 * (B6 * B6 / 2^12)) / 2^11
        x2 = (static_cast<int32_t>(_calibration.AC2) * b6) >> 11;               // AC2 * B6 / 2^11
        int32_t x3 = x1 + x2;
        int32_t b3 = (((static_cast<int32_t>(_calibration.AC1) * 4 + x3) << OSS) + 2) >> 2; // (((AC1 * 4 + X3) << oss) + 2) / 4

        x1 = (static_cast<int32_t>(_calibration.AC3) * b6) >> 13;               // AC3 * B6 / 2^13
        x2 = (static_cast<int32_t>(_calibration.B1) * ((b6 * b6) >> 12)) >> 16; // (B1 * (B6 * B6 / 2^12)) / 2^16
        x3 = ((x1 + x2) + 2) >> 2;                                              // ((X1 + X2) + 2) / 2^2

        // En la imagen B4 y B7 son "unsigned long" -> uint32_t
        uint32_t b4 = (static_cast<uint32_t>(_calibration.AC4) * static_cast<uint32_t>(x3 + 32768)) >> 15; // AC4 * (unsigned long)(X3 + 32768) / 2^15
        uint32_t b7 = (static_cast<uint32_t>(up) - static_cast<uint32_t>(b3)) * (50000UL >> OSS);          // ((unsigned long)UP - B3) * (50000 >> oss)

        int32_t p = b7 < 0x80000000UL ? static_cast<int32_t>((b7 * 2) / b4) : static_cast<int32_t>((b7 / b4) * 2);

        x1 = (p >> 8) * (p >> 8);        // (p / 2^8) * (p / 2^8)
        x1 = (x1 * 3038) >> 16;          // (X1 * 3038) / 2^16
        x2 = (-7357 * p) >> 16;          // (-7357 * p) / 2^16
        p = p + ((x1 + x2 + 3791) >> 4); // p + (X1 + X2 + 3791) / 2^4

        return static_cast<float>(p);
    }

public:
    void Initialice()
    {

        _communicationI2C.I2cWriteReg(ADD_BMP180, REG_CTRL_MEAS, MOD_CTRL_OSS);

        uint8_t calibration[22] = {};

        _communicationI2C.I2cReadRegs(ADD_BMP180, REG_CALIBRATION, calibration);

        _calibration.AC1 = static_cast<int16_t>(calibration[0] << 8 | calibration[1]);
        _calibration.AC2 = static_cast<int16_t>(calibration[2] << 8 | calibration[3]);
        _calibration.AC3 = static_cast<int16_t>(calibration[4] << 8 | calibration[5]);
        _calibration.AC4 = static_cast<uint16_t>(calibration[6] << 8 | calibration[7]);
        _calibration.AC5 = static_cast<uint16_t>(calibration[8] << 8 | calibration[9]);
        _calibration.AC6 = static_cast<uint16_t>(calibration[10] << 8 | calibration[11]);

        _calibration.B1 = static_cast<int16_t>(calibration[12] << 8 | calibration[13]);
        _calibration.B2 = static_cast<int16_t>(calibration[14] << 8 | calibration[15]);

        _calibration.MB = static_cast<int16_t>(calibration[16] << 8 | calibration[17]);
        _calibration.MC = static_cast<int16_t>(calibration[18] << 8 | calibration[19]);
        _calibration.MD = static_cast<int16_t>(calibration[20] << 8 | calibration[21]);
    };

    float GetTemperature()
    {
        return Calculate(GetRawTemperature()) / 10.0f; // Temperatura en °C
    }

    float GetAltitud()
    {
        return Calculate(GetRawTemperature(), GetRawPresure()) / 100.0f; // hPa
    };
};
