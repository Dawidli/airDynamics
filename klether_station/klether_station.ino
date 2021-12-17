/******************************************

  This example works for both Industrial and STEM users.

  Developed by Jose Garcia, https://github.com/jotathebest/

* ****************************************/

/****************************************
   Include Libraries
 ****************************************/
#include "UbidotsEsp32Mqtt.h"
#include <RunningMedian.h>

RunningMedian samplesWind = RunningMedian(5);

RunningMedian samplesTemp = RunningMedian(5);
/****************************************
   Define Constants
 ****************************************/
// //Simen sin account!
//const char *UBIDOTS_TOKEN = "BBFF-lxUrvf1VPtA5yDPTGKoTrtGif4tfHJ";  // Put here your Ubidots TOKEN
//const char *WIFI_SSID = "Get-2G-DCFCD1"; //"CanYouGuessIt";
//const char *WIFI_PASS = "347BECJ7T4"; //"H0wD1dY0uGuess1t???";
//const char *DEVICE_LABEL = "wind";   // Put here your Device label to which data  will be published
//const char *VARIABLE_LABEL_1 = "anemometer"; // Put here your Variable label to which data  will be published
//const char *VARIABLE_LABEL_2 = "temp"; // Put here your Variable label to which data  will be published

//Simon sin #2 account
const char *UBIDOTS_TOKEN = "BBFF-5GSwh0GFChtfrPnCWWXPvSdIlN3nei";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "Get-2G-DCFCD1";
const char *WIFI_PASS = "347BECJ7T4"; //"H0wD1dY0uGuess1t???";
const char *DEVICE_LABEL = "wind";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL_1 = "anemometer"; // Put here your Variable label to which data  will be published
const char *VARIABLE_LABEL_2 = "temp"; // Put here your Variable label to which data  will be published

////Trum sin account!
//const char *UBIDOTS_TOKEN = "BBFF-vyL7Jrf7sul9GFM3mP17xlZTLxjNvK";  // Put here your Ubidots TOKEN
//const char *WIFI_SSID = "Get-2G-DCFCD1"; //"CanYouGuessIt";
//const char *WIFI_PASS = "347BECJ7T4"; //"H0wD1dY0uGuess1t???";
//const char *DEVICE_LABEL = "wind";   // Put here your Device label to which data  will be published
//const char *VARIABLE_LABEL_1 = "anemometer"; // Put here your Variable label to which data  will be published
//const char *VARIABLE_LABEL_2 = "temp"; // Put here your Variable label to which data  will be published

// //Dawid sin account!
//const char *UBIDOTS_TOKEN = "BBFF-lxUrvf1VPtA5yDPTGKoTrtGif4tfHJ";  // Put here your Ubidots TOKEN
//const char *WIFI_SSID = "Get-2G-DCFCD1"; //"CanYouGuessIt";
//const char *WIFI_PASS = "347BECJ7T4"; //"H0wD1dY0uGuess1t???";
//const char *DEVICE_LABEL = "wind";   // Put here your Device label to which data  will be published
//const char *VARIABLE_LABEL_1 = "anemometer"; // Put here your Variable label to which data  will be published
//const char *VARIABLE_LABEL_2 = "temp"; // Put here your Variable label to which data  will be published

const int TRIGGER_FREQUENCY = 1000; // Update rate in milliseconds

unsigned long timer;
uint8_t anemometer = 34; // Pin used to read data from GPIO34 ADC_CH6.
uint8_t tempSens = 35;

Ubidots ubidots(UBIDOTS_TOKEN);

/****************************************
   Auxiliar Functions
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
   Main Functions
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
  if (abs(millis() - timer) > TRIGGER_FREQUENCY) // triggers the routine every 1 seconds
  {
    anemometer_reading( anemometer );
    tempRead( tempSens );
    timer = millis();
  }

  // put your main code here, to run repeatedly:
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  
  if ( bool isBufferFull = samplesWind.isFull() ) // triggers the routine every time the array is full
  {
    float nemometer_mvalue = samplesWind.getMedian();  //Tar median av hele bufferen som sendes til ubidots.
    float temp_mvalue      = samplesTemp.getMedian();  //Tar median av hele temp bufferen som sendes til ubidots.

    ubidots.add(VARIABLE_LABEL_1, nemometer_mvalue); // Insert your variable Labels and the value to be sent
    ubidots.add(VARIABLE_LABEL_2, temp_mvalue);
    ubidots.publish(DEVICE_LABEL);

    samplesWind.clear(); //Sletter alle verdier i bufferen. Aka tar vi x antall målinger finner median av de og publisher én verdi som er funnet via de x antall målingene.

  }
  ubidots.loop();
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


float anemometer_reading(float sensor)
{
  float sensorValue = analogRead(sensor);
  float voltage = (sensorValue * 0.001221001); //esp har en maks analogread på 5 v og bits avlesning på 4095 dermed deler en på 5/4095
  float wind_speed = mapfloat(voltage, 0.38, 2, 0, 32.4);

  samplesWind.add(wind_speed);  //Legger wind speeden inn i bufferen/arrayen.

  return wind_speed;
}

float tempRead(float sensor)
{
  float bits = analogRead(sensor);
  float voltage = (bits/1023);
  float degreesC = (voltage - 0.5) * 100.0;

  Serial.print("bits: ");
  Serial.println(bits);

  Serial.print("volt: ");
  Serial.println(voltage);

  Serial.print("degreesC: ");
  Serial.println(degreesC);


  samplesTemp.add(degreesC);

  return degreesC;

}

/* ME FANT UT AV DET BITCHES*/
