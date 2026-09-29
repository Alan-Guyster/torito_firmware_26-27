#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_GPS.h>
#include <SD.h>

// ─── Constants ───────────────────────────────────────────────────────────────
const int   RFM96W_CS_PIN    = 8;
const int   RFM96W_RST_PIN   = 9;
const int   RFM96W_INT_PIN   = 4;
const int   SD_CS_PIN        = 2;
const float RF_FREQUENCY     = 433.0;
const int   SERIAL_BAUD_RATE = 115200;
const int   GPS_BAUD_RATE    = 115200;

// ─── GPSReader Class ─────────────────────────────────────────────────────────
class GPSReader
{
public:
   Adafruit_GPS gps;

   GPSReader() : gps(&Serial2) {}

   void begin()
   {
      gps.begin(GPS_BAUD_RATE);
      gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
      gps.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);
   }

   void update()
   {
      gps.read();
      if (gps.newNMEAreceived())
         gps.parse(gps.lastNMEA());
   }

   bool hasFix()       { return gps.fix; }
   float latitude()    { return gps.latitudeDegrees; }
   float longitude()   { return gps.longitudeDegrees; }

   float distanceTo(float lat2, float lon2)
   {
      const float R = 6371000.0;
      float dLat = radians(lat2 - latitude());
      float dLon = radians(lon2 - longitude());
      float a = sin(dLat / 2) * sin(dLat / 2) +
                cos(radians(latitude())) * cos(radians(lat2)) *
                sin(dLon / 2) * sin(dLon / 2);
      return R * 2 * atan2(sqrt(a), sqrt(1 - a));
   }
};

// ─── SDLogger Class ───────────────────────────────────────────────────────────
class SDLogger
{
public:
   bool begin()
   {
      return SD.begin(SD_CS_PIN);
   }

   void log(float lat, float lon, float alt, float spd, int sats, int fix, int rssi, float dist)
   {
      File dataFile = SD.open("receiver_log.csv", FILE_WRITE);
      if (dataFile)
      {
         dataFile.print(lat, 6); dataFile.print(",");
         dataFile.print(lon, 6); dataFile.print(",");
         dataFile.print(alt);    dataFile.print(",");
         dataFile.print(spd);    dataFile.print(",");
         dataFile.print(sats);   dataFile.print(",");
         dataFile.print(fix);    dataFile.print(",");
         dataFile.print(rssi);   dataFile.print(",");
         dataFile.println(dist);
         dataFile.close();
      }
      else
      {
         Serial.println("ERROR: Could not write to SD card.");
      }
   }
};

// ─── RadioReceiver Class ──────────────────────────────────────────────────────
class RadioReceiver
{
public:
   RH_RF95 radio;

   RadioReceiver() : radio(RFM96W_CS_PIN, RFM96W_INT_PIN) {}

   bool begin()
   {
      pinMode(RFM96W_RST_PIN, OUTPUT);
      digitalWrite(RFM96W_RST_PIN, LOW);
      delay(10);
      digitalWrite(RFM96W_RST_PIN, HIGH);
      delay(10);

      if (!radio.init())            return false;
      if (!radio.setFrequency(RF_FREQUENCY)) return false;

      radio.setTxPower(-4, false);
      radio.setSignalBandwidth(125000);
      radio.setSpreadingFactor(7);
      return true;
   }

   bool available() { return radio.available(); }
   int  rssi()      { return radio.lastRssi(); }

   bool receive(float &lat, float &lon, float &alt, float &spd, int &sats, int &fix)
   {
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

      if (!radio.recv(buf, &len)) return false;

      buf[len] = '\0';
      sscanf((char*)buf, "%f,%f,%f,%f,%d,%d",
             &lat, &lon, &alt, &spd, &sats, &fix);
      return true;
   }
};

// ─── Global Objects ───────────────────────────────────────────────────────────
GPSReader      receiverGPS;
SDLogger       sdLogger;
RadioReceiver  receiver;

// ─── setup ────────────────────────────────────────────────────────────────────
void setup()
{
   Serial.begin(SERIAL_BAUD_RATE);
   while (!Serial)
      delay(10);

   Serial.println("SOAR USF - Main Receiver V1");

   receiverGPS.begin();

   if (!sdLogger.begin())
   {
      Serial.println("ERROR: SD card init failed!");
      while (1);
   }

   if (!receiver.begin())
   {
      Serial.println("ERROR: RFM96W init failed!");
      while (1);
   }

   Serial.println("Ready. Waiting for rocket data...");
   Serial.println("---");
}

// ─── loop ─────────────────────────────────────────────────────────────────────
void loop()
{
   receiverGPS.update();

   if (receiver.available())
   {
      float rocketLat = 0, rocketLon = 0, altitude = 0, speed = 0;
      int   satellites = 0, fixQuality = 0;

      if (receiver.receive(rocketLat, rocketLon, altitude, speed, satellites, fixQuality))
      {
         int   rssi = receiver.rssi();
         float dist = -1;

         Serial.println("=== Rocket GPS Data ===");
         Serial.print("Latitude:    "); Serial.println(rocketLat, 6);
         Serial.print("Longitude:   "); Serial.println(rocketLon, 6);
         Serial.print("Altitude:    "); Serial.print(altitude); Serial.println(" m");
         Serial.print("Speed:       "); Serial.print(speed); Serial.println(" knots");
         Serial.print("Satellites:  "); Serial.println(satellites);
         Serial.print("Fix Quality: "); Serial.println(fixQuality);
         Serial.print("RSSI:        "); Serial.print(rssi); Serial.println(" dBm");

         if (receiverGPS.hasFix())
         {
            dist = receiverGPS.distanceTo(rocketLat, rocketLon);
            Serial.print("Distance:    "); Serial.print(dist); Serial.println(" m");
         }
         else
         {
            Serial.println("Distance:    Waiting for receiver GPS fix...");
         }

         sdLogger.log(rocketLat, rocketLon, altitude, speed, satellites, fixQuality, rssi, dist);
         Serial.println("---");
      }
      else
      {
         Serial.println("ERROR: Receive failed.");
      }
   }
}