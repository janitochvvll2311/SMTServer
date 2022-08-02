#ifndef STATION_HPP
#define STATION_HPP

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "Storage.hpp"

const auto DEFAULT_HOSTNAME = "SMT_RACK";
const auto DEFAULT_HOSTPASS = "SMT_PASS";
const auto DEFAULT_APIP = IPAddress(192, 168, 4, 1);

EEPROMArray<char[256]> hostname;
EEPROMArray<char[256]> hostpass;
EEPROMArray<char[256]> ssidname;
EEPROMArray<char[256]> ssidpass;
EEPROMVar<IPAddress> localip;
EEPROMVar<IPAddress> gateway;
EEPROMVar<IPAddress> subnet;
bool station_initialized = false;

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

void initStation()
{
    if (!station_initialized)
    {
        station_initialized = true;
        initStorage();
        Serial.println("Initializing Station...");
        WiFi.mode(WIFI_AP_STA);
        //
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