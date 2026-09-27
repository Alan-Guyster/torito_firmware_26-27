#include <SPI.h>
#include <MS5607.h>

const int   MS5607_CS_PIN      = 10;
const int   SERIAL_BAUD_RATE   = 115200;
const int   READ_INTERVAL      = 100;

class MS5607Sensor
{
public:
   MS5607 sensor;
   float  maxAltitude;

   MS5607Sensor() : sensor(MS5607_CS_PIN, false), maxAltitude(-9999.0) {}

   void begin()
   {
      sensor.init();
   }

   float readTemperature()
   {
      sensor.read();
      return sensor.getTemperature() / 100.0;  // raw value is in centi-degrees C
   }

   float readPressure()
   {
      sensor.read();
      return sensor.getPressure() / 100.0;  // raw value is in Pa, convert to hPa
   }

   float readAltitude()
   {
      float pressure = readPressure();
      return 44330.0 * (1.0 - pow(pressure / 1013.25, 0.1903));
   }

   void updateMaxAltitude(float currentAltitude)
   {
      if (currentAltitude > maxAltitude)
         maxAltitude = currentAltitude;
   }

   float getMaxAltitude()
   {
      return maxAltitude;
   }

   void print()
   {
      sensor.read();
      float temp     = sensor.getTemperature() / 100.0;
      float pressure = sensor.getPressure() / 100.0;
      float altitude = 44330.0 * (1.0 - pow(pressure / 1013.25, 0.1903));

      updateMaxAltitude(altitude);

      Serial.println("=== MS5607 Data ===");
      Serial.print("Temperature:   "); Serial.print(temp); Serial.println(" C");
      Serial.print("Pressure:      "); Serial.print(pressure); Serial.println(" hPa");
      Serial.print("Altitude:      "); Serial.print(altitude); Serial.println(" m");
      Serial.print("Max Altitude:  "); Serial.print(maxAltitude); Serial.println(" m");
      Serial.println("---");
   }
};

MS5607Sensor altimeter;
uint32_t     lastReadTime = 0;

void setup()
{
   Serial.begin(SERIAL_BAUD_RATE);
   while (!Serial)
      delay(10);

   Serial.println("SOAR USF - MS5607 Reader");

   SPI.begin();
   altimeter.begin();

   Serial.println("Ready. Reading sensor data...");
   Serial.println("---");
}

void loop()
{
   if (millis() - lastReadTime >= READ_INTERVAL)
   {
      lastReadTime = millis();
      altimeter.print();
   }
}