/**
 * @file Server.hpp
 * @author Juan Jesus Chavez Villa (janitochvvll2311@gmail.com)
 * @brief Inline module for Handle HTTP Web requests
 * @version 0.1.0
 * @date 2022-08-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Arduino.h"          // Main Arduino library
#include <ESP8266WebServer.h> // Specific device Web Server library
#include <ArduinoJson.h>      // Json writer/reader
#include "Storage.hpp"        // Storage module
#include "Station.hpp"        // Station module
#include "Light.hpp"          // Light module
#include "html.hpp"           // html/css/js files

const int HTTP_OK = 200;                 // Ok HTTP status code
const int HTTP_BAD_REQUEST = 400;        // Bad Request HTTP status code
const int HTTP_UNAUTHORIZED = 401;       // Unauthorized HTTP status code
const int HTTP_NOT_FOUND = 404;          // Not Found HTTP status code
const int HTTP_METHOD_NOT_ALLOWED = 405; // Method Not Allowed HTTP status code
const int HTTP_SERVER_ERROR = 500;       // Internal Server Error HTTP status code

ESP8266WebServer server(80);     // Specific device Web Server instance
bool server_initialized = false; // Initialized Server module flag

const auto DEFAULT_USERNAME = "SMT_ADMIN"; // Default Server Admin username
const auto DEFAULT_USERPASS = "SMT_PASS";  // Default Server Admin password

EEPROMArray<char[256]> username;        // Server Admin username persistent var
EEPROMArray<char[256]> userpass;        // Server Admin password persistent var
uint64_t login_time = 0;                // Admin login timepoint
uint64_t login_timeout = 1000 * 60 * 5; // Admin login timeout
bool user_logged = false;               // Admin logged flag

StaticJsonDocument<1024> document; // Json Document cache

/**
 * @brief Get the HTTP Request query string param values
 */
String getArg(const ESP8266WebServer &server, const String &name)
{
    auto acount = server.args();
    for (size_t i = 0; i < acount; i++)
        if (server.argName(i) == name)
        {
            auto arg = server.arg(i);
            arg.trim();
            return arg;
        }
    return "";
}

/**
 * @brief Return server stylesheet
 */
void server_Styles()
{
    server.send(HTTP_OK, "text/css", CSS_STYLES);
}

/**
 * @brief Return server javascript
 */
void server_Scripts()
{
    server.send(HTTP_OK, "text/javascript", JS_SCRIPTS);
}

/**
 * @brief Proccess login form request
 */
void server_Login()
{
    // Serial.println("Request received!");
    if (server.method() == HTTP_GET)
        server.send(HTTP_OK, "text/html", HTML_LOGIN);
    else if (server.method() == HTTP_POST)
    {
        String uname = getArg(server, "username");
        String upass = getArg(server, "userpass");
        if (uname == "")
            server.send(HTTP_BAD_REQUEST);
        else if (uname != username.get(DEFAULT_USERNAME) || upass != userpass.get(DEFAULT_USERPASS))
        {
            Serial.println("User login failed!");
            server.send(HTTP_UNAUTHORIZED, "text/html", HTML_LOGIN);
        }
        else
        {
            user_logged = true;
            login_time = millis();
            Serial.println("User login successfull!");
            server.send(HTTP_OK, "text/html", HTML_CONFIG);
        }
    }
    else
    {
        server.send(HTTP_METHOD_NOT_ALLOWED);
    }
}

/**
 * @brief Proccess logout request
 */
void server_Logout()
{
    user_logged = false;
    Serial.println("User logout");
    server.send(HTTP_OK, "text/html", HTML_LOGIN);
}

/**
 * @brief Process main page request
 */
void server_Index()
{
    if (!user_logged)
        server.send(HTTP_UNAUTHORIZED, "text/html", HTML_LOGIN);
    else
        server.send(HTTP_OK, "text/html", HTML_CONFIG);
}

/**
 * @brief Process API call for request Server settings
 */
void server_Data()
{
    if (!user_logged)
        server.send(HTTP_UNAUTHORIZED);
    else
    {
        document.clear();
        document["hostname"] = hostname.get(DEFAULT_HOSTNAME);
        document["ssidname"] = ssidname.get("");
        document["username"] = username.get(DEFAULT_USERNAME);
        document["apip"] = WiFi.softAPIP().toString();
        document["localip"] = WiFi.localIP().toString();
        document["gateway"] = WiFi.gatewayIP().toString();
        document["subnet"] = WiFi.subnetMask().toString();
        document["ledcount"] = ledcount.get(0);
        document["rowcount"] = rowcount.get(0);
        String text;
        text.reserve(1024);
        serializeJson(document, text);
        server.send(HTTP_OK, "application/json", text);
    }
}

/**
 * @brief Process config page requests.
 * Also process API call for save settings
 */
