/******************************************
 *
 * This example works for both Industrial and STEM users.
 *
 * Developed by Jose Garcia, https://github.com/jotathebest/
 *
 * ****************************************/

/****************************************
 * Include Libraries
 ****************************************/
#include "UbidotsEsp32Mqtt.h"

/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBFF-QtOMqjFUap0Syj9j5m2BVCr1ksd03q";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "Utleie";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "1020304050";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "test-connection";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL = "live-value-m-s"; // Put here your Variable label to which data  will be published

const int PUBLISH_FREQUENCY = 500; // Update rate in milliseconds

unsigned long timer;
uint8_t analogPin = 34; // Pin used to read data from GPIO34 ADC_CH6.


Ubidots ubidots(UBIDOTS_TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  timer = millis();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  if (abs(millis() - timer) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    float sensorValue = analogRead(analogPin);
    float voltage = (sensorValue / 1023) * 5;
    float wind_speed = mapfloat(voltage, 0.4, 2, 0, 32.4);
    float speed_mph = ((wind_speed *3600)/1609.344);
    float speed_mps = speed_mph*0.447;
    
    ubidots.add(VARIABLE_LABEL, speed_mps); // Insert your variable Labels and the value to be sent
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
    Serial.println(speed_mps);
  }
  
  ubidots.loop();
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
    {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }