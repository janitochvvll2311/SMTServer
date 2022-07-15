#include "Storage.hpp"
#include "Station.hpp"
#include "Light.hpp"
#include "Server.hpp"

EEPROMVar<bool> reset;

String input()
{
    while (!Serial.available())
        delay(100);
    auto in = Serial.readString();
    in.trim();
    return in;
}

void resetDelay()
{
    if (reset.has())
        server_Reset();
    //
    reset.set(true);
    delay(1000);
    reset.erase();
}

void setup()
{
    Serial.begin(9600);
    Serial.println("--- --- --- --- ---");
    initStorage();
    // resetDelay();
    initLight();
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
