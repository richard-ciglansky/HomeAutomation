#include <Arduino.h>
#include <ArduinoJson.h>

#ifdef ESP32

#include <WiFi.h>
#include <AsyncTCP.h>

#elif defined(ESP8266)

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

#endif

#include "ESPAsyncWebServer.h"

#include <string>
#include <PubSubClient.h>


#include "blindersAPI.h"
#include "blindersMQTT.h"
#include "webServer.h"

#define RELAY_POWER_PIN            (12)
#define RELAY_DIRECTION_PIN        (13)

#define RELAY_POWER_PIN_B            (4)
#define RELAY_DIRECTION_PIN_B        (5)

#define TOPIC      "/output/poschodie/emka/blinders"
#define TOPIC_B    "/output/poschodie/galeria/blinders"

#define BLINDER_URL_A   "/blinders/A"
#define BLINDER_URL_B   "/blinders/B"


// WiFi
const char *ssid = "Aristoteles"; // Enter your WiFi name
const char *password = "4quaDulc34quaSala";  // Enter WiFi password


WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer webServer(80);

sk::softar::iot::Blinders blindersA(RELAY_POWER_PIN, RELAY_DIRECTION_PIN);
sk::softar::iot::Blinders blindersB(RELAY_POWER_PIN_B, RELAY_DIRECTION_PIN_B);


std::map<const char*, sk::softar::iot::Blinders*, sk::softar::iot::cmp_str> blindersMQTTMap = {
  { TOPIC, &blindersA },
  { TOPIC_B, &blindersB}
};

std::map<const char*, sk::softar::iot::Blinders*, sk::softar::iot::cmp_str> blindersWebMap = {
  { BLINDER_URL_A, &blindersA },
  { BLINDER_URL_B, &blindersB }
};

sk::softar::iot::BlindersMQTT blindersMQTT(blindersMQTTMap, client, "broker.emqx.io", "emqx", "public", 1883 );


void setup() {
  // Set software serial baud to 115200;
  Serial.begin(9600);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  IPAddress ipAddr = WiFi.localIP();
  String ipAddress = ipAddr.toString();
  Serial.print("Listens on : ");
  Serial.println(ipAddress);

  blindersMQTT.setup(); 
  setupWebServer(webServer, blindersWebMap);
  Serial.print("Setup done.");
}

void loop() {
  client.loop();
}