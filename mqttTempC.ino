#include <Ethernet.h>
#include <PubSubClient.h>
#include <math.h>
#include <SPI.h>

#define aref_voltage 3.3

int sensorPin = A0;
int sensorValue = 0;
int sensorOldValue = 0;

const int  micPin = 2;
int micClapCounter = 0;
int micState = 0;
int lastmicState = 0;

unsigned long lastTempPub = 0;
unsigned long lastClap = 0;

char sensorString[100];
char buf[12];

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
  analogReference(EXTERNAL);
  pinMode(micPin, INPUT);
}

void loop()
{
  
  if(client.connected())
  {
      client.loop();

      micState = digitalRead(micPin);

      if (micState != lastmicState)
      {
        if (micState == HIGH)
        {
          micClapCounter++;
          lastClap = millis();
          delay(15);
        }
      }

      lastmicState = micState;

      if(millis() - lastClap >= 500 && micClapCounter > 0)
      {
        itoa(micClapCounter, buf, 10);
        client.publish("byteli/clap", buf);
        micClapCounter = 0;
      }



      // is it time to pubslish the temperature again
      if(millis() < lastTempPub || millis() - lastTempPub >= 2500)
      {
        sensorValue = analogRead(sensorPin);
    
        if (sensorValue - sensorOldValue != 0)
        {
          float tempC = convertToTempC(sensorValue);
          dtostrf(tempC, 5, 2, sensorString);
          client.publish("byteli/temp/1", sensorString);
        }

        lastTempPub = millis();
        sensorOldValue = sensorValue;
      }
  
  
  }
  else
  {
    client.connect("arduinoTempC");
    delay(2000);
  }
 
}

float convertToTempC(int raw)
{
  
  float voltage = raw * aref_voltage;
  voltage /= 1024.0;
  
  float tempC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                         //to degrees ((volatge - 500mV) times 100)
  return tempC;
}
