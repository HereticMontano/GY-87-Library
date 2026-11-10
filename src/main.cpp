#include <Arduino.h>
#include <Wire.h>
#include "r4_init.h"
#include "Brujula.h"
#include "Common.h"
#include "Acelerometro.h"
#include "GY-87.h"

void loop(Gy87 &gy87);

int main()
{
    // 1. Inicializa el hardware base del Arduino (vectores IRQ, timers, USB, pines)
    init();

    // 2. Equivalente a setup()
    Serial.begin(9600);
    Wire.begin();

    Gy87 gy87;
    gy87.Initialice();

    // 3. Equivalente a loop()
    while (true)
    {
        loop(gy87);
    };

    return 0;
}

void loop(Gy87 &gy87)
{

    Serial.println("Grados N: " + String(gy87.GetNPoleRadians()));

    Vector3 gyro = gy87.GetGyro();

    Serial.println("Gyro X: " + String(gyro.X) + " | Gyro  Y: " + String(gyro.Y) +
                   " | Gyro Z: " + String(gyro.Z));

    Vector3F acceleration = gy87.GetAccelareation();

    Serial.println("V X: " + String(acceleration.X) + " | V Y: " + String(acceleration.Y) +
                   " | V Z: " + String(acceleration.Z));

    Serial.println("Cº: " + String(gy87.GetTemprature()));

    Serial.println("hPa: " + String(gy87.GetPresure()));

    delay(1000);
}
