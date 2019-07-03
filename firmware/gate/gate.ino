//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//
// Licensed under MIT License.
//
//
// Firmware for Gates and team picker (arduino)
// Controls 3x 7seg displays and buttons
//
// There are surely lot of ways how this code can be made better, but it works for the specific case we had.
// Lot of assumptions about sizes/commands/behaiours is hardcoded. If you change something, don't be
// surprised that it can break something else.
//
// Use "Arduino Pro or Pro Mini" board with "ATMega328P (5V, 16MHz)" setting
//
// But if you change something and it works, feel free to send a pull request!
//
// Happy hacking!
//

#include <EEPROM.h>
#include <TaskScheduler.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <TM1637Display.h>
#include <OneButton.h>

// enable serial line debug info output
#define LOG_ENABLED 1

#define SERIAL_COMMANDS 1


// Pin Assignments
// input - data input line from IR receiver
#define PIN_IRRX 2
// output - IR led
#define PIN_IRTX 3


#define PIN_D1_DIO A2
#define PIN_D1_CLK A3

#define PIN_D2_DIO 12
#define PIN_D2_CLK 11

#define PIN_D3_DIO A1
#define PIN_D3_CLK A0

#define PIN_BTN1_IN 6
#define PIN_BTN2_IN 8
#define PIN_BTN3_IN 7

#define PIN_BTN1_OUT 10
#define PIN_BTN2_OUT 5
#define PIN_BTN3_OUT 9



Scheduler scheduler;

// keep this function here otherwise this source will not compile!
void nothing() {}
