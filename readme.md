# Gateway Forwarder
Forwarder Node that acts as an intermediary for offline Sensor Nodes, and the cloud. Communication between Forwarder Node and Sensor Nodesare via a wireless protocol.

## How to use?
- Download or clone this repo to your own computer
- Look for `sensor.cpp` to program the sensor device and `forwarder.cpp` as gateway forwarder device
- Edit the configuration and LoRa pinouts below according to yours

```cpp
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
```

-- Now compile and upload the firmware according to platformio .env
### Functions in forwarder.cpp
#### setup wifi Functions (forwarder)
- WiFi.begin(ssid, pass);

| Parameters | Description |
| ------------- | ------------------------------ |
| `ssid` const char* | SSID_WIFI |
| `pass` const char* | PASS_WIFI |

- while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

To check status WiFi, if not connect ESP will reconnect (loop until connected)

#### LoRa Functions (forwarder)
- LoRa.setPins(SS, RST, DI00);

| Parameters | Description |
| ------------- | ------------------------------ |
| `SS` const char* | LoRa CS_PIN |
| `RST` const char* | LoRa RST_PIN |
| `DI00` const char* | LoRa IRQ_PIN |

- LoRa.begin(LoRa_Freq)
Begin LoRa with choose Frequency

| Parameters | Description |
| ------------- | ------------------------------ |
| `LoRa_Freq` double | LoRa Frequency |

- int packetSize = LoRa.parsePacket();
To receive and parse packet data from sender (sensor)

- String dataLora;
| Parameters | Description |
| ------------- | ------------------------------ |
| `dataLora` String | LoRa packet data |

- dataLora = dataLora + String((char)LoRa.read());
To read packet data from sender and get the value for buffer

#### MQTT Functions
- mqtt.setServer(broker, 1883);
Set MQTT server to broker.goiot.id and port 1883

- void reconnect()
If it not connect after 5 seconds will reconneting 

- mqtt.loop();
To allow the client to process incoming messages to send publish data and makes a refresh of the connection.

- mqtt.publish("v2/5d4bf30a855d6736c5e16242/Device_1/direct/LoRa", dataLora.c_str());
Publish to broker with payload dataLora

## Library Dependencies
- [LoRa](https://github.com/sandeepmistry/arduino-LoRa)
- [ESP Async WebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [PubSubClient](https://github.com/knolleary/pubsubclient)

 

### Functions in sensor.cpp
```cpp
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
```

#### DHT Functions
- DHT dht(DHTPIN, DHTTYPE);
- dht.begin();
float temperature = dht.readTemperature();

#### LoRa Functions (sensor)
- LoRa.setPins(SS, RST, DI00);

| Parameters | Description |
| ------------- | ------------------------------ |
| `SS` const char* | LoRa CS_PIN |
| `RST` const char* | LoRa RST_PIN |
| `DI00` const char* | LoRa IRQ_PIN |

- LoRa.begin(LoRa_Freq)
Begin LoRa with choose Frequency

- LoRa.beginPacket();
Begin the packet data
- LoRa.print(msgBuffer);
Send the packet data
- LoRa.endPacket();
End

#### Convert
- sprintf(msgBuffer, "%d.%02d", (int)temperature, (int)(temperature * 100) % 100);
convert value of temperature DHT to msgBuffer

## Library Dependencies
- [LoRa](https://github.com/sandeepmistry/arduino-LoRa)
- [DHT](https://github.com/adafruit/DHT-sensor-library)
