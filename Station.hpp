/**
 * @file Station.hpp
 * @author Juan Jesus Chavez Villa (janitochvvll2311@gmail.com)
 * @brief Inline module for WiFi connectivity
 * @version 0.1.0
 * @date 2022-08-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef STATION_HPP
#define STATION_HPP

#include "Arduino.h"     // Main Arduino library
#include <ESP8266WiFi.h> // Specific device WiFi library
#include "Storage.hpp"   // Storage module

const auto DEFAULT_HOSTNAME = "SMT_RACK";            // Default hostname and AP SSID
const auto DEFAULT_HOSTPASS = "SMT_PASS";            // Default AP SSID password
const auto DEFAULT_APIP = IPAddress(192, 168, 4, 1); // Default AP IP

EEPROMArray<char[256]> hostname;  // Hostname/AP SSID persistent var
EEPROMArray<char[256]> hostpass;  // AP SSID password persistent var
EEPROMArray<char[256]> ssidname;  // Internet SSID to connect persistent var
EEPROMArray<char[256]> ssidpass;  // Internet SSID to connect persistent var
EEPROMVar<IPAddress> localip;     // Device IP for Internet connection persistent var
EEPROMVar<IPAddress> gateway;     // Internet connection Gateway IP persistent var
EEPROMVar<IPAddress> subnet;      // Internet Subnet Mask persistent var
bool station_initialized = false; // Initialize Station module flag

/**
 * @brief Attempt to connect the device to WiFi
 * using the stored configuration (if exists)
 */
void connectWiFi()
{
    if (!ssidname.has())
    {
        Serial.println("No SSID/PASS configured");
        return;
    }
    //
    if (localip.has() && gateway.has() && subnet.has())
    {
        WiFi.config(localip.get(), gateway.get(), subnet.get());
    }
    //
    Serial.print("Connecting to ");
    Serial.print(ssidname.get(""));
    WiFi.begin(ssidname.get(""), ssidpass.get(""));
    for (size_t i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++)
    {
        Serial.print('.');
        delay(1000);
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println(" connected!");
        Serial.print("Gateway: ");
        Serial.println(WiFi.gatewayIP());
        Serial.print("Local IP: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println(" failed!");
    }
}

/**
 * @brief Initialize Station module
 */
void initStation()
{
    if (!station_initialized)
    {
        station_initialized = true;
        initStorage();
        Serial.println("Initializing Station...");
        WiFi.mode(WIFI_AP_STA);
        // Initialize local Access Point
        WiFi.hostname(hostname.get(DEFAULT_HOSTNAME));
        WiFi.softAP(hostname.get(DEFAULT_HOSTNAME), hostpass.get(DEFAULT_HOSTPASS));
        Serial.print("Hostname: ");
        Serial.println(WiFi.getHostname());
        Serial.print("AP IP: ");
        Serial.println(WiFi.softAPIP());
        //
        connectWiFi();
    }
}

/**
 * @brief Reset Station module stored settings
 */
void resetStation()
{
    hostname.erase();
    hostpass.erase();
    ssidname.erase();
    ssidpass.erase();
    localip.erase();
    gateway.erase();
    subnet.erase();
}

#endif