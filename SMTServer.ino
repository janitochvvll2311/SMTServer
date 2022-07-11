#include "Storage.hpp"
#include "Station.hpp"
#include "Server.hpp"

String input()
{
    while (!Serial.available())
        delay(100);
    return Serial.readString();
}

void setup()
{
    Serial.begin(9600);
    initStorage();
    initStation();
    initServer();
}

void loop()
{
    handleClient();
}
