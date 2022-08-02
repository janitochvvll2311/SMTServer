/**
 * @file SMTServer.ino
 * @author Juan Jesus Chavez Villa (janitochvvll2311@gmail.com)
 * @brief Main program file of SMT Server project for Pick to Light System
 * @version 0.1.0
 * @date 2022-08-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Storage.hpp" // Include Persistent Configuration Variables
#include "Station.hpp" // Include WiFi connectivity
#include "Light.hpp"   // Include LED Strip controller
#include "Server.hpp"  // Include Web Server

/**
 * @brief Flag to reset the configuration.
 * Press the Reset Button twice to reset.
 */
EEPROMVar<bool> reset;

/**
 * @brief Read serial input blocking
 */
String input()
{
    while (!Serial.available())
        delay(100);
    auto in = Serial.readString();
    in.trim();
    return in;
}

/**
 * @brief Reset boot step.
 */
void resetDelay()
{
    // If the reset flag is set
    // Reset all modules via server_Reset();
    if (reset.has())
        server_Reset();
    //
    reset.set(true);
    delay(1000);
    reset.erase();
}

/**
 * @brief Boot setup step.
 */
void setup()
{
    // Serial initialization
    Serial.begin(9600);
    Serial.println("--- --- --- --- ---");
    //
    initStorage(); // Initialize Storage module
    resetDelay();  // Reset step
    initLight();   // Initialize Light module
    initStation(); // Initialize Station module
    initServer();  // Initialize Server module
}

/**
 * @brief Main loop
 */
void loop()
{
    handleClient(); // Handle HTTP Web Request
    delay(100);
    // Read Serial commands
    if (Serial.available())
    {
        auto in = input();
        if (in == "reset")
            server_Reset();
    }
}
