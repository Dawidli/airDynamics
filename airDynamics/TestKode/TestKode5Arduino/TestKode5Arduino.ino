const int PIN = A0;
const int GTFO = 11;
int value = 0;

void setup() {
Serial.begin(9600);
pinMode(GTFO, OUTPUT);
}

void loop() {
  float sensorValue = analogRead(PIN);
  float voltage = (sensorValue / 1023) * 5;
  float wind_speed = mapfloat(voltage, 0.4, 2, 0, 32.4);
  float speed_mph = ((wind_speed *3600)/1609.344);
  float speed_mps = speed_mph*0.447;
  Serial.println(speed_mps);
  analogWrite(GTFO, sensorValue);
  
 

}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
    {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
