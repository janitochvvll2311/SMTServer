#include "Storage.hpp"
#include "Station.hpp"
#include "Server.hpp"

String input()
{
    while (!Serial.available())
        delay(100);
    auto in = Serial.readString();
    in.trim();
    return in;
}

void setup()
{
    Serial.begin(9600);
    Serial.println("--- --- --- --- ---");
    initStorage();
    initStation();
    initServer();
}

void loop()
{
    handleClient();
    delay(100);
    if (Serial.available())
    {
        auto in = input();
        if (in == "reset")
            server_Reset();
    }
}
