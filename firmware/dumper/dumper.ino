//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//
//
// Arduino firmware for portable transponder dumper and badge printer
//
// Use Arduino Pro Mini 3V3 8MHz (portable dumper)
// Or Arduino Genuino Uno board (badge printer)
//


// define this for badge printer
#define USE_PRINTER 1
#define SERIAL_COMMANDS 1

#if USE_PRINTER
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SoftwareSerial.h"
#include "Adafruit_Thermal.h"
#endif
#include <TaskScheduler.h>


#define LOG_ENABLED 1
#define PIN_IRRX 2
#define PIN_IRTX 3
#define PIN_IRRX_EN 4
#define PIN_PRINT_TX A0 // labeled RX on printer
#define PIN_PRINT_RX A1 // labeled TX on printer
#define PIN_PRINT_DTR A2

Scheduler scheduler;

#if SERIAL_COMMANDS
void printHex(const uint8_t b) {
  if (b < 16) Serial.print('0');
  Serial.print(b, HEX);
}

void printHexWord(const uint16_t w) {
  printHex(w >> 8);
  printHex(w & 0xff);
}
#endif
