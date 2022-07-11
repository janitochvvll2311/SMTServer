#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

const uint8_t BLINK = D4;
const char *const SSID = "SMT_RACK";
const char *const PASS = "SMT_PASS";

String _ssid = "";
String _pass = "";

AsyncWebServer server(80);

void setup()
{
    Serial.begin(9600);
    //
    WiFi.mode(WIFI_AP_STA);
    WiFi.hostname("SMT_RACK");
    WiFi.softAP(SSID, PASS);
    // WiFi.begin(_ssid, _pass);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    //
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        Serial.println("Request received");
        auto count = request->args();
        for(size_t i = 0; i<count; i++) {
            Serial.print(request->argName(i));
            Serial.print("=");
            Serial.println(request->arg(i));
        }
        request->send(200, "text/html", "<html><body><h1>It Works</h1></body></html>"); });

    server.on("/wireless", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if(!request->hasParam("ssid") || !request->hasParam("pass"))
            request->send(404, "text/plain", "Missing params");
        _ssid = request->getParam("ssid")->value();
        _pass = request->getParam("pass")->value();
        //
        request->send(200, "text/plain", "It Works"); });

    server.on("/test", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        Serial.println("Request received");
        if(request->hasArg("plain")) {
            Serial.println(request->arg("plain"));
        } else {
            Serial.println("Body does not received");
        }
        request->send(200, "text/plain", "It Works"); });
    //
    server.begin();
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED && _ssid != "" && _pass != "")
    {
        WiFi.begin(_ssid, _pass);
        Serial.print("Connecting");
        while (WiFi.status() != WL_CONNECTED)
        {
            Serial.print(".");
            delay(1000);
        }
        Serial.println("Connected");
    }
    delay(100);
}
