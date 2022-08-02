/**
 * @file Light.hpp
 * @author Juan Jesus Chavez Villa (janitochvvll2311@gmail.com)
 * @brief Inline module for LED Strip controller
 * @version 0.1.0
 * @date 2022-08-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Arduino.h"                  // Main Arduino Library
#define FASTLED_ESP8266_RAW_PIN_ORDER // Specific device LED controller macro
#include <FastLED.h>                  // LED Strip controller library
#include <SoftwareSerial.h>           // Auxiliar hardware command sender (optional)
#include "Storage.hpp"                // Storage module

const int LED_PIN = D8; // LED Strip control pin

EEPROMVar<int> ledcount;                        // Number of leds per row persistent var
EEPROMVar<int> rowcount;                        // Number of rows per rack persistent var
CRGB *leds = nullptr;                           // Specific library LED array data
const int light_Rx = D3;                        // Auxiliar hardware command sender RX pin
const int light_Tx = D4;                        // Auxiliar hardware command sender TX pin
SoftwareSerial lightSerial(light_Rx, light_Tx); // Auxiliar emulated hardware command sender instance
bool light_initialized = false;                 // Initializes Light module flag

/**
 * @brief Checks if the Light module is initialized
 */
bool isLightConfigured()
{
    return ledcount.has() && rowcount.has();
}

/**
 * @brief Compute the total amount of controlled LEDs
 */
int totalLedCount()
{
    return ledcount.get() * rowcount.get();
}

/**
 * @brief Turn off all leds
 */
void clearLight()
{
    FastLED.clear();
    FastLED.show();
    Serial.println("CLEAR");
    lightSerial.println("CLEAR");
    delay(1000);
}

/**
 * @brief Request to turn on a LED range
 */
void updateLight(int channel, int lower, int upper, int r, int g, int b, bool clear)
{
    Serial.println("Updatig light");
    auto base = channel * ledcount.get();
    Serial.print("Start LED: ");
    Serial.println(base + lower);
    Serial.print("End LED: ");
    Serial.println(base + upper);
    //
    if (clear)
        clearLight();
    if (lower < upper)
    {
        if (channel % 2)
        {
            for (int i = lower; i < upper; i++)
                leds[base + ledcount.get() - i] = CRGB(r, g, b);
        }
        else
        {
            for (int i = lower; i < upper; i++)
                leds[base + i] = CRGB(r, g, b);
        }
        FastLED.show();
        // "SET 0 0 0 0 0"
        String command = "SET";
        command += " ";
        command += (base + lower);
        command += " ";
        command += (base + upper);
        command += " ";
        command += r;
        command += " ";
        command += g;
        command += " ";
        command += b;
        Serial.println(command);
        lightSerial.println(command);
        //
        delay(1000);
    }
}

/**
 * @brief Initialize Light module
 */
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
            FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, limit).setCorrection(TypicalSMD5050); // Initialize Specific LED Strip library
            clearLight();
            pinMode(light_Rx, INPUT);
            pinMode(light_Tx, OUTPUT);
            lightSerial.begin(9600);
            lightSerial.setTimeout(100);
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

/**
 * @brief Reset Light module stored settings
 */
void resetLight()
{
    ledcount.erase();
    rowcount.erase();
}

#endif