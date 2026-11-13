#pragma once
#include <Wire.h>
#include "Brujula.h"
#include "Common.h"
#include "Acelerometro.h"
#include "Barometro.h"

class Gy87
{
private:
    static constexpr float GRAVITY_MPS2 = 9.80665f; // 1g = 9.80665 m/s^2;

    Brujula _brujula;
    Acelerometro _acelerometro;
    Barometro _barometro;

    uint32_t _lastMicros;
    Vector3F _lastAcceleration;

public:
    void Initialice()
    {
        _acelerometro.Initialice();
        _brujula.Initialize();
        _barometro.Initialice();
        
        _lastMicros = micros();
    }

    float GetTemprature()
    {
        return _barometro.GetTemperature();
    }

    float GetPresure()
    {
        return _barometro.GetAltitud();
    }

    Vector3F GetAcceleration()
    {

        constexpr float ACCEL_LSB_2G = 16384.0f; // Escala por defecto (±2g)

        Vector3 acceleration = _acelerometro.GetAcceleration();

        Vector3F accelerationF;
        accelerationF.X = (static_cast<float>(acceleration.X) / ACCEL_LSB_2G) * GRAVITY_MPS2;
        accelerationF.Y = (static_cast<float>(acceleration.Y) / ACCEL_LSB_2G) * GRAVITY_MPS2;
        accelerationF.Z = (static_cast<float>(acceleration.Z) / ACCEL_LSB_2G) * GRAVITY_MPS2;

        return accelerationF;
    }

    float GetLinearAcceleration()
    {

        Vector3F acc = GetAcceleration();
        uint32_t nowMicro = micros();
        uint32_t dt = static_cast<float>(_lastMicros - nowMicro) / 1000000.0f;
        _lastMicros = nowMicro;

        _lastAcceleration.X += acc.X * dt;
        _lastAcceleration.Y += acc.Y * dt;
        _lastAcceleration.Z += (acc.Z - GRAVITY_MPS2) * dt;

        return std::sqrt(_lastAcceleration.X * _lastAcceleration.X +
                         _lastAcceleration.Y * _lastAcceleration.Y +
                         _lastAcceleration.Z * _lastAcceleration.Z);
    }

    Vector3 GetGyro()
    {
        return _acelerometro.GetGyro();
    }

    float GetNPoleRadians()
    {
        Vector3 mag = _brujula.GetDirection();
        Vector3 acc = _acelerometro.GetAcceleration();

        float ax = static_cast<float>(acc.X);
        float ay = static_cast<float>(acc.Y);
        float az = static_cast<float>(acc.Z);

        float mx = static_cast<float>(mag.X);
        float my = static_cast<float>(mag.Y);
        float mz = static_cast<float>(mag.Z);

        // 1. Calcular los ángulos de inclinación (Roll y Pitch) con el acelerómetro
        float roll = std::atan2(ay, az);
        float pitch = std::atan2(-ax, std::sqrt(ay * ay + az * az));

        // 2. Proyectar los 3 ejes de la brújula (X, Y, Z) al plano horizontal de la Tierra
        float cosRoll = std::cos(roll);
        float sinRoll = std::sin(roll);
        float cosPitch = std::cos(pitch);
        float sinPitch = std::sin(pitch);

        float xHorizontal = mx * cosPitch + my * sinRoll * sinPitch + mz * cosRoll * sinPitch;
        float yHorizontal = my * cosRoll - mz * sinRoll;

        // 3. Calcular el rumbo hacia el Norte con las componentes ya niveladas
        float headingRad = std::atan2(yHorizontal, xHorizontal);

        if (headingRad < 0.0f)
        {
            headingRad += 2.0f * 3.14159265f;
        }

        return headingRad;
    }
};