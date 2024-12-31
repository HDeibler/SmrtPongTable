

#define SENSOR_PIN 7





void setup() {
  // Set up the TCRT5000 sensor pin
  Serial.begin(9600);
  pinMode(SENSOR_PIN, INPUT);
}

void loop() {
  // Read the sensor value
  int sensorValue = analogRead(SENSOR_PIN);
  Serial.println(sensorValue);
  delay(100);
}