void server_Config()
{
    if (server.method() == HTTP_GET)
    {
        if (!user_logged)
            server.send(HTTP_UNAUTHORIZED, "text/html", HTML_LOGIN);
        else
            server.send(HTTP_OK, "text/html", HTML_CONFIG);
    }
    else if (server.method() == HTTP_POST)
    {
        if (!user_logged)
            server.send(HTTP_UNAUTHORIZED, "text/html", HTML_LOGIN);
        else
        {
            Serial.print("Saving configuration...");
            auto hname = getArg(server, "hostname");
            auto hpass = getArg(server, "hostpass");
            auto sname = getArg(server, "ssidname");
            auto spass = getArg(server, "ssidpass");
            auto uname = getArg(server, "username");
            auto upass = getArg(server, "userpass");
            auto lcount = getArg(server, "ledcount");
            auto rcount = getArg(server, "rowcount");
            auto lip = getArg(server, "localip");
            auto gate = getArg(server, "gateway");
            auto snet = getArg(server, "subnet");
            //
            IPAddress cache;
            if (hname != "")
                hostname.set(hname.c_str(), hname.length() + 1);
            if (hpass != "")
                hostpass.set(hpass.c_str(), hpass.length() + 1);
            if (sname != "")
                ssidname.set(sname.c_str(), sname.length() + 1);
            if (spass != "")
                ssidpass.set(spass.c_str(), spass.length() + 1);
            if (uname != "")
                username.set(uname.c_str(), uname.length() + 1);
            if (upass != "")
                userpass.set(upass.c_str(), upass.length() + 1);
            if (lcount != "")
                ledcount.set(lcount.toInt());
            if (rcount != "")
                rowcount.set(rcount.toInt());
            if (lip != "")
            {
                cache.fromString(lip);
                localip.set(cache);
            }
            if (gate != "")
            {
                cache.fromString(gate);
                gateway.set(cache);
            }
            if (snet != "")
            {
                cache.fromString(snet);
                subnet.set(cache);
            }
            Serial.println(" complete");
            server.send(HTTP_OK, "text/html", HTML_CONFIG);
        }
    }
    else
    {
        server.send(HTTP_METHOD_NOT_ALLOWED);
    }
}

/**
 * @brief Reset all persistent vars
 */
void server_Reset()
{
    Serial.print("Reseting configuration...");
    resetLight();
    resetStation();
    username.erase();
    userpass.erase();
    Serial.println(" complete");
    server.send(HTTP_OK);
}

/**
 * @brief Process API calls for turn on LED Strip range
 */
void server_SetPixelRange()
{
    if (isLightConfigured())
    {
        auto payload = getArg(server, "plain");
        deserializeJson(document, payload);
        //
        if (
            !document["channel"].is<int>() ||
            !document["lower"].is<int>() ||
            !document["upper"].is<int>() ||
            !document["a"].is<int>() ||
            !document["r"].is<int>() ||
            !document["g"].is<int>() ||
            !document["b"].is<int>() ||
            !document["clear"].is<bool>())
            server.send(HTTP_BAD_REQUEST);
        else
        {
            updateLight(
                document["channel"].as<int>(),
                document["lower"].as<int>(),
                document["upper"].as<int>(),
                document["r"].as<int>(),
                document["g"].as<int>(),
                document["b"].as<int>(),
                document["clear"].as<bool>());
            server.send(HTTP_OK);
        }
    }
    else
    {
        server.send(HTTP_SERVER_ERROR);
    }
}

/**
 * @brief Process API calls for turn off LED Strip range
 */
void server_ClearPixelRange()
{
    if (isLightConfigured())
    {
        auto payload = getArg(server, "plain");
        deserializeJson(document, payload);
        //
        if (
            !document["channel"].is<int>() ||
            !document["lower"].is<int>() ||
            !document["upper"].is<int>())
            server.send(HTTP_BAD_REQUEST);
        else
        {
            updateLight(
                document["channel"].as<int>(),
                document["lower"].as<int>(),
                document["upper"].as<int>(),
                0, 0, 0,
                false);
            server.send(HTTP_OK);
        }
    }
    else
    {
        server.send(HTTP_SERVER_ERROR);
    }
}

/**
 * @brief Initialize Server module
 */
void initServer()
{
    if (!server_initialized)
    {
        server_initialized = true;
        Serial.println("Initializing Server...");
        //
        server.on("/styles.css", HTTP_GET, server_Styles);
        server.on("/scripts.js", HTTP_GET, server_Scripts);
        server.on("/data", HTTP_GET, server_Data);
        server.on("/login", HTTP_ANY, server_Login);
        server.on("/logout", HTTP_ANY, server_Logout);
        server.on("/config", HTTP_ANY, server_Config);
        // server.on("/reset", HTTP_POST, server_Reset);
        // server.on("/light", HTTP_POST, server_Light);
        server.on("/api/v1/leds/setpixelrange", HTTP_POST, server_SetPixelRange);
        server.on("/api/v1/leds/clearpixelrange", HTTP_POST, server_ClearPixelRange);
        server.on("/", HTTP_GET, server_Index);
        //
        server.enableCORS(true);
        server.begin();
        Serial.print("Server listen port: ");
        Serial.println(server.getServer().port());
        //
        Serial.print("Username: ");
        Serial.println(username.get(DEFAULT_USERNAME));
    }
}

/**
 * @brief Reset Server module stored settings
 */
void resetServer()
{
    username.erase();
    userpass.erase();
}

/**
 * @brief Handle client session time and requests
 */
void handleClient()
{
    if (user_logged && ((login_time + login_timeout) < millis()))
    {
        user_logged = false;
        Serial.println("User logout by timeout");
    }
    server.handleClient();
}

#endif