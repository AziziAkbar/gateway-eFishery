#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "ArduinoJson.h"

AsyncWebServer server(80);
DynamicJsonDocument docConfigDevice(1024);
struct Struct_Config_DataDevice
{
    char clientID[30] = "";
    char username[30] = "";
    char password[30] = "";
};
Struct_Config_DataDevice ConfigDataStructDevice;

const char *namaFileConfigDevice = "/config_device.json";
const char *http_username_vijet = "fishgateway1";
const char *http_password_vijet = "eFishery";

// Save config
bool saveConfigDevice()
{
    // Delete existing file, otherwise the configuration is appended to the file

    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return false;
    }

    File file = SPIFFS.open(namaFileConfigDevice, FILE_WRITE);

    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use arduinojson.org/assistant to compute the capacity.

    if (serializeJson(docConfigDevice, file) == 0)
    {

        Serial.println(F("Failed to write to file"));
        file.close();
        return false;
    }
    else
    {
        file.close();
        // reset char
        bzero(ConfigDataStructDevice.clientID, 30);
        bzero(ConfigDataStructDevice.username, 30);
        bzero(ConfigDataStructDevice.password, 30);
        // copy
        serializeJson(docConfigDevice, Serial);
        strcpy(ConfigDataStructDevice.clientID, docConfigDevice["clientID"]);
        strcpy(ConfigDataStructDevice.username, docConfigDevice["username"]);
        strcpy(ConfigDataStructDevice.password, docConfigDevice["password"]);
        Serial.println(F("sukses to write to file"));

        return true;
    }
}

void ESPAsyncTCP()
{
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    server.onNotFound([](AsyncWebServerRequest *request)
                      {
    if (request->method() == HTTP_OPTIONS)
    {
      request->send(200);
    }
    else
    {
      request->send(404);
    } });

    //set clientID mqtt
    server.on("/clientid", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    //Check if GET parameter exists
    if (!request->authenticate(http_username_vijet, http_password_vijet))
      return request->requestAuthentication();
    if (request->hasParam("clientID"))
    {

      const char *clientID = request->getParam("clientID")->value().c_str();
      if (strlen(clientID) < 5)
      {
        return request->send(400, "text/plain", "INVALID ip target");
      }
      docConfigDevice["clientID"] = clientID;
      bool isOk = saveConfigDevice();
      if (isOk)
      {
        String strnya = "";
        request->send(200, "text/plain", clientID);
        return;
      }
      else
      {
        return request->send(400, "text/plain", "failed saved config device");
      }
    }
    else
    {
      return request->send(400, "text/plain", "incomplete param");
    } });

    // set username mqtt
    server.on("/usermqtt", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    //Check if GET parameter exists
    if (!request->authenticate(http_username_vijet, http_password_vijet))
      return request->requestAuthentication();
    if (request->hasParam("username"))
    {

      const char *username = request->getParam("username")->value().c_str();
      if (strlen(username) < 5)
      {
        return request->send(400, "text/plain", "INVALID ip target");
      }
      docConfigDevice["username"] = username;
      bool isOk = saveConfigDevice();
      if (isOk)
      {
        String strnya = "";
        request->send(200, "text/plain", username);
        return;
      }
      else
      {
        return request->send(400, "text/plain", "failed saved config device");
      }
    }
    else
    {
      return request->send(400, "text/plain", "incomplete param");
    } });

    server.on("/passmqtt", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    //Check if GET parameter exists
    if (!request->authenticate(http_username_vijet, http_password_vijet))
      return request->requestAuthentication();
    if (request->hasParam("passsword"))
    {

      const char *passsword = request->getParam("passsword")->value().c_str();
      if (strlen(passsword) < 5)
      {
        return request->send(400, "text/plain", "INVALID ip target");
      }
      docConfigDevice["passsword"] = passsword;
      bool isOk = saveConfigDevice();
      if (isOk)
      {
        String strnya = "";
        request->send(200, "text/plain", passsword);
        return;
      }
      else
      {
        return request->send(400, "text/plain", "failed saved config device");
      }
    }
    else
    {
      return request->send(400, "text/plain", "incomplete param");
    } });

    server.begin();
}