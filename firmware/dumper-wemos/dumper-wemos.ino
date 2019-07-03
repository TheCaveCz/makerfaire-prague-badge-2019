//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//
// Licensed under MIT License.
//
//
// Firmware for Transponder dumper (wemos).
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
#include <TaskScheduler.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h> // uses version 5 of the library
#include <MQTT.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"
#include <OneButton.h>


#define MQTT_PORT 1883
#define MQTT_HOST ""
#define MQTT_USER ""
#define MQTT_PASS ""
#define MQTT_TOPIC "thecave/mf/printer-%s"
#define MQTT_CLIENT_ID "printer%s"
#define MQTT_RECONNECT_INTERVAL 5000UL

#define LOG_ENABLED 1
#define DEBUG_SERVER 0

#define WIFI_NAME ""
#define WIFI_PASS ""
#define WIFI_AP_NAME "printer-"

#define GATE_PAIR_DEFAULT_TIMEOUT 1800

#define OTA_PASSWORD ""


String chipId = String(ESP.getChipId(), HEX);
Scheduler scheduler;

void placeholder() {} // keep this function here
