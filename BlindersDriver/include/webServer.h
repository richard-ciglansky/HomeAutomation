#include <Arduino.h>

#ifdef ESP32

#include <WiFi.h>
#include <AsyncTCP.h>

#elif defined(ESP8266)

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

#endif

#include "ESPAsyncWebServer.h"

#include <string>
#include <map>
#include <ESP8266WiFi.h>

#include "blindersAPI.h"

void setupWebServer(AsyncWebServer& server, std::map<const char*, sk::softar::iot::Blinders*, sk::softar::iot::cmp_str>& blinders );
