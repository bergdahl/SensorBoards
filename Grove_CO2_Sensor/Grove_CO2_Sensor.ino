//---------------------------------------------------------------------------------------------
//
// Sketch for connecting the SEEED Studios Grove CO Sensor to a MQTT server.
//
// Requires a development board compatible with the WiFiClient.
//
// https://wiki.seeedstudio.com/Grove-Multichannel_Gas_Sensor/
//
// Jonny Bergdahl, 2020-06-12
//
//---------------------------------------------------------------------------------------------
//
// Includes
#include <WiFi.h>
#include <PubSubClient.h>                   // https://github.com/knolleary/pubsubclient
#include "secrets.h"
//---------------------------------------------------------------------------------------------
//
// Defines
//
#define RX_PIN 25
#define TX_PIN 26

#define MQTT_PUBLISH_INTERVAL 10000          // Time interval for MQTT publish in milliseconds
#define MQTT_CLIENT_NAME "co2sensor1"

#define CO2_TOPIC "cosensor1/co"             // MQTT topic for CO (Carbon Monoxide) value
#define TEMP_TOPIC "cosensor1/no"            // MQTT topic for NO (Nitric oxide) value
#define CO2_UNIT "PPM"
#define TEMP_UNIT "C"

const unsigned char cmd_get_sensor[] =
{
  0xff, 0x01, 0x86, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x79
};

unsigned char buffer[9];
HardwareSerial sensor(1);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
uint64_t lastPublish = 0;
int temperature;
int co2ppm;

//---------------------------------------------------------------------------------------------
//
// Arduino methods
//
void setup()
{
  Serial.begin(115200);
  Serial.println("TinyPICO Grove CO2 sensor starting up");
  Serial.println("-------------------------");
  sensor.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  setup_wifi();
  mqttClient.setServer(MQTT_SERVER, 1883);
}

void loop()
{
  if (!mqttClient.connected()) 
  {
    reconnect();
  }
  mqttClient.loop();      // Required for the MQTT client do it's job
  if (millis() - lastPublish >= MQTT_PUBLISH_INTERVAL) 
  {
    lastPublish = millis();

    if(readSensor())
    {
      Serial.println("Publishing values to MQTT:");
      publish(CO2_TOPIC, co2ppm, CO2_UNIT);
      publish(TEMP_TOPIC, temperature, TEMP_UNIT);
    }
  }
}

//---------------------------------------------------------------------------------------------
//
// Sensor methods
//
bool readSensor(void)
{
  int i = 0;

  Serial.print("Writing to sensor: ");
  for(i=0; i<sizeof(cmd_get_sensor); i++)
  {
    sensor.write(cmd_get_sensor[i]);
    Serial.print(cmd_get_sensor[i],HEX);
    Serial.print(" ");
  }
  delay(100);
  Serial.println("");
  //begin reveiceing data
  if(sensor.available())
  {
    Serial.print("Reading from sensor: ");
    while(sensor.available())
    {
      for(int i=0;i<9; i++)
      {
        buffer[i] = sensor.read();
        Serial.print(buffer[i],HEX);
        Serial.print(" ");
      }
    }
  }
  Serial.println("");

  // Calculate checksum
  if((i != 9) || (1 + (0xFF ^ (byte)(buffer[1] + buffer[2] + buffer[3] + buffer[4] + buffer[5] + buffer[6] + buffer[7]))) != buffer[8])
  {
    Serial.println("Checksum error, ignoring");
    return false;
  }

  // Set sensor. values
  co2ppm = (int)buffer[2] * 256 + (int)buffer[3];
  temperature = (int)buffer[4] - 40;

  return true;
}

//---------------------------------------------------------------------------------------------
//
// WiFi methods
//
void setup_wifi() 
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("Got IP address: ");
  Serial.println(WiFi.localIP());
}

//---------------------------------------------------------------------------------------------
//
// MQTT methods
//
void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("MQTT_CLIENT_NAME")) {
    if (mqttClient.connect(MQTT_CLIENT_NAME, MQTT_USER, MQTT_PASSWORD)) 
    {
      Serial.println("connected");
    } 
    else 
    {
      Serial.print("failed, state=");
      Serial.print(mqttClient.state());
      Serial.println(" trying again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void publish(char* topic, int value, char* unit)
{
  char buffer[32];
  sprintf(buffer, "%i", value);      // Format value with 2 decimals  
  Serial.print(topic);
  Serial.print(": ");
  Serial.print(buffer);
  Serial.print(" ");
  Serial.println(unit); 
  mqttClient.publish(topic, buffer, true);
}






