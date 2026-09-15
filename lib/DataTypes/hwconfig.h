#ifndef HWCONFIG_H
#define HWCONFIG_H

// Teensy 4.1 uses default I2C Wire pins (SDA=18, SCL=19)
// Wire.begin() automatically uses these pins

#define ADS1115_I2C_ADDR 0x48

// HX711 load cell amplifier pins and gain
#define HX711_DOUT_PIN 2 // placeholder for HX711 data pin
#define HX711_SCK_PIN 3  // placeholder for HX711 clock pin 
#define HX711_GAIN 128

#endif // HWCONFIG_H
