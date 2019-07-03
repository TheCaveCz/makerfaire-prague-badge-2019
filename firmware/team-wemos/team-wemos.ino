//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//
// Licensed under MIT License.
//
//
// Firmware for Team picker box (wemos).
//
// There are surely lot of ways how this code can be made better, but it works for the specific case we had.
// Lot of assumptions about sizes/commands/behaiours is hardcoded. If you change something, don't be
// surprised that it can break something else.
//
// Use "LOLIN Wemos D1 R2 & mini" board
//
// But if you change something and it works, feel free to send a pull request!
//
// Happy hacking!
//
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <TaskScheduler.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h> // uses version 5 of the library
#include <MQTT.h>
#include <FS.h>


#define MQTT_PORT 1883
#define MQTT_HOST ""
#define MQTT_USER ""
#define MQTT_PASS ""
#define MQTT_TOPIC "thecave/mf/team-%s"
#define MQTT_CLIENT_ID "team%s"
#define MQTT_RECONNECT_INTERVAL 5000UL

#define LOG_ENABLED 1

#define WIFI_NAME ""
#define WIFI_PASS ""
#define WIFI_AP_NAME "team-"

#define OTA_PASSWORD ""

#define LOGIC_TEAM_DEFAULT_DELTA 10

String chipId = String(ESP.getChipId(), HEX);
Scheduler scheduler;

void placeholder() {} // keep this function here
