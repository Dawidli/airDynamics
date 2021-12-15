  /******************************************
 *
 * This example works for both Industrial and STEM users.
 
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
const char *UBIDOTS_TOKEN = "BBFF-lxUrvf1VPtA5yDPTGKoTrtGif4tfHJ";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "Get-2G-DCFCD1"; //"CanYouGuessIt";      
const char *WIFI_PASS = "347BECJ7T4"; //"H0wD1dY0uGuess1t???";   
const char *DEVICE_LABEL = "wind";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL = "test"; // Put here your Variable label to which data  will be published

const int PUBLISH_FREQUENCY = 1000; // Update rate in milliseconds

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
    float voltage = (sensorValue*0.001221001);//esp har en maks analogread på 5 v og bits avlesning på 4095 dermed deler en på 5/4095
    float wind_speed = mapfloat(voltage, 0.4, 2, 0, 32.4);
    
    Serial.println(voltage);
    
    
    //float value = analogRead(analogPin);
    
    ubidots.add(VARIABLE_LABEL, wind_speed); // Insert your variable Labels and the value to be sent
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }
  ubidots.loop();
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
    {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }




    /* ME FANT UT AV DET BITCHES*/
