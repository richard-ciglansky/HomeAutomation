#pragma once

#include <Arduino.h>
#include <string>

namespace sk::softar::iot {


    struct cmp_str
    {
    bool operator()(char const *a, char const *b) const
    {
        return std::strcmp(a, b) < 0;
    }
    };

    class Blinders {
    public:
        Blinders(uint8_t aPowerPin, uint8_t aDirectionPin) 
        : powerPin(aPowerPin), directionPin(aDirectionPin)
        { }

        Blinders(const Blinders& b)
        : powerPin(b.powerPin), directionPin(b.directionPin)
        { }

        Blinders& operator=(const Blinders& b) {
            powerPin = b.powerPin;
            directionPin = b.directionPin;
            return *this;
        }

    private:
        uint8_t powerPin;
        uint8_t directionPin;

        virtual void blindersMove(uint8_t direction) {
            std::string m = "PIN " + std::to_string(directionPin) + std::string(" = ") + std::to_string(direction);
            Serial.println(m.c_str());
            m = "PIN " + std::to_string(powerPin) + std::string(" = ") + std::to_string(HIGH);
            Serial.println(m.c_str());
            digitalWrite(directionPin, direction);
            digitalWrite(powerPin, HIGH);
        };  

    public:
        virtual void blindersUp() { blindersMove(HIGH); }
        virtual void blindersDown() { blindersMove(LOW); }
        virtual void blindersStop() { 
                std::string m = "PIN " + std::to_string(powerPin) + std::string(" = ") + std::to_string(LOW);
                Serial.println(m.c_str());
                digitalWrite(powerPin, LOW); 
            }

    };

}