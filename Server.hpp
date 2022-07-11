#ifndef SERVER_HPP
#define SERVER_HPP

#include "Arduino.h"
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "Storage.hpp"
#include "Station.hpp"
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
            Serial.println("User login successfull!");
            server.send(HTTP_OK, "text/html", HTML_CONFIG);
        }
    }
    else
    {
        server.send(HTTP_METHOD_NOT_ALLOWED);
    }
}

void server_Index()
{
    if (!user_logged)
        server.send(HTTP_OK, "text/html", HTML_LOGIN);
    else
        return server.send(HTTP_OK, "text/html", HTML_CONFIG);
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
        String text;
        text.reserve(1024);
        serializeJson(document, text);
        server.send(HTTP_OK, "application/json", text);
    }
}

void server_Config()
{
    if (server.method() == HTTP_GET)
        server.send(HTTP_OK, "text/html", HTML_CONFIG);
    else if (server.method() == HTTP_POST)
    {
        if (!user_logged)
            server.send(HTTP_UNAUTHORIZED);
        else
        {
            Serial.print("Saving configuration...");
            auto hname = getArg(server, "hostname");
            auto hpass = getArg(server, "hostpass");
            auto sname = getArg(server, "ssidname");
            auto spass = getArg(server, "ssidpass");
            auto uname = getArg(server, "username");
            auto upass = getArg(server, "userpass");
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
    Serial.println(" complete");
    server.send(HTTP_OK);
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
        server.on("/config", HTTP_ANY, server_Config);
        server.on("/reset", HTTP_POST, server_Reset);
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
    server.handleClient();
}

#endif