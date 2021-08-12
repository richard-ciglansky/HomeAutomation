#include <Arduino.h>

#ifdef ESP32

#include <WiFi.h>
#include <AsyncTCP.h>

#elif defined(ESP8266)

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

#endif

#include <string>
#include <functional>
#include <map>
#include <PubSubClient.h>

#include "blindersAPI.h"

#define COMMAND_UP  "up"
#define COMMAND_DOWN "down"
#define COMMAND_STOP "stop"

namespace sk::softar::iot {


    class BlindersMQTT {
    public:
        std::map<const char*, Blinders*, sk::softar::iot::cmp_str>& blindersMap;
        PubSubClient& client;
        std::string broker;
        std::string userName;
        std::string password;
        int port;

        BlindersMQTT(std::map<const char*, Blinders*, sk::softar::iot::cmp_str>& aBlinders, PubSubClient& aClient, std::string aBroker, std::string anUserName, std::string aPassword, int aPort = 1883)
        : blindersMap(aBlinders), client(aClient), broker(aBroker), userName(anUserName), password(aPassword), port(aPort)
        { 
            // for( auto bpIt = aBlinders.begin(); bpIt != aBlinders.end(); bpIt++ )
            //     blindersMap[bpIt->first] = bpIt->second;
        }
        
        void setup();
        std::function<void(Blinders&)> getPayloadAction(char *topic, byte *payload, unsigned int length);
        Blinders* getPayloadBlinders(char *topic, byte *payload, unsigned int length);
        void callback(char *topic, byte *payload, unsigned int length);

    };
}
