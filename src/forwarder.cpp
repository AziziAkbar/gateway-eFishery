// Library
#include <Arduino.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include "ESPAsyncForwarder.h"

// LoRa pinouts
#define SCK 5   // GPIO5  SCK
#define MISO 19 // GPIO19 MISO
#define MOSI 27 // GPIO27 MOSI
#define SS 18   // GPIO18 CS
#define RST 14  // GPIO14 RESET
#define DI00 26 // GPIO26 IRQ(Interrupt Request)

// Configuration
#define LoRa_Freq 923E6

// MQTT component
const char* broker = "broker.goiot.id";
WiFiClient espClient;
PubSubClient mqtt(espClient);

// connection via wifi
const char *ssid = "goiot4G";
const char *pass = "depok212";

void setup_wifi()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);
  LoRa.setPins(SS, RST, DI00);
  setup_wifi();
  ESPAsyncTCP();

  if (!LoRa.begin(LoRa_Freq))
  {
    Serial.println("Starting LoRa failed!");
    while (true);
  }
  mqtt.setServer(broker, 1883);
}

void reconnect()
{
  while (!mqtt.connected())
  {
    Serial.print("Attempting MQTT connection...");
    char *idnya = ConfigDataStructDevice.clientID;     // clientID
    char *username = ConfigDataStructDevice.username;  // username
    char *password = ConfigDataStructDevice.password;  // password
    if (mqtt.connect(idnya, username, password))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!mqtt.connected())
  {
    reconnect();
  }
  mqtt.loop();

  //variable Buffer
  String dataLora;

  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available())
    {
      // Serial.print((char)LoRa.read());
      dataLora = dataLora + String((char)LoRa.read()); // getting packet data LoRa
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }

  mqtt.publish("v2/5d4bf30a855d6736c5e16242/Device_1/direct/LoRa", dataLora.c_str()); // publish data LoRa to broker
}