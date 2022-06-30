#pragma once

#include "Ticker.h"
#include <Arduino.h>
#include <string>

namespace sk::softar::iot {

    struct cmp_str
    {
        bool operator()(char const *a, char const *b) const    { return std::strcmp(a, b) < 0; }
    };

    /*
     *  Encapsulating blinder device : power pin and direction pin.
     */
    class Blinders {

    private:
        uint8_t powerPin;           // pin controlling the relay switching the power to the motor ON and OFF
        uint8_t directionPin;       // pin controlling the relay directing the power to one of its terminals  NO/NC
        Ticker movementTimeout;     // when blinder's motor starts to  work, it expects move command to be received at least each timeoutValue miliseconds, 
                                    // otherwise the movementTimeout timer fires and stops the movement.
        uint32_t timeoutValue;

    public:
        Blinders(uint8_t aPowerPin, uint8_t aDirectionPin, uint32_t aTimeoutValue = 500) 
        : powerPin(aPowerPin), directionPin(aDirectionPin), timeoutValue(aTimeoutValue)//, movementTimeout( [this](void) -> void { Serial.println("Timeout stopping movement."); this->blindersStop(); }, 500000, 1)
        { }

        Blinders(const Blinders& b)
        : powerPin(b.powerPin), directionPin(b.directionPin)//, movementTimeout( [this](void) -> void { Serial.println("Timeout stopping movement."); this->blindersStop(); }, 500000, 1)
        { }

        Blinders& operator=(const Blinders& b) {
            powerPin = b.powerPin;
            directionPin = b.directionPin;
            timeoutValue = b.timeoutValue;
            movementTimeout.detach();
            return *this;
        }


        virtual void blindersMove(uint8_t direction) {
            std::string m = "PIN " + std::to_string(directionPin) + std::string(" = ") + std::to_string(direction);
            Serial.println(m.c_str());
            m = "PIN " + std::to_string(powerPin) + std::string(" = ") + std::to_string(HIGH);
            Serial.println(m.c_str());

            // setup correct direction
            digitalWrite(directionPin, direction);

            // enforce power to the proper side of the motor
            digitalWrite(powerPin, HIGH);

            // setup timeout for working without another command
            movementTimeout.detach();
            movementTimeout.once_ms_scheduled(timeoutValue, [this](void) -> void { 
                // No command arrived during past timeoutValue miliseconds => stopping the blinder
                Serial.println("Timeout stopping movement."); 
                this->blindersStop(); 
                } );
        };  

    public:
        virtual void blindersUp() { blindersMove(HIGH); }
        virtual void blindersDown() { blindersMove(LOW); }
        virtual void blindersStop() { 
                std::string m = "PIN " + std::to_string(powerPin) + std::string(" = ") + std::to_string(LOW);
                Serial.println(m.c_str());

                // cutting OFF the power from the device
                digitalWrite(powerPin, LOW); 

                // cancel the timeout
                movementTimeout.detach();
            }

    };

}