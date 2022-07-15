#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Arduino.h"
#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
#include "Storage.hpp"

const int LED_PIN = D8;

EEPROMVar<int> ledcount;
EEPROMVar<int> rowcount;
CRGB *leds = nullptr;
bool light_initialized = false;

bool isLightConfigured()
{
    return ledcount.has() && rowcount.has();
}

int totalLedCount()
{
    return ledcount.get() * rowcount.get();
}

void clearLight()
{
    FastLED.clear();
    FastLED.show();
    delay(1000);
}

void updateLight(int channel, int lower, int upper, int r, int g, int b, bool clear)
{
    Serial.println("Updatig light");
    auto base = channel * ledcount.get();
    Serial.print("Start LED: ");
    Serial.println(base + lower);
    Serial.print("End LED: ");
    Serial.println(base + upper);
    if (clear)
        clearLight();
    if (lower < upper)
    {
        for (int i = lower; i < upper; i++)
            leds[base + i] = CRGB(r, g, b);
        FastLED.show();
        delay(1000);
    }
}

void initLight()
{
    if (!light_initialized)
    {
        light_initialized = true;
        Serial.println("Initializing light...");
        // pinMode(LED_PIN, OUTPUT);
        if (isLightConfigured())
        {
            if (leds)
            {
                delete[] leds;
                leds = nullptr;
            }
            auto limit = totalLedCount();
            leds = new CRGB[limit];
            FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, limit).setCorrection(TypicalSMD5050);
            clearLight();
            Serial.print("LEDs per row: ");
            Serial.println(ledcount.get());
            Serial.print("Row count: ");
            Serial.println(rowcount.get());
        }
        else
        {
            Serial.println("No LED configuration");
        }
    }
}

#endif