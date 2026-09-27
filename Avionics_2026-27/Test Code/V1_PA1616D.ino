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

#include <Adafruit_GPS.h>

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
const unsigned int GPS_BAUD_RATE     = 115200;  // Maximum baud rate for PA1616D
const unsigned int PRINT_INTERVAL    = 100;     // Print interval in milliseconds (10Hz)

//------------------------------------------------------------------------------
// GPSReader Class
//------------------------------------------------------------------------------
/**
 * Manages initialization, reading, and parsing of NMEA data from the
 * PA1616D GPS module via Serial1.
 */
class GPSReader
{
public:
   Adafruit_GPS gps;

   GPSReader() : gps(&Serial2) {}

   //--------------------------------------------------------------------------
   // begin
   //--------------------------------------------------------------------------
   /**
    * Initializes the GPS module with maximum baud rate and 10Hz update rate.
    */
   void begin()
   {
      gps.begin(9600);
      gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
      gps.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);
   }

   //--------------------------------------------------------------------------
   // update
   //--------------------------------------------------------------------------
   /**
    * Reads and parses incoming NMEA sentences from the GPS module.
    * Must be called repeatedly in the main loop.
    */
   void update()
   {
      gps.read();
      if (gps.newNMEAreceived())
         gps.parse(gps.lastNMEA());
   }

   bool  hasFix()     { return gps.fix; }
   float latitude()   { return gps.latitudeDegrees; }
   float longitude()  { return gps.longitudeDegrees; }
   float altitude()   { return gps.altitude; }
   float speed()      { return gps.speed; }
   int   satellites() { return (int)gps.satellites; }
   int   fixQuality() { return (int)gps.fixquality; }

   //--------------------------------------------------------------------------
   // print
   //--------------------------------------------------------------------------
   /**
    * Prints parsed GPS data to the Serial Monitor.
    */
   void print()
   {
      Serial.println("=== GPS Data ===");
      Serial.print("Latitude:    "); Serial.println(latitude(), 6);
      Serial.print("Longitude:   "); Serial.println(longitude(), 6);
      Serial.print("Altitude:    "); Serial.print(altitude()); Serial.println(" m");
      Serial.print("Speed:       "); Serial.print(speed()); Serial.println(" knots");
      Serial.print("Satellites:  "); Serial.println(satellites());
      Serial.print("Fix Quality: "); Serial.println(fixQuality());
      Serial.println("---");
   }
};

//------------------------------------------------------------------------------
// Global Objects
//------------------------------------------------------------------------------
GPSReader gpsReader;
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

   gpsReader.begin();
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
   gpsReader.update();

   if (millis() - lastPrintTime >= PRINT_INTERVAL)
   {
      lastPrintTime = millis();

      if (gpsReader.hasFix())
         gpsReader.print();
      else
         Serial.println("Waiting for GPS fix...");
   }
}