#include <SPI.h>
#include <MS5607.h>

MS5607 alt;
uint32_t     lastReadTime = 0;

void setup()
{
   Serial.begin(SERIAL_BAUD_RATE);
   while (!Serial)
      delay(10);

   Serial.println("SOAR USF - MS5607 Reader");

   SPI.begin();
   alt.begin();

   Serial.println("Ready. Reading sensor data...");
   Serial.println("---");
}

void loop()
{
   if (millis() - lastReadTime >= READ_INTERVAL)
   {
      lastReadTime = millis();
      alt.print();
   }
}