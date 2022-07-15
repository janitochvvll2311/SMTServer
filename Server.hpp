#ifndef SERVER_HPP
#define SERVER_HPP

#include "Arduino.h"
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "Storage.hpp"
#include "Station.hpp"
#include "Light.hpp"
#include "html.hpp"

const int HTTP_OK = 200;
const int HTTP_BAD_REQUEST = 400;
const int HTTP_UNAUTHORIZED = 401;
const int HTTP_NOT_FOUND = 404;
const int HTTP_METHOD_NOT_ALLOWED = 405;
const int HTTP_SERVER_ERROR = 500;

ESP8266WebServer server(80);
bool server_initialized = false;

const auto DEFAULT_USERNAME = "SMT_ADMIN";
const auto DEFAULT_USERPASS = "SMT_PASS";

EEPROMArray<char[256]> username;
EEPROMArray<char[256]> userpass;
uint64_t login_time = 0;
uint64_t login_timeout = 1000 * 60 * 5;
bool user_logged = false;

StaticJsonDocument<1024> document;

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

void server_Styles()
{
    server.send(HTTP_OK, "text/css", CSS_STYLES);
}

void server_Scripts()
{
    server.send(HTTP_OK, "text/javascript", JS_SCRIPTS);
}

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

void server_Logout()
{
    user_logged = false;
    Serial.println("User logout");
    server.send(HTTP_OK, "text/html", HTML_LOGIN);
}

void server_Index()
{
    if (!user_logged)
        server.send(HTTP_UNAUTHORIZED, "text/html", HTML_LOGIN);
    else
        server.send(HTTP_OK, "text/html", HTML_CONFIG);
}

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
        document["ledcount"] = ledcount.get(0);
        document["rowcount"] = rowcount.get(0);
        String text;
        text.reserve(1024);
        serializeJson(document, text);
        server.send(HTTP_OK, "application/json", text);
    }
}

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
            //
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
            Serial.println(" complete");
            server.send(HTTP_OK, "text/html", HTML_CONFIG);
        }
    }
    else
    {
        server.send(HTTP_METHOD_NOT_ALLOWED);
    }
}

void server_Reset()
{
    Serial.print("Reseting configuration...");
    hostname.erase();
    hostpass.erase();
    ssidname.erase();
    ssidpass.erase();
    username.erase();
    userpass.erase();
    ledcount.erase();
    rowcount.erase();
    Serial.println(" complete");
    server.send(HTTP_OK);
}

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
        server.begin();
        Serial.print("Server listen port: ");
        Serial.println(server.getServer().port());
        //
        Serial.print("Username: ");
        Serial.println(username.get(DEFAULT_USERNAME));
    }
}

void resetServer()
{
    username.erase();
    userpass.erase();
}

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