#include <Ethernet.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <dht.h>

dht DHT;

typedef struct {

  int DHT22_PIN;

  float hum;
  float temp;

  float lastHum;
  float lastTemp;

  String humTopic;
  String tempTopic;

  unsigned long lastCheck;

  unsigned long lastTempPub;
  unsigned long lastHumPub;

} sensor;

#define SENSOR_COUNT 2

sensor sensors[SENSOR_COUNT] = {
  {
    7,
    0,
    0,
    0,
    0,
    "vw/hum/1",
    "vw/temp/1",
    0,
    0,
    0
  },
  {
    8,
    0,
    0,
    0,
    0,
    "vw/hum/2",
    "vw/temp/2",
    0,
    0,
    0
  },
};

char sensorStringBuf[25];
char sensorTopicStringBuf[25];

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

// Update these with values suitable for your network.
byte mac[]    = {  0x90, 0xA2, 0xDA, 0x0D, 0xB9, 0x1D };
byte server[] = { 172, 16, 42, 70 };
byte ip[]     = { 172, 16, 42, 36 };

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

void setup()
{
  Ethernet.begin(mac, ip);
}

void loop()
{
  
  if(client.connected())
  {
      client.loop();

      for(int sensor=0; sensor<SENSOR_COUNT; sensor++) {


        // is it time to pubslish the temperature again
        if(millis() < sensors[sensor].lastCheck || millis() - sensors[sensor].lastCheck >= 2000)
        {
          int chk = DHT.read22(sensors[sensor].DHT22_PIN);

          sensors[sensor].hum = DHT.humidity;
          sensors[sensor].temp  = DHT.temperature;

          if (chk == DHTLIB_OK && (sensors[sensor].temp != sensors[sensor].lastTemp || millis() - sensors[sensor].lastTempPub >= 10000))
          {
            dtostrf(sensors[sensor].temp, 5, 1, sensorStringBuf);
            sensors[sensor].tempTopic.toCharArray(sensorTopicStringBuf, 25);
            client.publish(sensorTopicStringBuf, sensorStringBuf);
            sensors[sensor].lastTempPub = millis();
          }
          if (chk == DHTLIB_OK && (sensors[sensor].hum != sensors[sensor].lastHum || millis() - sensors[sensor].lastHumPub >= 10000))
          {
            dtostrf(sensors[sensor].hum, 5, 1, sensorStringBuf);
            sensors[sensor].humTopic.toCharArray(sensorTopicStringBuf, 25);
            client.publish(sensorTopicStringBuf, sensorStringBuf);
            sensors[sensor].lastHumPub = millis();
          }
          sensors[sensor].lastHum = sensors[sensor].hum;
          sensors[sensor].lastTemp = sensors[sensor].temp;
          sensors[sensor].lastCheck = millis();
        }
      }
  
  }
  else
  {
    client.connect("arduinoTempB");
    delay(2000);
  }
 
}
