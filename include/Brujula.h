#pragma once
#include <Arduino.h>
#include "Common.h"
#include "I2C.h"

class Brujula
{

private:
  static constexpr uint8_t ADD_QMC5883L = 0x0D;

  // Registers
  static constexpr uint8_t REG_SENSOR_START = 0x00;
  static constexpr uint8_t REG_SET_QMC5883L = 0x0B;
  static constexpr uint8_t REG_MOD_QMC5883L = 0x09;

  // Configuration Values
  static constexpr uint8_t SET_PERIOD = 0x01;
  static constexpr uint8_t MOD_OSR = (0b11 << 6);
  static constexpr uint8_t MOD_RNG = (0b00 << 4);
  static constexpr uint8_t MOD_ODR = (0b00 << 2);
  static constexpr uint8_t MOD_CONTINUES_MESURING = (0b01 << 0);

  I2C _communicationI2C;

public:
  void Initialize()
  {

    _communicationI2C.I2cWriteReg(ADD_QMC5883L, REG_SET_QMC5883L, SET_PERIOD);
    _communicationI2C.I2cWriteReg(ADD_QMC5883L, REG_MOD_QMC5883L,
                                  MOD_OSR | MOD_RNG | MOD_ODR | MOD_CONTINUES_MESURING);
  }

  Vector3 GetDirection()
  {
    uint8_t data[6] = {};
    _communicationI2C.I2cReadRegs(ADD_QMC5883L, REG_SENSOR_START, data);
    Vector3 magData;
    magData.X = static_cast<int16_t>((data[1] << 8) | data[0]);
    magData.Y = static_cast<int16_t>((data[3] << 8) | data[2]);
    magData.Z = static_cast<int16_t>((data[5] << 8) | data[4]);

    return magData;
  }
};
