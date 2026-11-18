#pragma once
#include "Common.h"
#include "I2C.h"
#include <array>

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

  I2C _communicationI2C = I2C(ADD_QMC5883L);

public:
  void Initialize()
  {
    _communicationI2C.Init();
    _communicationI2C.I2cWriteReg(REG_SET_QMC5883L, SET_PERIOD);
    _communicationI2C.I2cWriteReg(REG_MOD_QMC5883L,
                                  MOD_OSR | MOD_RNG | MOD_ODR | MOD_CONTINUES_MESURING);
  }

  Vector3 GetDirection()
  {
    std::array<uint8_t, 6> data = _communicationI2C.I2cReadRegs<6>(REG_SENSOR_START);
    return {
        .X = static_cast<int16_t>((data[1] << 8) | data[0]),
        .Y = static_cast<int16_t>((data[3] << 8) | data[2]),
        .Z = static_cast<int16_t>((data[5] << 8) | data[4])};
  }
};
