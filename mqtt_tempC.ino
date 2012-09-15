#include <Ethernet.h>
#include <PubSubClient.h>
#include <math.h>
#include <SPI.h>

/*
char* tempTopic = "byteli/temp/1";
char* message = "LoL";*/

int sensorPin = A0;
int sensorValue = 0;
//int celsius = 0;

char sensorString[100];

// Update these with values suitable for your network.
byte mac[]    = {  0x90, 0xA2, 0xDA, 0x00, 0x58, 0x54 };
byte server[] = { 172, 16, 0, 70 };
byte ip[]     = { 172, 16, 0, 45 };

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

PubSubClient client(server, 1883, callback);

void setup()
{
  Ethernet.begin(mac, ip);
  client.connect("arduinoWill");
  Serial.begin(9600);
  
}

void loop()
{
  
  //client.loop();
  
  sensorValue = analogRead(sensorPin);
  
  float tempC = convertToTempC(sensorValue);

  dtostrf(tempC, 5, 2, sensorString);

  client.publish("byteli/temp/1", sensorString);
  delay(2500);
 
}

float convertToTempC(int raw)
{
  
  float voltage = raw * 5.0;
  voltage /= 1024.0;
  
  float tempC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                                //to degrees ((volatge - 500mV) times 100)
  return tempC;
}
