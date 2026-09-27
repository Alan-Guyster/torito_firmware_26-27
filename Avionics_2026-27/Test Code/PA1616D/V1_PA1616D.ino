//$Header$
//------------------------------------------------------------------------------
//                         GpsReader_SerialOnly
//------------------------------------------------------------------------------
// SOAR USF: Student Organization for Aerospace Research
//
// **Legal** [Standard open-source license to be inserted before release]
//
// Author: Noureldin Zeinelabedin
// Created: 2026/09/10
//
/**
 * Reads NMEA strings from the PA1616D GPS module and prints parsed data
 * to the Serial Monitor. For testing purposes only — no radio transmission.
 *
 * @note GPS module is wired to Serial1 on Teensy 4.1.
 *       Requires Adafruit GPS Library.
 */
//------------------------------------------------------------------------------
#include "V1_PA1616D.h"

//------------------------------------------------------------------------------
// Global Objects
//------------------------------------------------------------------------------
GPSReader gps;
uint32_t  lastPrintTime = 0;

//------------------------------------------------------------------------------
// setup
//------------------------------------------------------------------------------
/**
 * Initializes Serial communication and GPS module.
 *
 * @note Exception to NASA naming standard: Arduino requires setup() and loop()
 *       to be lowercase in order to be recognized as program entry points.
 */
void setup()
{
   Serial.begin(115200);
   while (!Serial)
      delay(10);

   gps.begin();
   Serial.println("SOAR USF - GPS Reader (Serial Only)");
   Serial.println("---");
}

//------------------------------------------------------------------------------
// loop
//------------------------------------------------------------------------------
/**
 * Main loop. Continuously reads and prints GPS data at 10Hz.
 *
 * @note Exception to NASA naming standard: see setup() note above.
 */
void loop()
{
   gps.update();

   if (millis() - lastPrintTime >= PRINT_INTERVAL)
   {
      lastPrintTime = millis();

      if (gps.hasFix())
         gps.print();
      else
         Serial.println("Waiting for GPS fix...");
   }
}