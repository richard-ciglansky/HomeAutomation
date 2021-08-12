#include "blindersMQTT.h"
#include <ArduinoJson.h>

namespace sk::softar::iot {
    

    void BlindersMQTT::setup()
    {
        //connecting to a mqtt broker
        client.setServer(broker.c_str(), port);
        client.setCallback( [&](char *topic, byte *payload, unsigned int length) -> void { this->callback(topic, payload, length); } );
        while (!client.connected()) {
            String client_id = "esp8266-client-";
            client_id += String(WiFi.macAddress());
            Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
            if (client.connect(client_id.c_str(), userName.c_str(), password.c_str())) {
                Serial.println("Public emqx mqtt broker connected");
            } else {
                Serial.print("failed with state ");
                Serial.print(client.state());
                delay(2000);
            }
        }
        // publish and subscribe
        Serial.println("Publishing topic");
        client.publish("esp8266/clandestino/test", "hello emqx");
        for ( auto bit = blindersMap.begin(); bit != blindersMap.end(); bit++) {
            Serial.print("Subscribing to topic: ");
            Serial.println(bit->first);
            client.subscribe(bit->first);
        }

    }


    std::function<void(Blinders&)> BlindersMQTT::getPayloadAction(char *topic, byte *payload, unsigned int length)
    {
        DynamicJsonDocument jsonDoc(1024);
        Serial.println("Deserializing JSON");
        deserializeJson( jsonDoc, (const char*) payload, length);
        Serial.println("Deserialized OK.");
        std::string command = jsonDoc["command"].as<std::string>();
        Serial.print("JSON Command :");
        Serial.println(command.c_str());
        if ( !command.compare(COMMAND_UP) )
            return std::function<void(Blinders&)>( [&](Blinders& b) -> void { b.blindersUp(); } );
        else if (!command.compare(COMMAND_DOWN))
            return std::function<void(Blinders&)>( [&](Blinders& b) -> void { b.blindersDown(); } );
        else if (!command.compare(COMMAND_STOP))
            return std::function<void(Blinders&)>( [&](Blinders& b) -> void { b.blindersStop(); } );
        Serial.print("Command not recognized.");
        return std::function<void(Blinders&)>( [&](Blinders& b) -> void { b.blindersStop(); } );
    }

    Blinders* BlindersMQTT::getPayloadBlinders(char *topic, byte *payload, unsigned int length)
    {
        auto tit = blindersMap.find(topic);
        Serial.println("tit initialized");
        if ( tit != blindersMap.end()) {
            Serial.println("tit found");
            return tit->second; 
        }
        Serial.println("tit NOT found");
        return NULL;
    }


    void  BlindersMQTT::callback(char *topic, byte *payload, unsigned int length) {
        Serial.print("Message arrived in topic: ");
        Serial.println(topic);
        Serial.print("Message:");
        for (unsigned int i = 0; i < length; i++) {
            Serial.print((char) payload[i]);
        }
        Serial.println();
        Serial.println("-----------------------");

        Blinders* blinders = getPayloadBlinders(topic, payload, length);
        if ( blinders ) {
            Serial.println("Blinders found.");
            std::function<void(Blinders&)> action = getPayloadAction(topic, payload, length);
            Serial.println("Action found.");
            action(*blinders);
        } else {
            Serial.print("Cannot find topic :");
            Serial.println(topic);
        }
    }



}