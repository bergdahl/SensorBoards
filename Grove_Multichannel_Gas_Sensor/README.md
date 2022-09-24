# Sensor_GroveMultichannelGas

Arduoino sketch for connecting a [SEEED Multichannel Gas Sensor V1](https://wiki.seeedstudio.com/Grove-Multichannel_Gas_Sensor/) to a MQTT server.

## Usage

Create a file named `secrets.h` in the same directory as the .ino file with the following content.

```
#define WIFI_SSID "<Your SSID>"
#define WIFI_PASSWORD "<Your WiFi password>"

#define MQTT_SERVER "<MQTT Server IP>"
#define MQTT_USER "<MQTT user name>"
#define MQTT_PASSWORD "<MQTT user password>"
```

## Libraries

The sketch uses the following libraries:

  - PubSubClient - https://github.com/knolleary/pubsubclient
  - MutichannelGasSensor - https://github.com/Seeed-Studio/Mutichannel_Gas_Sensor

Due to the fact that the library that SEEED provides only supports AVR style boards, I copied the source files into the project folder and made the needed changes. 
