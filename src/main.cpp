#include "Common.h"
#include "GY-87.h"


void loop(Gy87 &gy87);

extern "C" void app_main(void)
{

    Gy87 gy87;
    gy87.Initialice();
    while (true)
    {
        loop(gy87);
    };

}

void loop(Gy87 &gy87)
{

    printf("Grados N: %d \n", gy87.GetNPoleRadians());

    Vector3 gyro = gy87.GetGyro();

    printf("Gyro X: %d | Gyro  Y: %d | Gyro Z: %d \n", gyro.X, gyro.Y, gyro.Z);

    Vector3F acceleration = gy87.GetAcceleration();

    printf("V X: %f  | V Y: %f  | V Z: %f \n", acceleration.X, acceleration.Y, acceleration.Z);

    printf("Velocidad: %f m/s \n", gy87.GetLinearAcceleration());

    printf("Cº: %f \n", gy87.GetTemprature());

    printf("hPa: %f \n", gy87.GetPresure());
}
