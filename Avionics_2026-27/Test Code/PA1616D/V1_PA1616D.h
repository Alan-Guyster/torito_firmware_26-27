#ifndef GPS_READER_H
#define GPS_READER_H
#include <Adafruit_GPS.h>

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
   const int   GPS_BAUD_RATE      = 9600;
   const int   SERIAL_BAUD_RATE   = 115200;
   const int   PRINT_INTERVAL     = 100;  // milliseconds
};