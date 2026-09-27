#ifndef V1_ICM20948_H
#define V1_ICM20948_H
#include <ICM20948.h>

class ICM20948
{
public:

bool begin();
float readAccel();
float readGyro();
float readMag();
float readlinearAccel();
float readQuaternion();
float readGravity();
float calculateVelocity();
float calculateOrientation();

private:
   const int   ICM20948_CS_PIN    = 36;
   const int   SERIAL_BAUD_RATE   = 115200;
   const int   READ_INTERVAL      = 100;  // milliseconds
}