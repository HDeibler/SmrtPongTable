
#define SENSOR_PIN_1 13

void setup() {
  Serial.begin(9600);
}

void loop() {
  readAndPrintSensorValue(SENSOR_PIN_1);
  delay(500);
}

void readAndPrintSensorValue(int pin) {
  int sensorValue = analogRead(pin);
  Serial.print("Sensor on pin ");
  Serial.print(pin);
  Serial.print(": ");
  Serial.println(sensorValue);
  
}
