/******************************************

  This example works for both Industrial and STEM users.

  Developed by Jose Garcia, https://github.com/jotathebest/

* ****************************************/

/****************************************
   Include Libraries
 ****************************************/
#include "UbidotsEsp32Mqtt.h"
#include <RunningMedian.h>

    //Calls on the buffers "samplesWind" and "samplesTemp".
RunningMedian samplesWind = RunningMedian(5);
RunningMedian samplesTemp = RunningMedian(5);
/****************************************
   Define Constants
 ****************************************/
    //Ubidot account
const char *UBIDOTS_TOKEN = "BBFF-5GSwh0GFChtfrPnCWWXPvSdIlN3nei";  //Put here your Ubidots TOKEN
const char *WIFI_SSID = "Wifi SSID / Name";
const char *WIFI_PASS = "Wifi password";
const char *DEVICE_LABEL = "wind";                                  //Put here your Device label to which data  will be published
const char *VARIABLE_LABEL_1 = "anemometer";                        //Put here your Variable label to which data  will be published
const char *VARIABLE_LABEL_2 = "temp";                              //Put here your Variable label to which data  will be published


const int TRIGGER_FREQUENCY = 1000;                                 // measurement rate in milliseconds

unsigned long timer;
uint8_t anemometer = 34;                                            // Pin used to read data from GPIO34 ADC_CH6.
uint8_t tempSens = 35;                                              // Pin used to read data from ESP32 integrated thermostat.

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
    //Sends data to ubidot.
void sendDataToUbidot()
{
  if ( bool isBufferFull = samplesWind.isFull() )       // triggers the routine every time wind-buffer is full
  {
    float nemometer_mvalue = samplesWind.getMedian();   //Takes the median of the entire anemometer-buffer.
    float temp_mvalue      = samplesTemp.getMedian();   //Takes the median of the entire temp-buffer.

    ubidots.add(VARIABLE_LABEL_1, nemometer_mvalue);    //Gets data ready to be sendt.
    ubidots.add(VARIABLE_LABEL_2, temp_mvalue);
    ubidots.publish(DEVICE_LABEL);                      //Sends measured data to ubidot.

    samplesWind.clear();                                //Deletes all values inside the wind-buffer.
    samplesTemp.clear();                                //Deletes all values inside the temp-buffer.

  }
  ubidots.loop();
}

    //Reads the value from the anemometer.
float anemometer_reading(float sensor)
{
  float sensorValue = analogRead(sensor);
  float voltage = (sensorValue * 0.001221001);          //Multiply the sensor value with (5v/4095bits).
  float wind_speed = mapfloat(voltage, 0.38, 2, 0, 32.4);

  samplesWind.add(wind_speed);                          //Adds the read wind speed to the buffer.
}

    //Reads the value off the thermostat and converts the value to celsius. 
float tempRead(float sensor)
{
  float bits = analogRead(sensor);
  float voltage = (bits / 1023);
  float degreesC = (voltage - 0.5) * 100.0;

  samplesTemp.add(degreesC);
}

    //Converts the sensor voltage to wind speed (m/s).
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
  if (abs(millis() - timer) > TRIGGER_FREQUENCY)  //Triggers the routine every 1 seconds
  {
    anemometer_reading( anemometer );
    tempRead( tempSens );
    timer = millis();
  }

  if (!ubidots.connected())                       //If the ESP didn't connect to ubidots it will retry.
  {
    ubidots.reconnect();
  }

  sendDataToUbidot();
}
