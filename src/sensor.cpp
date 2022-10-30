// Library
#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"

// LoRa pinouts
#define SCK 5   // GPIO5  SCK
#define MISO 19 // GPIO19 MISO
#define MOSI 27 // GPIO27 MOSI
#define SS 18   // GPIO18 CS
#define RST 14  // GPIO14 RESET
#define DI00 26 // GPIO26 IRQ(Interrupt Request)

// DHT pinout
#define DHTPIN A0

// Configuration
#define LoRa_Freq 923E6
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Variables
char msgBuffer[50];


void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    dht.begin();
    while (!Serial);
    LoRa.setPins(SS, RST, DI00);

    if (!LoRa.begin(LoRa_Freq))
    {
        Serial.println("Starting LoRa failed!");
        while (true);
    }
}

void loop()
{
    // variable
    float temperature = dht.readTemperature();
    sprintf(msgBuffer, "%d.%02d", (int)temperature, (int)(temperature * 100) % 100); // convert value of temperature DHT to msgBuffer

    // send packet
    LoRa.beginPacket();
    LoRa.print(msgBuffer);
    LoRa.endPacket();
    delay(200);
}