#include <Ethernet.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <dht.h>

dht DHT;

float hum, temp, lastHum, lastTemp;

unsigned long lastTempPub = 0;

char sensorString[100];

#define DHT22_PIN 7

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

// Update these with values suitable for your network.
byte mac[]    = {  0x90, 0xA2, 0xDA, 0x0D, 0xB9, 0x1D };
byte server[] = { 172, 16, 0, 70 };
byte ip[]     = { 172, 16, 42, 36 };

PubSubClient client(server, 1883, callback);

void setup()
{
  Ethernet.begin(mac, ip);
}

void loop()
{
  
  if(client.connected())
  {
      client.loop();


      // is it time to pubslish the temperature again
      if(millis() < lastTempPub || millis() - lastTempPub >= 2000)
      {
        int chk = DHT.read22(DHT22_PIN);
        
        hum = DHT.humidity;
        temp  = DHT.temperature;
    
        if (chk == DHTLIB_OK && temp != lastTemp)
        {
          dtostrf(temp, 5, 2, sensorString);
          client.publish("vw/temp/1", sensorString);
        }
        if (chk == DHTLIB_OK && hum != lastHum)
        {
          dtostrf(hum, 5, 2, sensorString);
          client.publish("vw/hum/1", sensorString);
        }

        lastTempPub = millis();
        lastHum = hum;
        lastTemp = temp;
      }
  
  }
  else
  {
    client.connect("arduinoTempB");
    delay(2000);
  }
 
}
