const int INSERT_NAME_HERE = "Navnet ditt her";


void setup() {
  Serial.begin(9600);
}

void loop() {
  congratz(INSERT_NAME_HERE);
}

int congratz(INSERT_NAME_HERE)
  {
  Serial.print("Welcome to the server ");
  Serial.println(INSERT_NAME_HERE); 
  }
